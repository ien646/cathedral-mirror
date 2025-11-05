#include <cathedral/editor2/dialogs/message_dialog.hpp>

#include <cathedral/editor2/colors.hpp>

#include <imgui.h>

namespace cathedral::editor2
{
    void message_dialog::set_title(std::string text)
    {
        _title = MOVE(text);
    }

    void message_dialog::set_text(std::string text)
    {
        _label = MOVE(text);
    }

    void message_dialog::set_mode(const message_dialog_mode mode)
    {
        _mode = mode;
    }

    void message_dialog::open()
    {
        _open_flag = true;
    }

    void message_dialog::tick()
    {
        const auto popup_id = _title + "##message_dialog";

        if (_open_flag)
        {
            ImGui::OpenPopup(popup_id.c_str());
            _open_flag = false;
        }

        switch (_mode)
        {
        case message_dialog_mode::INFO:
            ImGui::PushStyleColor(ImGuiCol_PopupBg, colors::BG_INFO_BLUE);
            break;
        case message_dialog_mode::WARNING:
            ImGui::PushStyleColor(ImGuiCol_PopupBg, colors::BG_WARNING_YELLOW);
            break;
        case message_dialog_mode::ERROR:
            ImGui::PushStyleColor(ImGuiCol_PopupBg, colors::BG_ERROR_RED);
            break;
        }

        if (ImGui::BeginPopupModal((popup_id).c_str(), nullptr, ImGuiWindowFlags_AlwaysAutoResize))
        {
            ImGui::Text("%s", _label.c_str());
            if (ImGui::Button("Accept"))
            {
                ImGui::CloseCurrentPopup();
            }
            ImGui::EndPopup();
        }

        ImGui::PopStyleColor();
    }
} // namespace cathedral::editor2