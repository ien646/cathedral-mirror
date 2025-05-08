#pragma once

#include <cathedral/core.hpp>

namespace cathedral::engine
{
    constexpr auto NODE_TYPESTR = "node";
    constexpr auto MESH3D_TYPESTR = "mesh3d_node";
    constexpr auto CAMERA2D_TYPESTR = "camera2d_node";
    constexpr auto CAMERA3D_TYPESTR = "camera3d_node";
    constexpr auto POINT_LIGHT_TYPESTR = "point_light_node";

    enum class node_type : uint8_t
    {
        NODE,
        MESH3D_NODE,
        CAMERA2D_NODE,
        CAMERA3D_NODE,
        POINT_LIGHT
    };

    constexpr const char* typestr_from_type(const node_type type)
    {
        switch (type)
        {
        case node_type::NODE:
            return NODE_TYPESTR;
        case node_type::MESH3D_NODE:
            return MESH3D_TYPESTR;
        case node_type::CAMERA2D_NODE:
            return CAMERA2D_TYPESTR;
        case node_type::CAMERA3D_NODE:
            return CAMERA3D_TYPESTR;
        case node_type::POINT_LIGHT:
            return POINT_LIGHT_TYPESTR;
        default:
            CRITICAL_ERROR("Unhandled node_type");
        }
    }
} // namespace cathedral::engine