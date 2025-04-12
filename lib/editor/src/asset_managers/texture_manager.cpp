#include <cathedral/editor/asset_managers/texture_manager.hpp>

#include <cathedral/editor/asset_managers/dialogs/new_texture_dialog.hpp>
#include <cathedral/editor/common/message.hpp>
#include <cathedral/editor/common/text_input_dialog.hpp>
#include <cathedral/editor/texture_utils.hpp>

#include <cathedral/engine/texture_decompression.hpp>
#include <cathedral/engine/texture_mip.hpp>

#include <cathedral/project/assets/texture_asset.hpp>
#include <cathedral/project/project.hpp>

#include <cathedral/core.hpp>

#include <ien/arithmetic.hpp>
#include <ien/initializers.hpp>
#include <ien/str_utils.hpp>

#include <QProgressDialog>
#include <QShowEvent>
#include <QtConcurrent>

#include <magic_enum.hpp>

#include <thread>

#include "ui_texture_manager.h"

namespace cathedral::editor
{
    constexpr ien::image_format texture_format_to_image_format(engine::texture_format format)
    {
        using namespace cathedral::engine;
        using namespace ien;
        switch (format)
        {
        case texture_format::R8_LINEAR:
        case texture_format::R8_SRGB:
            return image_format::R;
        case texture_format::R8G8_LINEAR:
        case texture_format::R8G8_SRGB:
            return image_format::RG;
        case texture_format::R8G8B8_LINEAR:
        case texture_format::R8G8B8_SRGB:
            return image_format::RGB;
        case texture_format::DXT1_BC1_SRGB:
        case texture_format::DXT5_BC3_SRGB:
        case texture_format::DXT1_BC1_LINEAR:
        case texture_format::DXT5_BC3_LINEAR:
        case texture_format::R8G8B8A8_SRGB:
        case texture_format::R8G8B8A8_LINEAR:
            return image_format::RGBA;
        }
        CRITICAL_ERROR("Unhandled texture format");
    }

    texture_manager::texture_manager(project::project* pro, QWidget* parent, bool allow_select)
        : QMainWindow(parent)
        , resource_manager_base(pro)
        , _ui(new Ui::texture_manager)
        , _allow_select(allow_select)
    {
        _ui->setupUi(this);

        _ui->label_Image->setMinimumSize(100, 100);

        connect(_ui->actionClose, &QAction::triggered, this, &SELF::close);
        connect(_ui->itemManagerWidget, &item_manager::add_clicked, this, &SELF::handle_add_texture);
        connect(_ui->itemManagerWidget, &item_manager::rename_clicked, this, &SELF::handle_rename_texture);
        connect(_ui->itemManagerWidget, &item_manager::delete_clicked, this, &SELF::handle_delete_texture);
        connect(_ui->itemManagerWidget, &item_manager::item_selection_changed, this, &SELF::handle_selected_texture_changed);

        if (_allow_select)
        {
            connect(_ui->pushButton_Select, &QPushButton::clicked, this, [this] {
                emit texture_selected(get_current_asset());
                close();
            });
            connect(_ui->pushButton_Cancel, &QPushButton::clicked, this, [this] { close(); });
        }
        else
        {
            delete _ui->pushButton_Cancel;
            delete _ui->pushButton_Select;
        }
    }

    item_manager* texture_manager::get_item_manager_widget()
    {
        return _ui->itemManagerWidget;
    }

    const item_manager* texture_manager::get_item_manager_widget() const
    {
        return _ui->itemManagerWidget;
    }

    void texture_manager::reload_current_image(bool force)
    {
        const auto selected_text = _ui->itemManagerWidget->current_text();
        if (selected_text.isEmpty())
        {
            return;
        }

        const auto name = selected_text.toStdString();
        const auto asset = get_assets().at(name);

        const auto adequate_mip_index = project::texture_asset::get_closest_sized_mip_index(
            _ui->label_Image->width(),
            _ui->label_Image->height(),
            asset->mip_sizes());
        if (force || _current_mip_index != adequate_mip_index)
        {
            _current_mip_index = adequate_mip_index;

            QtConcurrent::run([asset, mip_index = _current_mip_index] {
                const auto& mip_dim = asset->mip_sizes()[mip_index];
                return mip_to_qimage(asset->load_single_mip(mip_index), mip_dim.x, mip_dim.y, asset->format());
            }).then([this, saved_index = _image_update_sequence.load()](QImage img) {
                // Has someone else started loading an image?
                if (saved_index != _image_update_sequence)
                {
                    return;
                }
                _current_image = std::move(img);
                update_pixmap(_current_image);
            });
        }
        else
        {
            update_pixmap(_current_image);
        }
    }

    void texture_manager::update_pixmap(const QImage& image)
    {
        if (image.width() > 0)
        {
            _ui->label_Image->setAlignment(Qt::AlignmentFlag::AlignCenter);
            auto pixmap = QPixmap::fromImage(image).scaled(
                _ui->label_Image->size(),
                Qt::AspectRatioMode::KeepAspectRatio,
                Qt::TransformationMode::SmoothTransformation);
            _ui->label_Image->setPixmap(pixmap);
        }
    }

    void texture_manager::showEvent(QShowEvent* ev)
    {
        reload_item_list();
        ev->accept();
    }

    void texture_manager::resizeEvent([[maybe_unused]] QResizeEvent* ev)
    {
        if (_ui->itemManagerWidget->current_item() == nullptr)
        {
            _ui->label_Image->setPixmap(QPixmap{});
            return;
        }

        if (_resize_debouncer == nullptr)
        {
            _resize_debouncer = new QTimer(this);
            _resize_debouncer->setInterval(500);
            _resize_debouncer->setSingleShot(true);
            connect(_resize_debouncer, &QTimer::timeout, this, [this] { reload_current_image(true); });
        }

        set_empty_texture_loading();

        _resize_debouncer->start();
    }

    void texture_manager::closeEvent([[maybe_unused]] QCloseEvent* ev)
    {
        emit closed();
    }

    void texture_manager::set_empty_texture_loading()
    {
        _ui->label_Image->setPixmap({});
        _ui->label_Image->setStyleSheet("QLabel{color: white; background-color:black; font-size: 4em; font-weight: bold}");
        _ui->label_Image->setText("Loading...");
    }

    void texture_manager::handle_add_texture()
    {
        auto* newtex_diag = new new_texture_dialog(_ui->itemManagerWidget->get_texts(), this);
        if (newtex_diag->exec() != QDialog::DialogCode::Accepted)
        {
            return;
        }

        const auto format = magic_enum::enum_cast<engine::texture_format>(newtex_diag->format().toStdString());
        if (!format)
        {
            show_error_message("Invalid format", this);
            return;
        }

        const auto mip_levels = newtex_diag->mips();
        const auto mipgen_filter = magic_enum::enum_cast<ien::resize_filter>(newtex_diag->mipgen_filter().toStdString());
        if (!mipgen_filter)
        {
            show_error_message("Invalid mipmap generation filter", this);
            return;
        }

        constexpr auto MAX_PROGESS_RANGE = 1000;

        auto* progress_diag = new QProgressDialog(this);
        progress_diag->setWindowModality(Qt::WindowModality::WindowModal);
        progress_diag->setWindowTitle("Generating texture");
        progress_diag->setLabelText("Generating mips");
        progress_diag->setCancelButton(nullptr);
        progress_diag->setRange(0, MAX_PROGESS_RANGE);
        progress_diag->setAutoClose(false);
        progress_diag->show();

        std::jthread work_thread([this, newtex_diag, progress_diag, format, mip_levels, mipgen_filter] {
            const auto get_progress_for_mip_index = [](const int index) -> int {
                return MAX_PROGESS_RANGE - (MAX_PROGESS_RANGE / (2 << index));
            };

            const auto request_image_format = texture_format_to_image_format(*format);
            const ien::image source_image(newtex_diag->image_path().toStdString(), request_image_format);
            CRITICAL_CHECK(source_image.format() == request_image_format, "Image format mismatch");

            std::vector<std::vector<std::byte>> mips;
            std::vector<glm::uvec2> mip_sizes;
            mip_sizes.emplace_back(source_image.width(), source_image.height());

            auto mip0_data = [&] {
                if (is_compressed_format(*format))
                {
                    return create_compressed_texture_data(source_image, engine::get_format_compression_type(*format));
                }

                std::vector<std::byte> result(source_image.size());
                std::memcpy(result.data(), source_image.data(), source_image.size());
                return result;
            }();
            mips.emplace_back(std::move(mip0_data));
            QMetaObject::invokeMethod(this, [progress_diag, get_progress_for_mip_index] {
                progress_diag->setValue(get_progress_for_mip_index(1));
            });

            if (mip_levels > 1)
            {
                const auto generated_mips = engine::create_image_mips(source_image, *mipgen_filter, mip_levels - 1);
                for (size_t i = 0; i < generated_mips.size(); ++i)
                {
                    const auto& mip = generated_mips[i];
                    if (is_compressed_format(*format))
                    {
                        mips.push_back(create_compressed_texture_data(mip, engine::get_format_compression_type(*format)));
                    }
                    else
                    {
                        auto& vec = mips.emplace_back();
                        vec.resize(mip.size());
                        std::memcpy(vec.data(), mip.data(), vec.size());
                    }
                    mip_sizes.emplace_back(mip.width(), mip.height());
                    QMetaObject::invokeMethod(this, [progress_diag, get_progress_for_mip_index, i] {
                        progress_diag->setValue(get_progress_for_mip_index(static_cast<int>(i) + 1));
                    });
                }
            }

            const auto full_path = _project->name_to_abspath<project::texture_asset>(newtex_diag->name().toStdString());

            auto new_asset = std::make_shared<project::texture_asset>(_project, full_path);
            new_asset->set_width(static_cast<uint32_t>(source_image.width()));
            new_asset->set_height(static_cast<uint32_t>(source_image.height()));
            new_asset->set_format(*format);
            new_asset->set_mip_dimensions(mip_sizes);
            new_asset->mark_as_manually_loaded();
            new_asset->save();
            new_asset->save_mips(mips);

            QMetaObject::invokeMethod(this, [progress_diag] { progress_diag->accept(); });
        });
        progress_diag->exec();
        work_thread.join();

        _project->reload_texture_assets();
        reload_item_list();

        bool select_ok = _ui->itemManagerWidget->select_item(newtex_diag->name());
        CRITICAL_CHECK(select_ok, "Failure selecting item");
    }

    void texture_manager::handle_rename_texture()
    {
        rename_asset();
    }

    void texture_manager::handle_delete_texture()
    {
        delete_asset();
    }

    void texture_manager::handle_selected_texture_changed(std::optional<QString> selected)
    {
        _ui->label_Image->setPixmap(QPixmap{});
        _ui->label_Dimensions->setText("...");
        _ui->label_Format->setText("...");
        _ui->label_Mips->setText("...");
        _ui->label_Size->setText("...");

        const bool item_selected = selected.has_value() && !selected->isEmpty();
        if (_allow_select)
        {
            _ui->pushButton_Select->setEnabled(item_selected);
        }

        if (!item_selected)
        {
            return;
        }

        const auto selected_text = _ui->itemManagerWidget->current_text();
        const auto path = _project->name_to_abspath<project::texture_asset>(selected_text.toStdString());

        set_empty_texture_loading();

        const auto asset = _project->get_asset_by_path<project::texture_asset>(path);

        _ui->label_Dimensions->setText(QString::fromStdString(std::format("{}x{}", asset->width(), asset->height())));
        _ui->label_Format->setText(QString::fromStdString(std::string{ magic_enum::enum_name(asset->format()) }));
        _ui->label_Mips->setText(QString::number(asset->mip_sizes().size()));
        _ui->label_Size->setText("Loading...");
        _ui->label_Size->setText(
            QString::number(static_cast<float>(asset->texture_size_bytes()) / (1024 * 1024), 'g', 6) + "MiB");
        reload_current_image(true);
    }
} // namespace cathedral::editor