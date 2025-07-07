#pragma once

#include <cathedral/core.hpp>

#include <cathedral/geometry/aabb.hpp>
#include <cathedral/geometry/plane.hpp>

#include <glm/vec3.hpp>

FORWARD_CLASS(cathedral::engine, camera);

namespace cathedral::engine
{
    struct frustum_planes
    {
        plane near;
        plane far;
        plane left;
        plane right;
        plane top;
        plane bottom;
    };

    frustum_planes get_frustum_from_camera(const camera& camera);

    bool is_point_inside_frustum(glm::vec3 point, const frustum_planes& frustum, bool include_tangent);

    bool is_aabb_inside_frustum(const aabb& aabb, const frustum_planes& frustum);

    bool is_sphere_inside_frustum(const sphere& sphere, const frustum_planes& frustum);
} // namespace cathedral::engine