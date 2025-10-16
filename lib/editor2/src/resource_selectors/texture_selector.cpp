#include "imgui.h"

#include <cathedral/editor2/resource_selectors/texture_selector.hpp>

namespace cathedral::editor2
{
    void texture_selector::set_texture_list(std::vector<std::string> names)
    {
        _texture_list = std::move(names);
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

        if (ImGui::BeginPopupModal(ID))
        {
            if (ImGui::BeginListBox("##textures", ImGui::GetContentRegionAvail()))
            {
                ImGui::EndListBox();
            }
            ImGui::EndPopup();
        }
    }
} // namespace cathedral::editor2