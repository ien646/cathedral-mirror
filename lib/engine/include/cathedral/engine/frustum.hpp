#pragma once

#include <cathedral/engine/camera.hpp>

#include <glm/vec3.hpp>

namespace cathedral::engine
{
    struct frustum_normals
    {
        glm::vec3 near;
        glm::vec3 far;
        glm::vec3 left;
        glm::vec3 right;
        glm::vec3 top;
        glm::vec3 bottom;
    };

    frustum_normals get_frustum_from_camera(const perspective_camera& camera);
}