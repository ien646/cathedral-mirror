#include <cathedral/editor2/resource_managers/material_manager.hpp>

#include <cathedral/engine/scene.hpp>
#include <cathedral/project/project.hpp>

#include <imgui.h>
#include <imgui_internal.h>

#include <ranges>

namespace cathedral::editor2
{
    material_manager::material_manager(project::project& pro)
        : _window("Material manager", 800, 600, pro.get_settings())
        , _project(pro)
    {
        _available_material_names.append_range(_project.get_assets<project::material_asset>() | std::views::keys);
        init_scene();

        _rename_dialog.callbacks.accepted = [this] {
            const auto name = _rename_dialog.text();
            const auto new_abspath = _project.name_to_abspath<project::material_asset>(name);

            const auto asset = _project.get_asset_by_name<project::material_asset>(_selected_material);
            asset->move_path(new_abspath);

            _project.reload_material_assets();

            _available_material_names.clear();
            _available_material_names.append_range(_project.get_assets<project::material_asset>() | std::views::keys);

            _selected_material = name;
        };

        _delete_confirm_dialog.callbacks.accepted = [this] {
            const auto abs_path = _project.name_to_abspath<project::material_asset>(_selected_material);
            std::filesystem::remove(abs_path);
            _project.reload_material_assets();

            _available_material_names.clear();
            _available_material_names.append_range(_project.get_assets<project::material_asset>() | std::views::keys);

            _selected_material = {};
        };

        init_shaders();
    }

    void material_manager::execute()
    {
        while (_window.keep_open())
        {
            _scene->tick([this]([[maybe_unused]] const double deltatime) { _window.tick([this] { tick_gui(); }); });
        }
    }

    void material_manager::init_scene()
    {
        engine::scene_args args;
        args.loaders = _project.get_loader_funcs();
        args.name = "Material manager";
        args.prenderer = &_window.renderer();

        _scene = std::make_unique<engine::scene>(std::move(args));
    }

    void material_manager::init_shaders()
    {
        const auto all_shaders = _project.get_assets<project::shader_asset>();
        const auto vertex_shader_names =
            all_shaders
            | std::views::values
            | std::views::filter([](const std::shared_ptr<project::shader_asset>& shader_asset) {
                  return shader_asset->type() == gfx::shader_type::VERTEX;
              })
            | std::views::transform(
                [](const std::shared_ptr<project::shader_asset>& shader_asset) { return shader_asset->name(); })
            | std::ranges::to<std::vector<std::string>>();

        const auto fragment_shader_names =
            all_shaders
            | std::views::values
            | std::views::filter([](const std::shared_ptr<project::shader_asset>& shader_asset) {
                  return shader_asset->type() == gfx::shader_type::FRAGMENT;
              })
            | std::views::transform(
                [](const std::shared_ptr<project::shader_asset>& shader_asset) { return shader_asset->name(); })
            | std::ranges::to<std::vector<std::string>>();

        _available_vertex_shaders.emplace_back("None");
        _available_vertex_shaders.append_range(vertex_shader_names);

        _available_fragment_shaders.emplace_back("None");
        _available_fragment_shaders.append_range(fragment_shader_names);
    }

    void material_manager::tick_gui()
    {
        const auto filter_text = "Filter";
        const auto filter_text_size = ImGui::CalcTextSize(filter_text);

        const auto vp = ImGui::GetMainViewport();

        ImGui::DockSpaceOverViewport(
            ImGui::GetID("material_manager_dockspace"),
            ImGui::GetMainViewport(),
            ImGuiDockNodeFlags_PassthruCentralNode);

        // ReSharper disable once CppDFAConstantConditions
        // ReSharper disable once CppDFAUnreachableCode
        if (_first_tick.get_and_reset())
        {
            ImGuiID dockspace_id = ImGui::GetID("material_manager_dockspace");
            const ImGuiID dock_left = ImGui::DockBuilderSplitNode(dockspace_id, ImGuiDir_Left, 0.35F, nullptr, &dockspace_id);
            ImGui::DockBuilderGetNode(dock_left)->LocalFlags |= ImGuiDockNodeFlags_NoTabBar
                                                                | ImGuiDockNodeFlags_NoDockingOverMe;
            ImGui::DockBuilderDockWindow("Materials", dock_left);
            ImGui::DockBuilderFinish(dockspace_id);
        }

        float material_dock_width = 0;
        ImGui::Begin("Materials");
        {
            material_dock_width = ImGui::GetWindowWidth();

            ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x - filter_text_size.x);
            ImGui::InputText("Filter", _filter.data(), _filter.size());

            auto listbox_size = ImGui::GetContentRegionAvail();
            listbox_size.y -= ImGui::CalcTextSize("|").y
                              + (ImGui::GetStyle().FramePadding.y * 2)
                              + ImGui::GetStyle().ItemSpacing.y;

            if (ImGui::BeginListBox("##list", listbox_size))
            {
                for (size_t i = 0; i < _available_material_names.size(); ++i)
                {
                    const auto& name = _available_material_names.at(i);
                    ImGui::PushID(static_cast<int>(i));
                    if (ImGui::Selectable(name.c_str(), name == _selected_material))
                    {
                        _selected_material = name;
                    }
                    ImGui::PopID();
                }
                ImGui::EndListBox();
            }

            const auto button_size = (ImGui::GetContentRegionAvail().x - (ImGui::GetStyle().ItemSpacing.x * 2)) / 3;

            if (ImGui::Button("New", ImVec2(button_size, 0)))
            {
                // _add_material_dialog.open();
            }
            ImGui::SameLine();
            ImGui::BeginDisabled(_selected_material.empty());
            if (ImGui::Button("Rename", ImVec2(button_size, 0)))
            {
                _rename_dialog.set_text(_selected_material);
                _rename_dialog.set_validator([this](const std::string& text) -> bool {
                    return !std::ranges::contains(_available_material_names, text);
                });
                _rename_dialog.open();
            }
            ImGui::SameLine();
            if (ImGui::Button("Delete", ImVec2(button_size, 0)))
            {
                _delete_confirm_dialog.set_label(std::format("Delete material '{}'?", _selected_material));
                _delete_confirm_dialog.open();
            }
            ImGui::EndDisabled();
        }
        ImGui::End();

        ImGui::SetNextWindowPos({ material_dock_width + 1, 0.0F });
        ImGui::SetNextWindowSize({ vp->Size.x - material_dock_width, vp->Size.y });
        ImGui::Begin(
            "Properties",
            nullptr,
            ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar);
        {
            tick_properties();
        }
        ImGui::End();

        // _add_material_dialog.tick();
        _rename_dialog.tick();
        _delete_confirm_dialog.tick();
    }

    constexpr auto TABLE_FLAGS = ImGuiTableFlags_Borders | ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_Resizable;

    void uniform_var_table_headers_setup()
    {
        ImGui::TableSetupColumn("Index");
        ImGui::TableSetupColumn("Offset");
        ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthStretch);
        ImGui::TableSetupColumn("Type");
        ImGui::TableSetupColumn("Binding", ImGuiTableColumnFlags_WidthStretch);

        ImGui::TableHeadersRow();
    }

    template <bool Material>
    void draw_variable_row(
        const std::shared_ptr<project::material_asset>& asset,
        const engine::shader_variable& var,
        const size_t index,
        uint32_t& current_offset)
    {
        using shader_binding_type =
            std::conditional_t<Material, engine::shader_material_uniform_binding, engine::shader_node_uniform_binding>;

        ImGui::TableNextRow();

        ImGui::TableNextColumn();
        ImGui::Text("%lu", index);

        ImGui::TableNextColumn();
        ImGui::Text("%u", current_offset);
        current_offset += gfx::shader_data_type_offset(var.type, var.count, current_offset);

        ImGui::TableNextColumn();
        ImGui::Text("%s", var.name.c_str());

        ImGui::TableNextColumn();
        ImGui::Text("%s", std::string{ magic_enum::enum_name(var.type) }.c_str());

        std::vector<std::string> binding_names = { "None" };

        binding_names.append_range(
            magic_enum::enum_names<shader_binding_type>()
            | std::views::transform([](const std::string_view sv) { return std::string{ sv }; }));

        std::string current_binding;
        if constexpr (Material)
        {
            current_binding = !asset->material_variable_bindings().contains(var.name)
                                  ? "None"
                                  : std::string{ magic_enum::enum_name(asset->material_variable_bindings().at(var.name)) };
        }
        else
        {
            current_binding = !asset->node_variable_bindings().contains(var.name)
                                  ? "None"
                                  : std::string{ magic_enum::enum_name(asset->node_variable_bindings().at(var.name)) };
        }

        ImGui::TableNextColumn();
        ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
        if (ImGui::BeginCombo("##binding", current_binding.c_str()))
        {
            for (const auto& name : binding_names)
            {
                ImGui::PushID(name.c_str());
                if (ImGui::Selectable(name.c_str()))
                {
                    const auto value = magic_enum::enum_cast<shader_binding_type>(name);
                    if constexpr (Material)
                    {
                        asset->set_material_uniform_binding(var.name, value);
                    }
                    else
                    {
                        asset->set_node_uniform_binding(var.name, value);
                    }
                    asset->save();
                }
                ImGui::PopID();
            }
            ImGui::EndCombo();
        }
    }

    void material_manager::tick_material_uniform_vars_table(
        const std::shared_ptr<project::material_asset>& asset,
        const std::unordered_map<std::string, engine::material>::mapped_type& dummy_material) const
    {
        ImGui::Text("%s", "Material uniform variables");

        if (ImGui::BeginTable("Material uniform variables", 5, TABLE_FLAGS, ImVec2(ImGui::GetContentRegionAvail().x, 0)))
        {
            uniform_var_table_headers_setup();

            uint32_t var_offset = 0;
            for (size_t i = 0; i < dummy_material.material_uniform_variables().size(); ++i)
            {
                ImGui::PushID(static_cast<int>(i));
                draw_variable_row<false>(asset, dummy_material.material_uniform_variables()[i], i, var_offset);
                ImGui::PopID();
            }

            ImGui::EndTable();
        }
    }

    void material_manager::tick_node_uniform_vars_table(
        const std::shared_ptr<project::material_asset>& asset,
        const std::unordered_map<std::string, engine::material>::mapped_type& dummy_material) const
    {
        ImGui::Text("%s", "Node uniform variables");
        if (ImGui::BeginTable("Node uniform variables", 5, TABLE_FLAGS, ImVec2(ImGui::GetContentRegionAvail().x, 0)))
        {
            uniform_var_table_headers_setup();

            uint32_t var_offset = 0;
            for (size_t i = 0; i < dummy_material.node_uniform_variables().size(); ++i)
            {
                ImGui::PushID(static_cast<int>(i));
                draw_variable_row<false>(asset, dummy_material.node_uniform_variables()[i], i, var_offset);
                ImGui::PopID();
            }

            ImGui::EndTable();
        }
    }

    void texture_table_headers_setup()
    {
        ImGui::TableSetupColumn("Index");
        ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthStretch);
        ImGui::TableSetupColumn("Binding", ImGuiTableColumnFlags_WidthStretch);

        ImGui::TableHeadersRow();
    }

    template <bool Material>
    void draw_texture_row(const std::shared_ptr<project::material_asset>& asset, const std::string& name, const uint32_t index)
    {
        using binding_type =
            std::conditional_t<Material, engine::shader_material_texture_binding, engine::shader_node_texture_binding>;

        ImGui::TableNextRow();

        ImGui::TableNextColumn();
        ImGui::Text("%u", index);

        ImGui::TableNextColumn();
        ImGui::Text("%s", name.c_str());

        std::vector<std::string> binding_names = { "None" };

        binding_names.append_range(
            magic_enum::enum_names<binding_type>()
            | std::views::transform([](const std::string_view sv) { return std::string{ sv }; }));

        std::string current_binding;
        if constexpr (Material)
        {
            current_binding = !asset->material_texture_bindings().contains(name)
                                  ? "None"
                                  : std::string{ magic_enum::enum_name(asset->material_texture_bindings().at(name)) };
        }
        else
        {
            current_binding = !asset->node_texture_bindings().contains(name)
                                  ? "None"
                                  : std::string{ magic_enum::enum_name(asset->node_texture_bindings().at(name)) };
        }

        ImGui::TableNextColumn();
        ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
        if (ImGui::BeginCombo("##binding", current_binding.c_str()))
        {
            for (const auto& bname : binding_names)
            {
                ImGui::PushID(bname.c_str());
                if (ImGui::Selectable(bname.c_str()))
                {
                    const auto value = magic_enum::enum_cast<binding_type>(bname);
                    if constexpr (Material)
                    {
                        asset->set_material_texture_binding(name, value);
                    }
                    else
                    {
                        asset->set_node_texture_binding(name, value);
                    }
                    asset->save();
                }
                ImGui::PopID();
            }
            ImGui::EndCombo();
        }
    }

    void material_manager::tick_material_texture_table(
        const std::shared_ptr<project::material_asset>& asset,
        const engine::material& dummy_material) const
    {
        ImGui::Text("%s", "Material textures");
        if (ImGui::BeginTable("Material textures", 3, TABLE_FLAGS, ImVec2(ImGui::GetContentRegionAvail().x, 0)))
        {
            texture_table_headers_setup();

            for (uint32_t i = 0; i < dummy_material.material_texture_names().size(); ++i)
            {
                ImGui::PushID(static_cast<int>(i));
                draw_texture_row<true>(asset, dummy_material.material_texture_names()[i], i);
                ImGui::PopID();
            }

            ImGui::EndTable();
        }
    }

    void material_manager::tick_node_texture_table(
        const std::shared_ptr<project::material_asset>& asset,
        const engine::material& dummy_material) const
    {
        ImGui::Text("%s", "Node textures");
        if (ImGui::BeginTable("Node textures", 3, TABLE_FLAGS, ImVec2(ImGui::GetContentRegionAvail().x, 0)))
        {
            texture_table_headers_setup();

            for (uint32_t i = 0; i < dummy_material.node_texture_names().size(); ++i)
            {
                ImGui::PushID(static_cast<int>(i));
                draw_texture_row<false>(asset, dummy_material.node_texture_names()[i], i);
                ImGui::PopID();
            }

            ImGui::EndTable();
        }
    }

    void buffer_table_headers_setup()
    {
        ImGui::TableSetupColumn("Index");
        ImGui::TableSetupColumn("Descriptor Binding");
        ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthStretch);
        ImGui::TableSetupColumn("Binding", ImGuiTableColumnFlags_WidthStretch);

        ImGui::TableHeadersRow();
    }

    void material_manager::tick_material_buffer_table(
        const std::shared_ptr<project::material_asset>& asset,
        const engine::material& dummy_material) const
    {
        //...
    }

    void material_manager::tick_node_buffer_table(
        const std::shared_ptr<project::material_asset>& asset,
        const engine::material& dummy_material) const
    {
        //...
    }

    void material_manager::tick_properties()
    {
        if (_selected_material.empty())
        {
            ImGui::Text("Select a material");
        }
        else
        {
            auto asset = _project.get_asset_by_name<project::material_asset>(_selected_material);
            if (asset == nullptr)
            {
                ImGui::Text("%s", std::format("Failure loading material '{}'", _selected_material).c_str());
            }

            ImGui::SeparatorText("Definition");

            if (ImGui::BeginCombo(
                    "Vertex shader",
                    asset->vertex_shader_ref().empty() ? "None" : asset->vertex_shader_ref().c_str()))
            {
                for (const auto& name : _available_vertex_shaders)
                {
                    if (ImGui::Selectable(name.c_str(), name == asset->vertex_shader_ref()))
                    {
                        asset->set_vertex_shader_ref(name == "None" ? "" : name);
                        asset->save();
                    }
                }
                ImGui::EndCombo();
            }

            if (ImGui::BeginCombo(
                    "Fragment shader",
                    asset->vertex_shader_ref().empty() ? "None" : asset->fragment_shader_ref().c_str()))
            {
                for (const auto& name : _available_fragment_shaders)
                {
                    if (ImGui::Selectable(name.c_str(), name == asset->fragment_shader_ref()))
                    {
                        asset->set_fragment_shader_ref(name == "None" ? "" : name);
                        asset->save();
                    }
                }
                ImGui::EndCombo();
            }

            bool cull_backfaces = asset->cull_backfaces();
            if (ImGui::Checkbox("Cull backfaces", &cull_backfaces))
            {
                asset->set_cull_backfaces(cull_backfaces);
                asset->save();
            }

            bool wireframe = asset->wireframe();
            if (ImGui::Checkbox("Wireframe", &wireframe))
            {
                asset->set_wireframe(wireframe);
                asset->save();
            }

            bool flip_ff = asset->flip_front_faces();
            if (ImGui::Checkbox("Flip front faces", &flip_ff))
            {
                asset->set_flip_front_faces(flip_ff);
                asset->save();
            }

            ImGui::SeparatorText("Variables");

            if (asset->vertex_shader_ref().empty() || asset->fragment_shader_ref().empty())
            {
                return;
            }

            const std::string dummy_material_key =
                std::format("__{}:$:{}", asset->vertex_shader_ref(), asset->fragment_shader_ref());
            if (!_dummy_materials.contains(dummy_material_key))
            {
                engine::material_args args;
                args.name = dummy_material_key;
                args.vertex_shader_source =
                    _project.get_asset_by_name<project::shader_asset>(asset->vertex_shader_ref())->source();
                args.fragment_shader_source =
                    _project.get_asset_by_name<project::shader_asset>(asset->fragment_shader_ref())->source();

                _dummy_materials.emplace(dummy_material_key, engine::material::create_dummy_material(std::move(args)));
            }

            const auto& dummy_material = _dummy_materials.at(dummy_material_key);

            if (!dummy_material.material_uniform_variables().empty())
            {
                tick_material_uniform_vars_table(asset, dummy_material);
            }
            if (!dummy_material.node_uniform_variables().empty())
            {
                tick_node_uniform_vars_table(asset, dummy_material);
            }
            if (!dummy_material.material_texture_names().empty())
            {
                tick_material_texture_table(asset, dummy_material);
            }
            if (!dummy_material.node_texture_names().empty())
            {
                tick_node_texture_table(asset, dummy_material);
            }
            if (!dummy_material.material_buffer_names().empty())
            {
                tick_material_buffer_table(asset, dummy_material);
            }
            if (!dummy_material.node_buffer_names().empty())
            {
                tick_node_buffer_table(asset, dummy_material);
            }
        }
    }
} // namespace cathedral::editor2