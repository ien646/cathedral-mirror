#include <cathedral/editor2/resource_managers/font_manager.hpp>

#include <cathedral/engine/scene.hpp>
#include <cathedral/project/project.hpp>

#include <backends/imgui_impl_vulkan.h>
#include <imgui_internal.h>

namespace cathedral::editor2
{
    font_manager::font_manager(project::project& pro)
        : _window("Font manager", 800, 600, pro.get_settings())
        , _project(pro)
        , _filter(256, '\0')
    {
        _available_font_names.append_range(_project.get_assets<project::font_asset>() | std::views::keys);
        init_scene();

        _add_font_dialog.callbacks.create = [this](
                                                const std::string& name,
                                                const std::string& file,
                                                const std::array<int, 2> atlas_size,
                                                const int glyph_size,
                                                const int char_offset) {
            const auto font_data =
                engine::generate_font_data(file, glyph_size, glm::uvec2(atlas_size[0], atlas_size[1]), char_offset);

            const auto asset_abs_path = _project.name_to_abspath<project::font_asset>(name);
            const auto asset = std::make_shared<project::font_asset>(&_project, asset_abs_path);

            asset->mark_as_manually_loaded();
            asset->set_atlas_size({ font_data.atlas_image->width(), font_data.atlas_image->height() });
            asset->set_char_offset(font_data.char_offset);
            asset->set_glyph_boundind_box(font_data.glyph_bounding_box_size);
            asset->set_glyph_rects(std::move(font_data.glyph_infos));
            asset->save_atlas(*font_data.atlas_image);
            asset->set_kerning_table(std::move(font_data.kerning_table));
            asset->save();

            _project.reload_font_assets();

            _available_font_names.push_back(name);

            _selected_font = name;
        };

        _rename_dialog.callbacks.accepted = [this] {
            const auto name = _rename_dialog.text();
            const auto new_abspath = _project.name_to_abspath<project::font_asset>(name);

            const auto asset = _project.get_asset_by_name<project::font_asset>(_selected_font);
            asset->move_path(new_abspath);

            _project.reload_font_assets();

            _available_font_names.clear();
            _available_font_names.append_range(_project.get_assets<project::font_asset>() | std::views::keys);

            _texture_ids.erase(_selected_font);

            _selected_font = name;
        };

        _delete_confirm_dialog.callbacks.accepted = [this] {
            const auto abs_path = _project.name_to_abspath<project::font_asset>(_selected_font);
            std::filesystem::remove(abs_path);
            _project.reload_font_assets();

            _available_font_names.clear();
            _available_font_names.append_range(_project.get_assets<project::font_asset>() | std::views::keys);

            _texture_ids.erase(_selected_font);

            _selected_font = {};
        };
    }

    void font_manager::execute()
    {
        while (_window.keep_open())
        {
            _scene->tick([this]([[maybe_unused]] const double deltatime) { _window.tick([this] { tick_gui(); }); });
        }
    }

    void font_manager::init_scene()
    {
        engine::scene_args args;
        args.loaders = _project.get_loader_funcs();
        args.name = "font_manager";
        args.prenderer = &_window.renderer();

        _scene = std::make_unique<engine::scene>(std::move(args));
    }

    void font_manager::tick_gui()
    {
        const auto filter_text = "Filter";
        const auto filter_text_size = ImGui::CalcTextSize(filter_text);

        const auto vp = ImGui::GetMainViewport();

        ImGui::DockSpaceOverViewport(
            ImGui::GetID("font_manager_dockspace"),
            ImGui::GetMainViewport(),
            ImGuiDockNodeFlags_PassthruCentralNode);

        // ReSharper disable once CppDFAConstantConditions
        // ReSharper disable once CppDFAUnreachableCode
        if (_first_tick.get_and_reset())
        {
            ImGuiID dockspace_id = ImGui::GetID("font_manager_dockspace");
            const ImGuiID dock_left =
                ImGui::DockBuilderSplitNode(dockspace_id, ImGuiDir_Left, 0.35F, nullptr, &dockspace_id);
            ImGui::DockBuilderGetNode(dock_left)->LocalFlags |= ImGuiDockNodeFlags_NoTabBar |
                                                                ImGuiDockNodeFlags_NoDockingOverMe;
            ImGui::DockBuilderDockWindow("Fonts", dock_left);
            ImGui::DockBuilderFinish(dockspace_id);
        }

        float font_dock_width = 0;
        ImGui::Begin("Fonts");
        {
            font_dock_width = ImGui::GetWindowWidth();

            ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x - filter_text_size.x);
            ImGui::InputText("Filter", _filter.data(), _filter.size());

            auto listbox_size = ImGui::GetContentRegionAvail();
            listbox_size.y -=
                ImGui::CalcTextSize("|").y + (ImGui::GetStyle().FramePadding.y * 2) + ImGui::GetStyle().ItemSpacing.y;

            if (ImGui::BeginListBox("##list", listbox_size))
            {
                for (size_t i = 0; i < _available_font_names.size(); ++i)
                {
                    const auto& name = _available_font_names.at(i);
                    ImGui::PushID(static_cast<int>(i));
                    if (ImGui::Selectable(name.c_str(), name == _selected_font))
                    {
                        _selected_font = name;
                    }
                    ImGui::PopID();
                }
                ImGui::EndListBox();
            }

            const auto button_size = (ImGui::GetContentRegionAvail().x - (ImGui::GetStyle().ItemSpacing.x * 2)) / 3;

            if (ImGui::Button("New", ImVec2(button_size, 0)))
            {
                _add_font_dialog.open();
            }
            ImGui::SameLine();
            ImGui::BeginDisabled(_selected_font.empty());
            if (ImGui::Button("Rename", ImVec2(button_size, 0)))
            {
                _rename_dialog.set_text(_selected_font);
                _rename_dialog.set_validator(
                    [this](const std::string& text) -> bool { return !std::ranges::contains(_available_font_names, text); });
                _rename_dialog.open();
            }
            ImGui::SameLine();
            if (ImGui::Button("Delete", ImVec2(button_size, 0)))
            {
                _delete_confirm_dialog.set_label(std::format("Delete font '{}'?", _selected_font));
                _delete_confirm_dialog.open();
            }
            ImGui::EndDisabled();
        }
        ImGui::End();

        ImGui::SetNextWindowPos({ font_dock_width + 1, 0.0F });
        ImGui::SetNextWindowSize({ vp->Size.x - font_dock_width, vp->Size.y });
        ImGui::SetNextWindowBgAlpha(0.0F);
        ImGui::Begin(
            "Atlas",
            nullptr,
            ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar);
        {
            if (!_selected_font.empty())
            {
                const auto texture_id = "font_texture__" + _selected_font;
                auto& renderer = _scene->get_renderer();

                std::shared_ptr<engine::texture> texture;
                if (!renderer.textures().contains(texture_id))
                {
                    const auto font_asset = _project.get_asset_by_name<project::font_asset>(_selected_font);
                    const ien::image image = font_asset->load_atlas();
                    ien::image rgba_image(image.width(), image.height(), ien::image_format::RGBA);

                    for (size_t i = 0; i < image.pixel_count(); ++i)
                    {
                        std::memset(rgba_image.data() + (i * 4), image.data()[i], 4);
                    }

                    texture = renderer.create_color_texture(texture_id, rgba_image);

                    void* tex_id = ImGui_ImplVulkan_AddTexture(
                        texture->sampler().get_sampler(),
                        texture->imageview(),
                        VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

                    _texture_ids.emplace(_selected_font, tex_id);
                }
                else
                {
                    texture = renderer.textures().at(texture_id);
                }

                ImGui::ImageWithBg(_texture_ids.at(_selected_font), ImGui::GetContentRegionAvail(), { 0, 0 }, { 1, 1 });
            }
        }
        ImGui::End();

        _add_font_dialog.tick();
        _rename_dialog.tick();
        _delete_confirm_dialog.tick();
    }
} // namespace cathedral::editor2