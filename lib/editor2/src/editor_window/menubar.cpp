#include "cathedral/editor2/callback_impl.hpp"

#include <cathedral/editor2/editor_window/menubar.hpp>

#include <imgui.h>

namespace cathedral::editor2
{
    void editor_window_menubar::tick()
    {
        if (ImGui::BeginMainMenuBar())
        {
            if (ImGui::BeginMenu("File"))
            {
                if (ImGui::MenuItem("New Project"))
                {
                    CALLBACK(new_project());
                }
                if (ImGui::MenuItem("Open Project"))
                {
                    CALLBACK(open_project());
                }
                if (ImGui::MenuItem("Close"))
                {
                    CALLBACK(close());
                }
                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("Edit"))
            {
                if (ImGui::MenuItem("Settings"))
                {
                    CALLBACK(settings());
                }
                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("Scene"))
            {
                if (ImGui::MenuItem("New"))
                {
                    CALLBACK(new_scene());
                }
                if (ImGui::MenuItem("Open"))
                {
                    CALLBACK(open_scene());
                }
                if (ImGui::MenuItem("Save"))
                {
                    CALLBACK(save_scene());
                }
                if (ImGui::MenuItem("Save as"))
                {
                    CALLBACK(save_as_scene());
                }
                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("Resources"))
            {
                if (ImGui::MenuItem("Fonts"))
                {
                    CALLBACK(fonts());
                }
                if (ImGui::MenuItem("Materials"))
                {
                    CALLBACK(materials());
                }
                if (ImGui::MenuItem("Meshes"))
                {
                    CALLBACK(meshes());
                }
                if (ImGui::MenuItem("Shaders"))
                {
                    CALLBACK(shaders());
                }
                if (ImGui::MenuItem("Textures"))
                {
                    CALLBACK(textures());
                }
                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("Tools"))
            {
                if (ImGui::MenuItem("Capture screenshot"))
                {
                    CALLBACK(capture_screenshot());
                }
                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("Window"))
            {
                if (ImGui::MenuItem("Reset layout"))
                {
                    CALLBACK(reset_layout());
                }
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