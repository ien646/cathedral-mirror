#pragma once

#include <cathedral/core.hpp>
#include <cathedral/editor2/dialogs/confirm_dialog.hpp>
#include <cathedral/editor2/dialogs/text_input_dialog.hpp>
#include <cathedral/editor2/resource_managers/resource_manager_base.hpp>

FORWARD_CLASS(cathedral::engine, camera3d_node);
FORWARD_CLASS(cathedral::engine, mesh3d_node);

namespace cathedral::editor2
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
        std::vector<std::string> _filtered_mesh_names;

        std::string _filter;
        std::string _selected;

        // text_input_dialog _add_dialog{ "Add mesh", "Name" };
        text_input_dialog _rename_dialog{ "Rename mesh", "Name" };
        confirm_dialog _delete_confirm_dialog{ "Delete mesh", "Placeholder" };

        void init_callbacks();
    };
} // namespace cathedral::editor2