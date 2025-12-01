#include "backends/imgui_impl_vulkan.h"
#include "cathedral/engine/default_resources.hpp"

#include <cathedral/editor/widgets/mesh_widget.hpp>

#include <cathedral/project/project.hpp>

namespace cathedral::editor
{
    namespace
    {
        std::string get_thumbnail_texture_id(const std::string& mesh_name)
        {
            return "__$$thumbnail$$__" + mesh_name;
        }
    } // namespace

    mesh_widget::mesh_widget(std::string mesh_name, project::project& project, engine::renderer& renderer)
        : _project(project)
        , _renderer(renderer)
        , _mesh_name(MOVE(mesh_name))
    {
        init_texture();
    }

    void mesh_widget::tick()
    {
        constexpr float SPACING = 1.0F;
        const float image_height = (ImGui::CalcTextSize("|").y * 4) + (SPACING * 3);

        const auto asset = _project.get_asset_by_name<project::mesh_asset>(_mesh_name);

        ImGui::Image(_imgui_texture, ImVec2(image_height, image_height));
        ImGui::SameLine();
        ImGui::PushStyleVarY(ImGuiStyleVar_ItemSpacing, 1.0F);
        ImGui::BeginGroup();
        ImGui::Text("            Name: %s", asset->name().c_str());
        ImGui::Text("    Vertex count: %u", asset->vertex_count());
        ImGui::Text("     Index count: %u", asset->index_count());
        ImGui::EndGroup();
        ImGui::PopStyleVar();
    }

    std::pair<float, float> mesh_widget::size()
    {
        constexpr float SPACING = 1.0F;
        const float image_height = (ImGui::CalcTextSize("|").y * 4) + (SPACING * 3);
        return { image_height, image_height };
    }

    void mesh_widget::init_texture()
    {
        const auto texture_id = get_thumbnail_texture_id(_mesh_name);
        if (_renderer.textures().contains(texture_id))
        {
            _texture = _renderer.textures().at(texture_id);
        }
        else
        {
            const auto asset = _project.get_asset_by_name<project::mesh_asset>(_mesh_name);
            if (asset->has_thumbnail())
            {
                _texture = _renderer.create_color_texture(texture_id, *asset->load_thumbnail(), 1);
            }
            else
            {
                _texture = _renderer.create_color_texture(texture_id, engine::get_default_texture_image(), 1);
            }
        }

        _imgui_texture = ImGui_ImplVulkan_AddTexture(
            _texture->sampler().get_sampler(),
            _texture->imageview(),
            VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
    }
} // namespace cathedral::editor