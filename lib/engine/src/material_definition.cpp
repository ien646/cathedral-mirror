#include <cathedral/engine/material_definition.hpp>

#include <map>

namespace cathedral::engine
{
    void material_definition::set_material_variable(uint32_t index, variable var)
    {
        _material_variables.insert_or_assign(index, std::move(var));
        refresh_material_bindings();
    }

    void material_definition::set_node_variable(uint32_t index, variable var)
    {
        _node_variables.insert_or_assign(index, std::move(var));
        refresh_node_bindings();
    }

    void material_definition::clear_material_variable(uint32_t index)
    {
        _material_variables.erase(index);
    }

    void material_definition::clear_node_variable(uint32_t index)
    {
        _node_variables.erase(index);
    }

    void material_definition::refresh_material_bindings()
    {
        _material_bindings.clear();

        const std::map<uint32_t, variable> sorted_variables(_material_variables.begin(), _material_variables.end());

        uint32_t current_offset = 0;
        for (const auto& [index, var] : sorted_variables)
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

        const std::map<uint32_t, variable> sorted_variables(_node_variables.begin(), _node_variables.end());

        uint32_t current_offset = 0;
        for (const auto& [index, var] : sorted_variables)
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