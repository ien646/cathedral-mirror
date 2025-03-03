#include <cathedral/gfx/descriptor_set_definition.hpp>

#include <cathedral/gfx/vulkan_context.hpp>

#include <unordered_set>

namespace cathedral::gfx
{
    bool descriptor_set_definition::validate() const
    {
        std::unordered_set<uint32_t> used_bindings;
        for (const auto& e : entries)
        {
            if (used_bindings.contains(e.binding))
            {
                return false;
            }
            used_bindings.emplace(e.binding);
        }
        return true;
    }

    vk::UniqueDescriptorSetLayout descriptor_set_definition::create_descriptor_set_layout(const vulkan_context& vkctx) const
    {
        std::vector<vk::DescriptorSetLayoutBinding> bindings;
        for (const auto& entry : this->entries)
        {
            vk::DescriptorSetLayoutBinding binding;
            binding.binding = entry.binding;
            binding.descriptorCount = 1;
            binding.descriptorType = gfx::to_vk_descriptor_type(entry.type);
            binding.stageFlags = vk::ShaderStageFlagBits::eVertex | vk::ShaderStageFlagBits::eFragment;
            bindings.push_back(binding);
        }

        vk::DescriptorSetLayoutCreateInfo dset_layout_info;
        dset_layout_info.bindingCount = static_cast<uint32_t>(bindings.size());
        dset_layout_info.pBindings = bindings.data();

        return vkctx.device().createDescriptorSetLayoutUnique(dset_layout_info);
    }

    vk::DescriptorType to_vk_descriptor_type(cathedral::gfx::descriptor_type type)
    {
        switch (type)
        {
        case cathedral::gfx::descriptor_type::SAMPLER:
            return vk::DescriptorType::eCombinedImageSampler;
        case cathedral::gfx::descriptor_type::STORAGE:
            return vk::DescriptorType::eStorageBuffer;
        case cathedral::gfx::descriptor_type::UNIFORM:
            return vk::DescriptorType::eUniformBuffer;
        default:
            CRITICAL_ERROR("Unhandled descriptor type");
        }
    }
} // namespace cathedral::gfx