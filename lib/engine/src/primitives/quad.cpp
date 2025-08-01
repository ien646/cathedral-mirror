#include <cathedral/engine/primitives/quad.hpp>

#include <cathedral/engine/mesh.hpp>

#include <battery/embed.hpp>

namespace cathedral::engine::primitives
{
    const mesh& quad_mesh()
    {
        const static mesh result = [] {
            std::stringstream sstr(b::embed<"engine/meshes/quad.ply">().str());
            return mesh(sstr);
        }();

        return result;
    }
} // namespace cathedral::engine::primitives