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

        if (_first_tick)
        {
            _first_tick = false;

            ImGuiID dockspace_id = ImGui::GetID("font_manager_dockspace");
            ImGuiID dock_left = ImGui::DockBuilderSplitNode(dockspace_id, ImGuiDir_Left, 0.35F, nullptr, &dockspace_id);
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

            if (ImGui::BeginListBox("##list", ImGui::GetContentRegionAvail()))
            {
                for (size_t i = 0; i < _available_font_names.size(); ++i)
                {
                    const auto& name = _available_font_names.at(i);
                    ImGui::PushID(i);
                    if (ImGui::Selectable(name.c_str(), name == _selected_font))
                    {
                        _selected_font = name;
                    }
                    ImGui::PopID();
                }
                ImGui::EndListBox();
            }
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
    }
} // namespace cathedral::editor2