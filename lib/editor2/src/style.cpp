#include <cathedral/editor2/style.hpp>

#include <imgui.h>

namespace cathedral::editor2
{
    void setup_imgui_style()
    {
        ImGuiStyle& style = ImGui::GetStyle();

        style.Alpha = 1.0F;

        style.Colors[ImGuiCol_Border] = ImVec4(0.2F, 0.4F, 0.2F, 1.0F);
        style.Colors[ImGuiCol_BorderShadow] = ImVec4(0.1F, 0.3F, 0.1F, 1.0F);
        style.Colors[ImGuiCol_Button] = ImVec4(0.15F, 0.2F, 0.15F, 1.0F);
        style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.10F, 0.4F, 0.10F, 1.0F);
        style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.2F, 0.45F, 0.2F, 1.0F);
        style.Colors[ImGuiCol_CheckMark] = ImVec4(0.3F, 0.5F, 0.3F, 1.0F);
        style.Colors[ImGuiCol_ChildBg] = ImVec4(0.15F, 0.15F, 0.15F, 1.0F);
        style.Colors[ImGuiCol_DockingPreview] = ImVec4(0.2F, 0.4F, 0.2F, 1.0F);
        style.Colors[ImGuiCol_DockingEmptyBg] = ImVec4(0.0F, 0.0F, 0.0F, 0.0F);
        style.Colors[ImGuiCol_DragDropTarget] = ImVec4(0.4F, 0.6F, 0.4F, 1.0F);
        style.Colors[ImGuiCol_FrameBg] = ImVec4(0.1F, 0.125F, 0.1F, 1.0F);
        style.Colors[ImGuiCol_FrameBgActive] = ImVec4(0.2F, 0.2F, 0.2F, 1.0F);
        style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.12F, 0.15F, 0.12F, 1.0F);
        style.Colors[ImGuiCol_Header] = ImVec4(0.09F, 0.1F, 0.09F, 1.0F);
        style.Colors[ImGuiCol_HeaderActive] = ImVec4(0.07F, 0.09F, 0.07F, 1.0F);
        style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.15F, 0.15F, 0.15F, 1.0F);
        style.Colors[ImGuiCol_InputTextCursor] = ImVec4(0.4F, 0.6F, 0.4F, 1.0F);
        style.Colors[ImGuiCol_MenuBarBg] = ImVec4(0.06F, 0.065F, 0.06F, 1.0F);
        style.Colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.15F, 0.15F, 0.15F, 0.75F);
    }
} // namespace cathedral::editor2