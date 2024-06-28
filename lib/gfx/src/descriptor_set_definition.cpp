#include <cathedral/gfx/descriptor_set_definition.hpp>

#include <unordered_set>

namespace cathedral::gfx
{
    bool descriptor_set_definition::validate() const
    {
        std::unordered_set<uint32_t> used_bindings;
        for (const auto& e : entries)
        {
            if (used_bindings.count(e.binding))
            {
                return false;
            }
            used_bindings.emplace(e.binding);
        }
        return true;
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
        }
        die("Unhandled descriptor type");
        return static_cast<vk::DescriptorType>(0);
    }
} // namespace cathedral::gfx