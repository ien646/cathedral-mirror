#include <cathedral/engine/shader_preprocess.hpp>

#include <cathedral/engine/scene.hpp>

#include <cathedral/core.hpp>

#include <ien/str_utils.hpp>

#include <ranges>
#include <unordered_set>

#define FORWARD_UNEXPECTED(ex)                                                                                              \
    if (!ex.has_value())                                                                                                    \
    {                                                                                                                       \
        return std::unexpected(ex.error());                                                                                 \
    }

// This thing sucks stinky pp, but works, so eat pant

namespace cathedral::engine
{
    constexpr const char* MATERIAL_UNIFORM_TEXT = "$MATERIAL_VARIABLE";
    constexpr const char* MATERIAL_TEXTURES_TEXT = "$MATERIAL_TEXTURE";
    constexpr const char* NODE_UNIFORM_TEXT = "$NODE_VARIABLE";
    constexpr const char* NODE_TEXTURES_TEXT = "$NODE_TEXTURE";

    constexpr auto MATERIAL_SET_INDEX = 1;
    constexpr auto NODE_SET_INDEX = 2;
    constexpr auto UNIFORM_BINDING_INDEX = 0;
    constexpr auto TEXTURE_BINDING_INDEX = 1;

    constexpr const char* VERTEX_INPUTS = R"glsl(
layout (location = 0) in vec3 VERTEX_POSITION;
layout (location = 1) in vec2 VERTEX_UVCOORD;
layout (location = 2) in vec3 VERTEX_NORMAL;
layout (location = 3) in vec4 VERTEX_COLOR;
    )glsl";

    constexpr const char* SHADER_VERSION = "#version 450";

    // Parses variable declarations in the form of
    // type name (opt=array)
    std::expected<shader_variable, std::string> parse_shader_variable(std::string_view line)
    {
        auto segments_range = ien::str_splitv(line, ' ') |
                              std::views::filter([](const auto& elem) { return elem != " " && elem != ";"; });
        std::vector<std::string_view> segments(segments_range.begin(), segments_range.end());

        // Special case for sampler2D
        const auto type = gfx::shader_data_type_from_glslstr(std::string{ segments[0] });
        if (!type.has_value())
        {
            return std::unexpected("Invalid glsl data type");
        }

        auto name = std::string{ segments[1] };
        uint32_t count = 1;
        if (!line.contains("[")) // Not an array
        {
            name = ien::str_replace(name, ";", "");
        }
        else
        {
            name = ien::str_split(name, "[")[0];

            if (!line.contains(']'))
            {
                return std::unexpected("Invalid array syntax");
            }

            auto segments = ien::str_split(std::string{ line }, '[');
            if (segments.size() < 2)
            {
                return std::unexpected("Invalid array syntax");
            }

            segments = ien::str_split(segments[1], ']');

            const auto number_text = segments[0];
            if (!std::ranges::all_of(number_text, isdigit))
            {
                return std::unexpected("Invalid format for array dimension value");
            }

            count = std::stoul(number_text);
        }

        return shader_variable(*type, count, name, {});
    }

    std::expected<std::string, std::string> parse_texture_variable(std::string_view line)
    {
        if (line.contains('[') || line.contains(']'))
        {
            return std::unexpected("Texture arrays are not supported");
        }

        auto segments = ien::str_splitv(line, ' ');
        if (segments.size() > 1)
        {
            return std::unexpected("Invalid syntax for texture variable");
        }

        return std::string{ segments[0] };
    }

    std::expected<std::vector<shader_variable>, std::string> extract_shader_variables(
        inout_param<std::string> source,
        const char* tag)
    {
        std::vector<shader_variable> vars;
        std::string result_source;
        auto lines = ien::str_splitv(*source, '\n');
        for (const auto& line : lines)
        {
            auto clean_line = ien::str_trim(ien::str_trim(line), '\t');
            std::ranges::unique(clean_line);

            if (clean_line.starts_with(tag))
            {
                clean_line = clean_line.substr(strlen(tag) + 1);
                auto var = parse_shader_variable(clean_line);
                FORWARD_UNEXPECTED(var);
                vars.push_back(*var);
            }
            else
            {
                result_source += std::string{ line } + "\n";
            }
        }

        *source = std::move(result_source);

        return vars;
    }

    std::expected<std::vector<std::string>, std::string> extract_texture_variables(
        inout_param<std::string> source,
        const char* tag)
    {
        std::vector<std::string> vars;
        std::string result_source;
        auto lines = ien::str_splitv(*source, '\n');
        for (const auto& line : lines)
        {
            auto clean_line = ien::str_trim(ien::str_trim(line), '\t');
            std::ranges::unique(clean_line);
            if (clean_line.starts_with(tag))
            {
                clean_line = clean_line.substr(strlen(tag));
                auto var = parse_texture_variable(clean_line);
                FORWARD_UNEXPECTED(var);
                vars.push_back(*var);
            }
            else
            {
                result_source += std::string{ line } + "\n";
            }
        }

        *source = std::move(result_source);

        return vars;
    }

    std::string var_to_glsl(const shader_variable& var)
    {
        std::string result;
        result += gfx::shader_data_type_glslstr(var.type);
        result += " ";
        result += var.name;
        if (var.count > 1)
        {
            result += std::format("[{}]", var.count);
        }
        return result;
    }

    std::expected<std::string, std::string> generate_uniform_block(
        const std::vector<shader_variable>& vars,
        const std::string& block_name,
        int set_index,
        inout_param<std::unordered_set<std::string>> used_names)
    {
        std::string result = std::format(
            "layout (set = {}, binding = {}) uniform {} {{\n",
            set_index,
            UNIFORM_BINDING_INDEX,
            "_" + block_name + "_");

        for (const auto& var : vars)
        {
            if (used_names->contains(var.name))
            {
                return std::unexpected(std::format("Duplicated variable name '{}'", var.name));
            }
            used_names->emplace(var.name);
            result += "    " + var_to_glsl(var);
            result += ";\n";
        }

        result += "}}\n";

        for (const auto& var : vars)
        {
            result += std::format("#define {} {};\n", var.name, std::format("{}.{}", block_name, var.name));
        }

        return result;
    }

    std::expected<std::string, std::string> generate_texture_block(
        const std::vector<std::string>& texture_names,
        const std::string& block_name,
        int set_index,
        inout_param<std::unordered_set<std::string>> used_names)
    {
        if (texture_names.empty())
        {
            return {};
        }

        std::string result = std::format(
            "layout (set = {}, binding = {}) uniform sampler2D {}[{}];\n",
            set_index,
            TEXTURE_BINDING_INDEX,
            block_name,
            texture_names.size());

        for (const auto& name : texture_names)
        {
            if (used_names->contains(name))
            {
                return std::unexpected(name);
            }
            used_names->emplace(name);
            result += std::format("#define {} {};\n", name, std::format("{}.{}", block_name, name));
        }

        return result;
    }

    std::expected<shader_preprocess_result, std::string> preprocess_shader(gfx::shader_type type, std::string_view source)
    {
        std::string source_copy = std::string{ source };

        const auto mat_vars = extract_shader_variables(inout_param{ source_copy }, MATERIAL_UNIFORM_TEXT);
        FORWARD_UNEXPECTED(mat_vars);

        const auto node_vars = extract_shader_variables(inout_param{ source_copy }, NODE_UNIFORM_TEXT);
        FORWARD_UNEXPECTED(node_vars);

        const auto mat_textures = extract_texture_variables(inout_param{ source_copy }, MATERIAL_TEXTURES_TEXT);
        FORWARD_UNEXPECTED(mat_textures);

        const auto node_textures = extract_texture_variables(inout_param{ source_copy }, NODE_TEXTURES_TEXT);
        FORWARD_UNEXPECTED(node_textures);

        std::unordered_set<std::string> used_names;

        const auto mat_uniform_block =
            generate_uniform_block(*mat_vars, "_cathedral_material_uniform_", MATERIAL_SET_INDEX, inout_param{ used_names });
        FORWARD_UNEXPECTED(mat_uniform_block);

        const auto node_uniform_block =
            generate_uniform_block(*node_vars, "_cathedral_node_uniform_", NODE_SET_INDEX, inout_param{ used_names });
        FORWARD_UNEXPECTED(node_uniform_block);

        const auto material_texture_block = generate_texture_block(
            *mat_textures,
            "_cathedral_material_textures_",
            MATERIAL_SET_INDEX,
            inout_param{ used_names });
        FORWARD_UNEXPECTED(material_texture_block);

        const auto node_texture_block =
            generate_texture_block(*node_textures, "_cathedral_node_textures_", NODE_SET_INDEX, inout_param{ used_names });
        FORWARD_UNEXPECTED(node_texture_block);

        std::string result_source;
        result_source += std::string{ SHADER_VERSION } + '\n';

        if (type == gfx::shader_type::VERTEX)
        {
            result_source += std::string{ VERTEX_INPUTS } + '\n';
        }

        result_source += SCENE_UNIFORM_GLSLSTR + "\n";

        result_source += *mat_uniform_block + "\n";
        result_source += *material_texture_block + "\n";

        result_source += *node_uniform_block + "\n";
        result_source += *node_texture_block + "\n";

        result_source += source_copy;

        shader_preprocess_result result;
        result.source = std::move(result_source);
        result.material_vars = *mat_vars;
        result.node_vars = *node_vars;
        result.material_textures = *mat_textures;
        result.node_textures = *node_textures;

        return result;
    }
} // namespace cathedral::engine