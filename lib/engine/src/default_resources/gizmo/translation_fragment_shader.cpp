#include <cathedral/engine/default_resources.hpp>

namespace
{
    /* NOLINT */ constexpr const char DATA[] = R"glsl(
    layout(location = 0) in vec3 in_color;    
    layout(location = 0) out vec4 out_color;

    void main()
    {
        out_color = vec4(in_color, 1.0);
    }
    )glsl";
}

namespace cathedral::engine::gizmos
{
    const char* get_translation_gizmo_fragment_shader()
    {
        return DATA;
    }
}