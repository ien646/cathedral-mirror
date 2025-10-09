#include <cathedral/editor2/editor_window/logs_panel.hpp>

#include <cathedral/editor2/colors.hpp>

#include <imgui.h>

namespace cathedral::editor2
{
    void logs_panel::tick()
    {
        collect_logs();
        ImGui::Begin("Logs");
        {
            ImGui::Checkbox("Auto scroll to end", &_autoscroll);
            ImGui::SameLine(0, 50);
            ImGui::TextDisabled("%s", std::format("Stored log lines: {}", _lines.size()).c_str());

            const auto button_size = ImGui::CalcTextSize("Clear").x + (ImGui::GetStyle().FramePadding.x);
            ImGui::SameLine(ImGui::GetContentRegionAvail().x - button_size);
            if (ImGui::Button("Clear"))
            {
                _lines.clear();
            }

            if (ImGui::BeginListBox("##log_lines", ImGui::GetContentRegionAvail()))
            {
                size_t index = 0;
                for (const auto& [level, message] : _lines)
                {
                    const ImVec4 color = [level] -> ImVec4 {
                        switch (level)
                        {
                        case log_level::INFO:
                            return colors::TEXT_INFO_BLUE;
                        case log_level::WARNING:
                            return colors::TEXT_WARNING_YELLOW;
                        case log_level::ERROR:
                            return colors::TEXT_ERROR_RED;
                        default:
                            CRITICAL_ERROR("Unhandled log level");
                        }
                    }();

                    ImGui::PushStyleColor(ImGuiCol_Text, color);
                    ImGui::TextWrapped("%s##%zi", message.c_str(), index++);
                    ImGui::PopStyleColor();
                }
                if (_autoscroll)
                {
                    ImGui::SetScrollHereY();
                }
                ImGui::EndListBox();
            }
        }

        ImGui::End();
    }

    void logs_panel::collect_logs()
    {
        for (const auto& line : get_global_log_database().take_log_lines())
        {
            _lines.push_back(std::move(line));
        }
    }
} // namespace cathedral::editor2