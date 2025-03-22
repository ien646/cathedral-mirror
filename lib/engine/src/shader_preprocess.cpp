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

    const std::unordered_map<const char*, const char*> replacements = {
        { SCENE_UNIFORM_TEXT, "layout (set = 0, binding = 0) uniform struct __scene_uniform__" },
        { MATERIAL_UNIFORM_TEXT, "layout (set = 1, binding = 0) uniform struct __material_uniform__" },
        { MATERIAL_TEXTURES_TEXT, "layout (set = 1, binding = 1) uniform sampler2D" },
        { NODE_UNIFORM_TEXT, "layout (set = 2, binding = 0) uniform struct __node_uniform__" },
        { NODE_TEXTURES_TEXT, "layout (set = 2, binding = 1) uniform sampler2D" },
    };

    std::string preprocess_shader(std::string_view source)
    {
        std::string result = std::string{ source };
        for (const auto& [key, value] : replacements)
        {
            result = ien::str_replace(result, key, value);
        }

        return result;
    }
} // namespace cathedral::engine