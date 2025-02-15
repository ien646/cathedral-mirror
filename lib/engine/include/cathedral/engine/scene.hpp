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
        CATHEDRAL_ALIGNED_UNIFORM(glm::mat4, projection3d) = glm::mat4(1.0f);
        CATHEDRAL_ALIGNED_UNIFORM(glm::mat4, view3d) = glm::mat4(1.0f);
        CATHEDRAL_ALIGNED_UNIFORM(point_light_data, point_lights)[MAX_SCENE_POINT_LIGHTS];
    };

    // ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
    // --- GOD HELP YOU IF THESE TWO DON'T MATCH ---
    // vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
    
    constexpr const char* SCENE_UNIFORM_GLSLSTR = R"glsl(

struct scene_point_light
{
    bool enabled;
    vec3 position;
    vec3 color;
    float range;
    float falloff_coefficient;
};

layout(set = 0, binding = 0) uniform _scene_uniform_data {
    float deltatime;
    uint frame_index;
    mat4 projection3d;
    mat4 view3d;
    scene_point_light point_lights[20];
};)glsl";

    using scene_clock = std::chrono::high_resolution_clock;
    using scene_timepoint = scene_clock::time_point;

    class scene
    {
    public:
        explicit scene(renderer& renderer);

        const gfx::uniform_buffer& uniform_buffer() const { return *_uniform_buffer; }

        renderer& get_renderer() { return _renderer; }

        vk::DescriptorSet descriptor_set() const;

        void tick(const std::function<void(double deltatime)>&);

        template <typename T>
            requires(std::is_base_of_v<scene_node, T>)
        std::shared_ptr<T> add_root_node(const std::string& name)
        {
            auto node = std::make_shared<T>(name, nullptr);
            _root_nodes.emplace(name, node);
            return node;
        }

        std::shared_ptr<scene_node> get_node(const std::string& name);

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

    private:
        renderer& _renderer;
        std::unique_ptr<gfx::uniform_buffer> _uniform_buffer;
        vk::UniqueDescriptorSetLayout _descriptor_set_layout;
        vk::UniqueDescriptorSet _descriptor_set;
        scene_uniform_data _uniform_data;

        std::unordered_map<std::string, std::shared_ptr<scene_node>> _root_nodes;

        scene_timepoint _previous_frame_timepoint;

        mesh_buffer_storage _mesh_buffer_storage;

        void init_descriptor_set_layout();
        void init_descriptor_set();
    };
} // namespace cathedral::engine