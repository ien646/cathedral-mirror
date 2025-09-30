#pragma once

#include <cathedral/engine/debug/line.hpp>
#include <cathedral/engine/font.hpp>
#include <cathedral/engine/input.hpp>
#include <cathedral/engine/lights.hpp>
#include <cathedral/engine/material.hpp>
#include <cathedral/engine/mesh_buffer_storage.hpp>
#include <cathedral/engine/nodes/camera3d_node.hpp>
#include <cathedral/engine/renderer.hpp>
#include <cathedral/engine/scene_node.hpp>
#include <cathedral/gfx/aligned_uniform.hpp>
#include <cathedral/gfx/pipeline.hpp>

#include <chrono>

#ifndef CATHEDRAL_SCENE_MAX_DIRECTIONAL_LIGHTS
    #define CATHEDRAL_SCENE_MAX_DIRECTIONAL_LIGHTS 4
#endif

#if CATHEDRAL_SCENE_MAX_DIRECTIONAL_LIGHTS <= 0
    #error "At least one directional light is required"
#endif

#ifndef CATHEDRAL_SCENE_MAX_POINT_LIGHTS
    #define CATHEDRAL_SCENE_MAX_POINT_LIGHTS 20
#endif

#if CATHEDRAL_SCENE_MAX_POINT_LIGHTS <= 0
    #error "At least on point light is required"
#endif

namespace cathedral::engine
{
    struct scene_uniform_data
    {
        // clang-format off
        CATHEDRAL_ALIGNED_UNIFORM(float, deltatime) = 0.0;
        CATHEDRAL_ALIGNED_UNIFORM(uint32_t, frame_index) = 0;
        CATHEDRAL_ALIGNED_UNIFORM(uint32_t, enabled_point_lights) = 0;
        CATHEDRAL_ALIGNED_UNIFORM(uint32_t, enabled_directional_lights) = 0;
        CATHEDRAL_ALIGNED_UNIFORM(glm::vec3, ambient_light) = { 0.05F, 0.05F, 0.05F };
        CATHEDRAL_ALIGNED_UNIFORM(uint32_t, viewport_width) = 0;
        CATHEDRAL_ALIGNED_UNIFORM(uint32_t, viewport_height) = 0;
        CATHEDRAL_PADDING_32;
        CATHEDRAL_PADDING_32;
        CATHEDRAL_PADDING_32;
        CATHEDRAL_ALIGNED_UNIFORM(glm::mat4, projection2d) = glm::mat4(1.0F);
        CATHEDRAL_ALIGNED_UNIFORM(glm::mat4, projection3d) = glm::mat4(1.0F);
        CATHEDRAL_ALIGNED_UNIFORM(glm::mat4, view2d) = glm::mat4(1.0F);
        CATHEDRAL_ALIGNED_UNIFORM(glm::mat4, view3d) = glm::mat4(1.0F);
        CATHEDRAL_ALIGNED_UNIFORM(directional_light_data, directional_lights)[CATHEDRAL_SCENE_MAX_DIRECTIONAL_LIGHTS]; // NOLINT
        CATHEDRAL_ALIGNED_UNIFORM(point_light_data, point_lights)[CATHEDRAL_SCENE_MAX_POINT_LIGHTS]; // NOLINT
        // clang-format on
    };

    std::string get_scene_uniform_glslstr();

    using scene_clock = std::chrono::high_resolution_clock;
    using scene_timepoint = scene_clock::time_point;

    template <typename T>
    using loader_func = std::function<std::shared_ptr<T>(const std::string& name, scene& scn)>;

    template <typename T>
    using weak_loader_func = std::function<std::weak_ptr<T>(const std::string& name, scene& scn)>;

    struct scene_loader_funcs
    {
        weak_loader_func<material> material_loader = nullptr;
        loader_func<mesh> mesh_loader = nullptr;
        loader_func<texture> texture_loader = nullptr;
        loader_func<script> script_loader = nullptr;
        loader_func<font> font_loader = nullptr;
    };

    struct scene_args
    {
        std::string name;
        renderer* prenderer = nullptr;
        scene_loader_funcs loaders;
    };

    class scene final
    {
    public:
        explicit scene(scene_args args);
        ~scene();

        CATHEDRAL_NON_COPYABLE(scene);
        CATHEDRAL_DEFAULT_MOVABLE(scene);

        const gfx::uniform_buffer& uniform_buffer() const { return *_uniform_buffer; }

        renderer& get_renderer() const { return *_args.prenderer; }

        vk::DescriptorSet descriptor_set() const;

        void tick(const std::function<void(double deltatime)>&);

        scene_node* add_root_node(const std::string& name, node_type type);

        template <typename T>
        T* add_root_node(std::string name)
        {
            auto node = construct_node<T>(std::move(name), nullptr, true);
            T* result = node.get();
            _root_nodes.push_back(std::move(node));
            return result;
        }

        void add_root_node(std::unique_ptr<scene_node>&& node);

        scene_node* get_node(const std::string& name);

        void remove_node(const std::string& name);

        std::unique_ptr<scene_node> detach_node(const std::string& name);

        bool contains_node(const std::string& name) const;

        template <typename T>
            requires(std::is_base_of_v<scene_node, T>)
        T* get_node(const std::string& name)
        {
            return dynamic_cast<T*>(get_node(name));
        }

        const std::vector<std::unique_ptr<scene_node>>& root_nodes() const;

        void update_uniform(const std::function<void(scene_uniform_data&)>& func);

        std::shared_ptr<mesh_buffer> get_mesh_buffers(const std::string& mesh_path, const mesh& mesh);

        static gfx::pipeline_descriptor_set descriptor_set_definition();

        std::weak_ptr<material> load_material(const std::string& name);
        std::shared_ptr<mesh> load_mesh(const std::string& name);
        std::shared_ptr<texture> load_texture(const std::string& name);
        std::shared_ptr<script> load_script(const std::string& name);
        std::shared_ptr<font> load_font(const std::string& name);

        void load_nodes(std::vector<std::unique_ptr<scene_node>>&& root_nodes);

        void set_frame_point_light(const point_light_data& data);

        void set_frame_directional_light(const directional_light_data& data);

        void set_in_editor_mode(bool in_editor);
        bool in_editor_mode() const;

        std::vector<scene_node*> get_nodes_by_type(node_type type) const;

        double last_deltatime() const;

        const std::string& name() const { return _args.name; }

        void set_keyboard_input_interface(const std::shared_ptr<keyboard_input_interface>& i) { _keyboard_input = i; }

        std::shared_ptr<keyboard_input_interface> get_keyboard_input_interface() const { return _keyboard_input; }

        void set_mouse_input_interface(const std::shared_ptr<mouse_input_interface>& i) { _mouse_input = i; }

        std::shared_ptr<mouse_input_interface> get_mouse_input_interface() const { return _mouse_input; }

        void set_main_camera_3d_node(camera3d_node* node);
        camera3d_node* main_camera_3d_node() const;

        void draw_debug_line(std::vector<debug::line_vertex> vertices, double lifetime_seconds) const;

        const std::vector<std::unique_ptr<scene_node>>& get_node_siblings(const scene_node* node) const;
        const scene_node* get_node_sibling(const scene_node* node, const std::string& name) const;

        void reparent_node(const scene_node* node, scene_node* parent);

    private:
        scene_args _args;
        std::unique_ptr<gfx::uniform_buffer> _uniform_buffer;
        vk::UniqueDescriptorSetLayout _scene_descriptor_set_layout;
        vk::UniqueDescriptorSet _scene_descriptor_set;
        scene_uniform_data _scene_uniform_data;
        uint32_t _used_point_lights = 0;
        uint32_t _used_directional_lights = 0;
        bool _in_editor = false;
        double _last_deltatime = 0;

        std::vector<std::unique_ptr<scene_node>> _root_nodes;

        std::shared_ptr<keyboard_input_interface> _keyboard_input = nullptr;
        std::shared_ptr<mouse_input_interface> _mouse_input = nullptr;

        camera3d_node* _main_camera_3d = nullptr;

        scene_timepoint _previous_frame_timepoint;

        mesh_buffer_storage _mesh_buffer_storage;

        std::unique_ptr<debug::line_renderer> _debug_line_renderer;

        void init_descriptor_set_layout();
        void init_descriptor_set();
    };

} // namespace cathedral::engine