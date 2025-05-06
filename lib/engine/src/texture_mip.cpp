#include <cathedral/engine/texture_mip.hpp>

#include <cathedral/core.hpp>

#include <ien/image/image.hpp>

namespace cathedral::engine
{
    ien::image create_image_mip(const ien::image& source, const ien::resize_filter filter)
    {
        // https://registry.khronos.org/vulkan/specs/1.3-khr-extensions/html/chap12.html#resources-image-mip-level-sizing
        const uint32_t target_width = std::max<uint32_t>(static_cast<uint32_t>(source.width() / 2), 1);
        const uint32_t target_height = std::max<uint32_t>(static_cast<uint32_t>(source.height() / 2), 1);

        return source.resize(target_width, target_height, filter);
    }

    std::vector<ien::image> create_image_mips(const ien::image& source, ien::resize_filter filter, uint32_t mip_count)
    {
        std::vector<ien::image> result;
        result.reserve(mip_count);
        for (size_t i = 0; i < mip_count; ++i)
        {
            result.push_back(create_image_mip(result.empty() ? source : result.back(), filter));
        }
        return result;
    }
} // namespace cathedral::engine