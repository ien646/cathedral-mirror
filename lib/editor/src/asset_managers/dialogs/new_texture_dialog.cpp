#include <cathedral/editor/asset_managers/dialogs/new_texture_dialog.hpp>

#include <cathedral/editor/common/message.hpp>

#include <cathedral/engine/texture.hpp>

#include <cathedral/project/assets/texture_asset.hpp>

#include <QComboBox>
#include <QFileDialog>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QSpinBox>

#include <magic_enum.hpp>

namespace cathedral::editor
{
    new_texture_dialog::new_texture_dialog(QWidget* parent)
        : QDialog(parent)
    {
        setWindowTitle("New texture");
        setMinimumWidth(400);

        auto* name_edit = new QLineEdit("new_texture");

        auto* path_edit = new QLineEdit;
        path_edit->setDisabled(true);

        auto* browse_button = new QPushButton("...");

        auto* path_layout = new QHBoxLayout;
        path_layout->addWidget(path_edit, 1);
        path_layout->addWidget(browse_button);

        QStringList format_list;
        for (const auto& fmt : magic_enum::enum_names<engine::texture_format>())
        {
            format_list << QString::fromStdString(std::string{ fmt });
        }
        auto* format_combo = new QComboBox;
        format_combo->addItems(format_list);

        auto* mips_spinbox = new QSpinBox;
        mips_spinbox->setMaximum(1);
        mips_spinbox->setMinimum(1);

        QStringList filter_list;
        for (const auto& filter : magic_enum::enum_names<ien::resize_filter>())
        {
            filter_list << QString::fromStdString(std::string{ filter });
        }
        auto* filter_combo = new QComboBox;
        filter_combo->addItems(filter_list);

        auto* create_button = new QPushButton("Create");

        auto* main_layout = new QFormLayout;
        main_layout->addRow("Name: ", name_edit);
        main_layout->addRow("Path: ", path_layout);
        main_layout->addRow("Format: ", format_combo);
        main_layout->addRow("Mip levels: ", mips_spinbox);
        main_layout->addRow("Mip creation filter: ", filter_combo);
        main_layout->addRow("", create_button);

        setLayout(main_layout);

        connect(browse_button, &QPushButton::clicked, this, [=, this] {
            auto* diag = new QFileDialog(this, "Select texture image source");
            diag->setFileMode(QFileDialog::FileMode::ExistingFile);
            if (diag->exec() == QDialog::DialogCode::Rejected)
            {
                return;
            }
            if (diag->selectedFiles().empty())
            {
                return;
            }

            const auto& selected_file = diag->selectedFiles()[0];

            std::optional<ien::image_info> iinfo = ien::get_image_info(selected_file.toStdString());
            if (!iinfo)
            {
                mips_spinbox->setMaximum(1);
                show_error_message("Not a valid image file");
                return;
            }

            uint32_t max_mips = gfx::get_max_mip_levels(iinfo->width, iinfo->height);

            const bool is_compressed = engine::is_compressed_format(
                *magic_enum::enum_cast<engine::texture_format>(format_combo->currentText().toStdString()));

            if (is_compressed)
            {
                max_mips = std::max<int>(1, max_mips - 2);
            }
            
            mips_spinbox->setMinimum(1);
            mips_spinbox->setMaximum(max_mips);
            mips_spinbox->setValue(mips_spinbox->maximum());

            path_edit->setText(diag->selectedFiles()[0]);
        });

        connect(format_combo, &QComboBox::currentTextChanged, this, [=, this] {
            if(path_edit->text().isEmpty())
            {
                return;
            }

            std::optional<ien::image_info> iinfo = ien::get_image_info(path_edit->text().toStdString());
            if (!iinfo)
            {
                mips_spinbox->setMaximum(1);
                show_error_message("Not a valid image file");
                return;
            }

            uint32_t max_mips = gfx::get_max_mip_levels(iinfo->width, iinfo->height);
            const bool is_compressed = engine::is_compressed_format(
                *magic_enum::enum_cast<engine::texture_format>(format_combo->currentText().toStdString()));

            if (is_compressed)
            {
                mips_spinbox->setMaximum(std::max<int>(1, max_mips - 2));
            }
            else
            {
                mips_spinbox->setMaximum(max_mips);
            }
            mips_spinbox->setMinimum(1);
            mips_spinbox->setValue(mips_spinbox->maximum());
        });

        connect(create_button, &QPushButton::clicked, this, [=, this] {
            const auto& selected_path = path_edit->text();
            if (!std::filesystem::exists(selected_path.toStdString()))
            {
                show_error_message("Image path does not exist");
                return;
            }
            if (!ien::is_valid_image_file(selected_path.toStdString()))
            {
                show_error_message("Invalid image file");
                return;
            }

            if (name_edit->text().isEmpty())
            {
                show_error_message("Name can't be empty");
            }

            _name = name_edit->text();
            _path = selected_path;
            _format = format_combo->currentText();
            _mips = mips_spinbox->value();
            _mipfilter = filter_combo->currentText();

            accept();
        });
    }
} // namespace cathedral::editor