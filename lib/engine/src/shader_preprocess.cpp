#include <cathedral/engine/shader_preprocess.hpp>

#include <cathedral/core.hpp>

#include <ien/str_utils.hpp>

#include <unordered_map>

// This thing sucks stinky pp, but works, so eat pant

namespace cathedral::engine
{
    constexpr const char* SCENE_UNIFORM_TEXT = "$SCENE_UNIFORM";
    constexpr const char* MATERIAL_UNIFORM_TEXT = "$MATERIAL_UNIFORM";
    constexpr const char* MATERIAL_TEXTURES_TEXT = "$MATERIAL_TEXTURES";
    constexpr const char* NODE_UNIFORM_TEXT = "$NODE_UNIFORM";
    constexpr const char* NODE_TEXTURES_TEXT = "$NODE_TEXTURES";

    constexpr const char* VERTEX_INPUTS = R"glsl(
layout (location = 0) in vec3 VERTEX_POSITION;
layout (location = 1) in vec2 VERTEX_UVCOORD;
layout (location = 2) in vec3 VERTEX_NORMAL;
layout (location = 3) in vec4 VERTEX_COLOR;
    )glsl";

    constexpr const char* SHADER_VERSION = "#version 450";

    const std::unordered_map<const char*, const char*> replacements = {
        { SCENE_UNIFORM_TEXT, "layout (set = 0, binding = 0) uniform _scene_uniform_" },
        { MATERIAL_UNIFORM_TEXT, "layout (set = 1, binding = 0) uniform _material_uniform_" },
        { MATERIAL_TEXTURES_TEXT, "layout (set = 1, binding = 1) uniform sampler2D" },
        { NODE_UNIFORM_TEXT, "layout (set = 2, binding = 0) uniform _node_uniform_" },
        { NODE_TEXTURES_TEXT, "layout (set = 2, binding = 1) uniform sampler2D" },
    };

    std::string preprocess_shader(gfx::shader_type type, std::string_view source, bool skip_replace)
    {
        std::string result;
        result += std::string{ SHADER_VERSION } + '\n';

        if (type == gfx::shader_type::VERTEX)
        {
            result += std::string{ VERTEX_INPUTS } + '\n';
        }

        result += std::string{ source };

        if (!skip_replace)
        {
            for (const auto& [key, value] : replacements)
            {
                result = ien::str_replace(result, key, value);
            }
        }

        return result;
    }
} // namespace cathedral::engine