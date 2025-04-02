#include <cathedral/editor/asset_managers/dialogs/new_texture_dialog.hpp>

#include <cathedral/editor/common/message.hpp>

#include <cathedral/engine/texture.hpp>

#include <cathedral/project/assets/texture_asset.hpp>

#include <ien/arithmetic.hpp>
#include <ien/initializers.hpp>

#include <QComboBox>
#include <QFileDialog>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QSpinBox>

#include <magic_enum.hpp>
#include <utility>

namespace cathedral::editor
{
    new_texture_dialog::new_texture_dialog(QStringList banned_names, QWidget* parent)
        : QDialog(parent)
        , _banned_names(std::move(banned_names))
    {
        setWindowTitle("New texture");
        setMinimumWidth(400);

        int placeholder_increment = 0;
        QString placeholder_name = "new_texture";
        while (_banned_names.contains(placeholder_name))
        {
            placeholder_name = "new_texture" + QString::number(placeholder_increment++);
        }
        _name_edit = new QLineEdit(placeholder_name);

        _path_edit = new QLineEdit;
        _path_edit->setDisabled(true);

        auto* browse_button = new QPushButton("...");

        auto* path_layout = new QHBoxLayout;
        path_layout->addWidget(_path_edit, 1);
        path_layout->addWidget(browse_button);

        _format_combo = new QComboBox;

        _format_warning_label = new QLabel("?");
        _format_warning_label->setToolTip(
            "Compressed texture formats are available only for images which dimensions are a power of 2");
        _format_warning_label->setStyleSheet(R"(QLabel{ 
            border-width:1px; border-style:solid; border-color: red; 
            background-color: #444444; color: yellow;
        })");

        auto* format_combo_layout = new QHBoxLayout;
        format_combo_layout->addWidget(_format_combo, 1);
        format_combo_layout->addWidget(_format_warning_label, 0);

        _mips_spinbox = new QSpinBox;
        _mips_spinbox->setMaximum(1);
        _mips_spinbox->setMinimum(1);

        QStringList filter_list;
        for (const auto& filter : magic_enum::enum_names<ien::resize_filter>())
        {
            filter_list << QString::fromStdString(std::string{ filter });
        }
        _filter_combo = new QComboBox;
        _filter_combo->addItems(filter_list);

        _create_button = new QPushButton("Create");
        _create_button->setEnabled(false);

        auto* main_layout = new QFormLayout;
        main_layout->addRow("Name: ", _name_edit);
        main_layout->addRow("Path: ", path_layout);
        main_layout->addRow("Format: ", format_combo_layout);
        main_layout->addRow("Mip levels: ", _mips_spinbox);
        main_layout->addRow("Mip creation filter: ", _filter_combo);
        main_layout->addRow("", _create_button);

        setLayout(main_layout);

        connect(browse_button, &QPushButton::clicked, this, &new_texture_dialog::handle_browse_clicked);
        connect(_format_combo, &QComboBox::currentTextChanged, this, &new_texture_dialog::handle_format_changed);
        connect(_create_button, &QPushButton::clicked, this, &new_texture_dialog::handle_create_clicked);
        connect(_name_edit, &QLineEdit::textChanged, this, [this] { update_states(); });

        update_states();
    }

    void new_texture_dialog::clamp_mips(const ien::image_info& iinfo)
    {
        const auto format_opt = magic_enum::enum_cast<engine::texture_format>(_format_combo->currentText().toStdString());
        CRITICAL_CHECK(format_opt.has_value(), "Invalid enum value");

        const bool is_compressed = engine::is_compressed_format(*format_opt);

        const auto max_mips = IEN_CONDITIONAL_INIT_LAZY(
            uint32_t,
            is_compressed,
            gfx::get_max_mip_levels(iinfo.width, iinfo.height) - 2,
            gfx::get_max_mip_levels(iinfo.width, iinfo.height));

        _mips_spinbox->setMinimum(1);
        _mips_spinbox->setMaximum(static_cast<int>(max_mips));
        _mips_spinbox->setValue(_mips_spinbox->maximum());
    }

    void new_texture_dialog::update_states()
    {
        if (_path_edit->text().isEmpty())
        {
            _format_warning_label->setVisible(false);
            return;
        }

        std::optional<ien::image_info> iinfo = ien::get_image_info(_path_edit->text().toStdString());
        if (!iinfo)
        {
            _mips_spinbox->setMaximum(1);
            _format_warning_label->setEnabled(false);
            show_error_message("Not a valid image file");
            return;
        }

        const bool is_power_of_2 = ien::is_power_of_2(iinfo->width) && ien::is_power_of_2(iinfo->height);
        _format_warning_label->setVisible(!is_power_of_2);

        QStringList format_list;
        for (const auto& [fmt, name] : magic_enum::enum_entries<engine::texture_format>())
        {
            if (engine::is_compressed_format(fmt))
            {
                if (is_power_of_2)
                {
                    format_list << QString::fromStdString(std::string{ name });
                }
            }
            else
            {
                format_list << QString::fromStdString(std::string{ name });
            }
        }
        const auto previous_text = _format_combo->currentText();
        _format_combo->blockSignals(true);
        _format_combo->clear();
        _format_combo->addItems(format_list);
        if (_format_combo->findText(previous_text) >= 0)
        {
            _format_combo->setCurrentText(previous_text);
        }
        _format_combo->blockSignals(false);

        clamp_mips(*iinfo);
        _create_button->setEnabled(!_path_edit->text().isEmpty() && !_name_edit->text().isEmpty());
    }

    void new_texture_dialog::handle_browse_clicked()
    {
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

        _path_edit->setText(diag->selectedFiles()[0]);

        update_states();
    }

    void new_texture_dialog::handle_create_clicked()
    {
        const auto& selected_path = _path_edit->text();
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

        const auto& name = _name_edit->text();
        if (name.isEmpty())
        {
            show_error_message("Name can't be empty");
            return;
        }

        if (_banned_names.contains(name))
        {
            show_error_message(std::format("Texture with name '{}' already exists", name.toStdString()));
            return;
        }

        _name = _name_edit->text();
        _path = selected_path;
        _format = _format_combo->currentText();
        _mips = _mips_spinbox->value();
        _mipfilter = _filter_combo->currentText();

        accept();
    }

    void new_texture_dialog::handle_format_changed()
    {
        if (_path_edit->text().isEmpty())
        {
            return;
        }

        std::optional<ien::image_info> iinfo = ien::get_image_info(_path_edit->text().toStdString());
        if (!iinfo)
        {
            _mips_spinbox->setMaximum(1);
            show_error_message("Not a valid image file");
            return;
        }

        update_states();
    }
} // namespace cathedral::editor