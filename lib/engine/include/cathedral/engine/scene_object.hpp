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
    std::unique_ptr<T> construct_node(std::string name, scene_object* parent, bool enabled) = delete;

    template <typename T>
    node_type get_node_type()
    {
        return T{}.type();
    }

    class scene_object
    {
    public:
        scene_object() = default;
        explicit scene_object(std::string name, scene_object* parent = nullptr, bool enabled = true);
        virtual ~scene_object() = default;

        scene_object(const scene_object&) = delete;
        scene_object(scene_object&&) = default;

        scene_object& operator=(const scene_object&) = delete;
        scene_object& operator=(scene_object&&) = default;

        EXPORT_SCRIPT const std::string& name() const { return _name; }
        EXPORT_SCRIPT void set_name(const std::string_view name) { _name = name; }

        EXPORT_SCRIPT bool has_parent() const { return _parent != nullptr; }
        EXPORT_SCRIPT scene_object* parent() const { return _parent; }

        EXPORT_SCRIPT scene_object* add_child_node(const std::string& name, node_type type);
        scene_object* add_child_node(std::unique_ptr<scene_object> node);

        template <typename T>
        T* add_child_node(std::string name)
        {
            auto node = construct_node<T>(MOVE(name), this, true);
            node->_parent = this;
            T* result = node.get();
            _children.push_back(MOVE(node));
            return result;
        }

        const std::vector<std::unique_ptr<scene_object>>& children() const { return _children; }

        void set_children(std::vector<std::unique_ptr<scene_object>>&& children) { _children = MOVE(children); }

        EXPORT_SCRIPT void remove_child(const std::string& name);

        EXPORT_SCRIPT std::string get_full_name(const std::string& separator) const;
        std::vector<scene_object*> get_node_branch();
        std::vector<const scene_object*> get_node_branch() const;

        EXPORT_SCRIPT void disable();
        EXPORT_SCRIPT void enable();
        EXPORT_SCRIPT void set_enabled(bool enabled);

        EXPORT_SCRIPT bool enabled() const { return !_disabled; }

        EXPORT_SCRIPT bool contains_child(const std::string& name) const;
        EXPORT_SCRIPT scene_object* get_child(const std::string& name);

        std::unique_ptr<scene_object> orphan_child(const scene_object* node);
        std::unique_ptr<scene_object> orphan_child(const std::string& name);

        EXPORT_SCRIPT bool is_editor_node() const;

        EXPORT_SCRIPT void set_disabled_in_editor_mode(bool disabled);
        EXPORT_SCRIPT bool disabled_in_editor_mode() const;

        void add_script(const std::shared_ptr<script>& script);
        EXPORT_SCRIPT void add_script(std::string name);
        void remove_script(size_t index);
        EXPORT_SCRIPT void remove_script(const std::string& name);
        const std::vector<std::shared_ptr<script>>& scripts() const;
        EXPORT_SCRIPT const std::vector<std::string>& script_names() const;

        void fix_parent_references(scene_object* parent = nullptr);

        virtual void tick_setup(scene& scene) = 0;
        virtual void tick(scene& scene, double deltatime) = 0;
        virtual void editor_tick(scene& scene, double deltatime) = 0;

        virtual std::unique_ptr<scene_object> copy(const std::string& copy_name, bool copy_children) const = 0;

        EXPORT_SCRIPT constexpr virtual node_type type() const = 0;

    protected:
        uint32_t _uid = std::numeric_limits<uint32_t>::max();
        std::string _name;
        scene_object* _parent = nullptr;
        std::vector<std::unique_ptr<scene_object>> _children;
        bool _disabled = true;
        bool _disabled_in_editor = false;
        std::vector<std::string> _script_names;
        std::vector<std::shared_ptr<script>> _scripts;
        bool _initialized = false;
    };
} // namespace cathedral::engine