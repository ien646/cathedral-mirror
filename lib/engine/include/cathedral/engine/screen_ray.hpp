#pragma once

#include <cathedral/engine/camera.hpp>
#include <cathedral/geometry/ray.hpp>

namespace cathedral::engine
{
    ray get_screen_point_camera_ray(const camera& cam, glm::vec2 normalized_coord);
}