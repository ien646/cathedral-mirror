#include <cathedral/script/types/nodes.hpp>

#include <cathedral/engine/nodes/camera2d_node.hpp>
#include <cathedral/engine/nodes/camera3d_node.hpp>
#include <cathedral/engine/nodes/directional_light_node.hpp>
#include <cathedral/engine/nodes/mesh3d_node.hpp>
#include <cathedral/engine/nodes/node.hpp>
#include <cathedral/engine/nodes/point_light_node.hpp>
#include <cathedral/engine/nodes/text_node.hpp>
#include <cathedral/engine/scene_node.hpp>
#include <cathedral/script/helpers.hpp>

namespace cathedral::script
{
    void register_nodes(global_state& state)
    {
        using namespace engine;

        register_class_auto<scene_node>(state, "SceneNode");
        register_derived_class_auto<node, scene_node>(state, "Node");

        using c2d_base = internal::camera_node_base<orthographic_camera>;
        using c3d_base = internal::camera_node_base<perspective_camera>;

        register_derived_class_auto<c2d_base, node>(state, "_OrthographicCameraNode");
        register_derived_class_auto<c3d_base, node>(state, "_PerspectiveCameraNode");

        register_derived_class_auto<camera2d_node, c2d_base>(state, "Camera2DNode");
        register_derived_class_auto<camera3d_node, c3d_base>(state, "Camera3DNode");

        register_derived_class_auto<directional_light_node, node>(state, "DirectionalLightNode");

        register_derived_class_auto<drawable_node, node>(state, "_DrawableNode");

        register_derived_class_auto<mesh3d_node, drawable_node>(state, "Mesh3DNode");
        register_derived_class_auto<text_node, drawable_node>(state, "TextNode");

        register_class_auto<point_light_node>(state, "PointLightNode");
    }
} // namespace cathedral::script