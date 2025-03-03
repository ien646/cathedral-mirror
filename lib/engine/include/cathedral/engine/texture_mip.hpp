#pragma once

#include <ien/image/resize_filter.hpp>

#include <cstdint>
#include <vector>

namespace ien
{
    class image;
}

namespace cathedral::engine
{
    [[nodiscard]] ien::image create_image_mip(const ien::image& source, ien::resize_filter filter);
    [[nodiscard]] std::vector<ien::image> create_image_mips(
        const ien::image& source,
        ien::resize_filter filter,
        uint32_t mip_count);
} // namespace cathedral::engine