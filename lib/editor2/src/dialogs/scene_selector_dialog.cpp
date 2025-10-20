#include <cathedral/editor2/dialogs/scene_selector_dialog.hpp>

#include <cathedral/editor2/utils.hpp>
#include <cathedral/project/project.hpp>

#include <imgui.h>

namespace cathedral::editor2
{
    void scene_selector_dialog::tick(const project::project& project)
    {
        constexpr auto ID = "Select Scene";
        if (_open_flag)
        {
            ImGui::OpenPopup(ID);
            _open_flag = false;
        }

        if (ImGui::BeginPopupModal(ID))
        {
            const auto available_scenes = project.available_scenes();
            if (available_scenes.empty())
            {
                ImGui::Text("No scenes available");
            }
            else
            {
                if (ImGui::BeginListBox("##scenes", ImVec2(ImGui::GetContentRegionAvail().x, 0)))
                {
                    for (size_t i = 0; i < available_scenes.size(); ++i)
                    {
                        const auto& scene_name = available_scenes[i];

                        ImGui::PushID(static_cast<int>(i));
                        if (ImGui::Selectable(scene_name.c_str(), _selected.has_value() && scene_name == *_selected))
                        {
                            _selected = scene_name;
                        }
                        ImGui::PopID();
                    }
                    ImGui::EndListBox();
                }

                if (!_selected.has_value())
                {
                    ImGui::BeginDisabled();
                }
                if (ImGui::Button("Select"))
                {
                    try_call(callbacks.selected, *_selected);
                    ImGui::CloseCurrentPopup();
                }
                if (!_selected.has_value())
                {
                    ImGui::EndDisabled();
                }

                ImGui::SameLine();

                if (ImGui::Button("Cancel"))
                {
                    ImGui::CloseCurrentPopup();
                }
            }
            ImGui::EndPopup();
        }
    }

    void scene_selector_dialog::open()
    {
        _open_flag = true;
    }
} // namespace cathedral::editor2