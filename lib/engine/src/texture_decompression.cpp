#include <cathedral/engine/texture_decompression.hpp>

#include <cathedral/core.hpp>

#include <ien/arithmetic.hpp>

namespace cathedral::engine
{
    void bcdec__color_block(
        const void* __restrict__ compressed_block,
        void* __restrict__ decompressed_block,
        uint32_t image_width_bytes,
        bool only_opaque_mode)
    {
        std::array<uint32_t, 4> ref_colors; /* 0xAABBGGRR */

        const auto c0 = reinterpret_cast<const uint16_t*>(compressed_block)[0];
        const auto c1 = reinterpret_cast<const uint16_t*>(compressed_block)[1];

        /* Unpack 565 ref colors */
        const uint32_t r0 = (c0 >> 11) & 0x1F;
        const uint32_t g0 = (c0 >> 5) & 0x3F;
        const uint32_t b0 = c0 & 0x1F;

        const uint32_t r1 = (c1 >> 11) & 0x1F;
        const uint32_t g1 = (c1 >> 5) & 0x3F;
        const uint32_t b1 = c1 & 0x1F;

        /* Expand 565 ref colors to 888 */
        uint32_t r = (r0 * 527 + 23) >> 6;
        uint32_t g = (g0 * 259 + 33) >> 6;
        uint32_t b = (b0 * 527 + 23) >> 6;
        ref_colors[0] = 0xFF000000 | (b << 16) | (g << 8) | r;

        r = (r1 * 527 + 23) >> 6;
        g = (g1 * 259 + 33) >> 6;
        b = (b1 * 527 + 23) >> 6;
        ref_colors[1] = 0xFF000000 | (b << 16) | (g << 8) | r;

        if (c0 > c1 || only_opaque_mode) // BC1
        {
            r = ((2 * r0 + r1) * 351 + 61) >> 7;
            g = ((2 * g0 + g1) * 2763 + 1039) >> 11;
            b = ((2 * b0 + b1) * 351 + 61) >> 7;
            ref_colors[2] = 0xFF000000 | (b << 16) | (g << 8) | r;

            r = ((r0 + r1 * 2) * 351 + 61) >> 7;
            g = ((g0 + g1 * 2) * 2763 + 1039) >> 11;
            b = ((b0 + b1 * 2) * 351 + 61) >> 7;
            ref_colors[3] = 0xFF000000 | (b << 16) | (g << 8) | r;
        }
        else // BC1A
        {
            r = ((r0 + r1) * 1053 + 125) >> 8;
            g = ((g0 + g1) * 4145 + 1019) >> 11;
            b = ((b0 + b1) * 1053 + 125) >> 8;
            ref_colors[2] = 0xFF000000 | (b << 16) | (g << 8) | r;

            ref_colors[3] = 0x00000000;
        }

        uint32_t color_indices = reinterpret_cast<const uint32_t*>(compressed_block)[1];
        auto* dst_colors = reinterpret_cast<uint8_t*>(decompressed_block);

        for (uint8_t y = 0; y < 4; ++y)
        {
            auto* dst_colors_u32ptr = reinterpret_cast<uint32_t*>(dst_colors);
            for (uint8_t x = 0; x < 4; ++x)
            {
                const size_t idx = color_indices & 0x03;
                dst_colors_u32ptr[x] = ref_colors[idx];
                color_indices >>= 2;
            }

            dst_colors += image_width_bytes;
        }
    }

    void bcdec__smooth_alpha_block(
        const void* compressed_block,
        void* decompressed_block,
        uint32_t image_width_bytes,
        int pixel_size)
    {
        std::array<uint8_t, 8> alpha;
        unsigned long long indices;

        uint64_t block = *reinterpret_cast<const uint64_t*>(compressed_block);
        auto* decompressed = reinterpret_cast<uint8_t*>(decompressed_block);

        alpha[0] = block & 0xFF;
        alpha[1] = (block >> 8) & 0xFF;

        if (alpha[0] > alpha[1])
        {
            /* 6 interpolated alpha values. */
            alpha[2] = (6 * alpha[0] + alpha[1] + 1) / 7;     /* 6/7*alpha_0 + 1/7*alpha_1 */
            alpha[3] = (5 * alpha[0] + 2 * alpha[1] + 1) / 7; /* 5/7*alpha_0 + 2/7*alpha_1 */
            alpha[4] = (4 * alpha[0] + 3 * alpha[1] + 1) / 7; /* 4/7*alpha_0 + 3/7*alpha_1 */
            alpha[5] = (3 * alpha[0] + 4 * alpha[1] + 1) / 7; /* 3/7*alpha_0 + 4/7*alpha_1 */
            alpha[6] = (2 * alpha[0] + 5 * alpha[1] + 1) / 7; /* 2/7*alpha_0 + 5/7*alpha_1 */
            alpha[7] = (alpha[0] + 6 * alpha[1] + 1) / 7;     /* 1/7*alpha_0 + 6/7*alpha_1 */
        }
        else
        {
            /* 4 interpolated alpha values. */
            alpha[2] = (4 * alpha[0] + alpha[1] + 1) / 5;     /* 4/5*alpha_0 + 1/5*alpha_1 */
            alpha[3] = (3 * alpha[0] + 2 * alpha[1] + 1) / 5; /* 3/5*alpha_0 + 2/5*alpha_1 */
            alpha[4] = (2 * alpha[0] + 3 * alpha[1] + 1) / 5; /* 2/5*alpha_0 + 3/5*alpha_1 */
            alpha[5] = (alpha[0] + 4 * alpha[1] + 1) / 5;     /* 1/5*alpha_0 + 4/5*alpha_1 */
            alpha[6] = 0x00;
            alpha[7] = 0xFF;
        }

        indices = block >> 16;
        for (uint8_t y = 0; y < 4; ++y)
        {
            for (uint8_t x = 0; x < 4; ++x)
            {
                decompressed[x * pixel_size] = alpha[indices & 0x07];
                indices >>= 3;
            }

            decompressed += image_width_bytes;
        }
    }

    inline void bcdec_bc1(const void* compressed_block, void* decompressed_block, uint32_t image_width_bytes)
    {
        bcdec__color_block(compressed_block, decompressed_block, image_width_bytes, 0);
    }

    inline void bcdec_bc3(const void* compressed_block, void* decompressed_block, uint32_t image_width_bytes)
    {
        bcdec__color_block(reinterpret_cast<const uint8_t*>(compressed_block) + 8, decompressed_block, image_width_bytes, 1);
        bcdec__smooth_alpha_block(compressed_block, reinterpret_cast<uint8_t*>(decompressed_block) + 3, image_width_bytes, 4);
    }

    constexpr uint32_t get_texture_compression_block_size(texture_compression_type tctype)
    {
        switch (tctype)
        {
        case texture_compression_type::DXT1_BC1:
            return 8;
        case texture_compression_type::DXT5_BC3:
            return 16;
        default:
            CRITICAL_ERROR("Unhandled texture compression type");
        }
    }

    using texture_compression_func =
        void (*)(const void* compressed_data, void* uncompressed_data, uint32_t image_widt_bytes);

    constexpr texture_compression_func get_texture_compression_block_func(texture_compression_type tctype)
    {
        switch (tctype)
        {
        case texture_compression_type::DXT1_BC1:
            return &bcdec_bc1;
        case texture_compression_type::DXT5_BC3:
            return &bcdec_bc3;
        default:
            CRITICAL_ERROR("Unhandled texture compression type");
        }
    }

    namespace detail
    {
        void decompress_texture_data(
            const void* src_data,
            uint32_t image_width,
            uint32_t image_height,
            texture_compression_type type,
            void* dst_data)
        {
            const auto hblocks = image_width / 4;
            const auto* datau8 = reinterpret_cast<const uint8_t*>(src_data);
            const auto block_size = get_texture_compression_block_size(type);
            const auto decompress_func = get_texture_compression_block_func(type);

            for (size_t y = 0; y < image_height; y += 4)
            {
                const auto block_y = y / 4;
                for (size_t x = 0; x < image_width; x += 4)
                {
                    const auto block_x = x / 4;
                    const auto block_index = (block_y * hblocks) + block_x;

                    const auto* dataptr = datau8 + (block_size * block_index);

                    auto* dst = reinterpret_cast<uint8_t*>(dst_data) + ((y * image_width) + x) * 4;
                    decompress_func(dataptr, dst, image_width * 4);
                    dataptr += block_size;
                }
            }
        }
    } // namespace detail
} // namespace cathedral::engine