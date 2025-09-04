#pragma once

#include <cathedral/engine/frustum.hpp>
#include <cathedral/geometry/aabb.hpp>
#include <cathedral/geometry/ray.hpp>
#include <cathedral/geometry/sphere.hpp>

#include <variant>

namespace cathedral::engine
{
    using bounding_figure = std::variant<sphere, aabb>;

    struct bounds_collection
    {
        std::vector<bounding_figure> figures;

        bool ray_intersects(const ray& r) const;

        bool inside_frustum(const frustum_planes& frustum) const;
    };
} // namespace cathedral::engine