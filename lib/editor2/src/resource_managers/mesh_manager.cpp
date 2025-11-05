#include <cathedral/editor2/resource_managers/mesh_manager.hpp>

#include <cathedral/editor2/engine_window.hpp>
#include <cathedral/editor2/native/file_dialog.hpp>
#include <cathedral/engine/native_script.hpp>
#include <cathedral/engine/nodes/camera3d_node.hpp>
#include <cathedral/engine/nodes/directional_light_node.hpp>
#include <cathedral/engine/nodes/mesh3d_node.hpp>

#include <battery/embed.hpp>

#include <ien/fs_utils.hpp>

#include <imgui.h>
#include <imgui_internal.h>

namespace cathedral::editor2
{
    constexpr auto MATERIAL_NAME = "__cathedral_mesh_manager_material";

    void script_tick(engine::scene_node* snode, [[maybe_unused]] engine::scene& scene, const double deltatime)
    {
        if (auto* node = dynamic_cast<engine::node*>(snode))
        {
            node->rotate_degrees(glm::vec3{ 0.0F, deltatime * 90.0F, 0.0F });
        }
    }

    mesh_manager::mesh_manager(project::project& pro)
        : resource_manager_base(pro)
    {
        _window.set_title("Mesh manager");

        _available_mesh_names.append_range(_project.get_assets<project::mesh_asset>() | std::views::keys);

        init_callbacks();
        init_material();

        _camera_node = _scene->add_root_node<engine::camera3d_node>("camera");
        _camera_node->set_local_position({ 0.0F, 0.0F, -5.0F });
        _camera_node->set_main_camera(true);

        _mesh_node = _scene->add_root_node<engine::mesh3d_node>("mesh");
        _mesh_node->set_enabled(false);
        _mesh_node->set_material(MATERIAL_NAME);

        _mesh_node->add_script(engine::make_native_script("spin", nullptr, &script_tick, nullptr, nullptr));

        const auto sun = _scene->add_root_node<engine::directional_light_node>("sun");
        sun->set_intensity(0.9F);
        sun->set_color(glm::vec3{ 1.0F, 1.0F, 1.0F });
        sun->set_local_rotation(glm::vec3(-30.0F, -30.0F, 0.0F));
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

        if (!_window.editor_settings()->get(editor_setting::MESH_MANAGER_SETUP_COMPLETE).as_bool())
        {
            const auto dock_left = ImGui::DockBuilderSplitNode(dockspace_id, ImGuiDir_Left, 0.2F, nullptr, &dockspace_id);
            const auto dock_bottom = ImGui::DockBuilderSplitNode(dockspace_id, ImGuiDir_Down, 0.2F, nullptr, &dockspace_id);

            ImGui::DockBuilderDockWindow("Meshes", dock_left);
            ImGui::DockBuilderDockWindow("Properties", dock_bottom);

            ImGui::DockBuilderFinish(dockspace_id);
            _window.editor_settings()->set(editor_setting::MESH_MANAGER_SETUP_COMPLETE, true);
            _project.save_settings();
        }

        ImGui::Begin("Meshes");
        {
            _resource_filter.tick(_available_mesh_names, _filtered_mesh_names);

            auto listbox_size = ImGui::GetContentRegionAvail();
            listbox_size.y -= ImGui::CalcTextSize("|").y
                              + (ImGui::GetStyle().FramePadding.y * 2)
                              + ImGui::GetStyle().ItemSpacing.y;

            if (ImGui::BeginListBox("##mesh_list", listbox_size))
            {
                for (const auto& name : _filtered_mesh_names)
                {
                    if (ImGui::Selectable(name->c_str(), *name == _selected))
                    {
                        _selected = *name;
                        _mesh_node->set_mesh(*name);
                        _mesh_node->set_enabled(true);
                    }
                }
                ImGui::EndListBox();
            }

            const auto button_size = (ImGui::GetContentRegionAvail().x - (ImGui::GetStyle().ItemSpacing.x * 2)) / 3;

            if (ImGui::Button("New", ImVec2(button_size, 0)))
            {
                if (const auto file = native_open_file())
                {
                    const auto filename = std::filesystem::path(ien::get_file_name(*file)).replace_extension(".casset");
                    const auto abs_path =
                        (std::filesystem::path(_project.get_assets_path<project::mesh_asset>()) / filename).string();

                    auto use_path = abs_path;
                    uint32_t retries = 0;
                    while (std::filesystem::exists(use_path))
                    {
                        use_path = std::format("{}_{}", abs_path, ++retries);
                    }

                    const engine::mesh m(*file);
                    auto asset = std::make_shared<project::mesh_asset>(&_project, use_path);
                    asset->save_mesh(m);
                    asset->save();
                    _project.add_asset(asset);

                    _available_mesh_names.push_back(asset->name());
                }
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
            if (_mesh_node->mesh_name())
            {
                const auto asset = _project.get_asset_by_name<project::mesh_asset>(*_mesh_node->mesh_name());
                ImGui::Text("Vertex count: %u", asset->vertex_count());
                ImGui::Text(" Index count: %u", asset->index_count());
                ImGui::Text("        Size: %u", asset->uncompressed_size());
            }
        }
        ImGui::End(); // Properties

        ImGuiDockNode* node = ImGui::DockBuilderGetNode(dockspace_id);

        if (const auto* centralnode = node->CentralNode)
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

    void mesh_manager::init_material()
    {
        const auto vert_source = b::embed<"editor/shaders/mesh_viewer/vertex.glsl">().str();
        const auto frag_source = b::embed<"editor/shaders/mesh_viewer/fragment.glsl">().str();

        engine::material_args args;
        args.cull_backfaces = false;
        args.domain = engine::material_domain::OPAQUE;
        args.flip_front_faces = false;
        args.fragment_shader_source = frag_source;
        args.material_buffer_bindings = {};
        args.material_texture_bindings = {};
        args.material_uniform_bindings = {};
        args.node_buffer_bindings = {};
        args.node_texture_bindings = {};
        args.node_uniform_bindings = { { "node_model_matrix", engine::shader_node_uniform_binding::NODE_MODEL_MATRIX } };
        args.name = MATERIAL_NAME;
        args.vertex_shader_source = vert_source;
        args.wireframe = false;

        _material = _scene->get_renderer().create_material(MOVE(args)).lock();

        _material->set_material_uniform_variable_value("specular_intensity", 1.0F);
        _material->set_material_uniform_variable_value("specular_power", 1.0F);
    }
} // namespace cathedral::editor2