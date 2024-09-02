#pragma once

#include <cathedral/engine/texture_compression.hpp>

#include <cstdint>
#include <vector>

namespace cathedral::engine
{
    namespace detail
    {
        void decompress_texture_data(
            const std::byte* src_data,
            uint32_t image_width,
            uint32_t image_height,
            texture_compression_type type,
            std::byte* dst_data);
    }

    template <typename TVectorAllocator = std::vector<std::byte>::allocator_type>
    std::vector<std::byte, TVectorAllocator> decompress_texture_data(
        const std::byte* data,
        uint32_t image_width,
        uint32_t image_height,
        texture_compression_type type)
    {
        std::vector<std::byte, TVectorAllocator> result(image_width * image_height * 4);
        detail::decompress_texture_data(data, image_width, image_height, type, result.data());
        return result;
    }
} // namespace cathedral::engine