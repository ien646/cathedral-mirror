#pragma once

#include <cathedral/engine/node_type.hpp>
#include <cathedral/engine/nodes/node.hpp>

namespace cathedral::engine::internal
{
    template <typename TCamera>
    class camera_node_base : public node
    {
    public:
        using node::node;

        void set_main_camera(const bool is_main) { _is_main_camera = is_main; }

        bool is_main_camera() const { return _is_main_camera; }

        TCamera& camera() { return _camera; }

        const TCamera& camera() const { return _camera; }

    protected:
        TCamera _camera;
        bool _is_main_camera = false;

        template <typename TNode>
        std::unique_ptr<scene_node> copy_camera_node(const std::string& copy_name, const bool copy_children) const
        {
            auto result = std::make_unique<TNode>(copy_name, _parent, !_disabled);

            node::copy_into(*result, copy_children);
            result->set_main_camera(_is_main_camera);
            result->camera() = _camera;

            return result;
        }
    };
} // namespace cathedral::engine::internal