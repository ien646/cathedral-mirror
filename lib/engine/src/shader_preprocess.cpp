#include <cathedral/engine/shader_preprocess.hpp>

#include <cathedral/core.hpp>

#include <ien/str_utils.hpp>

#include <algorithm>
#include <cstdint>
#include <format>
#include <unordered_map>
#include <vector>

// This thing sucks stinky pp, but works, so eat pant

namespace cathedral::engine
{
    std::expected<std::string, std::string> preprocess_shader(std::string_view source)
    {
        std::string result = ien::str_replace(source, "#MATERIAL_TEXTURES", "layout(set = 1, binding = 1) uniform sampler2D");
        result = ien::str_replace(result, "#NODE_TEXTURES", "layout(set = 2, binding = 1) uniform sampler2D");

        return result;
    }
} // namespace cathedral::engine