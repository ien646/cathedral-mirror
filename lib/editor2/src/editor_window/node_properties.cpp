#include <cathedral/editor2/editor_window/node_properties.hpp>

#include <cathedral/engine/nodes/camera2d_node.hpp>
#include <cathedral/engine/nodes/camera3d_node.hpp>
#include <cathedral/engine/nodes/node.hpp>

#include <imgui.h>

namespace cathedral::editor2
{
    void draw_node_transform(engine::node* node)
    {
        ImGui::SeparatorText("Transform");

        auto position = node->local_position();
        auto rotation = node->local_rotation();
        auto scale = node->local_scale();

        if (ImGui::DragFloat3("Position", &position[0], 0.01F))
        {
            node->set_local_position(glm::vec3(position[0], position[1], position[2]));
        }
        if (ImGui::DragFloat3("Rotation", &rotation[0], 0.01F))
        {
            node->set_local_rotation(glm::vec3(rotation[0], rotation[1], rotation[2]));
        }
        if (ImGui::DragFloat3("Scale", &scale[0], 0.01F))
        {
            node->set_local_scale(glm::vec3(scale[0], scale[1], scale[2]));
        }
    }

    void draw_camera3d_properties(engine::camera3d_node* node)
    {
        ImGui::SeparatorText("Camera3D");

        float vfov = node->camera().vertical_fov();
        if (ImGui::DragFloat("V-FOV", &vfov, 0.1F))
        {
            node->camera().set_vertical_fov(vfov);
        }

        bool main_camera = node->is_main_camera();
        if (ImGui::Checkbox("Main Camera", &main_camera))
        {
            node->set_main_camera(main_camera);
        }

        ImGui::BeginDisabled();
        {
            const float ar = node->camera().aspect_ratio();
            ImGui::Text("Aspect ratio: %.2f", ar);

            auto view_matrix = glm::transpose(node->camera().get_view_matrix());
            auto projection_matrix = glm::transpose(node->camera().get_projection_matrix());
            ImGui::Text("View Matrix");
            ImGui::InputFloat4("##vmatrix0", &view_matrix[0][0], "%.2f", ImGuiInputTextFlags_ReadOnly);
            ImGui::InputFloat4("##vmatrix1", &view_matrix[1][0], "%.2f", ImGuiInputTextFlags_ReadOnly);
            ImGui::InputFloat4("##vmatrix2", &view_matrix[2][0], "%.2f", ImGuiInputTextFlags_ReadOnly);
            ImGui::InputFloat4("##vmatrix3", &view_matrix[3][0], "%.2f", ImGuiInputTextFlags_ReadOnly);
            ImGui::Text("Projection Matrix");
            ImGui::InputFloat4(
                "##pmatrix0",
                &projection_matrix[0][0],
                "%.2f",
                ImGuiInputTextFlags_ReadOnly | ImGuiInputTextFlags_ElideLeft);
            ImGui::InputFloat4("##pmatrix1", &projection_matrix[1][0], "%.2f", ImGuiInputTextFlags_ReadOnly);
            ImGui::InputFloat4("##pmatrix2", &projection_matrix[2][0], "%.2f", ImGuiInputTextFlags_ReadOnly);
            ImGui::InputFloat4("##pmatrix3", &projection_matrix[3][0], "%.2f", ImGuiInputTextFlags_ReadOnly);
        }
        ImGui::EndDisabled();
    }

    void node_properties::tick(const std::unordered_set<engine::scene_node*>& nodes)
    {
        ImGui::Begin(WINDOW_ID);
        {
            if (nodes.empty())
            {
                ImGui::Text("No selection...");
            }
            else if (nodes.size() > 1)
            {
                ImGui::Text("Multiple nodes selected...");
            }
            else
            {
                auto* node = *nodes.begin();
                if (const auto nodeptr = dynamic_cast<engine::node*>(node))
                {
                    draw_node_transform(nodeptr);
                }
                if (const auto nodeptr = dynamic_cast<engine::camera3d_node*>(node))
                {
                    draw_camera3d_properties(nodeptr);
                }
            }
        }
        ImGui::End();
    }
} // namespace cathedral::editor2