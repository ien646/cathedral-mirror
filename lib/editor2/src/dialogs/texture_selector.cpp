#include <algorithm>
#include <cathedral/editor2/dialogs/texture_selector.hpp>

#include <cathedral/editor2/utils.hpp>

#include <imgui.h>

#include <algorithm>

namespace cathedral::editor2
{
    void texture_selector::set_texture_list(std::vector<std::string> names)
    {
        _texture_list = std::move(names);
        std::ranges::sort(_texture_list);
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

        const auto button_height = ImGui::CalcTextSize("|").y + (ImGui::GetStyle().FramePadding.y * 2);

        if (ImGui::BeginPopupModal(ID))
        {
            if (ImGui::BeginListBox("##textures", ImVec2(ImGui::GetContentRegionAvail().x, button_height)))
            {
                for (const auto& tex : _texture_list)
                {
                    ImGui::Selectable(tex.c_str());
                }
                ImGui::EndListBox();
            }
            ImGui::EndPopup();
        }

        ImGui::BeginDisabled(_selected.empty());
        if (ImGui::Button("Select"))
        {
            try_call(callbacks.selected, _selected);
        }
        ImGui::EndDisabled();
    }
} // namespace cathedral::editor2