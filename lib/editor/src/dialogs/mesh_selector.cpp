#include <cathedral/editor/dialogs/mesh_selector.hpp>

#include <cathedral/editor/callback_impl.hpp>
#include <cathedral/editor/widgets/mesh_widget.hpp>
#include <cathedral/engine/scene.hpp>

#include <imgui.h>

#include <algorithm>

namespace cathedral::editor
{
    mesh_selector::mesh_selector(project::project& project, engine::scene& scene)
        : _project(project)
        , _scene(scene)
    {
    }

    void mesh_selector::set_mesh_list(std::vector<std::string> names)
    {
        _mesh_list = MOVE(names);
        std::ranges::sort(_mesh_list);

        _mesh_widgets.clear();
        for (const auto& name : _mesh_list)
        {
            _mesh_widgets.emplace_back(name, _project, _scene.get_renderer());
        }
    }

    void mesh_selector::open()
    {
        _open_flag.set(true);
    }

    void mesh_selector::tick()
    {
        constexpr auto ID = "Mesh selector";
        if (_open_flag.get_and_reset())
        {
            ImGui::OpenPopup(ID);
        }

        const auto button_height = ImGui::CalcTextSize("|").y
                                   + (ImGui::GetStyle().FramePadding.y * 2)
                                   + ImGui::GetStyle().ItemSpacing.y;

        if (ImGui::BeginPopupModal(ID))
        {
            if (ImGui::BeginListBox(
                    "##textures",
                    ImVec2(ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y - button_height)))
            {
                for (size_t i = 0; i < _mesh_widgets.size(); ++i)
                {
                    ImGui::PushID(i);

                    ImGui::SetNextItemAllowOverlap();
                    if (ImGui::Selectable(
                            "##mesh_widget",
                            _selected == _mesh_list[i],
                            0,
                            ImVec2(0, mesh_widget::size().second + ImGui::GetStyle().FramePadding.y)))
                    {
                        _selected = _mesh_list[i];
                    }

                    ImGui::SameLine();
                    _mesh_widgets[i].tick();

                    ImGui::PopID();
                }
                ImGui::EndListBox();
            }

            const auto button_width = (ImGui::GetContentRegionAvail().x - (ImGui::GetStyle().ItemSpacing.x)) / 2;

            const ImVec2 button_size(button_width, ImGui::GetContentRegionAvail().y);

            if (ImGui::Button("Cancel", button_size))
            {
                ImGui::CloseCurrentPopup();
            }
            ImGui::SameLine();

            ImGui::BeginDisabled(_selected.empty());
            if (ImGui::Button("Select", button_size))
            {
                CALLBACK(selected(_selected));
                ImGui::CloseCurrentPopup();
            }
            ImGui::EndDisabled();

            ImGui::EndPopup();
        }
    }
} // namespace cathedral::editor