#include <cathedral/editor2/resource_managers/mesh_manager.hpp>

#include <cathedral/editor2/engine_window.hpp>
#include <cathedral/engine/nodes/camera3d_node.hpp>
#include <cathedral/engine/nodes/mesh3d_node.hpp>

#include <imgui.h>
#include <imgui_internal.h>

namespace cathedral::editor2
{
    mesh_manager::mesh_manager(project::project& pro)
        : resource_manager_base(pro)
        , _filter(256, '\0')
    {
        _window.set_title("Mesh manager");

        _available_mesh_names.append_range(_project.get_assets<project::mesh_asset>() | std::views::keys);

        init_callbacks();

        _camera_node = _scene->add_root_node<engine::camera3d_node>("camera");
        _camera_node->set_local_position({ 0.0F, 0.0F, -5.0F });
        _camera_node->set_main_camera(true);
        _mesh_node = _scene->add_root_node<engine::mesh3d_node>("mesh");
        _mesh_node->set_enabled(false);
    }

    void mesh_manager::tick()
    {
        if (_window.keep_open())
        {
            _scene->tick([this]([[maybe_unused]] const double deltatime) { _window.tick([this] { tick_gui(); }); });
        }
    }

    void mesh_manager::tick_gui()
    {
        auto dockspace_id = ImGui::DockSpaceOverViewport(
            0,
            ImGui::GetMainViewport(),
            ImGuiDockNodeFlags_PassthruCentralNode | ImGuiDockNodeFlags_NoDockingOverCentralNode);

        if (!_window.editor_settings()->get(editor_settings::MESH_MANAGER_SETUP_COMPLETE).as_bool())
        {
            const auto dock_left = ImGui::DockBuilderSplitNode(dockspace_id, ImGuiDir_Left, 0.2F, nullptr, &dockspace_id);
            const auto dock_bottom = ImGui::DockBuilderSplitNode(dockspace_id, ImGuiDir_Down, 0.2F, nullptr, &dockspace_id);

            ImGui::DockBuilderDockWindow("Meshes", dock_left);
            ImGui::DockBuilderDockWindow("Properties", dock_bottom);

            ImGui::DockBuilderFinish(dockspace_id);
            _window.editor_settings()->set(editor_settings::MESH_MANAGER_SETUP_COMPLETE, true);
        }

        ImGui::Begin("Meshes");
        {
            if (ImGui::InputText("Filter", _filter.data(), _filter.size()))
            {
                // Filter items
            }

            auto listbox_size = ImGui::GetContentRegionAvail();
            listbox_size.y -= ImGui::CalcTextSize("|").y
                              + (ImGui::GetStyle().FramePadding.y * 2)
                              + ImGui::GetStyle().ItemSpacing.y;

            if (ImGui::BeginListBox("##mesh_list", listbox_size))
            {
                for (const auto& name : _available_mesh_names)
                {
                    if (ImGui::Selectable(name.c_str(), name == _selected))
                    {
                        _selected = name;
                        _mesh_node->set_mesh(name);
                        _mesh_node->set_material("monki");
                        _mesh_node->set_enabled(true);
                    }
                }
                ImGui::EndListBox();
            }

            const auto button_size = (ImGui::GetContentRegionAvail().x - (ImGui::GetStyle().ItemSpacing.x * 2)) / 3;

            if (ImGui::Button("New", ImVec2(button_size, 0)))
            {
                //...
            }
            ImGui::SameLine();
            ImGui::BeginDisabled(_selected.empty());
            if (ImGui::Button("Rename", ImVec2(button_size, 0)))
            {
                _rename_dialog.set_text(_selected);
                _rename_dialog.open();
            }
            ImGui::SameLine();
            if (ImGui::Button("Delete", ImVec2(button_size, 0)))
            {
                _delete_confirm_dialog.set_label(std::format("Delete mesh '{}'", _selected));
                _delete_confirm_dialog.open();
            }
            ImGui::EndDisabled();
        }
        ImGui::End(); // Meshes

        ImGui::Begin("Properties");
        {
        }
        ImGui::End(); // Properties

        ImGuiDockNode* node = ImGui::DockBuilderGetNode(dockspace_id);
        const auto* centralnode = node->CentralNode;

        if (centralnode)
        {
            const auto vp_pos = glm::vec2{ centralnode->Pos.x, centralnode->Pos.y } / ImGui::GetWindowDpiScale();
            const auto vp_size = glm::vec2{ centralnode->Size.x, centralnode->Size.y } / ImGui::GetWindowDpiScale();

            const auto scale = _window.window().get_scale();
            _scene->get_renderer().set_custom_viewport(std::make_pair(vp_pos * scale, (vp_pos + vp_size) * scale));
        }
    }

    void mesh_manager::init_callbacks()
    {
        _rename_dialog.callbacks.accepted = [this] {
            const auto name = _rename_dialog.text();
            const auto new_abspath = _project.name_to_abspath<project::mesh_asset>(name);

            const auto asset = _project.get_asset_by_name<project::mesh_asset>(_selected);
            asset->move_path(new_abspath);
            asset->save();

            _project.reload_mesh_assets();
        };

        _delete_confirm_dialog.callbacks.accepted = [this] {
            const auto asset = _project.get_asset_by_name<project::mesh_asset>(_selected);

            std::filesystem::remove(asset->bin_path());
            std::filesystem::remove(asset->absolute_path());

            _project.reload_mesh_assets();
        };
    }
} // namespace cathedral::editor2