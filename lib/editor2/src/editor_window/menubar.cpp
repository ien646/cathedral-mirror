#include <cathedral/editor2/editor_window/menubar.hpp>

#include <cathedral/editor2/utils.hpp>

#include <imgui.h>

namespace cathedral::editor2
{
    void editor_window_menubar::tick(const editor_settings_interface& settings)
    {
        if (ImGui::BeginMainMenuBar())
        {
            if (ImGui::BeginMenu("File"))
            {
                if (ImGui::MenuItem("New Project"))
                {
                    try_call(callbacks.new_project);
                }
                if (ImGui::MenuItem("Open Project"))
                {
                    try_call(callbacks.open_project);
                }
                if (ImGui::MenuItem("Close"))
                {
                    try_call(callbacks.close);
                }
                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("Scene"))
            {
                if (ImGui::MenuItem("New"))
                {
                    try_call(callbacks.new_scene);
                }
                if (ImGui::MenuItem("Open"))
                {
                    try_call(callbacks.open_scene);
                }
                if (ImGui::MenuItem("Save"))
                {
                    try_call(callbacks.save_scene);
                }
                if (ImGui::MenuItem("Save as"))
                {
                    try_call(callbacks.save_as_scene);
                }
                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("Resources"))
            {
                if (ImGui::MenuItem("Fonts"))
                {
                    try_call(callbacks.fonts);
                }
                if (ImGui::MenuItem("Materials"))
                {
                    try_call(callbacks.materials);
                }
                if (ImGui::MenuItem("Meshes"))
                {
                    try_call(callbacks.meshes);
                }
                if (ImGui::MenuItem("Shaders"))
                {
                    try_call(callbacks.shaders);
                }
                if (ImGui::MenuItem("Textures"))
                {
                    try_call(callbacks.textures);
                }
                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("Tools"))
            {
                if (ImGui::MenuItem("Capture screenshot"))
                {
                    try_call(callbacks.capture_screenshot);
                }
                ImGui::EndMenu();
            }

            constexpr auto BUTTON_WIDTH = 32; // Constant pixel size, so that buttons dont drift with clicks
            const auto menu_width = ImGui::GetWindowSize().x;

            ImGui::SameLine(menu_width - (BUTTON_WIDTH * 2) - (ImGui::GetStyle().ItemSpacing.x * 2));

            if (ImGui::Button("-", ImVec2(BUTTON_WIDTH, 0.0F)))
            {
                ImGui::GetIO().FontGlobalScale -= 0.1F;
                const auto scale = ImGui::GetIO().FontGlobalScale;
                settings.set(editor_settings::TEXT_SCALE, scale);
                try_call(callbacks.settings_changed);
            }
            if (ImGui::Button("+", ImVec2(BUTTON_WIDTH, 0.0f)))
            {
                ImGui::GetIO().FontGlobalScale += 0.1F;
                const auto scale = ImGui::GetIO().FontGlobalScale;
                settings.set(editor_settings::TEXT_SCALE, scale);
                try_call(callbacks.settings_changed);
            }

            ImGui::EndMainMenuBar();
        }
    }
} // namespace cathedral::editor2