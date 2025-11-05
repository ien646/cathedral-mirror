#include <cathedral/engine/shader_preprocess.hpp>

#include <cathedral/engine/scene.hpp>

#include <cathedral/core.hpp>

#include <ien/str_utils.hpp>

#include <ranges>
#include <unordered_set>

#define FORWARD_UNEXPECTED(ex)                                                                                              \
    if (!(ex).has_value())                                                                                                  \
    {                                                                                                                       \
        return std::unexpected((ex).error());                                                                               \
    }

namespace cathedral::engine
{
    constexpr auto MATERIAL_UNIFORM_TEXT = "$MATERIAL_VARIABLE";
    constexpr auto MATERIAL_BUFFER_TEXT = "$MATERIAL_BUFFER";
    constexpr auto MATERIAL_TEXTURES_TEXT = "$MATERIAL_TEXTURE";
    constexpr auto NODE_UNIFORM_TEXT = "$NODE_VARIABLE";
    constexpr auto NODE_BUFFER_TEXT = "$NODE_BUFFER";
    constexpr auto NODE_TEXTURES_TEXT = "$NODE_TEXTURE";

    constexpr auto MATERIAL_SET_INDEX = 1;
    constexpr auto NODE_SET_INDEX = 2;
    constexpr auto UNIFORM_BINDING_INDEX = 0;
    constexpr auto TEXTURE_BINDING_INDEX = 1;
    constexpr auto BUFFERS_STARTING_BINDING_INDEX = 2;

    constexpr auto VERTEX_INPUTS = R"glsl(
layout (location = 0) in vec3 VERTEX_POSITION;
layout (location = 1) in vec2 VERTEX_UVCOORD;
layout (location = 2) in vec3 VERTEX_NORMAL;
layout (location = 3) in vec4 VERTEX_COLOR;
)glsl";

    constexpr auto SHADER_VERSION = "#version 450";

    namespace
    {
        bool is_valid_variable_name(std::string_view name)
        {
            if (name.empty())
            {
                return false;
            }

            // Variable names must start with either letters or underscores
            // abcd0123 -> OK
            // _abc1232 -> OK
            // 123abcde -> INVALID
            if ((!std::isalpha(name[0], std::locale::classic())) && (name[0] != '_'))
            {
                return false;
            }

            // Valid characters are alphanumerics and underscores
            const auto& loc = std::locale::classic();
            return std::ranges::all_of(name, [&loc](const char ch) {
                return std::isalpha(ch, loc) || std::isdigit(ch, loc) || ch == '_';
            });
        }

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
                return std::unexpected(std::format("Invalid glsl data type '{}'", segments[0]));
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
                    return std::unexpected(std::format("Invalid array syntax '{}'", line));
                }

                auto array_segments = ien::str_split(std::string{ line }, '[');
                if (array_segments.size() < 2)
                {
                    return std::unexpected(std::format("Invalid array syntax '{}'", line));
                }

                array_segments = ien::str_split(array_segments[1], ']');

                const auto number_text = array_segments[0];
                if (!std::ranges::all_of(number_text, isdigit))
                {
                    return std::unexpected(std::format("Invalid format for array dimension value '{}'", line));
                }

                count = std::stoul(number_text);
            }

            if (!is_valid_variable_name(name))
            {
                return std::unexpected(std::format("Invalid name '{}'", name));
            }

            return shader_variable(*type, count, name);
        }

        std::expected<std::string, std::string> parse_texture_variable(std::string_view line)
        {
            if (line.contains('[') || line.contains(']'))
            {
                return std::unexpected(std::format("Texture arrays are not supported '{}'", line));
            }

            const auto segments = ien::str_splitv(line, ' ');
            if (segments.size() > 1)
            {
                return std::unexpected(std::format("Invalid syntax for texture variable '{}'", line));
            }

            auto name = ien::str_replace(std::string{ segments[0] }, ';', "");

            if (!is_valid_variable_name(name))
            {
                return std::unexpected(std::format("Invalid texture name '{}'", name));
            }

            return name;
        }

        std::expected<shader_preprocess_buffer_variable, std::string> parse_buffer_variable(std::string_view line)
        {
            const auto invalid_syntax = [&line] {
                return std::unexpected(std::format("Invalid syntax for buffer variable '{}'", line));
            };

            if (!line.contains('{') || !line.contains('}'))
            {
                return std::unexpected(std::format("Missing buffer structure block declaration", line));
            }

            const auto name_buffer_segments = ien::str_splitv(line, '{');
            if (name_buffer_segments.size() != 2)
            {
                return invalid_syntax();
            }

            auto name_segments = ien::str_split(ien::str_trim(name_buffer_segments[0]), ' ');
            if (name_segments.size() > 1)
            {
                return invalid_syntax();
            }

            auto name = name_segments[0];

            if (!is_valid_variable_name(name))
            {
                return std::unexpected(std::format("Invalid buffer name '{}'", name));
            }

            auto block_decl = std::string{ name_buffer_segments[1] };
            block_decl = ien::str_trim(block_decl);
            if (!block_decl.ends_with(';'))
            {
                return invalid_syntax();
            }

            block_decl = block_decl.substr(0, block_decl.size() - 1);
            block_decl = ien::str_trim(block_decl);
            if (!block_decl.ends_with('}'))
            {
                return invalid_syntax();
            }
            block_decl = block_decl.substr(0, block_decl.size() - 1);

            return shader_preprocess_buffer_variable{ .name = std::string{ name }, .decl_block = block_decl };
        }

        std::expected<std::vector<shader_variable>, std::string> extract_shader_variables(
            inout_param<std::string> source,
            const char* tag)
        {
            std::vector<shader_variable> vars;
            std::string result_source;
            for (const auto lines = ien::str_splitv(*source, '\n'); const auto& line : lines)
            {
                auto clean_line = ien::str_trim(ien::str_trim(line), '\t');
                auto erased_range =
                    std::ranges::unique(clean_line, [](const char lhs, const char rhs) { return lhs == ' ' && rhs == ' '; });
                clean_line.erase(erased_range.begin(), erased_range.end());

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

            *source = MOVE(result_source);

            return vars;
        }

        std::expected<std::vector<std::string>, std::string> extract_texture_variables(
            inout_param<std::string> source,
            const char* tag)
        {
            std::vector<std::string> vars;
            std::string result_source;
            for (const auto lines = ien::str_splitv(*source, '\n'); const auto& line : lines)
            {
                auto clean_line = ien::str_trim(ien::str_trim(line), '\t');
                auto erased_range =
                    std::ranges::unique(clean_line, [](const char lhs, const char rhs) { return lhs == ' ' && rhs == ' '; });
                clean_line.erase(erased_range.begin(), erased_range.end());

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

            *source = MOVE(result_source);

            return vars;
        }

        std::expected<std::vector<shader_preprocess_buffer_variable>, std::string> extract_buffer_variables(
            inout_param<std::string> source,
            const char* tag)
        {
            std::vector<shader_preprocess_buffer_variable> vars;
            std::string result_source;
            for (const auto lines = ien::str_splitv(*source, '\n'); const auto& line : lines)
            {
                auto clean_line = ien::str_trim(ien::str_trim(line), '\t');
                auto erased_range =
                    std::ranges::unique(clean_line, [](const char lhs, const char rhs) { return lhs == ' ' && rhs == ' '; });
                clean_line.erase(erased_range.begin(), erased_range.end());

                if (clean_line.starts_with(tag))
                {
                    clean_line = clean_line.substr(strlen(tag));
                    auto var = parse_buffer_variable(clean_line);
                    FORWARD_UNEXPECTED(var);
                    vars.push_back(*var);
                }
                else
                {
                    result_source += std::string{ line } + "\n";
                }
            }

            *source = MOVE(result_source);

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
            if (vars.empty())
            {
                return "";
            }

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

            result += std::format("}} {};\n", block_name);

            for (const auto& var : vars)
            {
                result += std::format("#define {} {}\n", var.name, std::format("{}.{}", block_name, var.name));
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

            for (size_t i = 0; i < texture_names.size(); ++i)
            {
                const auto& name = texture_names[i];
                if (used_names->contains(name))
                {
                    return std::unexpected(name);
                }
                used_names->emplace(name);
                result += std::format("#define {} {}[{}]\n", name, block_name, i);
            }

            return result;
        }

        std::expected<std::string, std::string> generate_buffer_block(
            const std::vector<shader_preprocess_buffer_variable>& vars,
            const int set_index,
            const int starting_binding_index,
            inout_param<std::unordered_set<std::string>> used_names)
        {
            if (vars.empty())
            {
                return {};
            }

            std::string result;

            uint32_t binding_index = starting_binding_index;
            for (const auto& var : vars)
            {
                if (var.name.empty() || var.decl_block.empty())
                {
                    return std::unexpected("Invalid buffer syntax");
                }

                const auto real_block_name = "_cathedral_buffer_" + var.name + "_";
                const auto block_var_name = "cathedral_buffer_" + var.name + "";

                result += std::format(
                    "layout (set = {}, binding = {}) readonly buffer {} {{ {} }} {};\n",
                    set_index,
                    binding_index++,
                    real_block_name,
                    var.decl_block,
                    block_var_name);

                result += std::format("#define {} {}\n", var.name, block_var_name);

                if (used_names->contains(var.name))
                {
                    return std::unexpected(var.name);
                }
                used_names->emplace(var.name);
            }

            return result;
        }

        std::expected<std::string, std::string> extract_main_function(inout_param<std::string> source)
        {
            const auto parse_failure = [] { return std::unexpected("Unable to parse main function"); };

            size_t start_offset = 0;
            while (true)
            {
                const auto index = ien::str_index_of(*source, "void", start_offset);
                if (!index.has_value())
                {
                    return parse_failure();
                }
                start_offset = *index;

                const auto parens_open_index_opt = ien::str_index_of(*source, "(", start_offset);
                if (!parens_open_index_opt.has_value())
                {
                    return parse_failure();
                }
                const auto parens_open_index = *parens_open_index_opt;

                auto name_segment = source->substr(start_offset, parens_open_index);
                if (name_segment.empty())
                {
                    return parse_failure();
                }
                name_segment = name_segment.substr(4); // remove 'void'

                name_segment = ien::str_trim(name_segment, ' ');
                name_segment = ien::str_trim(name_segment, '\r');
                name_segment = ien::str_trim(name_segment, '\n');
                name_segment = ien::str_trim(name_segment, '\t');
                if (name_segment != "main")
                {
                    start_offset += 1;
                    continue;
                }

                const auto bracket_close_indices = ien::str_indices_of(*source, "}", start_offset);
                if (bracket_close_indices.back() < start_offset)
                {
                    return parse_failure();
                }

                const auto main_block_len = bracket_close_indices.back() - start_offset + 1;

                auto main_block = std::string{ source->data() + start_offset, main_block_len };
                source->erase(start_offset, main_block_len);

                return main_block;
            }
        }
    } // namespace

    std::expected<shader_preprocess_data, std::string> get_shader_preprocess_data(std::string_view source)
    {
        // Remove CR from Window$ edited line endings
        std::string source_copy = ien::str_replace(source, '\r', "");

        auto mat_vars = extract_shader_variables(inout_param{ source_copy }, MATERIAL_UNIFORM_TEXT);
        FORWARD_UNEXPECTED(mat_vars);

        auto node_vars = extract_shader_variables(inout_param{ source_copy }, NODE_UNIFORM_TEXT);
        FORWARD_UNEXPECTED(node_vars);

        auto mat_textures = extract_texture_variables(inout_param{ source_copy }, MATERIAL_TEXTURES_TEXT);
        FORWARD_UNEXPECTED(mat_textures);

        auto node_textures = extract_texture_variables(inout_param{ source_copy }, NODE_TEXTURES_TEXT);
        FORWARD_UNEXPECTED(node_textures);

        auto mat_buffers = extract_buffer_variables(inout_param{ source_copy }, MATERIAL_BUFFER_TEXT);
        FORWARD_UNEXPECTED(mat_buffers);

        auto node_buffers = extract_buffer_variables(inout_param{ source_copy }, NODE_BUFFER_TEXT);
        FORWARD_UNEXPECTED(node_buffers);

        auto main_block = extract_main_function(inout_param{ source_copy });
        FORWARD_UNEXPECTED(main_block);

        shader_preprocess_data result;
        result.main_function_block = MOVE(*main_block);
        result.clean_source = MOVE(source_copy);
        result.material_uniform_vars = MOVE(*mat_vars);
        result.node_uniform_vars = MOVE(*node_vars);
        result.material_textures = MOVE(*mat_textures);
        result.node_textures = MOVE(*node_textures);
        result.material_buffers = MOVE(*mat_buffers);
        result.node_buffers = MOVE(*node_buffers);

        return result;
    }

    std::expected<std::string, std::string> preprocess_shader(
        const gfx::shader_type type,
        const shader_preprocess_data& merged_pp_data,
        const std::string& clean_source,
        const std::string& main_block)
    {
        std::unordered_set<std::string> used_names;

        const auto mat_uniform_block = generate_uniform_block(
            merged_pp_data.material_uniform_vars,
            "cathedral_material_uniform",
            MATERIAL_SET_INDEX,
            inout_param{ used_names });
        FORWARD_UNEXPECTED(mat_uniform_block);

        const auto node_uniform_block = generate_uniform_block(
            merged_pp_data.node_uniform_vars,
            "cathedral_node_uniform",
            NODE_SET_INDEX,
            inout_param{ used_names });
        FORWARD_UNEXPECTED(node_uniform_block);

        const auto material_texture_block = generate_texture_block(
            merged_pp_data.material_textures,
            "cathedral_material_textures",
            MATERIAL_SET_INDEX,
            inout_param{ used_names });
        FORWARD_UNEXPECTED(material_texture_block);

        const auto node_texture_block = generate_texture_block(
            merged_pp_data.node_textures,
            "cathedral_node_textures",
            NODE_SET_INDEX,
            inout_param{ used_names });
        FORWARD_UNEXPECTED(node_texture_block);

        const auto mat_buffer_block = generate_buffer_block(
            merged_pp_data.material_buffers,
            MATERIAL_SET_INDEX,
            BUFFERS_STARTING_BINDING_INDEX,
            inout_param{ used_names });
        FORWARD_UNEXPECTED(mat_buffer_block);

        const auto node_buffer_block = generate_buffer_block(
            merged_pp_data.node_buffers,
            NODE_SET_INDEX,
            BUFFERS_STARTING_BINDING_INDEX,
            inout_param{ used_names });
        FORWARD_UNEXPECTED(node_buffer_block);

        std::string result_source;
        result_source += std::string{ SHADER_VERSION } + '\n';

        if (type == gfx::shader_type::VERTEX)
        {
            result_source += std::string{ VERTEX_INPUTS };
        }

        static const auto scene_uniform_glslstr = get_scene_uniform_glslstr();
        result_source += scene_uniform_glslstr;

        result_source += clean_source;

        result_source += *mat_uniform_block + "\n";
        result_source += *material_texture_block + "\n";

        result_source += *node_uniform_block + "\n";
        result_source += *node_texture_block + "\n";

        result_source += *mat_buffer_block + "\n";
        result_source += *node_buffer_block + "\n";

        result_source += main_block + "\n";

        return result_source;
    }
} // namespace cathedral::engine