#include "cathedral/project/project.hpp"

#include <cathedral/editor/editor_window/node_properties.hpp>

#include <cathedral/editor/colors.hpp>
#include <cathedral/engine/nodes/camera2d_node.hpp>
#include <cathedral/engine/nodes/camera3d_node.hpp>
#include <cathedral/engine/nodes/directional_light_node.hpp>
#include <cathedral/engine/nodes/mesh3d_node.hpp>
#include <cathedral/engine/nodes/node.hpp>

#include <imgui.h>

namespace cathedral::editor
{
    void node_properties::draw_node_transform(engine::node* node)
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

    void node_properties::draw_camera2d_properties(engine::camera2d_node* node)
    {
        ImGui::SeparatorText("Camera 2D");

        ImGui::BeginDisabled();
        {
            auto vp_size = glm::vec2(node->camera().viewport_size());
            ImGui::InputFloat2("Viewport size", &vp_size[0], "%.1F");

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

    void node_properties::draw_camera3d_properties(engine::camera3d_node* node)
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

    void node_properties::draw_dirlight_properties(engine::directional_light_node* node)
    {
        ImGui::SeparatorText("Directional Light");

        auto intensity = node->intensity();
        if (ImGui::InputFloat("Intensity", &intensity))
        {
            node->set_intensity(intensity);
        }

        auto color = node->color();
        if (ImGui::ColorPicker3("Color", &color[0]))
        {
            node->set_color(color);
        }

        ImGui::BeginDisabled();
        {
            auto direction = node->direction();
            ImGui::InputFloat3("Direction", &direction[0]);
        }
        ImGui::EndDisabled();
    }

    void node_properties::draw_mesh3d_properties(engine::scene& scene, engine::mesh3d_node* node)
    {
        ImGui::SeparatorText("Mesh-3D");

        const auto handle_mesh_select = [&] {
            if (_mesh_selector_dialog == nullptr)
            {
                _mesh_selector_dialog = std::make_unique<mesh_selector>(_project, scene);
            }
            _mesh_selector_dialog->set_mesh_list(_project.mesh_assets() | std::views::keys | std::ranges::to<std::vector>());
            _mesh_selector_dialog->callbacks.selected = [node](const std::string& selected) { node->set_mesh(selected); };
            _mesh_selector_dialog->open();
        };

        const auto handle_material_select = [&] {
            if (_material_selector_dialog == nullptr)
            {
                _material_selector_dialog = std::make_unique<list_select_dialog>();
            }
            _material_selector_dialog->set_items(
                _project.material_assets() | std::views::keys | std::ranges::to<std::vector>());
            _material_selector_dialog->callbacks.selected = [node](const std::string& selected) {
                node->set_material(selected);
            };
            _material_selector_dialog->open();
        };

        if (ImGui::BeginChild("Mesh properties", ImVec2(0, 0), ImGuiChildFlags_Borders))
        {
            ImGui::SeparatorText("Mesh");
            if (!node->mesh_name().has_value())
            {
                ImGui::TextColored(colors::BG_WARNING_YELLOW, "Mesh: No mesh selected");
                ImGui::SameLine();
                if (ImGui::Button("Select##mesh"))
                {
                    handle_mesh_select();
                }
            }
            else
            {
                ImGui::Text("Mesh: %s", node->mesh_name().value().c_str());
                ImGui::SameLine();
                if (ImGui::Button("Select##mesh"))
                {
                    handle_mesh_select();
                }
            }
            ImGui::Separator();
            ImGui::SeparatorText("Material");
            if (!node->material_name().has_value())
            {
                ImGui::TextColored(colors::BG_WARNING_YELLOW, "Material: No material selected");
                ImGui::SameLine();
                if (ImGui::Button("Select##material"))
                {
                    handle_material_select();
                }
            }
            else
            {
                ImGui::Text("Material: %s", node->material_name().value().c_str());
                ImGui::SameLine();
                if (ImGui::Button("Select##material"))
                {
                    handle_material_select();
                }
            }
            ImGui::Separator();
            ImGui::SeparatorText("Node textures");
            const auto material_wptr = node->get_material();
            if (!material_wptr.expired() && (material_wptr.lock()->node_texture_slots() > 0))
            {
                const auto material = material_wptr.lock();
                for (uint32_t slot_index = 0; slot_index < material->node_texture_slots(); ++slot_index)
                {
                    ImGui::PushID(static_cast<int>(slot_index));
                    ImGui::Text("Slot: %u", slot_index);
                    ImGui::SameLine();
                    if ((material->node_texture_names().size() < slot_index)
                        || (material->node_texture_names()[slot_index] == engine::DEFAULT_TEXTURE_NAME))
                    {
                        ImGui::TextColored(colors::BG_WARNING_YELLOW, engine::DEFAULT_TEXTURE_NAME);
                    }
                    else
                    {
                        ImGui::Text("%s", material->node_texture_names()[slot_index].c_str());
                    }
                    ImGui::SameLine();
                    if (ImGui::Button("Select##node_texture"))
                    {
                        if (_texture_selector_dialog == nullptr)
                        {
                            _texture_selector_dialog = std::make_unique<texture_selector>(scene);
                        }
                        _texture_selector_dialog->set_texture_list(
                            _project.texture_assets() | std::views::keys | std::ranges::to<std::vector>());
                        _texture_selector_dialog->callbacks.selected = [node, slot_index](const std::string& selected) {
                            node->bind_node_texture_slot(selected, slot_index);
                        };
                        _texture_selector_dialog->open();
                    }
                    ImGui::PopID();
                }
            }
        }
        ImGui::EndChild();
    }

    node_properties::node_properties(project::project& project)
        : _project(project)
    {
    }

    void node_properties::tick(engine::scene& scene, const unordered_set<engine::scene_node*>& nodes)
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
                if (const auto nodeptr = dynamic_cast<engine::camera2d_node*>(node))
                {
                    draw_camera2d_properties(nodeptr);
                }
                if (const auto nodeptr = dynamic_cast<engine::camera3d_node*>(node))
                {
                    draw_camera3d_properties(nodeptr);
                }
                if (const auto nodeptr = dynamic_cast<engine::directional_light_node*>(node))
                {
                    draw_dirlight_properties(nodeptr);
                }
                if (const auto nodeptr = dynamic_cast<engine::mesh3d_node*>(node))
                {
                    draw_mesh3d_properties(scene, nodeptr);
                }
            }
        }
        ImGui::End();

        if (_mesh_selector_dialog != nullptr)
        {
            _mesh_selector_dialog->tick();
        }
        if (_material_selector_dialog != nullptr)
        {
            _material_selector_dialog->tick();
        }
        if (_texture_selector_dialog != nullptr)
        {
            _texture_selector_dialog->tick();
        }
    }
} // namespace cathedral::editor