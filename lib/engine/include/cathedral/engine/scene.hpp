#pragma once

#include <cathedral/engine/aligned_uniform.hpp>
#include <cathedral/engine/material.hpp>
#include <cathedral/engine/renderer.hpp>
#include <cathedral/engine/scene_node.hpp>
#include <cathedral/engine/vertex_buffer_storage.hpp>
#include <cathedral/gfx/pipeline.hpp>

#include <chrono>
#include <unordered_set>

namespace cathedral::engine
{
    struct scene_uniform_data
    {
        CATHEDRAL_ALIGNED_UNIFORM(float, deltatime) = 0.0;
        CATHEDRAL_ALIGNED_UNIFORM(uint32_t, frame_index) = 0;
        CATHEDRAL_ALIGNED_UNIFORM(glm::mat4, projection3d) = glm::mat4(1.0f);
        CATHEDRAL_ALIGNED_UNIFORM(glm::mat4, view3d) = glm::mat4(1.0f);

        bool operator==(const scene_uniform_data& rhs) const = default;
    };

    using scene_clock = std::chrono::high_resolution_clock;
    using scene_timepoint = scene_clock::time_point;

    class scene
    {
    public:
        scene(renderer& renderer);

        inline const gfx::uniform_buffer& uniform_buffer() const { return *_uniform_buffer; }
        inline renderer& get_renderer() { return _renderer; }

        vk::DescriptorSet descriptor_set() const;

        void tick(std::function<void(double deltatime)>);

        template <typename T>
            requires(std::is_base_of_v<scene_node, T>)
        std::shared_ptr<T> add_root_node(const std::string& name)
        {
            auto node = std::make_shared<T>(*this, name, nullptr);
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

        inline void register_material(material* mat) { _registered_materials.emplace(mat); }

        inline const std::unordered_map<std::string, std::shared_ptr<scene_node>>& root_nodes() const
        {
            return _root_nodes;
        }

        void update_uniform(std::function<void(scene_uniform_data&)> func);

        std::shared_ptr<gfx::vertex_buffer> get_vertex_buffer(const std::string& mesh_path);

        static gfx::pipeline_descriptor_set descriptor_set_definition();

    private:
        renderer& _renderer;
        std::unique_ptr<gfx::uniform_buffer> _uniform_buffer;
        vk::UniqueDescriptorSetLayout _descriptor_set_layout;
        vk::UniqueDescriptorSet _descriptor_set;
        scene_uniform_data _uniform_data;

        std::unordered_map<std::string, std::shared_ptr<scene_node>> _root_nodes;
        std::unordered_set<material*> _registered_materials;

        scene_timepoint _previous_frame_timepoint;

        vertex_buffer_storage _vxbuff_storage;

        void init_descriptor_set_layout();
        void init_descriptor_set();
    };
} // namespace cathedral::engine