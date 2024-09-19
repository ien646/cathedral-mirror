#include <cathedral/editor/asset_managers/shader_manager.hpp>

#include <cathedral/editor/asset_managers/dialogs/new_shader_dialog.hpp>
#include <cathedral/editor/asset_managers/shader_syntax_highlighter.hpp>

#include <cathedral/editor/common/code_editor.hpp>
#include <cathedral/editor/common/dock_title.hpp>
#include <cathedral/editor/common/message.hpp>
#include <cathedral/editor/common/text_input_dialog.hpp>

#include <cathedral/editor/styling.hpp>

#include <cathedral/engine/scene.hpp>

#include <cathedral/project/project.hpp>

#include <ien/fs_utils.hpp>
#include <ien/str_utils.hpp>

#include <magic_enum.hpp>

#include <QListWidget>
#include <QMessageBox>

#include "ui_shader_manager.h"

namespace fs = std::filesystem;

namespace cathedral::editor
{
    const QFont& get_edited_shader_font()
    {
        static std::unique_ptr<QFont> font;
        if (!font)
        {
            font = std::make_unique<QFont>(get_editor_font());
            font->setBold(true);
            font->setItalic(true);
        }
        return *font;
    }

    shader_manager::shader_manager(project::project& pro, QWidget* parent)
        : QMainWindow(parent)
        , resource_manager_base(pro)
        , _ui(new Ui::shader_manager())
    {
        _ui->setupUi(this);

        _code_editor = _ui->centralwidget;
        _code_editor->setEnabled(false);

        auto* text_widget = _code_editor->text_edit_widget();

        _highlighter = new shader_syntax_highlighter(text_widget->document());
        text_widget->setTabStopDistance(QFontMetrics(text_widget->font()).horizontalAdvance(' ') * 4);
        text_widget->setStyleSheet("QPlainTextEdit{background-color: #D0D0D0;}");

        _ui->dockWidget_ShaderList->setTitleBarWidget(new dock_title("Shaders", this));
        _ui->dockWidget_Right->setTitleBarWidget(new dock_title("Properties", this));

        connect(_ui->itemManagerWidget, &item_manager::item_selection_changed, this, &SELF::slot_selected_shader_changed);
        connect(_ui->itemManagerWidget, &item_manager::add_clicked, this, &SELF::slot_add_shader_clicked);
        connect(_ui->itemManagerWidget, &item_manager::rename_clicked, this, &SELF::slot_rename_clicked);
        connect(_ui->itemManagerWidget, &item_manager::delete_clicked, this, &SELF::slot_delete_clicked);

        connect(_ui->pushButton_Validate, &QPushButton::clicked, this, &SELF::slot_validate_clicked);
        connect(_ui->pushButton_Save, &QPushButton::clicked, this, &SELF::slot_save_clicked);

        connect(_code_editor->text_edit_widget(), &QPlainTextEdit::textChanged, this, [this] {
            _ui->pushButton_Save->setEnabled(false);
        });
        connect(_code_editor->text_edit_widget(), &QPlainTextEdit::textChanged, this, &SELF::slot_text_edited);
    }

    item_manager* shader_manager::get_item_manager_widget()
    {
        return _ui->itemManagerWidget;
    }

    void shader_manager::closeEvent(QCloseEvent* ev)
    {
        if (!_modified_shader_paths.empty())
        {
            if (show_confirm_dialog("Unsaved changes will be lost. Continue?", this))
            {
                close();
                ev->accept();
            }
            else
            {
                ev->ignore();
            }
        }
        else
        {
            close();
            ev->accept();
        }
    }

    void shader_manager::showEvent(QShowEvent* ev)
    {
        reload_item_list();
        ev->accept();
    }

    gfx::shader_type shader_manager::get_shader_type() const
    {
        return _ui->comboBox_Type->currentText() == "VERTEX" ? gfx::shader_type::VERTEX : gfx::shader_type::FRAGMENT;
    }

    void shader_manager::slot_selected_shader_changed()
    {
        const bool selected = _ui->itemManagerWidget->current_text() != nullptr;
        _ui->pushButton_Save->setEnabled(selected);
        _ui->pushButton_Validate->setEnabled(selected);
        _code_editor->setEnabled(selected);
        if (!selected)
        {
            return;
        }

        const auto selected_text = _ui->itemManagerWidget->current_text();
        const auto path = _project.name_to_abspath<project::shader_asset>(selected_text.toStdString());
        auto asset = _project.get_asset_by_path<project::shader_asset>(path);

        const QString source = [this, asset] {
            if (!_temp_sources.contains(asset->path()))
            {
                _temp_sources[asset->path()] = QString::fromStdString(asset->source());
            }
            return _temp_sources[asset->path()];
        }();

        _code_editor->text_edit_widget()->blockSignals(true);
        _code_editor->text_edit_widget()->setPlainText(source);
        _code_editor->text_edit_widget()->blockSignals(false);

        _ui->comboBox_Type->setCurrentText(asset->type() == gfx::shader_type::VERTEX ? "VERTEX" : "FRAGMENT");

        _ui->pushButton_Validate->setEnabled(true);
    }

    void shader_manager::slot_add_shader_clicked()
    {
        QStringList available_matdefs;
        for (const auto& [path, asset] : _project.material_definition_assets())
        {
            available_matdefs << QString::fromStdString(asset->relative_path());
        }

        auto* diag = new new_shader_dialog(available_matdefs, this);
        if (diag->exec() == QDialog::Accepted)
        {
            const auto name = diag->result();
            const auto path = _project.name_to_abspath<project::shader_asset>(name.toStdString());
            const auto type = magic_enum::enum_cast<gfx::shader_type>(diag->type().toStdString());

            if (_project.shader_assets().contains(path))
            {
                show_error_message(QString{ "Shader with name '" } + name + "' already exists");
                return;
            }

            auto new_asset = std::make_shared<project::shader_asset>(_project, path);
            new_asset->set_type(type ? *type : gfx::shader_type::VERTEX);
            new_asset->mark_as_manually_loaded();

            constexpr auto version_string = "#version 450";
            constexpr auto main_placeholder_string = "void main() \n{\n\t//...\n}\n";

            if (!diag->matdef().isEmpty())
            {
                const auto& matdef_name = diag->matdef();
                const auto matdef_asset_path =
                    _project.relpath_to_abspath<project::material_definition_asset>(matdef_name.toStdString());
                CRITICAL_CHECK(_project.material_definition_assets().count(matdef_asset_path));
                const auto matdef_asset = _project.material_definition_assets().at(matdef_asset_path);

                std::string source;
                source += std::string{ version_string } + "\n";
                if (!diag->type().isEmpty())
                {
                    if (type && *type == gfx::shader_type::VERTEX)
                    {
                        source += engine::STANDARD_VERTEX_INPUT_GLSLSTR;
                    }
                }
                source += std::string{ engine::SCENE_UNIFORM_GLSLSTR } + "\n\n";
                source += matdef_asset->get_definition().create_full_glsl_header() + "\n";
                source += main_placeholder_string;

                source = ien::str_trim(source, '\n');

                new_asset->set_source(source);
            }
            else
            {
                new_asset->set_source(std::string{ version_string } + "\n" + main_placeholder_string);
            }
            new_asset->save();

            _project.add_asset(new_asset);
            reload_item_list();

            bool select_ok = _ui->itemManagerWidget->select_item(name);
            CRITICAL_CHECK(select_ok);
        }
    }

    void shader_manager::slot_validate_clicked()
    {
        const auto type = get_shader_type();
        const auto source = _code_editor->text_edit_widget()->toPlainText().toStdString();

        const auto error_str = gfx::shader::validate(source, type);

        if (!error_str.empty())
        {
            show_error_message(QString::fromStdString(error_str));
        }
        else
        {
            show_info_message("Ok!");
            _ui->pushButton_Save->setEnabled(true);
        }
    }

    void shader_manager::slot_save_clicked()
    {
        if (_ui->itemManagerWidget->current_text().isEmpty())
        {
            return;
        }

        const auto selected_path = _ui->itemManagerWidget->current_text();
        const auto source = _code_editor->text_edit_widget()->toPlainText();
        const auto path = _project.name_to_abspath<project::shader_asset>(selected_path.toStdString());
        const auto type = get_shader_type();

        auto asset = _project.shader_assets().at(path);
        asset->set_source(source.toStdString());
        asset->set_type(type);
        asset->save();

        _ui->itemManagerWidget->current_item()->setFont(get_editor_font());
        _modified_shader_paths.erase(_ui->itemManagerWidget->current_text().toStdString());
    }

    void shader_manager::slot_rename_clicked()
    {
        rename_asset();
    }

    void shader_manager::slot_delete_clicked()
    {
        delete_asset();
        _code_editor->text_edit_widget()->clear();
        _ui->pushButton_Validate->setEnabled(false);
        _ui->pushButton_Save->setEnabled(false);
    }

    void shader_manager::slot_text_edited()
    {
        if (_ui->itemManagerWidget->current_text().isEmpty())
        {
            return;
        }

        const auto path =
            _project.name_to_abspath<project::shader_asset>(_ui->itemManagerWidget->current_text().toStdString());
        _temp_sources[path] = _code_editor->text();

        const auto selected_path = _ui->itemManagerWidget->current_text();
        if (!selected_path.isEmpty())
        {
            _modified_shader_paths.emplace(selected_path.toStdString());
            _ui->itemManagerWidget->current_item()->setFont(get_edited_shader_font());
        }
    }
} // namespace cathedral::editor