#pragma once

#include <cathedral/engine/texture_compression.hpp>

#include <cstdint>
#include <vector>

namespace cathedral::engine
{
    std::vector<uint8_t> decompress_texture_data(const void* data, size_t len, texture_compression_type type);
}