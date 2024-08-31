#include <cathedral/editor/asset_managers/texture_manager.hpp>

#include <cathedral/editor/asset_managers/dialogs/new_texture_dialog.hpp>
#include <cathedral/editor/common/message.hpp>
#include <cathedral/editor/common/text_input_dialog.hpp>

#include <cathedral/engine/texture_decompression.hpp>
#include <cathedral/engine/texture_mip.hpp>

#include <cathedral/project/assets/texture_asset.hpp>
#include <cathedral/project/project.hpp>

#include <cathedral/core.hpp>

#include <ien/arithmetic.hpp>
#include <ien/initializers.hpp>
#include <ien/str_utils.hpp>

#include <QProgressDialog>

#include <magic_enum.hpp>

#include <filesystem>
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

    std::vector<uint8_t> rgba_to_qrgba(const std::vector<uint8_t>& image_data)
    {
        const uint32_t pixel_count = image_data.size() / 4;
        std::vector<uint8_t> rgba_data(pixel_count * 4);
        auto* rgba_u32ptr = reinterpret_cast<uint32_t*>(rgba_data.data());

#pragma omp parallel for
        for (size_t i = 0; i < pixel_count; ++i)
        {
            size_t src_offset = i * 4;
            rgba_u32ptr[i] = qRgba(
                image_data[src_offset],
                image_data[src_offset + 1],
                image_data[src_offset + 2],
                image_data[src_offset + 3]);
        }
        return rgba_data;
    }

    std::vector<uint8_t> rgb_to_qrgba(const std::vector<uint8_t>& image_data)
    {
        const uint32_t pixel_count = image_data.size() / 3;
        std::vector<uint8_t> rgba_data(pixel_count * 4);
        auto* rgba_u32ptr = reinterpret_cast<uint32_t*>(rgba_data.data());
        for (size_t i = 0; i < pixel_count; ++i)
        {
            size_t src_offset = i * 3;
            rgba_u32ptr[i] = qRgba(image_data[src_offset], image_data[src_offset + 1], image_data[src_offset + 2], 255);
        }
        return rgba_data;
    }

    std::vector<uint8_t> rg_to_qrgba(const std::vector<uint8_t>& image_data)
    {
        const uint32_t pixel_count = image_data.size() / 2;
        std::vector<uint8_t> rgba_data(pixel_count * 4);
        auto* rgba_u32ptr = reinterpret_cast<uint32_t*>(rgba_data.data());
        for (size_t i = 0; i < pixel_count; ++i)
        {
            size_t src_offset = i * 2;
            rgba_u32ptr[i] = qRgba(image_data[src_offset], image_data[src_offset + 1], 0, 255);
        }
        return rgba_data;
    }

    std::vector<uint8_t> r_to_qrgba(const std::vector<uint8_t>& image_data)
    {
        std::vector<uint8_t> rgba_data(image_data.size() * 4);
        auto* rgba_u32ptr = reinterpret_cast<uint32_t*>(rgba_data.data());
        for (size_t i = 0; i < image_data.size(); ++i)
        {
            rgba_u32ptr[i] = qRgba(image_data[i], image_data[i], image_data[i], 255);
        }
        return rgba_data;
    }

    std::vector<uint8_t> image_data_to_rgba(const std::vector<uint8_t>& image_data, engine::texture_format format)
    {
        using enum engine::texture_format;
        switch (format)
        {
        case R8G8B8A8_SRGB:
        case R8G8B8A8_LINEAR:
        case DXT1_BC1_LINEAR:
        case DXT1_BC1_SRGB:
        case DXT5_BC3_LINEAR:
        case DXT5_BC3_SRGB:
            return rgba_to_qrgba(image_data);
        case R8G8B8_SRGB:
        case R8G8B8_LINEAR:
            return rgb_to_qrgba(image_data);
        case R8G8_SRGB:
        case R8G8_LINEAR:
            return rg_to_qrgba(image_data);
        case R8_SRGB:
        case R8_LINEAR:
            return r_to_qrgba(image_data);
        default:
            CRITICAL_ERROR("Unhandled texture format");
        }
    }

    texture_manager::texture_manager(project::project& pro, QWidget* parent)
        : QMainWindow(parent)
        , _ui(new Ui::texture_manager)
        , _project(pro)
    {
        _ui->setupUi(this);

        _ui->label_Image->setMinimumSize(100, 100);

        connect(_ui->actionClose, &QAction::triggered, this, &SELF::close);
        connect(_ui->itemManagerWidget, &item_manager::add_clicked, this, &SELF::slot_add_texture);
        connect(_ui->itemManagerWidget, &item_manager::rename_clicked, this, &SELF::slot_rename_texture);
        connect(_ui->itemManagerWidget, &item_manager::delete_clicked, this, &SELF::slot_delete_texture);
        connect(_ui->itemManagerWidget, &item_manager::item_selection_changed, this, &SELF::slot_selected_texture_changed);

        reload();
    }

    void texture_manager::reload()
    {
        _ui->itemManagerWidget->clear_items();

        for (const auto& [path, asset] : _project.texture_assets())
        {
            const auto relative_path = ien::str_trim(ien::str_split(path, _project.textures_path())[0], '/');
            const auto name = std::filesystem::path(relative_path).replace_extension().string();

            _ui->itemManagerWidget->add_item(QString::fromStdString(name));
        }

        _ui->itemManagerWidget->sort_items(Qt::SortOrder::AscendingOrder);
    }

    void texture_manager::resizeEvent([[maybe_unused]] QResizeEvent* ev)
    {
        const bool item_selected = _ui->itemManagerWidget->current_item().has_value();
        if (!item_selected)
        {
            _ui->label_Image->setPixmap(QPixmap{});
            _current_image = {};
            return;
        }

        if (!_current_image.isNull())
        {
            _ui->label_Image->setAlignment(Qt::AlignmentFlag::AlignCenter);
            _current_pixmap =
                QPixmap::fromImage(_current_image)
                    .scaled(_ui->label_Image->size(), Qt::AspectRatioMode::KeepAspectRatio, Qt::TransformationMode::SmoothTransformation);
            _ui->label_Image->setPixmap(_current_pixmap);
        }
    }

    void texture_manager::slot_add_texture()
    {
        QStringList banned_names;
        auto* diag = new new_texture_dialog(_ui->itemManagerWidget->get_texts(), this);
        if (diag->exec() != QDialog::DialogCode::Accepted)
        {
            return;
        }

        const auto format = magic_enum::enum_cast<engine::texture_format>(diag->format().toStdString());
        if (!format)
        {
            show_error_message("Invalid format", this);
            return;
        }

        const auto mip_levels = diag->mips();
        const auto mipgen_filter = magic_enum::enum_cast<ien::resize_filter>(diag->mipgen_filter().toStdString());
        if (!mipgen_filter)
        {
            show_error_message("Invalid mipmap generation filter", this);
            return;
        }

        auto* progress_diag = new QProgressDialog(this);
        progress_diag->setWindowModality(Qt::WindowModality::WindowModal);
        progress_diag->setWindowTitle("Generating texture");
        progress_diag->setLabelText("Generating mips");
        progress_diag->setCancelButton(nullptr);
        progress_diag->setRange(0, mip_levels);
        progress_diag->setAutoClose(false);
        progress_diag->show();

        std::thread work_thread([&] {
            const auto request_image_format = texture_format_to_image_format(*format);
            const ien::image source_image(diag->image_path().toStdString(), request_image_format);
            CRITICAL_CHECK(source_image.format() == request_image_format);

            std::vector<std::vector<uint8_t>> mips;
            std::vector<std::pair<uint32_t, uint32_t>> mip_sizes;
            mip_sizes.emplace_back(source_image.width(), source_image.height());

            auto mip0_data = [&] -> std::vector<uint8_t> {
                if (is_compressed_format(*format))
                {
                    return create_compressed_texture_data(source_image, engine::get_format_compression_type(*format));
                }
                else
                {
                    std::vector<uint8_t> result(source_image.size());
                    std::memcpy(result.data(), source_image.data(), source_image.size());
                    return result;
                }
            }();
            mips.emplace_back(std::move(mip0_data));
            QMetaObject::invokeMethod(this, [&] { progress_diag->setValue(1); });

            if (mip_levels > 1)
            {
                for (const auto& mip : engine::create_image_mips(source_image, *mipgen_filter, mip_levels - 1))
                {
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
                    QMetaObject::invokeMethod(this, [&] { progress_diag->setValue(progress_diag->value() + 1); });
                }
            }

            const auto full_path = _project.textures_path() + "/" + diag->name().toStdString() + ".casset";

            auto new_asset = std::make_shared<project::texture_asset>(_project, full_path);
            new_asset->set_width(source_image.width());
            new_asset->set_height(source_image.height());
            new_asset->set_format(*format);
            new_asset->set_mips(std::move(mips));
            new_asset->set_mip_sizes(std::move(mip_sizes));
            new_asset->mark_as_manually_loaded();
            new_asset->save();

            QMetaObject::invokeMethod(this, [&] { progress_diag->accept(); });
        });
        progress_diag->exec();
        work_thread.join();

        _project.reload_texture_assets();
        reload();

        bool select_ok = _ui->itemManagerWidget->select_item(diag->name());
        CRITICAL_CHECK(select_ok);
    }

    void texture_manager::slot_rename_texture()
    {
        if (!_ui->itemManagerWidget->current_text())
        {
            return;
        }

        const auto selected_path = *_ui->itemManagerWidget->current_text();
        auto old_path = (std::filesystem::path(_project.textures_path()) / selected_path.toStdString()).concat(".casset");

        auto* input = new text_input_dialog(this, "Rename", "New name", false, selected_path);
        input->exec();

        QString result = input->result();
        if (result.isEmpty())
        {
            return;
        }

        auto new_path = (std::filesystem::path(_project.textures_path()) / result.toStdString()).concat(".casset");

        auto asset = _project.get_asset_by_path<project::texture_asset>(old_path.string());
        CRITICAL_CHECK(asset);

        asset->move_path(new_path);

        const auto old_bin_path = old_path.replace_extension(".lz4");
        const auto new_bin_path = new_path.replace_extension(".lz4");
        std::filesystem::rename(old_bin_path, new_bin_path);

        _project.reload_texture_assets();
        reload();
    }

    void texture_manager::slot_delete_texture()
    {
        if (!_ui->itemManagerWidget->current_text())
        {
            return;
        }

        const auto selected_path = *_ui->itemManagerWidget->current_text();

        const bool confirm = show_confirm_dialog("Delete texture '" + selected_path + "'?");
        if (confirm)
        {
            auto full_path =
                (std::filesystem::path(_project.textures_path()) / selected_path.toStdString()).concat(".casset");
            std::filesystem::remove(full_path);

            const auto bin_path = full_path.replace_extension(".lz4");
            std::filesystem::remove(bin_path);

            _project.reload_texture_assets();
            reload();
        }
    }

    void texture_manager::slot_selected_texture_changed()
    {
        const bool item_selected = _ui->itemManagerWidget->current_item().has_value();
        if (!item_selected)
        {
            _ui->label_Image->setPixmap(QPixmap{});
            _current_image = {};
            _ui->label_Dimensions->setText("...");
            _ui->label_Format->setText("...");
            _ui->label_Mips->setText("...");
            _ui->label_Size->setText("...");
            return;
        }

        const auto selected_text = *_ui->itemManagerWidget->current_text() + ".casset";
        const auto path = std::filesystem::path(_project.textures_path()) / selected_text.toStdString();
        auto asset = _project.get_asset_by_path<project::texture_asset>(path.string());

        project::asset_load_guard load_guard(asset);

        size_t size = 0;
        for (const auto& mip : asset->mips())
        {
            size += mip.size();
        }

        _ui->label_Dimensions->setText(QString::fromStdString(std::format("{}x{}", asset->width(), asset->height())));
        _ui->label_Format->setText(QString::fromStdString(std::string{ magic_enum::enum_name(asset->format()) }));
        _ui->label_Mips->setText(QString::number(asset->mips().size()));
        _ui->label_Size->setText(QString::number(static_cast<float>(size) / (1024 * 1024), 'g', 6) + "MiB");

        std::vector<uint8_t> image_data = [&] -> std::vector<uint8_t> {
            if (engine::is_compressed_format(asset->format()))
            {
                return engine::decompress_texture_data(
                    asset->mips()[0].data(),
                    asset->mips()[0].size(),
                    asset->width(),
                    asset->height(),
                    engine::get_format_compression_type(asset->format()));
            }
            else
            {
                return asset->mips()[0];
            }
        }();

        const auto rgba_data = image_data_to_rgba(image_data, asset->format());

        _current_image = QImage(asset->width(), asset->height(), QImage::Format::Format_RGBA8888);
        std::memcpy(_current_image.bits(), rgba_data.data(), rgba_data.size());

        _current_pixmap =
            QPixmap::fromImage(_current_image)
                .scaled(_ui->label_Image->size(), Qt::AspectRatioMode::KeepAspectRatio, Qt::TransformationMode::SmoothTransformation);

        _ui->label_Image->setAlignment(Qt::AlignmentFlag::AlignCenter);
        _ui->label_Image->setPixmap(_current_pixmap);
    }
} // namespace cathedral::editor