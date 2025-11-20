#pragma once

#include "../../../../../../.cache/imguicolortextedit/c167bc661c755c3569c6e2f2fe8eb60fd28cd75d/TextEditor.h"

#include <cathedral/core.hpp>
#include <cathedral/editor/dialogs/confirm_dialog.hpp>
#include <cathedral/editor/dialogs/text_input_dialog.hpp>
#include <cathedral/editor/resource_managers/resource_filter.hpp>
#include <cathedral/editor/resource_managers/resource_manager_base.hpp>

FORWARD_CLASS(cathedral::engine, scene);
FORWARD_CLASS(cathedral::project, project);

namespace cathedral::editor
{
    class shader_manager final : public resource_manager_base
    {
    public:
        explicit shader_manager(project::project& pro, editor_settings_interface& editor_settings);

        void tick() override;

        CATHEDRAL_DECLARE_CALLBACKS(
            (shader_added, std::string name),
            (shader_renamed, std::string old_name, std::string new_name),
            (shader_removed, std::string name),
            (shader_modified, std::string name));

    private:
        editor_settings_interface& _editor_settings;
        resource_filter _resource_filter;
        std::vector<std::string> _available_shaders;
        std::vector<const std::string*> _filtered_shaders;
        std::unordered_map<std::string, std::string> _modified_sources;
        TextEditor _text_editor;
        std::string _selected;

        text_input_dialog _rename_dialog{ "Rename shader", "Name" };
        confirm_dialog _delete_confirm_dialog{ "Delete shader", "Placeholder" };

        void tick_gui();

        void save_current_shader();
        void save_all_shaders();
    };
} // namespace cathedral::editor