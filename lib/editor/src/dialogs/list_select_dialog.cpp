#include <cathedral/editor/dialogs/list_select_dialog.hpp>

#include <cathedral/editor/callback_impl.hpp>

#include <imgui.h>

namespace cathedral::editor
{
    void list_select_dialog::set_items(std::vector<std::string> items)
    {
        _items = MOVE(items);
    }

    void list_select_dialog::open()
    {
        _open_flag.set(true);
    }

    void list_select_dialog::tick()
    {
        constexpr auto ID = "List selector";

        if (_open_flag.get_and_reset())
        {
            ImGui::OpenPopup(ID);
        }

        ImGui::SetNextWindowSizeConstraints(ImVec2(300, 200), ImVec2(99999, 99999));
        if (ImGui::BeginPopupModal(ID))
        {
            const auto button_height = ImGui::CalcTextSize("|").y
                                       + (ImGui::GetStyle().FramePadding.y * 2)
                                       + ImGui::GetStyle().ItemSpacing.y;

            if (ImGui::BeginListBox(
                    "##list",
                    ImVec2(ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y - button_height)))
            {
                for (const auto& item : _items)
                {
                    if (ImGui::Selectable(item.c_str()))
                    {
                        _selected = item;
                    }
                }
                ImGui::EndListBox();
            }

            const auto button_width = (ImGui::GetContentRegionAvail().x - (ImGui::GetStyle().ItemSpacing.x)) / 2;

            const ImVec2 button_size(button_width, ImGui::GetContentRegionAvail().y);

            if (ImGui::Button("Cancel", button_size))
            {
                ImGui::CloseCurrentPopup();
            }
            ImGui::SameLine();

            ImGui::BeginDisabled(_selected.empty());
            if (ImGui::Button("Select", button_size))
            {
                CALLBACK(selected(_selected));
                ImGui::CloseCurrentPopup();
            }
            ImGui::EndDisabled();

            ImGui::EndPopup();
        }
    }
} // namespace cathedral::editor