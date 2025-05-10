#include <cathedral/editor/asset_managers/shader_manager.hpp>

#include <cathedral/editor/asset_managers/dialogs/new_shader_dialog.hpp>
#include <cathedral/editor/asset_managers/shader_syntax_highlighter.hpp>

#include <cathedral/editor/common/code_editor.hpp>
#include <cathedral/editor/common/dock_title.hpp>
#include <cathedral/editor/common/message.hpp>
#include <cathedral/editor/common/text_output_dialog.hpp>

#include <cathedral/editor/styling.hpp>
#include <cathedral/editor/utils.hpp>

#include <cathedral/engine/scene.hpp>
#include <cathedral/engine/shader_preprocess.hpp>

#include <cathedral/project/project.hpp>

#include <ien/fs_utils.hpp>
#include <ien/str_utils.hpp>

#include <magic_enum.hpp>

#include <ranges>

#include "ui_shader_manager.h"

#include <qdiriterator.h>

namespace cathedral::editor
{
    constexpr auto SHADERS_RESOURCE_PATH = ":/shaders/";

    namespace
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

        std::optional<QPixmap> shader_manager_icon_filter(const project::shader_asset& asset)
        {
            switch (asset.type())
            {
            case gfx::shader_type::VERTEX:
                return QPixmap(":/icons/vertex_shader.png");
            case gfx::shader_type::FRAGMENT:
                return QPixmap(":/icons/fragment_shader.png");
            case gfx::shader_type::UNDEFINED:
            default:
                return std::nullopt;
            }
        }

        QStringList get_shader_templates()
        {
            QStringList result;
            if (const QDir dir(SHADERS_RESOURCE_PATH); dir.exists())
            {
                QDirIterator iterator(SHADERS_RESOURCE_PATH, QDirIterator::IteratorFlag::Subdirectories);
                while (iterator.hasNext())
                {
                    result << QFile(iterator.next()).fileName().replace(SHADERS_RESOURCE_PATH, "");
                }
            }
            return result;
        }
    } // namespace

    shader_manager::shader_manager(project::project* pro, engine::scene& scene, QWidget* parent)
        : QMainWindow(parent)
        , resource_manager_base(pro, &shader_manager_icon_filter)
        , _ui(new Ui::shader_manager())
        , _scene(scene)
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
        _ui->dockWidget_Right->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);

        _ui->listWidget_Templates->addItems(get_shader_templates());

        connect(_ui->itemManagerWidget, &item_manager::item_selection_changed, this, &SELF::handle_selected_shader_changed);
        connect(_ui->itemManagerWidget, &item_manager::add_clicked, this, &SELF::handle_add_shader_clicked);
        connect(_ui->itemManagerWidget, &item_manager::rename_clicked, this, &SELF::handle_rename_clicked);
        connect(_ui->itemManagerWidget, &item_manager::delete_clicked, this, &SELF::handle_delete_clicked);

        connect(_ui->pushButton_ShowProcessed, &QPushButton::clicked, this, &SELF::handle_show_processed_clicked);
        connect(_ui->pushButton_Validate, &QPushButton::clicked, this, &SELF::handle_validate_clicked);
        connect(_ui->pushButton_Save, &QPushButton::clicked, this, &SELF::handle_save_clicked);

        connect(_code_editor->text_edit_widget(), &QPlainTextEdit::textChanged, this, [this] {
            _ui->pushButton_Save->setEnabled(false);
        });
        connect(_code_editor->text_edit_widget(), &QPlainTextEdit::textChanged, this, &SELF::handle_text_edited);

        connect(_ui->listWidget_Templates, &QListWidget::itemSelectionChanged, this, [this] {
            const auto seletion_empty = _ui->listWidget_Templates->selectedItems().empty();
            _ui->pushButton_InsertTemplate->setEnabled(!seletion_empty);
        });

        connect(_ui->pushButton_InsertTemplate, &QPushButton::clicked, this, [this] {
            handle_shader_template_insert_clicked();
        });
    }

    item_manager* shader_manager::get_item_manager_widget()
    {
        return _ui->itemManagerWidget;
    }

    const item_manager* shader_manager::get_item_manager_widget() const
    {
        return _ui->itemManagerWidget;
    }

    void shader_manager::closeEvent(QCloseEvent* ev)
    {
        if (!_modified_shader_paths.empty())
        {
            if (show_confirm_dialog("Unsaved changes will be lost. Continue?", this))
            {
                emit closed();

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
            emit closed();

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

    void shader_manager::handle_selected_shader_changed()
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
        const auto path = _project->name_to_abspath<project::shader_asset>(selected_text.toStdString());
        auto asset = _project->get_asset_by_path<project::shader_asset>(path);

        const QString source = [this, asset] {
            if (!_temp_sources.contains(asset->name()))
            {
                _temp_sources[asset->name()] = QString::fromStdString(asset->source());
            }
            return _temp_sources[asset->name()];
        }();

        _code_editor->text_edit_widget()->blockSignals(true);
        _code_editor->text_edit_widget()->setPlainText(source);
        _code_editor->text_edit_widget()->blockSignals(false);

        _ui->comboBox_Type->setCurrentText(asset->type() == gfx::shader_type::VERTEX ? "VERTEX" : "FRAGMENT");

        _ui->pushButton_Validate->setEnabled(true);
    }

    void shader_manager::handle_add_shader_clicked()
    {
        auto* diag = new new_shader_dialog(this);
        if (diag->exec() == QDialog::Accepted)
        {
            const auto name = diag->result();
            const auto path = _project->name_to_abspath<project::shader_asset>(name.toStdString());
            const auto type = magic_enum::enum_cast<gfx::shader_type>(diag->type().toStdString());
            CRITICAL_CHECK(type.has_value(), "Invalid shader type");

            if (_project->shader_assets().contains(path))
            {
                show_error_message(QString{ "Shader with name '" } + name + "' already exists");
                return;
            }

            auto new_asset = std::make_shared<project::shader_asset>(_project, path);
            new_asset->set_type(type ? *type : gfx::shader_type::VERTEX);
            new_asset->mark_as_manually_loaded();

            const auto* const main_placeholder_string = *type == gfx::shader_type::VERTEX
                                                            ? "void main() \n{\n\tgl_Position = vec4(0, 0, 0, 0);\n}\n"
                                                            : "void main() \n{\n\t//...\n}\n";

            std::string source;
            source += main_placeholder_string;

            source = ien::str_trim(source, '\n');

            new_asset->set_source(source);
            new_asset->save();

            _project->add_asset(new_asset);
            reload_item_list();

            const bool select_ok = _ui->itemManagerWidget->select_item(name);
            CRITICAL_CHECK(select_ok, "Failure selecting item");
        }
    }

    void shader_manager::handle_show_processed_clicked()
    {
        const auto type = get_shader_type();
        const auto source = _code_editor->text_edit_widget()->toPlainText().toStdString();
        const auto pp_data = engine::get_shader_preprocess_data(source);
        if (!pp_data.has_value())
        {
            show_error_message(std::format("Preprocessing failed: {}", pp_data.error()), this);
            return;
        }
        const auto preprocessed_source = engine::preprocess_shader(type, *pp_data);
        if (!preprocessed_source.has_value())
        {
            show_error_message(std::format("Failure obtaining preprocessed source: {}", preprocessed_source.error()), this);
            return;
        }

        auto* diag = new text_output_dialog("Result", "Shader", QSTR(*preprocessed_source), this);
        diag->exec();
    }

    void shader_manager::handle_validate_clicked()
    {
        const auto type = get_shader_type();
        const auto source = _code_editor->text_edit_widget()->toPlainText().toStdString();
        const auto pp_data = engine::get_shader_preprocess_data(source);
        if (!pp_data.has_value())
        {
            show_error_message(std::format("Preprocessing failed: {}", pp_data.error()), this);
            return;
        }
        const auto preprocessed_source = engine::preprocess_shader(type, *pp_data);
        if (!preprocessed_source.has_value())
        {
            show_error_message(std::format("Failure obtaining preprocessed source: {}", preprocessed_source.error()), this);
            return;
        }

        const auto error_str = gfx::shader::validate(*preprocessed_source, type);

        if (!error_str.empty())
        {
            show_error_message(QSTR(error_str));
            return;
        }

        show_info_message("Ok!");
        _ui->pushButton_Save->setEnabled(true);
    }

    void shader_manager::handle_save_clicked()
    {
        if (_ui->itemManagerWidget->current_text().isEmpty())
        {
            return;
        }

        const auto selected_path = _ui->itemManagerWidget->current_text();
        const auto source = _code_editor->text_edit_widget()->toPlainText();
        const auto name = selected_path.toStdString();
        const auto type = get_shader_type();

        auto asset = _project->shader_assets().at(name);
        asset->set_source(source.toStdString());
        asset->set_type(type);
        asset->save();

        _ui->itemManagerWidget->current_item()->setFont(get_editor_font());
        _modified_shader_paths.erase(_ui->itemManagerWidget->current_text().toStdString());

        auto& renderer = _scene.get_renderer();
        const auto& material_assets = _project->get_assets<project::material_asset>();

        // Regenerate loaded materials that depend on this shader
        std::vector<std::string> regen_material_list;
        for (const auto& material_name : renderer.materials() | std::views::keys)
        {
            if (material_name.starts_with("__")) // skip embedded materials
            {
                continue;
            }

            const auto& mat_asset = material_assets.at(material_name);
            if (mat_asset->vertex_shader_ref() == name || mat_asset->fragment_shader_ref() == name)
            {
                regen_material_list.emplace_back(material_name);
            }
        }

        for (const auto& mat_name : regen_material_list)
        {
            renderer.vkctx().device().waitIdle();
            renderer.materials().erase(mat_name);
            std::ignore = _scene.load_material(mat_name);
        }
    }

    void shader_manager::handle_rename_clicked()
    {
        rename_asset();
    }

    void shader_manager::handle_delete_clicked()
    {
        delete_asset();
        _code_editor->text_edit_widget()->clear();
        _ui->pushButton_Validate->setEnabled(false);
        _ui->pushButton_Save->setEnabled(false);
    }

    void shader_manager::handle_text_edited()
    {
        if (_ui->itemManagerWidget->current_text().isEmpty())
        {
            return;
        }

        const auto name = _ui->itemManagerWidget->current_text().toStdString();
        _temp_sources[name] = _code_editor->text();

        const auto selected_path = _ui->itemManagerWidget->current_text();
        if (!selected_path.isEmpty())
        {
            _modified_shader_paths.emplace(selected_path.toStdString());
            _ui->itemManagerWidget->current_item()->setFont(get_edited_shader_font());
        }
    }

    void shader_manager::handle_shader_template_insert_clicked() const
    {
        if (_ui->listWidget_Templates->selectedItems().empty())
        {
            return;
        }

        const auto& selected_text = _ui->listWidget_Templates->selectedItems().at(0)->text();
        const auto full_path = SHADERS_RESOURCE_PATH + selected_text;

        QFile file(full_path);
        file.open(QFile::OpenModeFlag::ReadOnly);
        const QString text = file.readAll();
        file.close();

        _code_editor->text_edit_widget()->insertPlainText(text);
    }
} // namespace cathedral::editor