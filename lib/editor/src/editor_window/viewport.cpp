#include <cathedral/editor/editor_window/viewport.hpp>

#include <imgui.h>
#include <imgui_internal.h>

namespace cathedral::editor
{
    void editor_viewport::tick(const ImGuiID dockspace_id)
    {
        ImGuiDockNode* node = ImGui::DockBuilderGetNode(dockspace_id);
        const auto* centralnode = node->CentralNode;

        if (centralnode)
        {
            ImGui::SetNextWindowPos(centralnode->Pos);
            ImGui::SetNextWindowSize(centralnode->Size);

            _position = glm::vec2{ centralnode->Pos.x, centralnode->Pos.y } / ImGui::GetWindowDpiScale();
            _size = glm::vec2{ centralnode->Size.x, centralnode->Size.y } / ImGui::GetWindowDpiScale();
        }

        ImGui::Begin(
            "Viewport",
            nullptr,
            ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoCollapse |
                ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoNavFocus |
                ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoMouseInputs);
        {
        }
        ImGui::End();
    }

    glm::ivec2 editor_viewport::position()
    {
        return _position;
    }

    glm::ivec2 editor_viewport::size()
    {
        return _size;
    }
} // namespace cathedral::editor