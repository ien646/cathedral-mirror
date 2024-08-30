#pragma once

#include <cathedral/core.hpp>

#include <cathedral/gfx/buffers.hpp>
#include <cathedral/gfx/pipeline.hpp>
#include <cathedral/gfx/shader_data_types.hpp>

#include <cathedral/engine/material_uniform_bindings.hpp>
#include <cathedral/engine/texture.hpp>

#include <cstdint>
#include <unordered_map>

namespace cathedral::engine
{
    class material_definition
    {
    public:
        material_definition(
            uint32_t material_texture_slots,
            uint32_t node_texture_slots,
            uint32_t material_uniform_size,
            uint32_t node_uniform_size);

        uint32_t material_uniform_block_size() const { return _material_uniform_size; }
        uint32_t node_uniform_block_size() const { return _node_uniform_size; }

        void add_material_uniform_binding(uint32_t offset, material_uniform_binding binding);
        void add_node_uniform_binding(uint32_t offset, material_uniform_binding binding);

        uint32_t material_texture_slot_count() const { return _material_tex_slots; }
        uint32_t node_texture_slot_count() const { return _node_tex_slots; }

        const auto& material_uniform_bindings() const { return _material_bindings; }
        const auto& node_uniform_bindings() const { return _node_bindings; }

        struct variable
        {
            variable(
                gfx::shader_data_type type,
                uint32_t count,
                std::string name)
                : type(type)
                , count(count)
                , name(std::move(name))
            {
            }

            gfx::shader_data_type type;
            uint32_t count;
            std::string name;
        };

        void set_material_variable(uint32_t index, variable var);
        void set_node_variable(uint32_t index, variable var);
        void clear_material_variable(uint32_t index);
        void clear_node_variable(uint32_t index);

    private:
        const uint32_t _material_uniform_size;
        const uint32_t _node_uniform_size;
        const uint32_t _material_tex_slots;
        const uint32_t _node_tex_slots;
        std::unordered_map<material_uniform_binding, uint32_t> _material_bindings;
        std::unordered_map<material_uniform_binding, uint32_t> _node_bindings;

        std::unordered_map<uint32_t, variable> _material_variables;
        std::unordered_map<uint32_t, variable> _node_variables;
    };
} // namespace cathedral::engine