#pragma once

#include <cathedral/core.hpp>

#include <nlohmann/json_fwd.hpp>

#include <vector>

namespace cathedral::engine
{
    class scene;

    class scene_node : public uid_type
    {
    public:
        scene_node() = default;
        scene_node(std::string name, scene_node* parent = nullptr, bool enabled = true);
        virtual ~scene_node() = default;

        scene_node(const scene_node&) = delete;
        scene_node(scene_node&&) = default;

        const std::string& name() const { return _name; }

        void set_name(std::string_view name) { _name = name; }

        bool has_parent() const { return _parent != nullptr; }

        scene_node* parent() { return _parent; }

        const scene_node* parent() const { return _parent; }

        void set_parent(scene_node* parent) { _parent = parent; }

        template <typename T>
            requires(std::is_base_of_v<scene_node, T>)
        std::shared_ptr<T> add_child_node(const std::string& name)
        {
            auto node = std::make_shared<T>(name, this);
            _children.push_back(node);
            return node;
        }

        const std::vector<std::shared_ptr<scene_node>>& children() const { return _children; }

        void set_children(std::vector<std::shared_ptr<scene_node>> children) { _children = std::move(children); }

        std::shared_ptr<scene_node> get_child(const std::string& name);

        std::string get_full_name(const std::string& separator) const;
        std::vector<scene_node*> get_node_branch();
        std::vector<const scene_node*> get_node_branch() const;

        void disable();
        void enable();
        void set_enabled(bool enabled);

        bool enabled() const { return !_disabled; }

        bool contains_child(const std::string& name) const;
        std::shared_ptr<engine::scene_node> get_child(const std::string& name) const;

        virtual void tick(scene& scene, double deltatime) = 0;
        virtual void editor_tick(scene& scene, double deltatime) = 0;

        virtual constexpr const char* typestr() const = 0;

    protected:
        std::string _name;
        scene_node* _parent = nullptr;
        std::vector<std::shared_ptr<scene_node>> _children;
        bool _disabled = true;
    };
} // namespace cathedral::engine