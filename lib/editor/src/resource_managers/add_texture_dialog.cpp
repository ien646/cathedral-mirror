#include "cathedral/bits/enum.hpp"
#include "ien/image/resize_filter.hpp"
#include "magic_enum.hpp"

#include <cathedral/editor/resource_managers/add_texture_dialog.hpp>

#include <cathedral/editor/callback_impl.hpp>
#include <cathedral/editor/native/file_dialog.hpp>

#include <imgui.h>
#include <misc/cpp/imgui_stdlib.h>

#include <filesystem>

namespace cathedral::editor
{
    constexpr auto ADD_TEXTURE_DIALOG_ID = "Add Texture";

    void add_texture_dialog::tick()
    {
        if (_open_flag.get_and_reset())
        {
            ImGui::OpenPopup(ADD_TEXTURE_DIALOG_ID);
        }

        if (ImGui::BeginPopupModal(ADD_TEXTURE_DIALOG_ID, nullptr, ImGuiWindowFlags_AlwaysAutoResize))
        {
            ImGui::InputText("Name", &_name);
            ImGui::InputText("File path", &_texture_file);
            if (ImGui::Button("Browse", ImVec2(ImGui::GetContentRegionAvail().x, 0)))
            {
                const auto result = native_open_file();
                if (result.has_value())
                {
                    _texture_file = *result;
                }
            }

            if (ImGui::BeginCombo("Format", CATHEDRAL_ENUM_TO_CSTR(_texture_format)))
            {
                for (const auto& [value, name] : magic_enum::enum_entries<engine::texture_format>())
                {
                    if (ImGui::Selectable(std::string{ name }.c_str()))
                    {
                        _texture_format = value;
                    }
                }
                ImGui::EndCombo();
            }

            if (ImGui::InputInt("Mip count", &_mip_count, 1, 1))
            {
                _mip_count = std::clamp(_mip_count, 1, 16);
            }

            if (ImGui::BeginCombo("Mip generation filter", CATHEDRAL_ENUM_TO_CSTR(_mipgen_filter)))
            {
                for (const auto& [value, name] : magic_enum::enum_entries<ien::resize_filter>())
                {
                    if (ImGui::Selectable(std::string{ name }.c_str()))
                    {
                        _mipgen_filter = value;
                    }
                }
            }

            const auto button_size = (ImGui::GetContentRegionAvail().x - ImGui::GetStyle().ItemSpacing.x) / 2;

            ImGui::BeginDisabled(!validate_fields());
            if (ImGui::Button("Accept", ImVec2(button_size, 0)))
            {
                CALLBACK(create(_name, _texture_file, _texture_format, _mip_count, _mipgen_filter));
                ImGui::CloseCurrentPopup();
            }
            ImGui::EndDisabled();

            ImGui::SameLine();

            if (ImGui::Button("Cancel", ImVec2(button_size, 0)))
            {
                ImGui::CloseCurrentPopup();
            }

            ImGui::EndPopup();
        }
    }

    void add_texture_dialog::set_forbidden_names(std::vector<std::string> names)
    {
        _forbidden_names = MOVE(names);
    }

    void add_texture_dialog::open()
    {
        _open_flag.set(true);
        _name.clear();
        _texture_file.clear();
    }

    bool add_texture_dialog::validate_fields() const
    {
        return !_name.empty() && std::filesystem::exists(_texture_file);
    }
} // namespace cathedral::editor