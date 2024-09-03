#pragma once

#include <cathedral/engine/texture.hpp>

#include <ien/alloc.hpp>

class QImage;

namespace cathedral::editor
{
    std::vector<std::byte, ien::aligned_allocator<std::byte, 4>> image_data_to_qrgba(
        std::span<const std::byte> image_data,
        engine::texture_format format);

    const QImage& get_default_texture_qimage();

    QImage mip_to_qimage(std::span<const std::byte> data, uint32_t width, uint32_t height, engine::texture_format format);
} // namespace cathedral::editor