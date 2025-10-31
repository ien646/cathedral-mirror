#include <cathedral/editor2/widgets/texture_widget.hpp>

#include <backends/imgui_impl_vulkan.h>

#include <magic_enum.hpp>

namespace cathedral::editor2
{
    texture_widget::texture_widget(std::shared_ptr<engine::texture> texture)
        : _texture(std::move(texture))
    {
        _imgui_texture = ImGui_ImplVulkan_AddTexture(
            _texture->sampler().get_sampler(),
            _texture->imageview(),
            VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
    }

    texture_widget::~texture_widget()
    {
        ImGui_ImplVulkan_RemoveTexture(static_cast<VkDescriptorSet>(_imgui_texture));
        for (const auto& tex : _trash_imgui_textures)
        {
            ImGui_ImplVulkan_RemoveTexture(static_cast<VkDescriptorSet>(tex));
        }
    }

    void texture_widget::tick()
    {
        if (_texture_changed)
        {
            _imgui_texture = ImGui_ImplVulkan_AddTexture(
                _texture->sampler().get_sampler(),
                _texture->imageview(),
                VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
            _texture_changed = false;
        }
        constexpr float SPACING = 1.0F;
        const float image_height = (ImGui::CalcTextSize("|").y * 4) + (SPACING * 3);
        const auto tex_size =
            static_cast<float>(
                engine::calc_texture_size(_texture->image().width(), _texture->image().height(), _texture->format()))
            / 1'000'000;

        ImGui::Image(_imgui_texture, ImVec2(image_height, image_height));
        ImGui::SameLine();
        ImGui::PushStyleVarY(ImGuiStyleVar_ItemSpacing, 1.0F);
        ImGui::BeginGroup();
        ImGui::Text("      Name: %s", _texture->name().c_str());
        ImGui::Text("    Format: %s", std::string{ magic_enum::enum_name(_texture->image().format()) }.c_str());
        ImGui::Text("      Size: %.3fMB", tex_size);
        ImGui::Text("Dimensions:  %u x %u", _texture->image().width(), _texture->image().height());
        ImGui::EndGroup();
        ImGui::PopStyleVar();
    }

    void texture_widget::set_texture(std::shared_ptr<engine::texture> texture)
    {
        _trash_textures.push_back(std::move(_texture));
        _trash_imgui_textures.push_back(_imgui_texture);

        _texture = std::move(texture);
        _texture_changed = true;
    }

    std::pair<float, float> texture_widget::size()
    {
        constexpr float SPACING = 1.0F;
        const float image_height = (ImGui::CalcTextSize("|").y * 4) + (SPACING * 3);
        return { image_height, image_height };
    }
} // namespace cathedral::editor2