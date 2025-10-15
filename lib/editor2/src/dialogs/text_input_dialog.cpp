#include <cathedral/editor2/dialogs/text_input_dialog.hpp>

#include <cathedral/editor2/utils.hpp>

#include <imgui.h>

namespace cathedral::editor2
{
    text_input_dialog::text_input_dialog(std::string title, std::string label, const size_t initial_length)
        : _title(std::move(title))
        , _label(std::move(label))
        , _buffer(initial_length, '\0')
    {
    }

    void text_input_dialog::set_buffer_size(const size_t length)
    {
        _buffer.resize(length, '\0');
    }

    void text_input_dialog::set_text(const std::string& text)
    {
        if (_buffer.size() < text.size())
        {
            _buffer.resize(text.size(), '\0');
        }
        std::ranges::copy(text, _buffer.begin());
    }

    std::string text_input_dialog::text() const
    {
        return _buffer.c_str();
    }

    void text_input_dialog::set_validator(std::function<bool(const std::string&)> validator)
    {
        _validator = std::move(validator);
    }

    void text_input_dialog::open()
    {
        _open_flag = true;
    }

    void text_input_dialog::tick()
    {
        const auto popup_id = _title + "##text_input_dialog";

        if (_open_flag)
        {
            ImGui::OpenPopup(popup_id.c_str());
            _open_flag = false;
        }

        if (ImGui::BeginPopupModal((popup_id).c_str(), nullptr, ImGuiWindowFlags_AlwaysAutoResize))
        {
            ImGui::InputText(_label.c_str(), _buffer.data(), _buffer.size());

            const bool is_text_valid = !_validator || (_validator && _validator(_buffer.c_str()));

            if (!is_text_valid)
            {
                ImGui::BeginDisabled();
            }

            if (ImGui::Button("Accept"))
            {
                ImGui::CloseCurrentPopup();
                try_call(callbacks.accepted);
            }

            if (!is_text_valid)
            {
                ImGui::EndDisabled();
            }

            ImGui::SameLine();
            if (ImGui::Button("Cancel"))
            {
                ImGui::CloseCurrentPopup();
                try_call(callbacks.cancelled);
            }

            ImGui::EndPopup();
        }
    }
} // namespace cathedral::editor2