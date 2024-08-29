#include <cathedral/compression.hpp>

#include <cathedral/core.hpp>

#include <lz4.h>

namespace cathedral
{
    std::vector<uint8_t> compress_data(const void* src, size_t len)
    {
        std::vector<uint8_t> result(LZ4_compressBound(len));

        const auto compressed_size = LZ4_compress_default(
            reinterpret_cast<const char*>(src),
            reinterpret_cast<char*>(result.data()),
            len,
            result.size());

        CRITICAL_CHECK(compressed_size > 0);

        result.resize(compressed_size);
        result.shrink_to_fit();

        return result;
    }

    std::vector<uint8_t> decompress_data(const void* src, size_t len, size_t uncompressed_size)
    {
        std::vector<uint8_t> result(uncompressed_size);

        const auto decompressed_size = LZ4_decompress_safe(
            reinterpret_cast<const char*>(src),
            reinterpret_cast<char*>(result.data()),
            len,
            result.size());

        CRITICAL_CHECK(decompressed_size > 0);
        CRITICAL_CHECK(uncompressed_size == static_cast<size_t>(decompressed_size));

        return result;
    }
} // namespace cathedral