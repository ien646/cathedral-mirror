#pragma once

#include <cathedral/core.hpp>

#include <memory>

FORWARD_CLASS(cathedral::engine, camera2d_node);
FORWARD_CLASS(cathedral::engine, camera3d_node);
FORWARD_CLASS(cathedral::engine, scene);

namespace cathedral::editor::cameras
{
    engine::camera2d_node* get_editor_camera2d_node(engine::scene& scene);
    engine::camera3d_node* get_editor_camera3d_node(engine::scene& scene);
}