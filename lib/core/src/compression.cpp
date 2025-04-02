#include <cathedral/compression.hpp>

#include <cathedral/core.hpp>

#include <lz4.h>

namespace cathedral
{
    std::vector<std::byte> compress_data(std::span<const std::byte> data)
    {
        std::vector<std::byte> result(LZ4_compressBound(static_cast<int>(data.size())));

        const auto compressed_size = LZ4_compress_default(
            reinterpret_cast<const char*>(data.data()),
            reinterpret_cast<char*>(result.data()),
            static_cast<int>(data.size()),
            static_cast<int>(result.size()));

        CRITICAL_CHECK(compressed_size > 0, "LZ4 compression failure");

        result.resize(compressed_size);
        result.shrink_to_fit();

        return result;
    }

    std::vector<std::byte> decompress_data(std::span<const std::byte> data, size_t uncompressed_size)
    {
        std::vector<std::byte> result(uncompressed_size);

        const auto decompressed_size = LZ4_decompress_safe(
            reinterpret_cast<const char*>(data.data()),
            reinterpret_cast<char*>(result.data()),
            static_cast<int>(data.size()),
            static_cast<int>(result.size()));

        CRITICAL_CHECK(decompressed_size > 0, "LZ4 decompression failure");
        CRITICAL_CHECK(uncompressed_size == static_cast<size_t>(decompressed_size), "LZ4 returned unexpected decompressed size");

        return result;
    }
} // namespace cathedral