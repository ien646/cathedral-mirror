#pragma once

#include <cathedral/core.hpp>

#include <vector>

namespace cathedral::engine
{
    class scene;

    class scene_node : public uid_type
    {
    public:
        scene_node(scene& scn, std::string name, scene_node* parent = nullptr);
        virtual ~scene_node() = default;

        scene_node(const scene_node&) = delete;
        scene_node(scene_node&&) = default;

        const std::string& name() const { return _name; }

        void set_name(std::string_view name) { _name = name; }

        bool has_parent() const { return _parent != nullptr; }

        scene_node* parent() { return _parent; }

        const scene_node* parent() const { return _parent; }

        void set_parent(scene_node* parent) { _parent = parent; }

        scene& get_scene() { return _scene; }

        template <typename T>
            requires(std::is_base_of_v<scene_node, T>)
        std::shared_ptr<T> add_child_node(const std::string& name)
        {
            auto node = std::make_shared<T>(_scene, name, this);
            _children.push_back(node);
            return node;
        }

        const std::vector<std::shared_ptr<scene_node>>& children() const { return _children; }

        std::shared_ptr<scene_node> get_child(const std::string& name);

        std::string get_full_name(const std::string& separator) const;
        std::vector<scene_node*> get_node_branch();
        std::vector<const scene_node*> get_node_branch() const;

        void disable();
        void enable();

        virtual void tick(double deltatime) = 0;
        virtual void editor_tick(double deltatime) = 0;

    protected:
        scene& _scene;
        std::string _name;
        scene_node* _parent = nullptr;
        std::vector<std::shared_ptr<scene_node>> _children;
        bool _disabled = false;
    };
} // namespace cathedral::engine