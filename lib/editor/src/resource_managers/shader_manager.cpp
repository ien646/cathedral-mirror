#include <cathedral/editor/resource_managers/shader_manager.hpp>

#include <cathedral/editor/callback_impl.hpp>
#include <cathedral/editor/native/file_dialog.hpp>

#include <ien/fs_utils.hpp>

#include <imgui_internal.h>
#include <misc/cpp/imgui_stdlib.h>

#include <TextEditor.h>

namespace cathedral::editor
{
    shader_manager::shader_manager(project::project& pro, editor_settings_interface& editor_settings)
        : resource_manager_base(pro)
        , _editor_settings(editor_settings)
    {
        _available_shaders = _project.shader_assets() | std::views::keys | std::ranges::to<std::vector>();
        std::ranges::sort(_available_shaders);

        _filtered_shaders = _available_shaders
                            | std::views::transform([](const std::string& name) { return &name; })
                            | std::ranges::to<std::vector>();

        _text_editor.SetLanguageDefinition(TextEditor::LanguageDefinition::GLSL());

        auto palette = TextEditor::GetDarkPalette();
        palette[static_cast<int>(TextEditor::PaletteIndex::Background)] = IM_COL32(0, 0, 0, 128);
        _text_editor.SetPalette(palette);
    }

    void shader_manager::tick()
    {
        if (_window.keep_open())
        {
            _scene->tick([this]([[maybe_unused]] const double deltatime) { _window.tick([this] { tick_gui(); }); });
        }
    }

    void shader_manager::save_current_shader()
    {
        if (_selected.empty())
        {
            return;
        }

        if (!_modified_sources.contains(_selected))
        {
            return;
        }

        const auto asset = _project.get_asset_by_name<project::shader_asset>(_selected);
        asset->set_source(std::move(_modified_sources[_selected]));
        asset->save();

        _modified_sources.erase(_selected);
    }

    void shader_manager::save_all_shaders()
    {
        for (auto& [name, src] : _modified_sources)
        {
            const auto asset = _project.get_asset_by_name<project::shader_asset>(name);
            asset->set_source(std::move(src));
            asset->save();
        }
        _modified_sources.clear();
    }

    void shader_manager::tick_gui()
    {
        auto dockspace_id = ImGui::DockSpaceOverViewport(0, ImGui::GetMainViewport(), ImGuiDockNodeFlags_PassthruCentralNode);

        if (!_window.editor_settings()->get(editor_setting::SHADER_MANAGER_SETUP_COMPLETE).as_bool())
        {
            const auto dock_left = ImGui::DockBuilderSplitNode(dockspace_id, ImGuiDir_Left, 0.2F, nullptr, &dockspace_id);

            ImGui::DockBuilderDockWindow("shaders", dock_left);

            const auto central_node = ImGui::DockBuilderGetCentralNode(dockspace_id);
            ImGui::DockBuilderDockWindow("Editor", central_node->ID);

            ImGui::DockBuilderFinish(dockspace_id);
            _window.editor_settings()->set(editor_setting::SHADER_MANAGER_SETUP_COMPLETE, true);
            _project.save_settings();
        }

        if (ImGui::BeginMainMenuBar())
        {
            if (ImGui::BeginMenu("File"))
            {
                ImGui::BeginDisabled(_selected.empty() || _modified_sources.empty());
                if (ImGui::MenuItem("Save"))
                {
                    save_current_shader();
                }
                ImGui::EndDisabled();
                ImGui::BeginDisabled(_modified_sources.empty());
                if (ImGui::MenuItem("Save All"))
                {
                    save_all_shaders();
                }
                ImGui::EndDisabled();
                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("Tools"))
            {
                if (ImGui::MenuItem("Show processed shader"))
                {
                    NOT_IMPLEMENTED();
                }
                if (ImGui::MenuItem("Snippets"))
                {
                    NOT_IMPLEMENTED();
                }
                if (ImGui::MenuItem("Templates"))
                {
                    NOT_IMPLEMENTED();
                }
                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("Window"))
            {
                if (ImGui::MenuItem("Reset layout"))
                {
                    _editor_settings.set(editor_setting::SHADER_MANAGER_SETUP_COMPLETE, false);
                }
                ImGui::EndMenu();
            }
            ImGui::EndMainMenuBar();
        }

        ImGui::Begin("shaders");
        {
            _resource_filter.tick(_available_shaders, _filtered_shaders);

            auto listbox_size = ImGui::GetContentRegionAvail();
            listbox_size.y -= ImGui::CalcTextSize("|").y
                              + (ImGui::GetStyle().FramePadding.y * 2)
                              + ImGui::GetStyle().ItemSpacing.y;

            if (ImGui::BeginListBox("##shader_list", listbox_size))
            {
                for (const auto& name : _filtered_shaders)
                {
                    if (ImGui::Selectable(
                            (*name + (_modified_sources.contains(*name) ? " *" : "")).c_str(),
                            *name == _selected))
                    {
                        _selected = *name;
                        if (_modified_sources.contains(_selected))
                        {
                            _text_editor.SetText(_modified_sources.at(_selected));
                        }
                        else
                        {
                            _text_editor.SetText(_project.get_asset_by_name<project::shader_asset>(_selected)->source());
                        }
                    }
                }
                ImGui::EndListBox();
            }

            const auto button_size = (ImGui::GetContentRegionAvail().x - (ImGui::GetStyle().ItemSpacing.x * 2)) / 3;

            if (ImGui::Button("New", ImVec2(button_size, 0)))
            {
                if (const auto file = native_open_file())
                {
                    const auto filename = std::filesystem::path(ien::get_file_name(*file))
                                              .replace_extension(project::get_asset_extension<project::shader_asset>());
                    const auto abs_path =
                        (std::filesystem::path(_project.get_assets_path<project::mesh_asset>()) / filename).string();

                    auto use_path = abs_path;
                    uint32_t retries = 0;
                    while (std::filesystem::exists(use_path))
                    {
                        use_path = std::format("{}_{}", abs_path, ++retries);
                    }

                    const engine::mesh m(*file);
                    const auto asset = std::make_shared<project::shader_asset>(&_project, use_path);
                    asset->save();
                    _project.add_asset(asset);

                    _available_shaders.push_back(asset->name());

                    CALLBACK(shader_added(asset->name()));
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
                _delete_confirm_dialog.set_label(std::format("Delete shader '{}'?", _selected));
                _delete_confirm_dialog.open();
            }
            ImGui::EndDisabled();
        }
        ImGui::End(); // shaders

        ImGui::Begin("Editor");
        {
            if (ImGui::Shortcut(ImGuiKey_LeftCtrl | ImGuiKey_S))
            {
                save_current_shader();
            }

            if (ImGui::Shortcut(ImGuiKey_LeftCtrl | ImGuiKey_LeftShift | ImGuiKey_S))
            {
                save_all_shaders();
            }

            if (!_selected.empty())
            {
                _text_editor.Render("##source", ImGui::GetContentRegionAvail());
                if (_text_editor.IsTextChanged())
                {
                    _modified_sources[_selected] = _text_editor.GetText();
                }
            }
        }
        ImGui::End(); // Editor

        _rename_dialog.tick();
        _delete_confirm_dialog.tick();
    }
} // namespace cathedral::editor