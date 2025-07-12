#include <cathedral/editor2/dialogs/open_scene_dialog.hpp>

#include <imgui.h>

namespace cathedral::editor2
{
    open_scene_dialog::open_scene_dialog(project::project& pro)
        : _project(pro)
    {
    }

    void open_scene_dialog::tick()
    {
        if (_open)
        {
            ImGui::OpenPopup("Open scene");
        }

        if (ImGui::BeginPopupModal(
                "Open scene",
                &_open,
                ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_AlwaysAutoResize))
        {
            if (ImGui::BeginListBox("##Scene List", ImVec2(250, 0)))
            {
                for (const auto& scene : _project.available_scenes())
                {
                    if (ImGui::Selectable(scene.c_str(), _selected == scene))
                    {
                        _selected = scene;
                    }
                }
                ImGui::EndListBox();
            }
            if (!_selected.empty())
            {
                if (ImGui::Button("Accept", ImVec2(-1, 24)))
                {
                    ImGui::CloseCurrentPopup();
                    _open = false;
                    callback(_callbacks.selected, _selected);
                    _selected = {};
                }
            }
            ImGui::EndPopup();
        }
    }

    void open_scene_dialog::open()
    {
        _open = true;
    }
} // namespace cathedral::editor2