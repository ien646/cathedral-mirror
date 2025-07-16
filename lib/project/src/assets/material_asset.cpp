#include <cathedral/project/assets/material_asset.hpp>

#include <cathedral/project/project.hpp>

#include <ien/io_utils.hpp>

#include <magic_enum.hpp>

#include <fstream>

namespace cathedral::project
{
    CATHEDRAL_ASSET_SUBCLASS_IMPL(material_asset);

    void material_asset::set_material_variable_binding(
        const std::string& var_name,
        const std::optional<engine::shader_material_uniform_binding> binding)
    {
        if (binding.has_value())
        {
            _material_variable_bindings[var_name] = binding.value();
        }
        else
        {
            _material_variable_bindings.erase(var_name);
        }
    }

    void material_asset::set_node_variable_binding(
        const std::string& var_name,
        const std::optional<engine::shader_node_uniform_binding> binding)
    {
        if (binding.has_value())
        {
            _node_variable_bindings[var_name] = binding.value();
        }
        else
        {
            _node_variable_bindings.erase(var_name);
        }
    }
} // namespace cathedral::project