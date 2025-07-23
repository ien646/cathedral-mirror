#include <cathedral/editor/asset_managers/atlas_manager.hpp>

#include <cathedral/editor/utils.hpp>

#include <QComboBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QSpinBox>

namespace cathedral::editor
{
    atlas_manager::atlas_manager(project::project& pro, engine::scene& scene, QWidget* parent, const bool allow_select)
        : resource_manager_base(&pro, {})
        , QMainWindow(parent)
        , _project(pro)
        , _scene(scene)
        , _allow_select(allow_select)
    {
        setMinimumWidth(600);
        setMinimumHeight(600);

        auto* main_widget = new QWidget;
        setCentralWidget(main_widget);

        auto* main_layout = new QHBoxLayout;
        main_widget->setLayout(main_layout);

        _item_manager = new item_manager(this);
        main_layout->addWidget(_item_manager);

        _placeholder_frame = new QFrame;
        auto* placeholder_layout = new QVBoxLayout;
        _placeholder_frame->setLayout(placeholder_layout);
        placeholder_layout->addWidget(new QLabel("Select an atlas"), 1, Qt::AlignHCenter | Qt::AlignVCenter);

        main_layout->addWidget(_placeholder_frame, 1);

        _edit_frame = new QFrame;
        main_layout->addWidget(_edit_frame, 1);

        _edit_layout = new QVBoxLayout;
        _edit_frame->setLayout(_edit_layout);

        auto* texture_layout = new QHBoxLayout;
        auto* texture_combo = new QComboBox;
        for (const auto& name : _project.texture_assets() | std::views::keys)
        {
            texture_combo->addItem(QSTR(name));
        }
        texture_layout->addWidget(texture_combo);

        auto* texture_browse_button = new QPushButton("Browse...");
        texture_layout->addWidget(texture_browse_button);

        _edit_layout->addLayout(texture_layout);

        auto* glyph_size_spinbox = new QSpinBox;
        glyph_size_spinbox->setMinimum(1);

        _edit_layout->addWidget(glyph_size_spinbox);

        // ... Atlas editor

        _edit_frame->hide();

        connect(_item_manager, &item_manager::add_clicked, this, [this] { handle_add_clicked(); });
        connect(_item_manager, &item_manager::rename_clicked, this, [this] { handle_rename_clicked(); });
        connect(_item_manager, &item_manager::delete_clicked, this, [this] { handle_delete_clicked(); });

        reload_item_list();
    }

    item_manager* atlas_manager::get_item_manager_widget()
    {
        return _item_manager;
    }

    const item_manager* atlas_manager::get_item_manager_widget() const
    {
        return _item_manager;
    }

    void atlas_manager::handle_add_clicked()
    {
        auto* diag = new text_input_dialog(this, "New atlas", "Name", false, "New atlas");
        if (diag->exec() == QDialog::Accepted)
        {
            const auto& name = diag->result_input().toStdString();
            const auto abs_path = _project.name_to_abspath<project::atlas_asset>(name);

            const auto asset = std::make_shared<project::atlas_asset>(&_project, abs_path);
            asset->mark_as_manually_loaded();
            asset->set_texture_ref(std::nullopt);
            asset->set_glyph_bounding_box({ 0, 0 });
            asset->set_glyph_rects({});
            asset->save();

            _project.reload_atlas_assets();
            reload_item_list();
        }
    }

    void atlas_manager::handle_rename_clicked()
    {
        const auto rename_result = rename_asset();
        if (rename_result.has_value())
        {
            [[maybe_unused]] const auto& [old_name, new_name] = rename_result.value();
            //...
            log_warning("Atlas manager: Unhandled asset rename propagation");
        }
    }

    void atlas_manager::handle_delete_clicked()
    {
        const auto delete_result = delete_asset();
        if (delete_asset().has_value())
        {
            [[maybe_unused]] const auto& name = delete_result.value();
            //...
            log_warning("Atlas manager: Unhandled asset deletion propagation");
        }
    }
} // namespace cathedral::editor