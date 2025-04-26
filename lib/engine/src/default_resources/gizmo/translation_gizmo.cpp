#include <cathedral/engine/default_resources.hpp>

#include <cathedral/engine/mesh.hpp>

#include "battery/embed.hpp"

namespace cathedral::engine::gizmos
{
    const std::string& get_translation_gizmo_vertex_shader()
    {   
        static const auto data = b::embed<"resources/translation_gizmo/vertex_shader.glsl">().str();
        return data;
    }

    const std::string& get_translation_gizmo_fragment_shader()
    {
        static const auto data = b::embed<"resources/translation_gizmo/fragment_shader.glsl">().str();
        return data;
    }

    const engine::mesh& get_translation_gizmo_mesh()
    {
        static const auto data = [] { 
            std::stringstream sstr(b::embed<"resources/translation_gizmo/mesh.ply">().str());
            return engine::mesh(sstr);
        }();
        return data;
    }
}