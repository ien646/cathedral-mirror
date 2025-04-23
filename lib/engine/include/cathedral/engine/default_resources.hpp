#pragma once

#include <cathedral/core.hpp>

FORWARD_CLASS(ien, image);
FORWARD_CLASS(cathedral::engine, mesh);

namespace cathedral::engine
{
    const ien::image& get_default_texture_image();

    namespace gizmo
    {
        const engine::mesh& get_gizmo_mesh();
        const char* get_gizmo_vertex_shader();
        const char* get_gizmo_fragment_shader();
    } // namespace gizmo
} // namespace cathedral::engine