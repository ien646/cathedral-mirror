#include "cathedral/editor2/callback_impl.hpp"

#include <cathedral/editor2/dialogs/confirm_dialog.hpp>

#include <imgui.h>

namespace cathedral::editor2
{
    confirm_dialog::confirm_dialog(std::string title, std::string label)
        : _title(MOVE(title))
        , _label(MOVE(label))
    {
    }

    void confirm_dialog::set_label(std::string text)
    {
        _label = MOVE(text);
    }

    void confirm_dialog::open()
    {
        _open_flag = true;
    }

    void confirm_dialog::tick()
    {
        const auto popup_id = _title + "##confirm_dialog";

        if (_open_flag)
        {
            ImGui::OpenPopup(popup_id.c_str());
            _open_flag = false;
        }

        if (ImGui::BeginPopupModal((popup_id).c_str(), nullptr, ImGuiWindowFlags_AlwaysAutoResize))
        {
            ImGui::Text("%s", _label.c_str());

            if (ImGui::Button("Accept"))
            {
                ImGui::CloseCurrentPopup();
                CALLBACK(accepted());
            }

            ImGui::SameLine();

            if (ImGui::Button("Cancel"))
            {
                ImGui::CloseCurrentPopup();
                CALLBACK(cancelled());
            }

            ImGui::EndPopup();
        }
    }
} // namespace cathedral::editor2