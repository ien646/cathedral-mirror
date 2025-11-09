#pragma once

#include <cathedral/editor/dialogs/confirm_dialog.hpp>
#include <cathedral/editor/dialogs/text_input_dialog.hpp>
#include <cathedral/editor/resource_managers/resource_filter.hpp>
#include <cathedral/editor/resource_managers/resource_manager_base.hpp>

FORWARD_CLASS(cathedral::engine, camera3d_node);
FORWARD_CLASS(cathedral::engine, mesh3d_node);

namespace cathedral::editor
{
    class mesh_manager final : public resource_manager_base
    {
    public:
        explicit mesh_manager(project::project& pro);

        void tick() override;

    private:
        engine::camera3d_node* _camera_node = nullptr;
        engine::mesh3d_node* _mesh_node = nullptr;
        std::vector<std::string> _available_mesh_names;
        std::shared_ptr<engine::material> _material;

        std::string _selected;

        std::vector<const std::string*> _filtered_mesh_names;
        resource_filter _resource_filter;

        text_input_dialog _rename_dialog{ "Rename mesh", "Name" };
        confirm_dialog _delete_confirm_dialog{ "Delete mesh", "Placeholder" };

        void init_callbacks();
        void init_material();

        void tick_gui();
    };
} // namespace cathedral::editor