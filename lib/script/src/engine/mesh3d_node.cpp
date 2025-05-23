#include <../include/cathedral/script/engine/mesh3d_node.hpp>

#include <cathedral/script/init_macros.hpp>

#include <cathedral/engine/nodes/mesh3d_node.hpp>

namespace cathedral::script::engine
{
    void mesh3d_node_initializer::initialize(state& s)
    {
        AUTO_INIT_NEW_TYPE(s, cathedral::engine, mesh3d_node);
        AUTO_BASE_CLASS(cathedral::engine::node);
        // clang-format off
        AUTO_FUNC_OVERLOAD(set_mesh, void, (std::optional<std::string>));
        // clang-format on
        AUTO_FUNC(mesh_name);
        AUTO_FUNC(get_material);
        AUTO_FUNC_OVERLOAD(bind_node_texture_slot, void, (const std::string&, uint32_t));
        AUTO_FUNC(bound_textures);
    }
} // namespace cathedral::script::engine