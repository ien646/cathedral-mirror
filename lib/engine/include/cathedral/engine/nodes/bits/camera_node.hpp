#pragma once

#include <cathedral/engine/node_type.hpp>
#include <cathedral/engine/nodes/node.hpp>

namespace cathedral::engine::internal
{
    template <typename TCamera, node_type NodeType>
    class camera_node_base : public node
    {
    public:
        using node::node;

        void set_main_camera(const bool is_main) { _is_main_camera = is_main; }

        bool is_main_camera() const { return _is_main_camera; }

        TCamera& camera() { return _camera; }

        const TCamera& camera() const { return _camera; }

        constexpr node_type type() const override { return NodeType; }

        constexpr const char* typestr() const override { return typestr_from_type(NodeType); }

    protected:
        TCamera _camera;
        bool _is_main_camera = false;

        template <typename TNode>
        std::shared_ptr<scene_node> copy_camera_node(const std::string& copy_name, const bool copy_children) const
        {
            auto result = std::make_shared<TNode>(copy_name, _parent, !_disabled);

            result->set_local_transform(_local_transform);
            result->set_main_camera(_is_main_camera);
            result->camera() = _camera;

            if (copy_children)
            {
                copy_children_into(*result);
            }

            return result;
        }
    };
} // namespace cathedral::engine::internal