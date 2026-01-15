#pragma once

#include <cathedral/editor/callback_decl.hpp>
#include <cathedral/editor/dialogs/confirm_dialog.hpp>
#include <cathedral/editor/dialogs/text_input_dialog.hpp>
#include <cathedral/editor/dialogs/texture_selector.hpp>
#include <cathedral/editor/resource_managers/resource_filter.hpp>
#include <cathedral/editor/resource_managers/resource_manager_base.hpp>
#include <cathedral/editor/widgets/texture_widget.hpp>
#include <cathedral/event_bus.hpp>
#include <cathedral/project/assets/material_asset.hpp>

FORWARD_CLASS(cathedral::engine, scene);
FORWARD_CLASS(cathedral::project, project);

namespace cathedral::editor
{
    struct material_added_event
    {
        std::string name;
    };

    struct material_renamed_event
    {
        std::string old_name;
        std::string new_name;
    };

    struct material_deleted_event
    {
        std::string name;
    };

    struct material_modified_event
    {
        std::string name;
    };

    class material_manager final : public resource_manager_base
    {
    public:
        explicit material_manager(project::project& pro, editor_settings_interface& editor_settings, event_bus& bus);

        void tick() override;

    private:
        editor_settings_interface& _editor_settings;
        event_bus& _event_bus;
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

        [[nodiscard]] bool tick_material_uniform_vars_table(
            const std::shared_ptr<project::material_asset>& asset,
            const engine::material& dummy_material) const;

        [[nodiscard]] bool tick_node_uniform_vars_table(
            const std::shared_ptr<project::material_asset>& asset,
            const engine::material& dummy_material) const;

        [[nodiscard]] bool tick_material_texture_table(
            const std::shared_ptr<project::material_asset>& asset,
            const engine::material& dummy_material) const;

        [[nodiscard]] bool tick_node_texture_table(
            const std::shared_ptr<project::material_asset>& asset,
            const engine::material& dummy_material) const;

        [[nodiscard]] bool tick_material_buffer_table(
            const std::shared_ptr<project::material_asset>& asset,
            const engine::material& dummy_material) const;

        [[nodiscard]] bool tick_node_buffer_table(
            const std::shared_ptr<project::material_asset>& asset,
            const engine::material& dummy_material) const;

        void tick_material_textures(
            const std::shared_ptr<project::material_asset>& asset,
            const engine::material& dummy_material);

        void tick_properties();
    };
} // namespace cathedral::editor