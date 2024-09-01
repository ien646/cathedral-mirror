#pragma once

#include <span>
#include <vector>

namespace cathedral
{
    std::vector<std::byte> compress_data(std::span<const std::byte>);
    std::vector<std::byte> decompress_data(std::span<const std::byte>, size_t uncompressed_size);
} // namespace cathedral