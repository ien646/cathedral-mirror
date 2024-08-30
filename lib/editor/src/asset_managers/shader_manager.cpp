#include <cathedral/editor/asset_managers/shader_manager.hpp>

#include <cathedral/editor/asset_managers/shader_syntax_highlighter.hpp>

#include <cathedral/editor/common/code_editor.hpp>
#include <cathedral/editor/common/message.hpp>
#include <cathedral/editor/common/text_input_dialog.hpp>

#include <ien/fs_utils.hpp>
#include <ien/str_utils.hpp>

#include <filesystem>

#include <QMessageBox>

#include "ui_shader_manager.h"

namespace fs = std::filesystem;

namespace cathedral::editor
{
    shader_manager::shader_manager(project::project& pro)
        : _ui(new Ui::shader_manager())
        , _project(pro)
    {
        _ui->setupUi(this);

        delete _ui->plainTextEdit_editorPlaceHolder;
        _code_editor = new code_editor(this);

        auto* box_layout = dynamic_cast<QBoxLayout*>(layout());
        box_layout->insertWidget(1, _code_editor, 1);

        auto* text_widget = _code_editor->text_edit_widget();

        auto* highlighter = new shader_syntax_highlighter(text_widget->document());
        text_widget->setTabStopDistance(
            QFontMetrics(text_widget->font()).horizontalAdvance(' ') * 4);
        text_widget->setStyleSheet("QPlainTextEdit{background-color: #D0D0D0;}");

        connect(_ui->treeWidget_shaders, &QTreeWidget::itemSelectionChanged, this, &shader_manager::slot_selected_shader_changed);
        connect(_ui->pushButton_Validate, &QPushButton::clicked, this, &shader_manager::slot_validate_clicked);
        connect(_ui->pushButton_addShader, &QPushButton::clicked, this, &shader_manager::slot_add_shader_clicked);
        connect(_ui->pushButton_Save, &QPushButton::clicked, this, &shader_manager::slot_save_clicked);
        connect(_ui->pushButton_Rename, &QPushButton::clicked, this, &shader_manager::slot_rename_clicked);
        connect(_code_editor->text_edit_widget(), &QPlainTextEdit::textChanged, this, [this]{
            _ui->pushButton_Save->setEnabled(false);
        });

        reload();
    }

    void shader_manager::reload()
    {
        _ui->treeWidget_shaders->clear();

        for (auto sh : _project.shader_assets())
        {
            const auto relative_path = ien::str_trim(ien::str_split(sh->path(), _project.shaders_path())[0], '/');
            const auto name = std::filesystem::path(relative_path).replace_extension().string();

            auto item = new QTreeWidgetItem(_ui->treeWidget_shaders, { QString::fromStdString(name) });
            _ui->treeWidget_shaders->addTopLevelItem(item);
        }
    }

    gfx::shader_type shader_manager::get_shader_type() const
    {
        return _ui->comboBox_shaderType->currentText() == "VERTEX" ? gfx::shader_type::VERTEX : gfx::shader_type::FRAGMENT;
    }

    std::shared_ptr<project::shader_asset> shader_manager::get_shader_asset_by_path(const std::string& path) const
    {
        for (auto& asset : _project.shader_assets())
        {
            if (asset->path() == path)
            {
                return asset;
            }
        }
        return {};
    }

    void shader_manager::slot_selected_shader_changed()
    {
        if (_ui->treeWidget_shaders->selectedItems().empty())
        {
            _ui->pushButton_Save->setEnabled(false);
            _ui->pushButton_Validate->setEnabled(false);
            return;
        }

        const auto selected_text = _ui->treeWidget_shaders->selectedItems()[0]->text(0) + ".casset";
        const auto path = fs::path(_project.shaders_path()) / selected_text.toStdString();
        auto asset = get_shader_asset_by_path(path);
        if (!asset->is_loaded())
        {
            asset->load();
        }
        _code_editor->text_edit_widget()->setPlainText(QString::fromStdString(asset->source()));
        _ui->comboBox_shaderType->setCurrentText(asset->type() == gfx::shader_type::VERTEX ? "VERTEX" : "FRAGMENT");

        _ui->pushButton_Validate->setEnabled(true);
    }

    void shader_manager::slot_add_shader_clicked()
    {
        auto* diag = new text_input_dialog(this, "Create new shader", "Name:", false, "new_shader");
        if (diag->exec() == QDialog::Accepted)
        {
            const auto path = (fs::path(_project.shaders_path()) / diag->result().toStdString()).string() + ".casset";

            auto new_asset = std::make_shared<project::shader_asset>(path);
            new_asset->set_type(gfx::shader_type::VERTEX);
            new_asset->mark_as_manually_loaded();
            new_asset->save();

            _project.add_asset(new_asset);
            reload();
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
        if (_ui->treeWidget_shaders->selectedItems().empty())
        {
            return;
        }
        const auto selected_path = _ui->treeWidget_shaders->selectedItems()[0]->text(0) + ".casset";
        const auto source = _code_editor->text_edit_widget()->toPlainText();
        const auto path = fs::path(_project.shaders_path()) / selected_path.toStdString();
        const auto type = get_shader_type();
        for (auto& shasset : _project.shader_assets())
        {
            if (shasset->path() == path)
            {
                shasset->set_source(source.toStdString());
                shasset->set_type(type);
                shasset->save();
                return;
            }
        }
    }

    void shader_manager::slot_rename_clicked()
    {
        if (_ui->treeWidget_shaders->selectedItems().empty())
        {
            return;
        }
        const auto selected_path = _ui->treeWidget_shaders->selectedItems()[0]->text(0);
        const auto old_path = (fs::path(_project.shaders_path()) / selected_path.toStdString()).string() + ".casset";

        auto* input = new text_input_dialog(this, "Rename", "New name", false, selected_path);
        input->exec();

        QString result = input->result();
        if (result.isEmpty())
        {
            return;
        }

        const auto new_path = (fs::path(_project.shaders_path()) / result.toStdString()).string() + ".casset";

        auto asset = get_shader_asset_by_path(old_path);
        CRITICAL_CHECK(asset);

        asset->move_path(new_path);

        reload();
    }
} // namespace cathedral::editor