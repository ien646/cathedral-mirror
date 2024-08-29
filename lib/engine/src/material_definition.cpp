#include <cathedral/engine/material_definition.hpp>

namespace cathedral::engine
{
    material_definition::material_definition(
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

    void material_definition::add_material_uniform_binding(uint32_t offset, material_uniform_binding binding)
    {
        CRITICAL_CHECK(offset + sizeof_material_uniform_binding(binding) <= material_uniform_block_size());
        _material_bindings.emplace(binding, offset);
    }

    void material_definition::add_node_uniform_binding(uint32_t offset, material_uniform_binding binding)
    {
        CRITICAL_CHECK(offset + sizeof_material_uniform_binding(binding) <= node_uniform_block_size());
        _node_bindings.emplace(binding, offset);
    }

    void material_definition::set_material_variable(uint32_t index, variable var)
    {
        _material_variables.insert_or_assign(index, std::move(var));
    }

    void material_definition::set_node_variable(uint32_t index, variable var)
    {
        _node_variables.insert_or_assign(index, std::move(var));
    }

    void material_definition::clear_material_variable(uint32_t index)
    {
        _material_variables.erase(index);
    }

    void material_definition::clear_node_variable(uint32_t index)
    {
        _node_variables.erase(index);
    }
} // namespace cathedral::engine