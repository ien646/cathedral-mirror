#pragma once

#include <cathedral/editor/callback_decl.hpp>
#include <cathedral/editor/dialogs/confirm_dialog.hpp>
#include <cathedral/editor/dialogs/message_dialog.hpp>
#include <cathedral/editor/dialogs/text_input_dialog.hpp>
#include <cathedral/editor/resource_managers/add_texture_dialog.hpp>
#include <cathedral/editor/resource_managers/resource_filter.hpp>
#include <cathedral/editor/resource_managers/resource_manager_base.hpp>

namespace cathedral::editor
{
    class texture_manager : public resource_manager_base
    {
    public:
        explicit texture_manager(project::project& pro, editor_settings_interface& editor_settings);

        void tick() override;

        CATHEDRAL_DECLARE_CALLBACKS(
            (texture_added, std::string name),
            (texture_renamed, std::string old_name, std::string new_name),
            (texture_removed, std::string name));

    private:
        editor_settings_interface& _editor_settings;
        resource_filter _resource_filter;

        add_texture_dialog _add_texture_dialog;
        text_input_dialog _rename_dialog{ "Rename texture", "Name" };
        confirm_dialog _delete_confirm_dialog{ "Delete texture", "Placeholder" };
        message_dialog _message_dialog;

        std::vector<std::string> _available_texture_names;
        std::vector<const std::string*> _filtered_texture_names;
        std::string _selected_texture;

        bool _texture_changed = false;
        std::shared_ptr<engine::texture> _texture;
        void* _imgui_texture = nullptr;

        void tick_gui();

        void handle_texture_creation(
            std::string name,
            std::string path,
            engine::texture_format format,
            uint8_t mip_count,
            ien::resize_filter mipgen_filter);
    };
} // namespace cathedral::editor