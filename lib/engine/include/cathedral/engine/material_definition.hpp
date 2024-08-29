#pragma once

#include <cathedral/core.hpp>

#include <cathedral/gfx/buffers.hpp>
#include <cathedral/gfx/pipeline.hpp>

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
            uint32_t node_uniform_size)
            : _material_tex_slots(material_texture_slots)
            , _node_tex_slots(node_texture_slots)
            , _material_uniform_size(material_uniform_size)
            , _node_uniform_size(node_uniform_size)
        {
        }

        uint32_t material_uniform_block_size() const { return _material_uniform_size; }
        uint32_t node_uniform_block_size() const { return _node_uniform_size; }

        void add_material_uniform_binding(uint32_t offset, material_uniform_binding binding)
        {
            CRITICAL_CHECK(offset + sizeof_material_uniform_binding(binding) <= material_uniform_block_size());
            _material_bindings.emplace(binding, offset);
        }

        void add_node_uniform_binding(uint32_t offset, material_uniform_binding binding)
        {
            CRITICAL_CHECK(offset + sizeof_material_uniform_binding(binding) <= node_uniform_block_size());
            _node_bindings.emplace(binding, offset);
        }

        uint32_t material_texture_slot_count() const { return _material_tex_slots; }
        uint32_t node_texture_slot_count() const { return _node_tex_slots; }

        const auto& material_uniform_bindings() const { return _material_bindings; }
        const auto& node_uniform_bindings() const { return _node_bindings; }

    private:
        const uint32_t _material_uniform_size;
        const uint32_t _node_uniform_size;
        const uint32_t _material_tex_slots;
        const uint32_t _node_tex_slots;
        std::unordered_map<material_uniform_binding, uint32_t> _material_bindings;
        std::unordered_map<material_uniform_binding, uint32_t> _node_bindings;
    };
} // namespace cathedral::engine