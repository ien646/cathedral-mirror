#pragma once

#include <cathedral/core.hpp>
#include <cathedral/engine/atlas.hpp>

#include <ien/image/image.hpp>

#include <glm/vec2.hpp>

#include <memory>
#include <string>

namespace cathedral::engine
{
    struct atlas_image_args
    {
        glm::ivec2 atlas_size = { 1024, 1024 };
        int char_box_size = 64;
        int char_gen_offset = 0;
    };

    class font
    {
    public:
        explicit font(std::string font_path);

        std::pair<std::unique_ptr<ien::image>, atlas_args> generate_image_and_atlas_args(const atlas_image_args& args) const;

    private:
        std::string _font_path;
    };
} // namespace cathedral::engine