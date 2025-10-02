#include <cathedral/editor2/editor_window/viewport.hpp>

#include <imgui.h>
#include <imgui_internal.h>

namespace cathedral::editor2
{
    void editor_viewport::tick(const ImGuiID dockspace_id)
    {
        ImGuiDockNode* node = ImGui::DockBuilderGetNode(dockspace_id);
        const auto* centralnode = node->CentralNode;

        if (centralnode)
        {
            ImGui::SetNextWindowPos(centralnode->Pos);
            ImGui::SetNextWindowSize(centralnode->Size);
        }

        ImGui::Begin("Viewport", nullptr, ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoCollapse);
        {
            _position = { ImGui::GetWindowPos().x, ImGui::GetWindowPos().y };
            _size = { ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y };
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
} // namespace cathedral::editor2