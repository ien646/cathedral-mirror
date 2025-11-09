#include <cathedral/editor/dialogs/settings_dialog.hpp>

#include <cathedral/editor/settings.hpp>
#include <cathedral/project/project.hpp>

#include <imgui.h>

namespace cathedral::editor
{
    std::optional<std::string> editor_setting_filter(editor_setting s)
    {
        switch (s)
        {
        case editor_setting::TEXT_SCALE:
            return "Text scale";
        case editor_setting::FONT_MANAGER_SETUP_COMPLETE:
        case editor_setting::MATERIAL_MANAGER_SETUP_COMPLETE:
        case editor_setting::MESH_MANAGER_SETUP_COMPLETE:
        case editor_setting::EDITOR_WINDOW_SETUP_COMPLETE:
        case editor_setting::SCRIPT_MANAGER_SETUP_COMPLETE:
            return std::nullopt;
        default:
            CRITICAL_ERROR(std::format("Unhandled editor setting: {}", static_cast<int>(s)));
        }
    }

    std::optional<std::string> engine_setting_filter(engine::engine_setting s)
    {
        switch (s)
        {
        case engine::engine_setting::UPLOAD_QUEUE_SIZE_MB:
            return "Upload queue size (MB)";
        case engine::engine_setting::VSYNC_ENABLED:
            return "Vsync enabled";
        case engine::engine_setting::VSYNC_MAILBOX:
            return "Vsync mailbox";
        case engine::engine_setting::MSAA_SAMPLES:
            return "MSAA samples";
        case engine::engine_setting::MSAA_SAMPLE_SHADING:
            return "MSAA sample shading";
        default:
            CRITICAL_ERROR(std::format("Unhandled engine setting: {}", static_cast<int>(s)));
        }
    }

    void settings_dialog::open()
    {
        _open_flag.set(true);
    }

    bool draw_value(settings& settings, const std::string& name, const setting_value& value)
    {
        const auto id = "##setting_value>" + name;
        switch (value.type())
        {
        case setting_type::EMPTY:
            ImGui::Text("-");
            break;
        case setting_type::BOOLEAN: {
            bool b = value.as_bool();
            if (ImGui::Checkbox(id.c_str(), &b))
            {
                settings.set(name, b);
                return true;
            }
            break;
        }
        case setting_type::INT64: {
            int i = value.as_int();
            if (ImGui::InputInt(id.c_str(), &i))
            {
                settings.set(name, i);
                return true;
            }
            break;
        }
        case setting_type::DOUBLE: {
            double d = value.as_double();
            if (ImGui::InputDouble(id.c_str(), &d))
            {
                settings.set(name, d);
                return true;
            }
            break;
        }
        case setting_type::STRING: {
            std::string s = value.as_string();
            if (ImGui::InputText(id.c_str(), s.data(), s.size()))
            {
                settings.set(name, s);
                return true;
            }
            break;
        }
        case setting_type::ENUM: {
            setting_enum_value e = value.as_enum();

            if (ImGui::BeginCombo(id.c_str(), e.enum_values[e.current_value].c_str()))
            {
                for (uint32_t i = 0; i < e.enum_values.size(); ++i)
                {
                    const auto& entry = e.enum_values[i];
                    if (ImGui::Selectable(entry.c_str()))
                    {
                        e.current_value = i;
                        settings.set(name, e);
                    }
                }
                ImGui::EndCombo();
            }
        }
        }
        return false;
    }

    void settings_dialog::tick(project::project& pro, const editor_settings_interface& editor_settings)
    {
        constexpr auto ID = "Settings";
        if (_open_flag.get_and_reset())
        {
            ImGui::OpenPopup(ID);
        }

        const auto settings = pro.get_settings();
        const auto engine_settings = pro.get_engine_settings();

        if (ImGui::BeginPopupModal(ID))
        {
            if (ImGui::IsKeyPressed(ImGuiKey_Escape))
            {
                ImGui::CloseCurrentPopup();
            }

            const auto button_size =
                ImGui::CalcTextSize("|").y + ImGui::GetStyle().FramePadding.y * 2 + ImGui::GetStyle().ItemSpacing.y;

            if (ImGui::BeginChild(
                    "##Settings tables",
                    ImVec2(ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y - button_size)))
            {
                ImGui::SeparatorText("Editor settings");
                if (ImGui::BeginTable("##editor_table", 2, ImGuiTableFlags_Borders | ImGuiTableFlags_Resizable))
                {
                    for (const auto& v : magic_enum::enum_values<editor_setting>())
                    {
                        if (auto filtered_value = editor_setting_filter(v))
                        {
                            ImGui::TableNextRow();
                            ImGui::TableNextColumn();
                            ImGui::Text("%s", std::string{ *filtered_value }.c_str());
                            ImGui::TableNextColumn();
                            if (draw_value(*settings, editor_settings.get_setting_key(v), editor_settings.get(v)))
                            {
                                pro.save_settings();
                            }
                        }
                    }
                    ImGui::EndTable();
                }

                ImGui::SeparatorText("Engine settings");
                if (ImGui::BeginTable("##engine_table", 2, ImGuiTableFlags_Borders | ImGuiTableFlags_Resizable))
                {
                    for (const auto& v : magic_enum::enum_values<engine::engine_setting>())
                    {
                        if (auto filtered_value = engine_setting_filter(v))
                        {
                            ImGui::TableNextRow();
                            ImGui::TableNextColumn();
                            ImGui::Text("%s", std::string{ *filtered_value }.c_str());
                            ImGui::TableNextColumn();
                            if (draw_value(*settings, engine_settings->get_setting_key(v), engine_settings->get(v)))
                            {
                                pro.save_settings();
                            }
                        }
                    }
                    ImGui::EndTable();
                }

                ImGui::EndChild();
            }

            if (ImGui::Button("Close", ImGui::GetContentRegionAvail()))
            {
                ImGui::CloseCurrentPopup();
            }

            ImGui::EndPopup();
        }
    }
} // namespace cathedral::editor
