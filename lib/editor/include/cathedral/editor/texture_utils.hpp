#pragma once

#include <cathedral/engine/texture.hpp>

class QImage;

namespace cathedral::editor
{
    std::vector<uint8_t> image_data_to_qrgba(const std::vector<uint8_t>& image_data, engine::texture_format format);
    const QImage& get_default_texture_qimage();
}