#pragma once

#include <cstdint>
#include <vector>

namespace cathedral
{
    std::vector<uint8_t> compress_data(const void* src, size_t len);
    std::vector<uint8_t> decompress_data(const void* src, size_t len, size_t uncompressed_size);
}