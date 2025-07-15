#include <cathedral/editor2/dialogs/text_input_dialog.hpp>

#include <imgui.h>
#include <misc/cpp/imgui_stdlib.h>

namespace cathedral::editor2
{
    text_input_dialog::text_input_dialog(
        std::string title,
        std::string label,
        const bool allow_empty,
        std::unordered_set<std::string> forbidden_inputs)
        : _allow_empty(allow_empty)
        , _title(std::move(title))
        , _label(std::move(label))
        , _forbidden_inputs(std::move(forbidden_inputs))
    {
    }

    void text_input_dialog::tick()
    {
        if (_open)
        {
            ImGui::OpenPopup(_title.c_str());
        }

        if (ImGui::BeginPopupModal(
                _title.c_str(),
                &_open,
                ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_AlwaysAutoResize))
        {
            if (_first_open)
            {
                ImGui::SetKeyboardFocusHere();
                _first_open = false;
            }

            ImGui::InputText(_label.c_str(), &_input);

            ImGui::BeginDisabled(_forbidden_inputs.contains(_input) || (_input.empty() && !_allow_empty));
            if (ImGui::Button("OK") || (ImGui::IsKeyPressed(ImGuiKey_Enter) && ImGui::IsWindowFocused()))
            {
                ImGui::CloseCurrentPopup();
                _open = false;
                callback(_callbacks.selected, _input);
                _input = {};
            }
            ImGui::EndDisabled();

            ImGui::EndPopup();
        }
    }

    void text_input_dialog::open()
    {
        _open = true;
        _first_open = true;
    }
} // namespace cathedral::editor2