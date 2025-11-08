#pragma once

#include <cathedral/core.hpp>
#include <cathedral/editor2/dialogs/confirm_dialog.hpp>
#include <cathedral/editor2/dialogs/text_input_dialog.hpp>
#include <cathedral/editor2/resource_managers/resource_filter.hpp>
#include <cathedral/editor2/resource_managers/resource_manager_base.hpp>

FORWARD_CLASS(cathedral::engine, scene);
FORWARD_CLASS(cathedral::project, project);

namespace cathedral::editor2
{
    class script_manager final : public resource_manager_base
    {
    public:
        explicit script_manager(project::project& pro, editor_settings_interface& editor_settings);

        void tick() override;

    private:
        editor_settings_interface& _editor_settings;
        resource_filter _resource_filter;
        std::vector<std::string> _available_scripts;
        std::vector<const std::string*> _filtered_scripts;
        std::unordered_map<std::string, std::string> _modified_sources;
        std::string _selected;

        text_input_dialog _rename_dialog{ "Rename mesh", "Name" };
        confirm_dialog _delete_confirm_dialog{ "Delete mesh", "Placeholder" };

        void tick_gui();

        void save_current_script();
        void save_all_scripts();
    };
} // namespace cathedral::editor2