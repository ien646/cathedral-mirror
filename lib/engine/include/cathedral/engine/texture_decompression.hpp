#pragma once

#include <cathedral/engine/texture_compression.hpp>

#include <cstdint>
#include <vector>

namespace cathedral::engine
{
    namespace detail
    {
        void decompress_texture_data(
            const void* src_data,
            uint32_t image_width,
            uint32_t image_height,
            texture_compression_type type,
            void* dst_data);
    }

    template <typename TVectorAllocator = std::vector<uint8_t>::allocator_type>
    std::vector<uint8_t, TVectorAllocator> decompress_texture_data(
        const void* data,
        uint32_t image_width,
        uint32_t image_height,
        texture_compression_type type)
    {
        std::vector<uint8_t, TVectorAllocator> result(image_width * image_height * 4);
        detail::decompress_texture_data(data, image_width, image_height, type, result.data());
        return result;
    }
} // namespace cathedral::engine