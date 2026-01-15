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
                MENU_CALLBACK("New Project", new_project_event);
                MENU_CALLBACK("Open Project", open_project_event);
                MENU_CALLBACK("Close", close_editor_event);
                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("Edit"))
            {
                MENU_CALLBACK("Settings", settings);
                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("Scene"))
            {
                MENU_CALLBACK("New", new_scene_event);
                MENU_CALLBACK("Open", open_scene_event);
                MENU_CALLBACK("Save", save_scene_event);
                MENU_CALLBACK("Save as", save_as_scene_event);
                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("Resources"))
            {
                MENU_CALLBACK("Fonts", font_manager_open_event);
                MENU_CALLBACK("Materials", material_manager_open_event);
                MENU_CALLBACK("Meshes", mesh_manager_open_event);
                MENU_CALLBACK("Scripts", script_manager_open_event);
                MENU_CALLBACK("Shaders", shader_manager_open_event);
                MENU_CALLBACK("Textures", texture_manager_open_event);
                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("Tools"))
            {
                MENU_CALLBACK("Capture screenshot", capture_screenshot_event);
                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("Window"))
            {
                MENU_CALLBACK("Reset layout", editor_window_reset_layout_event);
                ImGui::EndMenu();
            }

            constexpr auto BUTTON_WIDTH = 32; // Constant pixel size, so that buttons dont drift with clicks
            const auto menu_width = ImGui::GetWindowSize().x;

            ImGui::SameLine(menu_width - (BUTTON_WIDTH * 2) - (ImGui::GetStyle().ItemSpacing.x * 2));

            if (ImGui::Button("-", ImVec2(BUTTON_WIDTH, 0.0F)))
            {
                _event_bus.publish(global_text_scale_down_event{});
            }
            if (ImGui::Button("+", ImVec2(BUTTON_WIDTH, 0.0f)))
            {
                _event_bus.publish(global_text_scale_up_event{});
            }

            ImGui::EndMainMenuBar();
        }
    }
} // namespace cathedral::editor