#include "cathedral/sdl/event.hpp"

#include <cathedral/editor/resource_managers/texture_manager.hpp>

#include <backends/imgui_impl_vulkan.h>
#include <imgui.h>
#include <imgui_internal.h>

namespace cathedral::editor
{
    texture_manager::texture_manager(project::project& pro, editor_settings_interface& editor_settings)
        : resource_manager_base(pro)
        , _editor_settings(editor_settings)
    {
        _window.set_title("Texture manager");

        _available_texture_names = _project.texture_assets() | std::views::keys | std::ranges::to<std::vector>();
        std::ranges::sort(_available_texture_names);

        _filtered_texture_names = _available_texture_names
                                  | std::views::transform([](const std::string& name) { return &name; })
                                  | std::ranges::to<std::vector>();
    }

    void texture_manager::tick()
    {
        if (_window.keep_open())
        {
            sdl::global_poll_events();
            _scene->tick([this]([[maybe_unused]] const double deltatime) { _window.tick([this] { tick_gui(); }); });
        }
    }

    void texture_manager::tick_gui()
    {
        auto dockspace_id = ImGui::DockSpaceOverViewport(
            ImGui::GetID("texture_manager_dockspace"),
            ImGui::GetMainViewport(),
            ImGuiDockNodeFlags_PassthruCentralNode);

        if (!_window.editor_settings()->get(editor_setting::TEXTURE_MANAGER_SETUP_COMPLETE).as_bool())
        {
            const ImGuiID dock_left = ImGui::DockBuilderSplitNode(dockspace_id, ImGuiDir_Left, 0.35F, nullptr, &dockspace_id);
            ImGui::DockBuilderGetNode(dock_left)->LocalFlags |= ImGuiDockNodeFlags_NoTabBar
                                                                | ImGuiDockNodeFlags_NoDockingOverMe;
            ImGui::DockBuilderDockWindow("Textures", dock_left);

            const ImGuiID dock_bottom =
                ImGui::DockBuilderSplitNode(dockspace_id, ImGuiDir_Down, 0.15F, nullptr, &dockspace_id);
            ImGui::DockBuilderDockWindow("Properties", dock_bottom);

            auto* central_node = ImGui::DockBuilderGetCentralNode(dockspace_id);
            central_node->LocalFlags |= ImGuiDockNodeFlags_NoTabBar;
            ImGui::DockBuilderDockWindow("Display", central_node->ID);

            ImGui::DockBuilderFinish(dockspace_id);

            _window.editor_settings()->set(editor_setting::TEXTURE_MANAGER_SETUP_COMPLETE, true);
            _project.save_settings();
        }

        ImGui::Begin("Textures");
        {
            _resource_filter.tick(_available_texture_names, _filtered_texture_names);

            auto listbox_size = ImGui::GetContentRegionAvail();
            listbox_size.y -= ImGui::CalcTextSize("|").y
                              + (ImGui::GetStyle().FramePadding.y * 2)
                              + ImGui::GetStyle().ItemSpacing.y;

            if (ImGui::BeginListBox("##list", listbox_size))
            {
                for (size_t i = 0; i < _filtered_texture_names.size(); ++i)
                {
                    const auto& name = *_filtered_texture_names.at(i);
                    ImGui::PushID(static_cast<int>(i));
                    if (ImGui::Selectable(name.c_str(), name == _selected_texture))
                    {
                        _selected_texture = name;
                        _texture_changed = true;
                    }
                    ImGui::PopID();
                }
                ImGui::EndListBox();
            }

            const auto button_size = (ImGui::GetContentRegionAvail().x - (ImGui::GetStyle().ItemSpacing.x * 2)) / 3;

            if (ImGui::Button("New", ImVec2(button_size, 0)))
            {
                NOT_IMPLEMENTED();
            }
            ImGui::SameLine();
            ImGui::BeginDisabled(_selected_texture.empty());
            if (ImGui::Button("Rename", ImVec2(button_size, 0)))
            {
                _rename_dialog.set_text(_selected_texture);
                _rename_dialog.set_validator([this](const std::string& text) -> bool {
                    return !std::ranges::contains(_available_texture_names, text);
                });
                _rename_dialog.open();
            }
            ImGui::SameLine();
            if (ImGui::Button("Delete", ImVec2(button_size, 0)))
            {
                _delete_confirm_dialog.set_label(std::format("Delete texture '{}'?", _selected_texture));
                _delete_confirm_dialog.open();
            }
            ImGui::EndDisabled();
        }
        ImGui::End(); // Textures

        ImGui::Begin("Properties");
        {
            if (!_selected_texture.empty())
            {
                const auto asset = _project.get_asset_by_name<project::texture_asset>(_selected_texture);
                const auto texture_mb = static_cast<float>(asset->texture_size_bytes()) / 1'000'000;
                ImGui::Text("      Name: %s", asset->name().c_str());
                ImGui::Text("Dimensions: %u x %u", asset->width(), asset->height());
                ImGui::Text("      Size: %.1fMB", texture_mb);
                ImGui::Text("    Format: %s", std::string{ magic_enum::enum_name(asset->format()) }.c_str());
            }
        }
        ImGui::End(); // Properties

        ImGui::Begin("Display");
        {
            if (!_selected_texture.empty())
            {
                if (_texture_changed)
                {
                    _texture = _scene->load_texture(_selected_texture);

                    _imgui_texture = ImGui_ImplVulkan_AddTexture(
                        _texture->sampler().get_sampler(),
                        _texture->imageview(),
                        VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

                    _texture_changed = false;
                }

                const float aspect_ratio = static_cast<float>(_texture->image().width()) / _texture->image().height();
                ImVec2 size;
                if (ImGui::GetContentRegionAvail().x / aspect_ratio > ImGui::GetContentRegionAvail().y)
                {
                    size = { ImGui::GetContentRegionAvail().y, ImGui::GetContentRegionAvail().y * aspect_ratio };
                }
                else
                {
                    size = { ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().x * aspect_ratio };
                }

                ImVec2 position((ImGui::GetWindowSize().x - size.x) * 0.5F, (ImGui::GetWindowSize().y - size.y) * 0.5f);

                ImGui::SetCursorPos(position);
                ImGui::ImageWithBg(_imgui_texture, size, { 0, 0 }, { 1, 1 }, ImVec4(0, 0, 0, 1));
            }
        }
        ImGui::End(); // Displays

        if (ImGui::BeginMainMenuBar())
        {
            if (ImGui::BeginMenu("Window"))
            {
                if (ImGui::MenuItem("Reset layout"))
                {
                    _editor_settings.set(editor_setting::TEXTURE_MANAGER_SETUP_COMPLETE, false);
                    _project.save_settings();
                }
                ImGui::EndMenu();
            }
            ImGui::EndMainMenuBar();
        }
    }
} // namespace cathedral::editor