#include <cathedral/engine/default_resources.hpp>

#include <cathedral/engine/mesh.hpp>

#include "battery/embed.hpp"

namespace cathedral::engine::gizmos
{
    const std::string& get_translation_gizmo_vertex_shader()
    {   
        static const auto data = b::embed<"engine/shaders/translation_gizmo/vertex.glsl">().str();
        return data;
    }

    const std::string& get_translation_gizmo_fragment_shader()
    {
        static const auto data = b::embed<"engine/shaders/translation_gizmo/fragment.glsl">().str();
        return data;
    }

    const mesh& get_translation_gizmo_mesh()
    {
        static const auto data = [] { 
            std::stringstream sstr(b::embed<"engine/meshes/translation_gizmo.ply">().str());
            return mesh(sstr);
        }();
        return data;
    }
}