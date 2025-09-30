#include "cathedral/editor2/native/file_dialog.hpp"

#include <cathedral/editor2/project_selection_window.hpp>

namespace cathedral::editor2
{
    project_selection_window::project_selection_window()
        : _window("Projects", 400, 200, std::make_shared<settings>())
    {
    }

    std::optional<std::string> project_selection_window::execute()
    {
        std::string result;
        while (result.empty() && _window.keep_open())
        {
            _window.tick([&] {
                const ImGuiViewport* vp = ImGui::GetMainViewport();
                ImGui::SetNextWindowPos(vp->WorkPos);
                ImGui::SetNextWindowSize(vp->WorkSize);
                ImGui::Begin("#project_selection_window", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoResize);
                {
                    if (ImGui::BeginTable(
                            "#buttons_table",
                            2,
                            ImGuiTableFlags_SizingStretchSame,
                            ImGui::GetContentRegionAvail()))
                    {
                        ImGui::TableNextColumn();
                        if (ImGui::Button("New Project", ImGui::GetContentRegionAvail()))
                        {
                            if (const auto dir = native_open_dir())
                            {
                                result = *dir;
                            }
                        }
                        ImGui::TableNextColumn();
                        if (ImGui::Button("Open Project", ImGui::GetContentRegionAvail()))
                        {
                            if (const auto dir = native_open_dir())
                            {
                                result = *dir;
                            }
                        }
                        ImGui::EndTable();
                    }
                }
                ImGui::End();
            });
        }
        return _window.keep_open() ? std::optional{ result } : std::nullopt;
    }
} // namespace cathedral::editor2