#include <cathedral/editor2/editor_window/menubar.hpp>

#include <imgui.h>

namespace cathedral::editor2
{
    void editor_window_menubar::tick(const double deltatime)
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

                ImGui::TextColored(ImVec4(1.0F, 1.0F, 0.0F, 1.0F), "%.2f FPS", 1.0 / deltatime);
            }
            ImGui::EndMainMenuBar();
        }
    }
} // namespace cathedral::editor2