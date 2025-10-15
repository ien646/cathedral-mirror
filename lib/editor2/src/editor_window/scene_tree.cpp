#include <cathedral/editor2/editor_window/scene_tree.hpp>

#include <cathedral/engine/nodes/camera2d_node.hpp>
#include <cathedral/engine/nodes/directional_light_node.hpp>
#include <cathedral/engine/nodes/mesh3d_node.hpp>
#include <cathedral/engine/nodes/point_light_node.hpp>
#include <cathedral/engine/nodes/text_node.hpp>

#include <cathedral/editor2/utils.hpp>
#include <cathedral/engine/scene.hpp>

#include <imgui.h>

#include <ranges>

namespace cathedral::editor2
{
    void scene_tree::tick(engine::scene& scene)
    {
        context_menu(scene);

        ImGui::Begin("Scene Tree");
        {
            // Keep local copy of node pointers to avoid iterator invalidation
            const auto nodes =
                scene.root_nodes() |
                std::views::transform([](const std::unique_ptr<engine::scene_node>& node) { return node.get(); }) |
                std::ranges::to<std::vector<engine::scene_node*>>();

            ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
            for (const auto& node : nodes)
            {
                draw_node(scene, *node);
            }
            ImGui::PopStyleVar();

            if (!_open_context_menu_flag && ImGui::IsWindowHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Right))
            {
                _selected_nodes.clear();
                _open_context_menu_flag = true;
            }
        }
        ImGui::End();

        if (ImGui::IsKeyPressed(ImGuiKey_F2) && _selected_nodes.size() == 1)
        {
            _rename_mode = true;
            _rename_buffer = (*_selected_nodes.begin())->name();
            _rename_buffer.resize(256, 0);
        }
    }

    void scene_tree::draw_node(engine::scene& scene, engine::scene_node& node)
    {
        if (_rename_mode)
        {
            if (_selected_nodes.empty())
            {
                log_error("Attempt to rename node without node selection");
                _rename_mode = false;
            }
            if (_selected_nodes.size() > 1)
            {
                log_error("Attempt to rename with multiple nodes selected");
                _rename_mode = false;
            }
        }

        if (_reparent_mode)
        {
            if (_selected_nodes.empty())
            {
                log_error("Attempt to reparent node without node selection");
                _reparent_mode = false;
            }
        }

        const bool rename = _rename_mode && _selected_nodes.size() == 1 && _selected_nodes.contains(&node);

        const std::string id = rename ? "##" + node.name() : node.name();

        ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_DrawLinesFull | ImGuiTreeNodeFlags_OpenOnDoubleClick |
                                   ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth;
        if (node.children().empty())
        {
            flags |= ImGuiTreeNodeFlags_Bullet | ImGuiTreeNodeFlags_Leaf;
        }
        if (_selected_nodes.contains(&node) && !rename)
        {
            flags |= ImGuiTreeNodeFlags_Selected;
        }

        if (_reparent_mode && _selected_nodes.contains(&node))
        {
            ImGui::BeginDisabled();
        }
        if (&node == _force_expand_node)
        {
            _force_expand_node = nullptr;
            ImGui::SetNextItemOpen(true);
        }
        const bool node_open = ImGui::TreeNodeEx(id.c_str(), flags);
        if (_reparent_mode && _selected_nodes.contains(&node))
        {
            ImGui::EndDisabled();
        }

        const bool node_clicked_left = ImGui::IsItemClicked(ImGuiMouseButton_Left);
        const bool node_clicked_right = ImGui::IsItemClicked(ImGuiMouseButton_Right);
        const bool ctrl_clicked = ImGui::IsKeyDown(ImGuiKey_LeftCtrl) || ImGui::IsKeyDown(ImGuiKey_RightCtrl);

        if (rename)
        {
            ImGui::SameLine();
            ImGui::SetKeyboardFocusHere();

            ImGui::InputText("##scene_tree_rename_input", _rename_buffer.data(), _rename_buffer.size());
            if (ImGui::IsItemDeactivatedAfterEdit())
            {
                _rename_buffer = _rename_buffer.c_str();
                auto target_name = _rename_buffer;
                size_t attempts = 0;
                while (scene.get_node_sibling(&node, target_name))
                {
                    ++attempts;
                    target_name = _rename_buffer.c_str() + std::format("({})", attempts);
                }
                node.set_name(target_name);
                _rename_mode = false;

                _selected_nodes.clear();
                _selected_nodes.emplace(&node);
            }
            else if (ImGui::IsItemDeactivated())
            {
                _rename_mode = false;
            }
        }

        if (_reparent_mode && node_clicked_left && !_selected_nodes.contains(&node))
        {
            for (auto& selected_node : _selected_nodes)
            {
                scene.reparent_node(selected_node, &node);
            }
            _reparent_mode = false;
        }

        if (node_open)
        {
            for (const auto& child : node.children())
            {
                draw_node(scene, *child);
            }
            ImGui::TreePop();
        }

        if (node_clicked_left)
        {
            if (_selected_nodes.contains(&node))
            {
                if (!ctrl_clicked)
                {
                    _selected_nodes.clear();
                }
                _selected_nodes.erase(&node);
            }
            else
            {
                if (!ctrl_clicked)
                {
                    _selected_nodes.clear();
                }
                _selected_nodes.insert(&node);
            }
        }

        if (node_clicked_right)
        {
            if (!_selected_nodes.contains(&node))
            {
                if (!ctrl_clicked)
                {
                    _selected_nodes.clear();
                }
                _selected_nodes.insert(&node);
            }
            _open_context_menu_flag = true;
        }
    }

    void scene_tree::context_menu(engine::scene& scene)
    {
        if (_open_context_menu_flag)
        {
            _open_context_menu_flag = false;
            ImGui::OpenPopup("##scene_tree_context_menu");
            ImGui::SetNextWindowPos(ImGui::GetMousePos());
        }

        if (ImGui::BeginPopup("##scene_tree_context_menu"))
        {
            if (_selected_nodes.empty())
            {
                new_root_node_menu(scene);
            }
            else if (_selected_nodes.size() == 1)
            {
                new_child_node_menu();

                if (ImGui::Selectable("Rename"))
                {
                    _rename_mode = true;
                    _rename_buffer = (*_selected_nodes.begin())->name();
                    _rename_buffer.resize(256, 0);
                }
                if (ImGui::Selectable("Reparent"))
                {
                    _reparent_mode = true;
                }
                if (ImGui::Selectable("Duplicate"))
                {
                    duplicate_selected_nodes(scene);
                }
                if (ImGui::Selectable("Make root"))
                {
                    scene.reparent_node(*_selected_nodes.begin(), nullptr);
                }
                if (ImGui::Selectable("Delete"))
                {
                    delete_selected_nodes(scene);
                }
            }
            else if (_selected_nodes.size() > 1)
            {
                if (ImGui::Selectable("Reparent"))
                {
                    _reparent_mode = true;
                }
                if (ImGui::Selectable("Duplicate"))
                {
                    duplicate_selected_nodes(scene);
                }
                if (ImGui::Selectable("Make root"))
                {
                    for (const auto* selected_node : _selected_nodes)
                    {
                        scene.reparent_node(selected_node, nullptr);
                    }
                }
                if (ImGui::Selectable("Delete"))
                {
                    delete_selected_nodes(scene);
                }
            }
            ImGui::EndPopup();
        }
    }

    void scene_tree::delete_selected_nodes(engine::scene& scene)
    {
        for (auto* node : _selected_nodes)
        {
            if (node->has_parent())
            {
                node->parent()->remove_child(node->name());
            }
            else
            {
                scene.remove_node(node->name());
            }
        }
        _selected_nodes.clear();
    }

    void scene_tree::new_root_node_menu(engine::scene& scene)
    {
        const auto get_available_name = [&] {
            std::string target_name = "new node";
            size_t attempts = 0;
            while (scene.get_node(target_name) != nullptr)
            {
                target_name = std::format("new node ({})", ++attempts);
            }
            return target_name;
        };

        engine::scene_node* created_node = nullptr;

        if (ImGui::BeginMenu("New root node"))
        {
            if (ImGui::MenuItem("Camera2D"))
            {
                created_node = scene.add_root_node<engine::camera2d_node>(get_available_name());
            }
            if (ImGui::MenuItem("Camera3D"))
            {
                created_node = scene.add_root_node<engine::camera3d_node>(get_available_name());
            }
            if (ImGui::MenuItem("Directional Light"))
            {
                created_node = scene.add_root_node<engine::directional_light_node>(get_available_name());
            }
            if (ImGui::MenuItem("Node"))
            {
                created_node = scene.add_root_node<engine::node>(get_available_name());
            }
            if (ImGui::MenuItem("Mesh3D"))
            {
                created_node = scene.add_root_node<engine::mesh3d_node>(get_available_name());
            }
            if (ImGui::MenuItem("Point Light"))
            {
                created_node = scene.add_root_node<engine::point_light_node>(get_available_name());
            }
            if (ImGui::MenuItem("Text"))
            {
                created_node = scene.add_root_node<engine::text_node>(get_available_name());
            }
            ImGui::EndMenu();
        }

        if (created_node != nullptr)
        {
            _selected_nodes.clear();
            _selected_nodes.insert(created_node);

            _rename_buffer = created_node->name();
            _rename_buffer.resize(256, 0);
            _rename_mode = true;
        }
    }

    void scene_tree::new_child_node_menu()
    {
        auto* selected_node = *_selected_nodes.begin();

        const auto get_available_name = [&] {
            std::string target_name = "new node";
            size_t attempts = 0;
            while (selected_node->contains_child(target_name))
            {
                target_name = std::format("new node ({})", ++attempts);
            }
            return target_name;
        };

        engine::scene_node* created_node = nullptr;

        if (ImGui::BeginMenu("New child node"))
        {
            if (ImGui::MenuItem("Camera2D"))
            {
                created_node = selected_node->add_child_node<engine::camera2d_node>(get_available_name());
            }
            if (ImGui::MenuItem("Camera3D"))
            {
                created_node = selected_node->add_child_node<engine::camera3d_node>(get_available_name());
            }
            if (ImGui::MenuItem("Directional Light"))
            {
                created_node = selected_node->add_child_node<engine::directional_light_node>(get_available_name());
            }
            if (ImGui::MenuItem("Node"))
            {
                created_node = selected_node->add_child_node<engine::node>(get_available_name());
            }
            if (ImGui::MenuItem("Mesh3D"))
            {
                created_node = selected_node->add_child_node<engine::mesh3d_node>(get_available_name());
            }
            if (ImGui::MenuItem("Point Light"))
            {
                created_node = selected_node->add_child_node<engine::point_light_node>(get_available_name());
            }
            if (ImGui::MenuItem("Text"))
            {
                created_node = selected_node->add_child_node<engine::text_node>(get_available_name());
            }
            ImGui::EndMenu();
        }

        if (created_node != nullptr)
        {
            _selected_nodes.clear();
            _selected_nodes.insert(created_node);

            _rename_buffer = created_node->name();
            _rename_buffer.resize(256, 0);
            _rename_mode = true;

            _force_expand_node = created_node->parent();
        }
    }

    void scene_tree::duplicate_selected_nodes(engine::scene& scene)
    {
        const auto get_available_root_name = [&](const engine::scene_node* node) {
            std::string target_name = node->name();
            size_t attempts = 0;
            while (scene.get_node(target_name) != nullptr)
            {
                target_name = std::format("{} ({})", node->name(), ++attempts);
            }
            return target_name;
        };

        const auto get_available_child_name = [&](const engine::scene_node* node, const engine::scene_node* parent) {
            std::string target_name = node->name();
            size_t attempts = 0;
            while (parent->contains_child(target_name))
            {
                target_name = std::format("{} ({})", node->name(), ++attempts);
            }
            return target_name;
        };

        for (const auto* node : _selected_nodes)
        {
            const auto name = node->has_parent() ? get_available_child_name(node, node->parent())
                                                 : get_available_root_name(node);
            auto copy = node->copy(name, true);
            if (node->has_parent())
            {
                node->parent()->add_child_node(std::move(copy));
            }
            else
            {
                scene.add_root_node(std::move(copy));
            }
        }
    }
} // namespace cathedral::editor2