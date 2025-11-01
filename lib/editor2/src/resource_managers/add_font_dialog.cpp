#include <cathedral/editor2/resource_managers/add_font_dialog.hpp>

#include <cathedral/editor2/native/file_dialog.hpp>
#include <cathedral/editor2/utils.hpp>

#include <imgui.h>
#include <misc/cpp/imgui_stdlib.h>

#include <filesystem>

namespace cathedral::editor2
{
    constexpr auto ADD_FONT_DIALOG_ID = "Add Font";

    namespace
    {
        constexpr std::array<const char*, 7> ATLAS_SIZE_STRINGS = { "64", "128", "256", "512", "1024", "2048", "4096" };
    }

    void add_font_dialog::tick()
    {
        if (_open_flag.get_and_reset())
        {
            ImGui::OpenPopup(ADD_FONT_DIALOG_ID);
        }

        if (ImGui::BeginPopupModal(ADD_FONT_DIALOG_ID))
        {
            ImGui::InputText("Name", &_name);
            ImGui::InputText("FontFile", &_font_file);
            if (ImGui::Button("Browse", ImVec2(ImGui::GetContentRegionAvail().x, 0)))
            {
                const auto result = native_open_file();
                if (result.has_value())
                {
                    _font_file = *result;
                }
            }

            if (ImGui::BeginCombo("Atlas width", std::to_string(_atlas_size[0]).c_str()))
            {
                for (const auto& size : ATLAS_SIZE_STRINGS)
                {
                    if (ImGui::Selectable(size))
                    {
                        _atlas_size[0] = std::atoi(size);
                    }
                }
                ImGui::EndCombo();
            }

            if (ImGui::BeginCombo("Atlas height", std::to_string(_atlas_size[1]).c_str()))
            {
                for (const auto& size : ATLAS_SIZE_STRINGS)
                {
                    if (ImGui::Selectable(size))
                    {
                        _atlas_size[1] = std::atoi(size);
                    }
                }
                ImGui::EndCombo();
            }

            if (ImGui::InputInt("Glyph height", &_glyph_height, 1, 1))
            {
                _glyph_height = std::max(1, _glyph_height);
            }

            if (ImGui::InputInt("Char offset", &_char_offset))
            {
                _char_offset = std::max(0, _char_offset);
            }

            const auto button_size = (ImGui::GetContentRegionAvail().x - ImGui::GetStyle().ItemSpacing.x) / 2;

            ImGui::BeginDisabled(!validate_fields());
            if (ImGui::Button("Accept", ImVec2(button_size, 0)))
            {
                try_call(callbacks.create, _name, _font_file, _atlas_size, _glyph_height, _char_offset);
                ImGui::CloseCurrentPopup();
            }
            ImGui::EndDisabled();

            if (ImGui::Button("Cancel", ImVec2(button_size, 0)))
            {
                ImGui::CloseCurrentPopup();
            }

            ImGui::EndPopup();
        }
    }

    void add_font_dialog::set_forbidden_names(std::vector<std::string> names)
    {
        _forbidden_names = std::move(names);
    }

    void add_font_dialog::open()
    {
        _open_flag.set(true);
        _name.clear();
        _font_file.clear();
    }

    bool add_font_dialog::validate_fields() const
    {
        return !_name.empty() && std::filesystem::exists(_font_file) && !std::ranges::contains(_forbidden_names, _font_file);
    }
} // namespace cathedral::editor2