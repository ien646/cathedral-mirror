#pragma once

#include <cathedral/core.hpp>

#include <cathedral/gfx/buffers.hpp>
#include <cathedral/gfx/pipeline.hpp>
#include <cathedral/gfx/shader_data_types.hpp>

#include <cathedral/engine/shader_variable.hpp>

#include <cstdint>
#include <unordered_map>

namespace cathedral::engine
{
    enum class material_definition_domain : uint8_t
    {
        OPAQUE,
        TRANSPARENT,
        OVERLAY
    };

    class material_definition
    {
    public:
        uint32_t material_uniform_block_size() const { return _material_uniform_size; }

        uint32_t node_uniform_block_size() const { return _node_uniform_size; }

        void set_material_texture_slot_count(uint32_t count) { _material_tex_slots = count; }

        void set_node_texture_slot_count(uint32_t count) { _node_tex_slots = count; }

        uint32_t material_texture_slot_count() const { return _material_tex_slots; }

        uint32_t node_texture_slot_count() const { return _node_tex_slots; }

        void set_domain(material_definition_domain domain) { _domain = domain; }

        material_definition_domain domain() const { return _domain; }

        void add_material_variable(shader_variable var);
        void add_node_variable(shader_variable var);
        void clear_material_variable(uint32_t index);
        void clear_material_variables();
        void clear_node_variable(uint32_t index);
        void clear_node_variables();

        const auto& material_variables() const { return _material_variables; }

        const auto& node_variables() const { return _node_variables; }

        const auto& material_uniform_bindings() const { return _material_bindings; }

        const auto& node_uniform_bindings() const { return _node_bindings; }

        std::string create_material_uniform_glsl_struct() const;
        std::string create_node_uniform_glsl_struct() const;
        std::string create_material_uniform_cpp_struct() const;
        std::string create_node_uniform_cpp_struct() const;

        std::string create_material_sampler_glsl_struct() const;
        std::string create_node_sampler_glsl_struct() const;

        std::string create_full_glsl_header() const;

    private:
        uint32_t _material_uniform_size = 0;
        uint32_t _node_uniform_size = 0;
        uint32_t _material_tex_slots = 0;
        uint32_t _node_tex_slots = 0;

        material_definition_domain _domain = material_definition_domain::OPAQUE;

        std::vector<shader_variable> _material_variables;
        std::vector<shader_variable> _node_variables;

        std::unordered_map<shader_uniform_binding, uint32_t> _material_bindings;
        std::unordered_map<shader_uniform_binding, uint32_t> _node_bindings;

        void refresh_material_bindings();
        void refresh_node_bindings();
    };
} // namespace cathedral::engine