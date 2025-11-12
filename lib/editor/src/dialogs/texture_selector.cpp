#include <cathedral/editor/dialogs/texture_selector.hpp>

#include <cathedral/editor/callback_impl.hpp>
#include <cathedral/editor/widgets/texture_widget.hpp>
#include <cathedral/engine/scene.hpp>

#include <imgui.h>

#include <algorithm>

namespace cathedral::editor
{
    texture_selector::texture_selector(engine::scene& scene)
        : _scene(scene)
    {
    }

    void texture_selector::set_texture_list(std::vector<std::string> names)
    {
        _texture_list = MOVE(names);
        std::ranges::sort(_texture_list);

        _texture_widgets.clear();
        for (const auto& name : _texture_list)
        {
            _texture_widgets.emplace_back(_scene.load_texture(name));
        }
    }

    void texture_selector::open()
    {
        _open_flag.set(true);
    }

    void texture_selector::tick()
    {
        const auto ID = "Texture selector";
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
                for (size_t i = 0; i < _texture_widgets.size(); ++i)
                {
                    ImGui::PushID(i);

                    ImGui::SetNextItemAllowOverlap();
                    if (ImGui::Selectable(
                            "##texture_widget",
                            _selected == _texture_list[i],
                            0,
                            ImVec2(0, texture_widget::size().second + ImGui::GetStyle().FramePadding.y)))
                    {
                        _selected = _texture_list[i];
                    }

                    ImGui::SameLine();
                    _texture_widgets[i].tick();

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