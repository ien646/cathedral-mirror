#pragma once

#include <cathedral/core.hpp>

namespace cathedral::engine
{
    enum class node_type : uint8_t
    {
        NODE,
        MESH3D_NODE,
        CAMERA2D_NODE,
        CAMERA3D_NODE
    };

    constexpr const char* typestr_from_type(const node_type type)
    {
        switch (type)
        {
        case node_type::NODE:
            return "node";
        case node_type::MESH3D_NODE:
            return "mesh3d_node";
        case node_type::CAMERA2D_NODE:
            return "camera2d_node";
        case node_type::CAMERA3D_NODE:
            return "camera3d_node";
        default:
            CRITICAL_ERROR("Unhandled node_type");
        }
    }
} // namespace cathedral::engine