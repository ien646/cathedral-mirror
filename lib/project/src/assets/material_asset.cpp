#include <cathedral/project/assets/material_asset.hpp>

#include <cathedral/project/project.hpp>

#include <ien/io_utils.hpp>

#include <magic_enum.hpp>

#include <fstream>

namespace cathedral::project
{
    CATHEDRAL_ASSET_SUBCLASS_IMPL(material_asset);

    void material_asset::set_material_uniform_binding(
        const std::string& var_name,
        const std::optional<engine::shader_material_uniform_binding> binding)
    {
        if (binding.has_value())
        {
            _material_uniform_bindings[var_name] = binding.value();
        }
        else
        {
            _material_uniform_bindings.erase(var_name);
        }
    }

    void material_asset::set_node_uniform_binding(
        const std::string& var_name,
        const std::optional<engine::shader_node_uniform_binding> binding)
    {
        if (binding.has_value())
        {
            _node_uniform_bindings[var_name] = binding.value();
        }
        else
        {
            _node_uniform_bindings.erase(var_name);
        }
    }

    void material_asset::set_material_texture_binding(
        const std::string& var_name,
        const std::optional<engine::shader_material_texture_binding> binding)
    {
        if (binding.has_value())
        {
            _material_texture_bindings[var_name] = binding.value();
        }
        else
        {
            _material_texture_bindings.erase(var_name);
        }
    }

    void material_asset::set_node_texture_binding(
        const std::string& var_name,
        const std::optional<engine::shader_node_texture_binding> binding)
    {
        if (binding.has_value())
        {
            _node_texture_bindings[var_name] = binding.value();
        }
        else
        {
            _node_texture_bindings.erase(var_name);
        }
    }

    void material_asset::set_material_buffer_binding(
        const std::string& var_name,
        const std::optional<engine::shader_material_buffer_binding> binding)
    {
        if (binding.has_value())
        {
            _material_buffer_bindings[var_name] = binding.value();
        }
        else
        {
            _material_buffer_bindings.erase(var_name);
        }
    }

    void material_asset::set_node_buffer_binding(
        const std::string& var_name,
        const std::optional<engine::shader_node_buffer_binding> binding)
    {
        if (binding.has_value())
        {
            _node_buffer_bindings[var_name] = binding.value();
        }
        else
        {
            _node_buffer_bindings.erase(var_name);
        }
    }
} // namespace cathedral::project