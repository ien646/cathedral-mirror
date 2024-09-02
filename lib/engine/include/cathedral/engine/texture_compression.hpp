#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace ien
{
    class image;
}

namespace cathedral::engine
{
    enum class texture_compression_type
    {
        DXT1_BC1,
        DXT5_BC3
    };

    [[nodiscard]] std::vector<std::byte> create_compressed_texture_data(
        const std::string& image_path,
        texture_compression_type type);
    [[nodiscard]] std::vector<std::byte> create_compressed_texture_data(const ien::image& image, texture_compression_type type);
} // namespace cathedral::engine
