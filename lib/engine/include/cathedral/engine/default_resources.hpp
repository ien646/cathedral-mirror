#pragma once

#include <cathedral/core.hpp>

FORWARD_CLASS(ien, image);
FORWARD_CLASS(cathedral::engine, mesh);

namespace cathedral::engine
{
    const ien::image& get_default_texture_image();

    namespace gizmos
    {
        const engine::mesh& get_translation_gizmo_mesh();
        const std::string& get_translation_gizmo_vertex_shader();
        const std::string& get_translation_gizmo_fragment_shader();
    } // namespace gizmo
} // namespace cathedral::engine