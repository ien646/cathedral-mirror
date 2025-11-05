#pragma once

#include <cathedral/editor2/callback_decl.hpp>
#include <cathedral/editor2/dialogs/confirm_dialog.hpp>
#include <cathedral/editor2/dialogs/text_input_dialog.hpp>
#include <cathedral/editor2/dialogs/texture_selector.hpp>
#include <cathedral/editor2/resource_managers/resource_filter.hpp>
#include <cathedral/editor2/resource_managers/resource_manager_base.hpp>
#include <cathedral/editor2/widgets/texture_widget.hpp>
#include <cathedral/project/assets/material_asset.hpp>

FORWARD_CLASS(cathedral::engine, scene);
FORWARD_CLASS(cathedral::project, project);

namespace cathedral::editor2
{
    class material_manager final : public resource_manager_base
    {
    public:
        explicit material_manager(project::project& pro);

        void tick() override;

        CATHEDRAL_DECLARE_CALLBACKS(
            (material_added, std::string name),
            (material_removed, std::string name),
            (material_renamed, std::string name),
            (material_modified, std::string name));

    private:
        text_input_dialog _add_dialog{ "Add material", "Name" };
        text_input_dialog _rename_dialog{ "Rename material", "Name" };
        confirm_dialog _delete_confirm_dialog{ "Delete material", "Placeholder" };
        texture_selector _texture_selector;
        resource_filter _resource_filter;

        std::vector<std::string> _available_material_names;
        std::vector<std::string> _available_vertex_shaders;
        std::vector<std::string> _available_fragment_shaders;
        unordered_map<std::string, engine::material> _dummy_materials;
        std::string _selected_material;
        std::vector<const std::string*> _filtered_material_names;

        unordered_map<std::string, std::unique_ptr<texture_widget>> _texture_widgets;

        void init_shaders();

        void init_callbacks();

        void tick_gui();

        void tick_material_uniform_vars_table(
            const std::shared_ptr<project::material_asset>& asset,
            const engine::material& dummy_material) const;

        void tick_node_uniform_vars_table(
            const std::shared_ptr<project::material_asset>& asset,
            const engine::material& dummy_material) const;

        void tick_material_texture_table(
            const std::shared_ptr<project::material_asset>& asset,
            const engine::material& dummy_material) const;

        void tick_node_texture_table(
            const std::shared_ptr<project::material_asset>& asset,
            const engine::material& dummy_material) const;

        void tick_material_buffer_table(
            const std::shared_ptr<project::material_asset>& asset,
            const engine::material& dummy_material) const;

        void tick_node_buffer_table(
            const std::shared_ptr<project::material_asset>& asset,
            const engine::material& dummy_material) const;

        void tick_material_textures(
            const std::shared_ptr<project::material_asset>& asset,
            const engine::material& dummy_material);

        void tick_properties();
    };
} // namespace cathedral::editor2