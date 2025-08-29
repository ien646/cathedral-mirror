#pragma once

#include <cathedral/engine/material_domain.hpp>
#include <cathedral/engine/shader_bindings.hpp>
#include <cathedral/engine/texture_compression.hpp>
#include <cathedral/project/asset_macros.hpp>

CATHEDRAL_SERIALIZE_ENUM_AUTO(cathedral::engine::texture_compression_type);
CATHEDRAL_SERIALIZE_ENUM_AUTO(cathedral::engine::shader_material_uniform_binding);
CATHEDRAL_SERIALIZE_ENUM_AUTO(cathedral::engine::shader_node_uniform_binding);
CATHEDRAL_SERIALIZE_ENUM_AUTO(cathedral::engine::shader_material_texture_binding);
CATHEDRAL_SERIALIZE_ENUM_AUTO(cathedral::engine::shader_node_texture_binding);
CATHEDRAL_SERIALIZE_ENUM_AUTO(cathedral::engine::shader_material_buffer_binding);
CATHEDRAL_SERIALIZE_ENUM_AUTO(cathedral::engine::shader_node_buffer_binding);
CATHEDRAL_SERIALIZE_ENUM_AUTO(cathedral::engine::material_domain);