#include <cathedral/engine/default_resources.hpp>

namespace
{
    /* NOLINT */ constexpr const char DATA[] = R"glsl(
    $NODE_VARIABLE mat4 model_matrix;

    layout(location = 0) out vec3 out_color;

    void main()
    {
        gl_Position = PROJECTION_3D * VIEW_3D * model_matrix * vec4(VERTEX_POSITION, 1.0);
        out_color = VERTEX_COLOR;
    }
    )glsl";
}

namespace cathedral::engine::gizmos
{
    const char* get_translation_gizmo_vertex_shader()
    {
        return DATA;
    }
} // namespace cathedral::engine::gizmo