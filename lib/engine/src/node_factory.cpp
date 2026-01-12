#include <cathedral/engine/node_factory.hpp>

#include <cathedral/ds.hpp>
#include <cathedral/engine/nodes/camera2d_node.hpp>
#include <cathedral/engine/nodes/camera3d_node.hpp>
#include <cathedral/engine/nodes/directional_light_node.hpp>
#include <cathedral/engine/nodes/mesh3d_node.hpp>
#include <cathedral/engine/nodes/node.hpp>
#include <cathedral/engine/nodes/point_light_node.hpp>
#include <cathedral/engine/nodes/text_node.hpp>

#define DEFAULT_INIT_NODE(T) { get_node_type<T>().id(), ctor_for_type<T>() }

namespace cathedral::engine
{
    namespace
    {
        template <typename T>
        node_ctor ctor_for_type()
        {
            return [] { return std::make_unique<T>(); };
        }

        // clang-format off
        unordered_map<node_type::id_type, node_ctor> constructors = {
            DEFAULT_INIT_NODE(camera2d_node),
            DEFAULT_INIT_NODE(camera3d_node),
            DEFAULT_INIT_NODE(directional_light_node),
            DEFAULT_INIT_NODE(mesh3d_node),
            DEFAULT_INIT_NODE(node),
            DEFAULT_INIT_NODE(point_light_node),
            DEFAULT_INIT_NODE(text_node)
        };
        // clang-format on
    } // namespace

    void register_node_constructor(const node_type type, node_ctor ctor)
    {
        CRITICAL_CHECK(
            !constructors.contains(type.id()),
            std::format("Attempt to register duplicate node type: '{}'", type.string_view()));
        constructors.emplace(type.id(), MOVE(ctor));
    }

    std::unique_ptr<scene_node> construct_node(const node_type type)
    {
        return constructors.at(type.id())();
    }
} // namespace cathedral::engine