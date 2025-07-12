#include <cathedral/editor2/menubar.hpp>

#include <imgui.h>

#include <string>

namespace cathedral::editor2
{
    void map_menu_item_callbacks(std::initializer_list<std::pair<const char*, std::function<void()>>> map)
    {
        for (const auto& [text, cb] : map)
        {
            if (ImGui::MenuItem(text))
            {
                callback(cb);
            }
        }
    }

    void menubar::tick()
    {
        if (ImGui::BeginMainMenuBar())
        {
            if (ImGui::BeginMenu("File"))
            {
                map_menu_item_callbacks(
                    { { "New Project", _callbacks.new_project },
                      { "Open Project", _callbacks.open_project },
                      { "Close", _callbacks.close } });
                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("Scene"))
            {
                map_menu_item_callbacks(
                    { { "New", _callbacks.new_scene },
                      { "Open", _callbacks.open_scene },
                      { "Save", _callbacks.save_scene },
                      { "Save as", _callbacks.save_scene_as } });
                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("Resources"))
            {
                map_menu_item_callbacks(
                    { { "Materials", _callbacks.material_manager },
                      { "Meshes", _callbacks.mesh_manager },
                      { "Scripts", _callbacks.script_manager },
                      { "Shaders", _callbacks.shader_manager },
                      { "Textures", _callbacks.texture_manager } });
                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("Tools"))
            {
                if (ImGui::MenuItem("Capture Screenshot"))
                {
                    callback(_callbacks.capture_screenshot);
                }
                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("Help"))
            {
                if (ImGui::MenuItem("About"))
                {
                    callback(_callbacks.about);
                }
                ImGui::EndMenu();
            }
            ImGui::EndMainMenuBar();
        }
    }
} // namespace cathedral::editor2