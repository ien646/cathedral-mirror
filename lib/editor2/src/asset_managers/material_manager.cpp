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
        };
    }

    void material_manager::tick()
    {
        _new_material_dialog->tick();
        _rename_material_dialog->tick();
        _delete_material_dialog->tick();

        if (!_open)
        {
            return;
        }

        if (ImGui::Begin("Material Manager", &_open))
        {
            if (_first_open)
            {
                _first_open = false;
            }

            ImGui::Columns(2, "Material Manage layoutr");
            ImGui::SetColumnWidth(0, 220);

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

            if (_selected_material.has_value())
            {
                ImGui::NextColumn();
                if (ImGui::BeginChild("Material properties"))
                {
                    ImGui::Text(_selected_material.value().c_str());
                }
                ImGui::EndChild();
            }

            ImGui::EndColumns();
        }
        ImGui::End();
    }

    void material_manager::open()
    {
        _open = true;
        _first_open = true;
    }
} // namespace cathedral::editor2