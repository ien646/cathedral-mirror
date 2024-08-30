#pragma once

#include <cathedral/engine/texture_compression.hpp>

#include <cstdint>
#include <vector>

namespace cathedral::engine
{
    std::vector<uint8_t> decompress_texture_data(const void* data, size_t len, uint32_t image_width, uint32_t image_height, texture_compression_type type);
}