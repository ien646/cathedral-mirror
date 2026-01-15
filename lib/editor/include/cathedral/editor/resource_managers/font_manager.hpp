#pragma once

#include <cathedral/editor/dialogs/confirm_dialog.hpp>
#include <cathedral/editor/dialogs/text_input_dialog.hpp>
#include <cathedral/editor/resource_managers/add_font_dialog.hpp>
#include <cathedral/editor/resource_managers/resource_filter.hpp>
#include <cathedral/editor/resource_managers/resource_manager_base.hpp>
#include <cathedral/event_bus.hpp>

FORWARD_CLASS(cathedral::engine, scene);
FORWARD_CLASS(cathedral::project, project);

namespace cathedral::editor
{
    struct font_added_event : event
    {
        std::string name;
    };

    struct font_renamed_event : event
    {
        std::string old_name;
        std::string new_name;
    };

    struct font_removed_event : event
    {
        std::string name;
    };

    class font_manager final : public resource_manager_base
    {
    public:
        explicit font_manager(project::project& pro, editor_settings_interface& settings, event_bus& bus);

        void tick() override;

    private:
        editor_settings_interface& _editor_settings;
        event_bus& _event_bus;
        add_font_dialog _add_font_dialog;
        text_input_dialog _rename_dialog{ "Rename font", "Name" };
        confirm_dialog _delete_confirm_dialog{ "Delete font", "Placeholder" };
        resource_filter _resource_filter;

        std::vector<std::string> _available_font_names;
        std::vector<const std::string*> _filtered_font_names;
        std::string _selected_font;

        bool _loading_texture = false;
        unordered_map<std::string, void*> _texture_ids;
        glm::uvec2 _atlas_size = {};

        void tick_gui();
    };
} // namespace cathedral::editor