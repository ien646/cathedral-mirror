#pragma once

#include <cathedral/engine/material.hpp>
#include <cathedral/engine/mesh_buffer_storage.hpp>
#include <cathedral/engine/point_light.hpp>
#include <cathedral/engine/renderer.hpp>
#include <cathedral/engine/scene_node.hpp>

#include <cathedral/gfx/aligned_uniform.hpp>
#include <cathedral/gfx/pipeline.hpp>

#include <chrono>

namespace cathedral::engine
{
    constexpr auto MAX_SCENE_POINT_LIGHTS = 20;

    struct scene_uniform_data
    {
        CATHEDRAL_ALIGNED_UNIFORM(float, deltatime) = 0.0;
        CATHEDRAL_ALIGNED_UNIFORM(uint32_t, frame_index) = 0;
        CATHEDRAL_ALIGNED_UNIFORM(glm::mat4, projection3d) = glm::mat4(1.0F);
        CATHEDRAL_ALIGNED_UNIFORM(glm::mat4, view3d) = glm::mat4(1.0F);
        CATHEDRAL_ALIGNED_UNIFORM(point_light_data, point_lights)[MAX_SCENE_POINT_LIGHTS]; // nolint
    };

    // ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
    // --- GOD HELP YOU IF THESE TWO DON'T MATCH ---
    // vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv

    const std::string scene_uniform_glslstr = R"glsl(

struct scene_point_light
{
    bool enabled;
    vec3 position;
    vec3 color;
    float range;
    float falloff_coefficient;
};

layout(set = 0, binding = 0) uniform _scene_uniform_data_ {
    float deltatime;
    uint frame_index;
    mat4 projection3d;
    mat4 view3d;
    scene_point_light point_lights[20];
} scene_uniform_data;

#define DELTATIME scene_uniform_data.deltatime
#define FRAME_INDEX scene_uniform_data.frame_index
#define PROJECTION_3D scene_uniform_data.projection3d
#define VIEW_3D scene_uniform_data.view3d
#define POINT_LIGHTS scene_uniform_data.point_lights
)glsl";

    using scene_clock = std::chrono::high_resolution_clock;
    using scene_timepoint = scene_clock::time_point;

    template <typename T>
    using loader_func = std::function<std::shared_ptr<T>(const std::string& name, scene& scn)>;

    template<typename T>
    using weak_loader_func = std::function<std::weak_ptr<T>(const std::string& name, scene& scn)>;

    struct scene_loader_funcs
    {
        weak_loader_func<material> material_loader = nullptr;
        loader_func<mesh> mesh_loader = nullptr;
        loader_func<texture> texture_loader = nullptr;
    };

    struct scene_args
    {
        renderer* prenderer = nullptr;
        scene_loader_funcs loaders;
    };

    class scene
    {
    public:
        explicit scene(scene_args args);
        virtual ~scene();
        CATHEDRAL_NON_COPYABLE(scene);
        CATHEDRAL_DEFAULT_MOVABLE(scene);

        const gfx::uniform_buffer& uniform_buffer() const { return *_uniform_buffer; }

        renderer& get_renderer() const { return *_args.prenderer; }

        vk::DescriptorSet descriptor_set() const;

        void tick(const std::function<void(double deltatime)>&);

        template <typename T>
            requires(std::is_base_of_v<scene_node, T>)
        std::shared_ptr<T> add_root_node(const std::string& name)
        {
            auto node = std::make_shared<T>(name, nullptr);
            _root_nodes.emplace_back(node);
            return node;
        }

        std::shared_ptr<engine::scene_node> add_root_node(const std::string& name, node_type type);

        std::shared_ptr<scene_node> get_node(const std::string& name);

        void remove_node(const std::string& name);

        bool contains_node(const std::string& name) const;

        template <typename T>
            requires(std::is_base_of_v<scene_node, T>)
        std::shared_ptr<T> get_node(const std::string& name)
        {
            return std::dynamic_pointer_cast<T>(get_node(name));
        }

        const auto& root_nodes() const { return _root_nodes; }

        void update_uniform(const std::function<void(scene_uniform_data&)>& func);

        std::shared_ptr<mesh_buffer> get_mesh_buffers(const std::string& mesh_path, const engine::mesh& mesh);

        static gfx::pipeline_descriptor_set descriptor_set_definition();

        std::weak_ptr<engine::material> load_material(const std::string& name);
        std::shared_ptr<engine::mesh> load_mesh(const std::string& name);
        std::shared_ptr<engine::texture> load_texture(const std::string& name);

        void load_nodes(std::vector<std::shared_ptr<scene_node>>&& nodes);

    private:
        scene_args _args;
        std::unique_ptr<gfx::uniform_buffer> _uniform_buffer;
        vk::UniqueDescriptorSetLayout _scene_descriptor_set_layout;
        vk::UniqueDescriptorSet _scene_descriptor_set;
        scene_uniform_data _scene_uniform_data;

        std::vector<std::shared_ptr<scene_node>> _root_nodes;

        scene_timepoint _previous_frame_timepoint;

        mesh_buffer_storage _mesh_buffer_storage;

        void init_descriptor_set_layout();
        void init_descriptor_set();

        void reload_tree_parenting();
    };
} // namespace cathedral::engine