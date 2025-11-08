#include <cathedral/editor2/editor_window/menubar.hpp>

#include <cathedral/editor2/callback_impl.hpp>

#include <imgui.h>

#define MENU_CALLBACK(name, cb)                                                                                             \
    if (ImGui::MenuItem(name))                                                                                             \
    {                                                                                                                       \
        CALLBACK(cb());                                                                                                     \
    }

namespace cathedral::editor2
{
    void editor_window_menubar::tick() const
    {
        if (ImGui::BeginMainMenuBar())
        {
            if (ImGui::BeginMenu("File"))
            {
                MENU_CALLBACK("New Project", new_project);
                MENU_CALLBACK("Open Project", open_project);
                MENU_CALLBACK("Close", close);
                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("Edit"))
            {
                MENU_CALLBACK("Settings", settings);
                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("Scene"))
            {
                MENU_CALLBACK("New", new_scene);
                MENU_CALLBACK("Open", open_scene);
                MENU_CALLBACK("Save", save_scene);
                MENU_CALLBACK("Save as", save_as_scene);
                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("Resources"))
            {
                MENU_CALLBACK("Fonts", fonts);
                MENU_CALLBACK("Materials", materials);
                MENU_CALLBACK("Meshes", meshes);
                MENU_CALLBACK("Scripts", scripts);
                MENU_CALLBACK("Shaders", shaders);
                MENU_CALLBACK("Textures", textures);
                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("Tools"))
            {
                MENU_CALLBACK("Capture screenshot", capture_screenshot);
                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("Window"))
            {
                MENU_CALLBACK("Reset layout", reset_layout);
                ImGui::EndMenu();
            }

            constexpr auto BUTTON_WIDTH = 32; // Constant pixel size, so that buttons dont drift with clicks
            const auto menu_width = ImGui::GetWindowSize().x;

            ImGui::SameLine(menu_width - (BUTTON_WIDTH * 2) - (ImGui::GetStyle().ItemSpacing.x * 2));

            if (ImGui::Button("-", ImVec2(BUTTON_WIDTH, 0.0F)))
            {
                CALLBACK(text_scale_down());
            }
            if (ImGui::Button("+", ImVec2(BUTTON_WIDTH, 0.0f)))
            {
                CALLBACK(text_scale_up());
            }

            ImGui::EndMainMenuBar();
        }
    }
} // namespace cathedral::editor2