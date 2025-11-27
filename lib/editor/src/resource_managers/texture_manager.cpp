#include <cathedral/editor/resource_managers/texture_manager.hpp>

#include <cathedral/editor/callback_impl.hpp>
#include <cathedral/engine/texture_mip.hpp>
#include <cathedral/sdl/event.hpp>

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

        _add_texture_dialog.callbacks.create = [this](auto&&... args) {
            handle_texture_creation(std::forward<decltype(args)>(args)...);
        };
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
                _add_texture_dialog.open();
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

        _add_texture_dialog.tick();
        _rename_dialog.tick();
        _delete_confirm_dialog.tick();
        _message_dialog.tick();
    }

    void texture_manager::handle_texture_creation(
        std::string name,
        std::string path,
        const engine::texture_format format,
        const uint8_t mip_count,
        const ien::resize_filter mipgen_filter)
    {
        CRITICAL_CHECK(std::filesystem::exists(path), std::format("Texture file path '{}' not found", path));
        auto image_info = ien::get_image_info(path);

        if (!ien::is_power_of_2(image_info->width) || !ien::is_power_of_2(image_info->height))
        {
            _message_dialog.set_text(
                "Unable to create texture from non power of two sized image\nImages used in texture creation must have "
                "power of two sizes (64, 128, 256, 512, 1024, etc.)");
            _message_dialog.set_mode(message_dialog_mode::ERROR);
            _message_dialog.set_title("Error");
            _message_dialog.open();
            return;
        }

        std::vector<std::vector<std::byte>> mips_data;
        std::vector<glm::uvec2> mip_sizes;
        {
            std::vector<ien::image> mips;
            mips.push_back(ien::image(path));

            if (mip_count > 1)
            {
                for (auto& mip : engine::create_image_mips(mips[0], mipgen_filter, mip_count - 1))
                {
                    mips.push_back(MOVE(mip));
                }
            }

            if (engine::is_compressed_format(format))
            {
                engine::texture_compression_type compression_type = [&] {
                    using enum engine::texture_format;
                    switch (format)
                    {
                    case DXT1_BC1_LINEAR:
                    case DXT1_BC1_SRGB:
                        return engine::texture_compression_type::DXT1_BC1;
                    case DXT5_BC3_LINEAR:
                    case DXT5_BC3_SRGB:
                        return engine::texture_compression_type::DXT5_BC3;
                    default:
                        CRITICAL_ERROR("Should never arrive here");
                    }
                }();

                for (const auto& mip : mips)
                {
                    mips_data.push_back(engine::create_compressed_texture_data(mip, compression_type));
                    mip_sizes.push_back(glm::uvec2(mip.width(), mip.height()));
                }
            }
            else
            {
                for (const auto& mip : mips)
                {
                    std::vector<std::byte> data;
                    data.resize(mip.size());
                    std::memcpy(data.data(), mip.data(), mip.size());
                    mips_data.push_back(MOVE(data));
                    mip_sizes.push_back(glm::uvec2(mip.width(), mip.height()));
                }
            }
        }

        auto abs_path = _project.name_to_abspath<project::texture_asset>(name);

        const auto asset = std::make_shared<project::texture_asset>(&_project, MOVE(abs_path));
        asset->set_format(format);
        asset->set_width(image_info->width);
        asset->set_height(image_info->height);
        asset->save_mips(mips_data, MOVE(mip_sizes));
        asset->save();

        _project.reload_texture_assets();

        CALLBACK(texture_added(name));
    }
} // namespace cathedral::editor