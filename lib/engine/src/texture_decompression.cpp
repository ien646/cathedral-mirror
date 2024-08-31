#include <cathedral/engine/texture_decompression.hpp>

#include <ien/arithmetic.hpp>

#define RGBA(r, g, b, a)                                                                                               \
    ien::construct4<                                                                                                   \
        uint32_t>(static_cast<uint8_t>(r), static_cast<uint8_t>(g), static_cast<uint8_t>(b), static_cast<uint8_t>(a))

namespace cathedral::engine
{
    void decompress_block_dxt1(uint32_t width, const void* block, void* image)
    {
        const uint16_t color0 = *reinterpret_cast<const uint16_t*>(block);
        const uint16_t color1 = *reinterpret_cast<const uint16_t*>(block + 2);

        const auto tr0 = (color0 >> 11) * 255 + 16;
        const auto r0 = static_cast<uint8_t>((tr0 / 32 + tr0) / 32);
        const auto tg0 = ((color0 & 0x07E0) >> 5) * 255 + 32;
        const auto g0 = static_cast<uint8_t>((tg0 / 64 + tg0) / 64);
        const auto tb0 = (color0 & 0x001F) * 255 + 16;
        const auto b0 = static_cast<uint8_t>((tb0 / 32 + tb0) / 32);

        const auto tr1 = (color1 >> 11) * 255 + 16;
        const auto r1 = static_cast<uint8_t>((tr1 / 32 + tr1) / 32);
        const auto tg1 = ((color1 & 0x07E0) >> 5) * 255 + 32;
        const auto g1 = static_cast<uint8_t>((tg1 / 64 + tg1) / 64);
        const auto tb1 = (color1 & 0x001F) * 255 + 16;
        const auto b1 = static_cast<uint8_t>((tb1 / 32 + tb1) / 32);

        const uint32_t code = *reinterpret_cast<const uint32_t*>(block + 4);
        for (uint8_t y = 0; y < 4; y++)
        {
            for (uint8_t x = 0; x < 4; x++)
            {
                const uint8_t position_index = (code >> 2 * ((4 * y) + x)) & 0x03;
                const uint32_t final_color = [&] -> uint32_t {
                    if (color0 > color1)
                    {
                        switch (position_index)
                        {
                        case 0:
                            return RGBA(r0, g0, b0, 255);
                        case 1:
                            return RGBA(r1, g1, b1, 255);
                        case 2:
                            return RGBA((2 * r0 + r1) / 3, (2 * g0 + g1) / 3, (2u * b0 + b1) / 3, 255);
                        case 3:
                            return RGBA((r0 + 2 * r1) / 3, (g0 + 2 * g1) / 3, (b0 + 2 * b1) / 3, 255);
                        }
                    }
                    else
                    {
                        switch (position_index)
                        {
                        case 0:
                            return RGBA(r0, g0, b0, 255);
                        case 1:
                            return RGBA(r1, g1, b1, 255);
                        case 2:
                            return RGBA((r0 + r1) / 2, (g0 + g1) / 2, (b0 + b1) / 2, 255);
                        case 3:
                            return RGBA(0, 0, 0, 255);
                        }
                    }
                    return 0u;
                }();

                if (x < width)
                {
                    reinterpret_cast<uint32_t*>(image)[(y * width) + x] = final_color;
                }
            }
        }
    }

    void decompress_block_dxt5(uint32_t width, const void* block, void* image)
    {
        const auto* blockptr = reinterpret_cast<const uint8_t*>(block);
        const uint8_t alpha0 = *blockptr;
        const uint8_t alpha1 = *(blockptr + 1);

        const uint8_t* bits = blockptr + 2;
        const uint32_t alpha_code1 = bits[2] | (bits[3] << 8) | (bits[4] << 16) | (bits[5] << 24);
        const uint16_t alpha_code2 = bits[0] | (bits[1] << 8);

        uint16_t color0 = *reinterpret_cast<const uint16_t*>(blockptr + 8);
        uint16_t color1 = *reinterpret_cast<const uint16_t*>(blockptr + 10);

        const auto tr0 = (color0 >> 11) * 255 + 16;
        const auto r0 = static_cast<uint8_t>((tr0 / 32 + tr0) / 32);
        const auto tg0 = ((color0 & 0x07E0) >> 5) * 255 + 32;
        const auto g0 = static_cast<uint8_t>((tg0 / 64 + tg0) / 64);
        const auto tb0 = (color0 & 0x001F) * 255 + 16;
        const auto b0 = static_cast<uint8_t>((tb0 / 32 + tb0) / 32);

        const auto tr1 = (color1 >> 11) * 255 + 16;
        const auto r1 = static_cast<uint8_t>((tr1 / 32 + tr1) / 32);
        const auto tg1 = ((color1 & 0x07E0) >> 5) * 255 + 32;
        const auto g1 = static_cast<uint8_t>((tg1 / 64 + tg1) / 64);
        const auto tb1 = (color1 & 0x001F) * 255 + 16;
        const auto b1 = static_cast<uint8_t>((tb1 / 32 + tb1) / 32);

        const uint32_t code = *reinterpret_cast<const uint32_t*>(block + 12);
        for (uint8_t y = 0; y < 4; y++)
        {
            for (uint8_t x = 0; x < 4; x++)
            {
                const int32_t alpha_code_index = 3 * ((4 * y) + x);

                const int32_t alpha_code = [&] -> int32_t {
                    if (alpha_code_index <= 12)
                    {
                        return (alpha_code2 >> alpha_code_index) & 0x07;
                    }
                    else if (alpha_code_index == 15)
                    {
                        return (alpha_code2 >> 15) | ((alpha_code1 << 1) & 0x06);
                    }
                    else
                    {
                        return (alpha_code1 >> (alpha_code_index - 16)) & 0x07;
                    }
                }();

                const uint8_t finalAlpha = [&] -> uint8_t {
                    if (alpha_code == 0)
                    {
                        return alpha0;
                    }
                    else if (alpha_code == 1)
                    {
                        return alpha1;
                    }
                    else
                    {
                        if (alpha0 > alpha1)
                        {
                            return ((8 - alpha_code) * alpha0 + (alpha_code - 1) * alpha1) / 7;
                        }
                        else
                        {
                            if (alpha_code == 6)
                                return 0;
                            else if (alpha_code == 7)
                                return 255;
                            else
                                return ((6 - alpha_code) * alpha0 + (alpha_code - 1) * alpha1) / 5;
                        }
                    }
                }();

                const uint8_t color_code = (code >> 2 * ((4 * y) + x)) & 0x03;
                const uint32_t final_color = [&] -> uint32_t {
                    switch (color_code)
                    {
                    case 0:
                        return RGBA(r0, g0, b0, finalAlpha);
                        break;
                    case 1:
                        return RGBA(r1, g1, b1, finalAlpha);
                        break;
                    case 2:
                        return RGBA((2 * r0 + r1) / 3, (2 * g0 + g1) / 3, (2 * b0 + b1) / 3, finalAlpha);
                        break;
                    case 3:
                        return RGBA((r0 + 2 * r1) / 3, (g0 + 2 * g1) / 3, (b0 + 2 * b1) / 3, finalAlpha);
                        break;
                    }
                    return 0;
                }();

                if (x < width)
                {
                    reinterpret_cast<uint32_t*>(image)[(y * width) + x] = final_color;
                }
            }
        }
    }

    std::vector<uint8_t> decompress_texture_data(const void* data, size_t len, texture_compression_type type)
    {
        if (type == texture_compression_type::DXT1_BC1)
        {
            std::vector<uint8_t> result(len * 8);
            for (size_t i = 0; i < len; i += 8)
            {
                decompress_block_dxt1(4, reinterpret_cast<const uint8_t*>(data) + i, result.data() + (i * 8));
            }
            return result;
        }
        if(type == texture_compression_type::DXT5_BC3)
        {
            std::vector<uint8_t> result(len * 4);
            for (size_t i = 0; i < len; i += 16)
            {
                decompress_block_dxt5(4, reinterpret_cast<const uint8_t*>(data) + i, result.data() + (i * 4));
            }
            return result;
        }
    }

} // namespace cathedral::engine