#include <cathedral/engine/texture_compression.hpp>

#include <cathedral/core.hpp>

#include <ien/arithmetic.hpp>
#include <ien/image/image.hpp>

#include <stb_dxt.h>

#include <algorithm>

namespace cathedral::engine
{
    std::vector<std::byte> compress_dxt1(const ien::image& img)
    {
        std::vector<std::byte> result;
        for (size_t y = 0; y < img.height(); y += 4)
        {
            for (size_t x = 0; x < img.width(); x += 4)
            {
                std::array<std::byte, 8> block_data;
                std::ranges::fill(block_data, static_cast<std::byte>(0));
                const auto image_block = img.copy_rect(x, y, 4, 4);
                stb_compress_dxt_block(
                    reinterpret_cast<unsigned char*>(block_data.data()),
                    image_block.data(),
                    0,
                    STB_DXT_HIGHQUAL);
                std::ranges::copy(block_data, std::back_inserter(result));
            }
        }
        return result;
    }

    std::vector<std::byte> compress_dxt5(const ien::image& img)
    {
        std::vector<std::byte> result;
        for (size_t y = 0; y < img.height(); y += 4)
        {
            for (size_t x = 0; x < img.width(); x += 4)
            {
                std::array<std::byte, 16> block_data;
                std::ranges::fill(block_data, static_cast<std::byte>(0));
                const auto image_block = img.copy_rect(x, y, 4, 4);
                stb_compress_dxt_block(
                    reinterpret_cast<unsigned char*>(block_data.data()),
                    image_block.data(),
                    1,
                    STB_DXT_HIGHQUAL);
                std::ranges::copy(block_data, std::back_inserter(result));
            }
        }
        return result;
    }

    std::vector<std::byte> create_compressed_texture_data(const std::string& image_path, texture_compression_type type)
    {
        ien::image source_image(image_path);
        return create_compressed_texture_data(source_image, type);
    }

    std::vector<std::byte> create_compressed_texture_data(const ien::image& image, texture_compression_type type)
    {
        CRITICAL_CHECK(ien::is_power_of_2(image.width()));
        CRITICAL_CHECK(ien::is_power_of_2(image.height()));

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