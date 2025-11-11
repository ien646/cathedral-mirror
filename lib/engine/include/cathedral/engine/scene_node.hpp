#pragma once

#include <cathedral/core.hpp>

#include <cathedral/engine/node_type.hpp>
#include <cathedral/engine/script.hpp>

#include <memory>
#include <vector>

namespace cathedral::engine
{
    class scene;

    template <typename T>
    std::unique_ptr<T> construct_node(std::string name, scene_node* parent, bool enabled) = delete;

    template <typename T>
    node_type get_node_type()
    {
        return T{}.type();
    }

    class scene_node
    {
    public:
        scene_node() = default;
        explicit scene_node(std::string name, scene_node* parent = nullptr, bool enabled = true);
        virtual ~scene_node() = default;

        scene_node(const scene_node&) = delete;
        scene_node(scene_node&&) = default;

        scene_node& operator=(const scene_node&) = delete;
        scene_node& operator=(scene_node&&) = default;

        const std::string& name() const { return _name; }

        void set_name(const std::string_view name) { _name = name; }

        bool has_parent() const { return _parent != nullptr; }

        scene_node* parent() const { return _parent; }

        scene_node* add_child_node(const std::string& name, node_type type);
        scene_node* add_child_node(std::unique_ptr<scene_node> node);

        template <typename T>
        T* add_child_node(std::string name)
        {
            auto node = construct_node<T>(MOVE(name), this, true);
            node->_parent = this;
            T* result = node.get();
            _children.push_back(MOVE(node));
            return result;
        }

        const std::vector<std::unique_ptr<scene_node>>& children() const { return _children; }

        void set_children(std::vector<std::unique_ptr<scene_node>>&& children) { _children = MOVE(children); }

        void remove_child(const std::string& name);

        std::string get_full_name(const std::string& separator) const;
        std::vector<scene_node*> get_node_branch();
        std::vector<const scene_node*> get_node_branch() const;

        void disable();
        void enable();
        void set_enabled(bool enabled);

        bool enabled() const { return !_disabled; }

        bool contains_child(const std::string& name) const;
        scene_node* get_child(const std::string& name);

        std::unique_ptr<scene_node> orphan_child(const scene_node* node);
        std::unique_ptr<scene_node> orphan_child(const std::string& name);

        bool is_editor_node() const;

        void set_disabled_in_editor_mode(bool disabled);
        bool disabled_in_editor_mode() const;

        void add_script(const std::shared_ptr<script>& script);
        void add_script(std::string name);
        void remove_script(size_t index);
        void remove_script(const std::string& name);
        const std::vector<std::shared_ptr<script>>& scripts() const;
        const std::vector<std::string>& script_names() const;

        void fix_parent_references(scene_node* parent = nullptr);

        virtual void tick_setup(scene& scene) = 0;
        virtual void tick(scene& scene, double deltatime) = 0;
        virtual void editor_tick(scene& scene, double deltatime) = 0;

        virtual std::unique_ptr<scene_node> copy(const std::string& copy_name, bool copy_children) const = 0;

        constexpr virtual node_type type() const = 0;

    protected:
        uint32_t _uid = std::numeric_limits<uint32_t>::max();
        std::string _name;
        scene_node* _parent = nullptr;
        std::vector<std::unique_ptr<scene_node>> _children;
        bool _disabled = true;
        bool _disabled_in_editor = false;
        std::vector<std::string> _script_names;
        std::vector<std::shared_ptr<script>> _scripts;
        bool _initialized = false;
    };
} // namespace cathedral::engine