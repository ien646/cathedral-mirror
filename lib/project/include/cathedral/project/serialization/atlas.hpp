#pragma once

#include <cereal/cereal.hpp>

#include <cathedral/engine/atlas.hpp>

namespace cereal
{
    template <typename Archive>
    void CEREAL_SERIALIZE_FUNCTION_NAME(Archive& ar, cathedral::engine::atlas& atlas)
    {
        ar(make_nvp("texture_name", atlas.texture_name()),
           make_nvp("glyph_bounding_box", atlas.glyph_bounding_box()),
           make_nvp("glyph_rects", atlas.glyph_rects()));
    }
} // namespace cereal