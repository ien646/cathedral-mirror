#include <cathedral/editor/editor_window/menubar.hpp>

#include <cathedral/editor/callback_impl.hpp>

#include <imgui.h>

#define MENU_CALLBACK(name, event)                                                                                          \
    if (ImGui::MenuItem(name))                                                                                              \
    {                                                                                                                       \
        _event_bus.publish(event{});                                                                                        \
    }

namespace cathedral::editor
{
    editor_window_menubar::editor_window_menubar(event_bus& bus)
        : _event_bus(bus)
    {
    }

    void editor_window_menubar::tick() const
    {
        if (ImGui::BeginMainMenuBar())
        {
            if (ImGui::BeginMenu("File"))
            {
                MENU_CALLBACK("New Project", events::new_project);
                MENU_CALLBACK("Open Project", events::open_project);
                MENU_CALLBACK("Close", events::close_editor);
                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("Edit"))
            {
                MENU_CALLBACK("Settings", events::settings_dialog_open);
                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("Scene"))
            {
                MENU_CALLBACK("New", events::new_scene);
                MENU_CALLBACK("Open", events::open_scene);
                MENU_CALLBACK("Save", events::save_scene);
                MENU_CALLBACK("Save as", events::save_as_scene);
                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("Resources"))
            {
                MENU_CALLBACK("Fonts", events::font_manager_open);
                MENU_CALLBACK("Materials", events::material_manager_open);
                MENU_CALLBACK("Meshes", events::mesh_manager_open);
                MENU_CALLBACK("Scripts", events::script_manager_open);
                MENU_CALLBACK("Shaders", events::shader_manager_open);
                MENU_CALLBACK("Textures", events::texture_manager_open);
                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("Tools"))
            {
                MENU_CALLBACK("Capture screenshot", events::capture_screenshot);
                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("Window"))
            {
                MENU_CALLBACK("Reset layout", events::editor_window_reset_layout);
                ImGui::EndMenu();
            }

            constexpr auto BUTTON_WIDTH = 32; // Constant pixel size, so that buttons dont drift with clicks
            const auto menu_width = ImGui::GetWindowSize().x;

            ImGui::SameLine(menu_width - (BUTTON_WIDTH * 2) - (ImGui::GetStyle().ItemSpacing.x * 2));

            if (ImGui::Button("-", ImVec2(BUTTON_WIDTH, 0.0F)))
            {
                _event_bus.publish(events::global_text_scale_down{});
            }
            if (ImGui::Button("+", ImVec2(BUTTON_WIDTH, 0.0f)))
            {
                _event_bus.publish(events::global_text_scale_up{});
            }

            ImGui::EndMainMenuBar();
        }
    }
} // namespace cathedral::editor