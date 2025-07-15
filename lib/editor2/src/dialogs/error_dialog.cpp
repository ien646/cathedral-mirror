#include <cathedral/editor2/dialogs/error_dialog.hpp>

#include <imgui.h>

namespace cathedral::editor2
{
    void error_dialog::tick()
    {
        if (_open)
        {
            ImGui::OpenPopup(_title.c_str());
        }

        ImGui::PushStyleColor(ImGuiCol_PopupBg, ImVec4{ 0.6F, 0.05F, 0.05F, 1.0F });
        if (ImGui::BeginPopupModal(
                _title.c_str(),
                nullptr,
                ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_AlwaysAutoResize))
        {
            ImGui::Text(_message.c_str());
            if (ImGui::Button("OK"))
            {
                _open = false;
                ImGui::CloseCurrentPopup();
            }

            ImGui::EndPopup();
        }
        ImGui::PopStyleColor();
    }

    void error_dialog::open(std::string title, std::string message)
    {
        _title = std::move(title);
        _message = std::move(message);

        _open = true;
    }
} // namespace cathedral::editor2