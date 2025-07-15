#include "imgui_internal.h"

#include <cathedral/editor2/dialogs/confirm_dialog.hpp>

#include <imgui.h>

namespace cathedral::editor2
{
    void confirm_dialog::set_title(std::string title)
    {
        _title = std::move(title);
    }

    void confirm_dialog::set_message(std::string message)
    {
        _message = std::move(message);
    }

    void confirm_dialog::open()
    {
        _open = true;
        _first_open = true;
    }

    void confirm_dialog::tick()
    {
        if (_open && _first_open)
        {
            ImGui::OpenPopup(_title.c_str());
            _first_open = false;
        }

        if (ImGui::BeginPopupModal(
                _title.c_str(),
                &_open,
                ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_AlwaysAutoResize))
        {
            ImGui::Text(_message.c_str());
            if (ImGui::Button("Yes", {40, 0}))
            {
                callback(_callbacks.selected, true);
                _open = false;
            }
            ImGui::SameLine();
            if (ImGui::Button("No", {40, 0}))
            {
                callback(_callbacks.selected, false);
                _open = false;
            }

            ImGui::EndPopup();
        }
    }
} // namespace cathedral::editor2