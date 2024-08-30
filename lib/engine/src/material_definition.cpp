#include <cathedral/engine/material_definition.hpp>

#include <sstream>

namespace cathedral::engine
{
    void material_definition::add_material_variable(shader_variable var)
    {
        _material_variables.push_back(std::move(var));
        refresh_material_bindings();
    }

    void material_definition::add_node_variable(shader_variable var)
    {
        _node_variables.push_back(std::move(var));
        refresh_node_bindings();
    }

    void material_definition::clear_material_variable(uint32_t index)
    {
        _material_variables.erase(_material_variables.begin() + index);
        refresh_material_bindings();
    }

    void material_definition::clear_material_variables()
    {
        _material_variables.clear();
        refresh_material_bindings();
    }

    void material_definition::clear_node_variable(uint32_t index)
    {
        _node_variables.erase(_node_variables.begin() + index);
        refresh_node_bindings();
    }

    void material_definition::clear_node_variables()
    {
        _node_variables.clear();
        refresh_node_bindings();
    }

    namespace detail
    {
        std::string generate_glsl_var_field(const shader_variable& var)
        {
            if (var.count > 1)
            {
                return std::format("\t{} {}[{}];\n", shader_data_type_glslstr(var.type), var.name, var.count);
            }
            else
            {
                return std::format("\t{} {};\n", shader_data_type_glslstr(var.type), var.name);
            }
        }
    }; // namespace detail

    std::string material_definition::create_material_uniform_glsl_struct() const
    {
        if (_material_variables.empty())
        {
            return {};
        }
        std::stringstream ss;
        ss << "layout (set = 1, binding = 0) uniform _material_uniform_data { \n";
        for (const auto& var : _material_variables)
        {
            ss << detail::generate_glsl_var_field(var);
        }
        ss << "} material_uniform_data; \n";
        return ss.str();
    }

    std::string material_definition::create_node_uniform_glsl_struct() const
    {
        if (_node_variables.empty())
        {
            return {};
        }
        std::stringstream ss;
        ss << "layout (set = 2, binding = 0) uniform _node_uniform_data { \n";
        for (const auto& var : _node_variables)
        {
            ss << detail::generate_glsl_var_field(var);
        }
        ss << "} node_uniform_data; \n";
        return ss.str();
    }

    namespace detail
    {
        std::string generate_cpp_var_field(const shader_variable& var)
        {
            if (var.count > 1)
            {
                return std::format(
                    "\tCATHEDRAL_ALIGNED_UNIFORM({}, {})[{}];\n",
                    shader_data_type_cppstr(var.type),
                    var.name,
                    var.count);
            }
            else
            {
                return std::format("\tCATHEDRAL_ALIGNED_UNIFORM({}, {});\n", shader_data_type_cppstr(var.type), var.name);
            }
        }
    }; // namespace detail

    std::string material_definition::create_material_uniform_cpp_struct() const
    {
        if (_material_variables.empty())
        {
            return {};
        }
        std::stringstream ss;
        ss << "struct material_uniform_data_t { \n";
        for (const auto& var : _material_variables)
        {
            ss << detail::generate_cpp_var_field(var);
        }
        ss << "}; \n";
        return ss.str();
    }

    std::string material_definition::create_node_uniform_cpp_struct() const
    {
        if (_node_variables.empty())
        {
            return {};
        }
        std::stringstream ss;
        ss << "struct node_uniform_data_t { \n";
        for (const auto& var : _node_variables)
        {
            ss << detail::generate_cpp_var_field(var);
        }
        ss << "}; \n";
        return ss.str();
    }

    std::string material_definition::create_material_sampler_glsl_struct() const
    {
        if (_material_tex_slots == 0)
        {
            return {};
        }
        if (_material_tex_slots == 1)
        {
            return "layout (set = 1, binding = 1) uniform sampler2D material_texture;\n";
        }
        return std::format("layout (set = 1, binding = 1) uniform sampler2D material_textures[{}];\n", _material_tex_slots);
    }

    std::string material_definition::create_node_sampler_glsl_struct() const
    {
        if (_node_tex_slots == 0)
        {
            return {};
        }
        if (_node_tex_slots == 1)
        {
            return "layout (set = 2, binding = 1) uniform sampler2D node_texture;\n";
        }
        return std::format("layout (set = 2, binding = 1) uniform sampler2D node_textures[{}];\n", _node_tex_slots);
    }

    std::string material_definition::create_full_glsl_header() const
    {
        std::stringstream ss;

        ss << create_material_uniform_glsl_struct() << "\n";
        ss << create_material_sampler_glsl_struct() << "\n";
        ss << create_node_uniform_glsl_struct() << "\n";
        ss << create_node_sampler_glsl_struct();

        return ss.str();
    }

    void material_definition::refresh_material_bindings()
    {
        _material_bindings.clear();

        uint32_t current_offset = 0;
        for (const auto& var : _material_variables)
        {
            if (var.binding)
            {
                _material_bindings.emplace(*var.binding, current_offset);
            }
            current_offset += gfx::shader_data_type_offset(var.type) * var.count;
        }

        _material_uniform_size = current_offset;
    }

    void material_definition::refresh_node_bindings()
    {
        _node_bindings.clear();

        uint32_t current_offset = 0;
        for (const auto& var : _node_variables)
        {
            if (var.binding)
            {
                _node_bindings.emplace(*var.binding, current_offset);
            }
            current_offset += gfx::shader_data_type_offset(var.type) * var.count;
        }

        _node_uniform_size = current_offset;
    }
} // namespace cathedral::engine