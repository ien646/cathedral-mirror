#include <cathedral/editor2/widgets/texture_widget.hpp>

#include <backends/imgui_impl_vulkan.h>

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
    }

    void texture_widget::tick()
    {
        const ImVec2 image_size = { 64 * ImGui::GetWindowDpiScale(), 64 * ImGui::GetWindowDpiScale() };

        ImGui::Image(_imgui_texture, image_size);
        ImGui::SameLine();
        ImGui::TextWrapped("%s", _texture->name().c_str());
    }
} // namespace cathedral::editor2