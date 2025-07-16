#include "cathedral/engine/material.hpp"
#include "imgui_internal.h"

#include <cathedral/editor2/asset_managers/material_manager.hpp>

#include <cathedral/project/project.hpp>

#include <imgui.h>

namespace cathedral::editor2
{
    material_manager::material_manager(project::project& project)
        : _project(project)
    {
        _new_material_dialog = std::make_unique<text_input_dialog>("New material", "Name", false);
        _rename_material_dialog = std::make_unique<text_input_dialog>("Rename material", "Name", false);
        _delete_material_dialog = std::make_unique<confirm_dialog>();
        _delete_material_dialog->set_title("Confirm");

        _new_material_dialog->callbacks().selected = [this](const std::string& selected) {
            const auto abs_path = _project.name_to_abspath<project::material_asset>(selected);
            const auto asset = std::make_shared<project::material_asset>(&_project, abs_path);
            asset->set_domain(engine::material_domain::OPAQUE);
            asset->mark_as_manually_loaded();
            asset->save();

            _project.reload_material_assets();
        };

        _rename_material_dialog->callbacks().selected = [this](const std::string& new_name) {
            if (!_selected_material.has_value())
            {
                return;
            }

            const auto asset = _project.material_assets().at(*_selected_material);
            const auto new_abs_path = _project.name_to_abspath<project::material_asset>(new_name);
            asset->move_path(new_abs_path);

            _project.reload_material_assets();
        };

        _delete_material_dialog->callbacks().selected = [this](const bool yes) {
            if (!_selected_material.has_value() || !yes)
            {
                return;
            }

            const auto asset = _project.material_assets().at(*_selected_material);
            const auto abs_path = asset->absolute_path();
            std::filesystem::remove(abs_path);

            _project.reload_material_assets();

            _selected_material = {};
        };
    }

    void material_manager::tick()
    {
        constexpr auto popup_id = "Material Manager";

        if (_first_open)
        {
            ImGui::OpenPopup(popup_id);
            _open = true;
            _first_open = false;
        }

        if (ImGui::BeginPopupModal(popup_id, &_open))
        {
            ImGui::Columns(2, "Material Manager layout");
            ImGui::SetColumnWidth(0, 220);

            tick_material_list();

            if (_selected_material.has_value())
            {
                tick_shader_combos();
                tick_material_vars();
            }

            ImGui::EndColumns();
            ImGui::EndPopup();
        }
    }

    void material_manager::open()
    {
        _first_open = true;
    }

    void material_manager::tick_material_list()
    {
        if (ImGui::BeginChild("Material List"))
        {
            if (ImGui::BeginListBox("##Materials", { 200, ImGui::GetWindowHeight() - 28 }))
            {
                for (const auto& name : _project.material_assets() | std::views::keys)
                {
                    if (ImGui::Selectable(name.c_str(), _selected_material && _selected_material.value() == name.c_str()))
                    {
                        _selected_material = name;
                    }
                }
                ImGui::EndListBox();
            }
            if (ImGui::Button("New", { 61, 20 }))
            {
                const auto material_names = _project.material_assets() | std::views::keys;
                _new_material_dialog->set_forbidden_inputs(
                    std::unordered_set<std::string>{ material_names.begin(), material_names.end() });
                _new_material_dialog->open();
            }
            ImGui::SameLine();
            if (ImGui::Button("Rename", { 61, 20 }))
            {
                const auto material_names = _project.material_assets() | std::views::keys;
                _rename_material_dialog->set_forbidden_inputs(
                    std::unordered_set<std::string>{ material_names.begin(), material_names.end() });
                _rename_material_dialog->open();
            }

            ImGui::SameLine();
            ImGui::BeginDisabled(!_selected_material.has_value());
            if (ImGui::Button("Delete", { 61, 20 }))
            {
                _delete_material_dialog->set_message(std::format("Delete material '{}'?", *_selected_material));
                _delete_material_dialog->open();
            }
            ImGui::EndDisabled();
        }
        ImGui::EndChild();

        _new_material_dialog->tick();
        _rename_material_dialog->tick();
        _delete_material_dialog->tick();
    }

    void material_manager::tick_shader_combos() const
    {
        const auto asset = _project.material_assets().at(*_selected_material);

        const auto get_shaders_by_type = [this](gfx::shader_type type) {
            return _project.shader_assets() | std::views::values |
                   std::views::filter(
                       [type](const std::shared_ptr<project::shader_asset>& asset) { return asset->type() == type; });
        };

        ImGui::NextColumn();
        if (ImGui::BeginCombo(
                "Vertex shader",
                asset->vertex_shader_ref().empty() ? "None" : asset->vertex_shader_ref().c_str()))
        {
            if (ImGui::Selectable("None"))
            {
                asset->set_vertex_shader_ref("");
                asset->save();

                _project.reload_material_assets();
            }
            for (const auto& shader : get_shaders_by_type(gfx::shader_type::VERTEX))
            {
                if (ImGui::Selectable(shader->name().c_str()))
                {
                    asset->set_vertex_shader_ref(shader->name());
                    asset->save();

                    _project.reload_material_assets();
                }
            }
            ImGui::EndCombo();
        }
        if (ImGui::BeginCombo(
                "Fragment shader",
                asset->fragment_shader_ref().empty() ? "None" : asset->fragment_shader_ref().c_str()))
        {
            if (ImGui::Selectable("None"))
            {
                asset->set_fragment_shader_ref("");
                asset->save();

                _project.reload_material_assets();
            }
            for (const auto& shader : get_shaders_by_type(gfx::shader_type::FRAGMENT))
            {
                if (ImGui::Selectable(shader->name().c_str()))
                {
                    asset->set_fragment_shader_ref(shader->name());
                    asset->save();

                    _project.reload_material_assets();
                }
            }
            ImGui::EndCombo();
        }
    }

    void material_manager::tick_material_vars()
    {
        const auto asset = _project.material_assets().at(*_selected_material);

        if (asset->vertex_shader_ref().empty() || asset->fragment_shader_ref().empty())
        {
            ImGui::Text("Incomplete material");
            return;
        }

        const std::string material_key = asset->vertex_shader_ref() + "$$" + asset->fragment_shader_ref();
        if (!_dummy_materials.contains(material_key))
        {
            const auto vx_shader_asset = _project.shader_assets().at(asset->vertex_shader_ref());
            const auto fg_shader_asset = _project.shader_assets().at(asset->fragment_shader_ref());

            engine::material_args args;
            args.name = "temp_material";
            args.vertex_shader_source = vx_shader_asset->source();
            args.fragment_shader_source = fg_shader_asset->source();

            engine::material dummy = engine::material::create_dummy_material(std::move(args));

            _dummy_materials.emplace(material_key, std::move(dummy));
        }
        const auto& dummy_material = _dummy_materials.at(material_key);

        constexpr auto table_flags = ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_Resizable;

        ImGui::Text("Material variables:");
        if (ImGui::BeginTable("Material variables", 4, table_flags))
        {
            ImGui::TableNextRow(ImGuiTableRowFlags_Headers);
            ImGui::TableNextColumn();
            ImGui::Text("Name");
            ImGui::TableNextColumn();
            ImGui::Text("Type");
            ImGui::TableNextColumn();
            ImGui::Text("Count");
            ImGui::TableNextColumn();
            ImGui::Text("Bindings");

            for (const auto& var : dummy_material.material_variables())
            {
                ImGui::TableNextRow();

                ImGui::TableNextColumn();
                ImGui::Text(var.name.c_str());
                ImGui::TableNextColumn();
                ImGui::Text(std::string{ magic_enum::enum_name(var.type) }.c_str());
                ImGui::TableNextColumn();
                ImGui::Text(std::to_string(var.count).c_str());
                ImGui::TableNextColumn();
                if (ImGui::BeginCombo(("##Material bindings" + var.name).c_str(), "None"))
                {
                    ImGui::EndCombo();
                }
            }
            ImGui::EndTable();
        }

        ImGui::Text("Node variables:");
        if (ImGui::BeginTable("Node variables", 4, table_flags))
        {
            ImGui::TableNextRow(ImGuiTableRowFlags_Headers);
            ImGui::TableNextColumn();
            ImGui::Text("Name");
            ImGui::TableNextColumn();
            ImGui::Text("Type");
            ImGui::TableNextColumn();
            ImGui::Text("Count");
            ImGui::TableNextColumn();
            ImGui::Text("Bindings");

            for (const auto& var : dummy_material.node_variables())
            {
                ImGui::TableNextRow();

                ImGui::TableNextColumn();
                ImGui::Text(var.name.c_str());
                ImGui::TableNextColumn();
                ImGui::Text(std::string{ magic_enum::enum_name(var.type) }.c_str());
                ImGui::TableNextColumn();
                ImGui::Text(std::to_string(var.count).c_str());
                ImGui::TableNextColumn();
                if (ImGui::BeginCombo(("##Node bindings" + var.name).c_str(), "None"))
                {
                    if (ImGui::Selectable("None"))
                    {
                        NOT_IMPLEMENTED();
                    }

                    for (const auto& [value, name] : magic_enum::enum_entries<engine::shader_node_uniform_binding>())
                    {
                        if (ImGui::Selectable(std::string{ name }.c_str()))
                        {
                            NOT_IMPLEMENTED();
                        }
                    }
                    ImGui::EndCombo();
                }
            }
            ImGui::EndTable();
        }
    }
} // namespace cathedral::editor2