#include <cathedral/engine/texture_compression.hpp>

#include <cathedral/core.hpp>

#include <ien/image/image.hpp>

#include <stb_dxt.h>

namespace cathedral::engine
{
    std::vector<uint8_t> compress_dxt1(const ien::image& img)
    {
        std::vector<uint8_t> result;
        for (size_t y = 0; y < img.height(); y += 4)
        {
            for (size_t x = 0; x < img.width(); x += 4)
            {
                std::array<uint8_t, 8> block_data;
                std::fill(block_data.begin(), block_data.end(), 0);
                const auto image_block = img.copy_rect(x, y, 4, 4);
                stb_compress_dxt_block(block_data.data(), image_block.data(), 0, STB_DXT_HIGHQUAL);
                std::copy(block_data.begin(), block_data.end(), std::back_inserter(result));
            }
        }
        return result;
    }

    std::vector<uint8_t> compress_dxt5(const ien::image& img)
    {
        std::vector<uint8_t> result;
        for (size_t y = 0; y < img.height(); y += 4)
        {
            for (size_t x = 0; x < img.width(); x += 4)
            {
                std::array<uint8_t, 16> block_data;
                std::fill(block_data.begin(), block_data.end(), 0);
                const auto image_block = img.copy_rect(x, y, 4, 4);
                stb_compress_dxt_block(block_data.data(), image_block.data(), 1, STB_DXT_HIGHQUAL);
                std::copy(block_data.begin(), block_data.end(), std::back_inserter(result));
            }
        }
        return result;
    }

    std::vector<uint8_t> create_compressed_texture_data(const std::string& image_path, texture_compression_type type)
    {
        ien::image source_image(image_path);
        return create_compressed_texture_data(source_image, type);
    }

    std::vector<uint8_t> create_compressed_texture_data(const ien::image& image, texture_compression_type type)
    {
        CRITICAL_CHECK(image.width() % 4 == 0);
        CRITICAL_CHECK(image.height() % 4 == 0);

        switch (type)
        {
        case texture_compression_type::DXT1_BC1:
            return compress_dxt1(image);
        case texture_compression_type::DXT5_BC3:
            return compress_dxt5(image);
        }
        CRITICAL_ERROR("Unhandled texture compression type");
    }
} // namespace cathedral::engine