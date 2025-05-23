#pragma once

#include <cathedral/gfx/descriptor_set_definition.hpp>
#include <cathedral/gfx/shader.hpp>
#include <cathedral/gfx/types.hpp>

#include <unordered_map>

namespace cathedral::gfx
{
    FORWARD_CLASS_INLINE(vulkan_context);

    struct pipeline_descriptor_set
    {
        uint32_t set_index = 0;
        descriptor_set_definition definition;
    };

    struct pipeline_args
    {
        const vulkan_context* vkctx = nullptr;
        bool color_blend_enable = true;
        bool enable_depth = true;
        bool enable_stencil = true;
        vk::PrimitiveTopology input_topology = vk::PrimitiveTopology::eTriangleList;
        vk::PolygonMode polygon_mode = vk::PolygonMode::eFill;
        bool cull_backfaces = true;
        float line_width = 1.0F;
        vertex_input_description vertex_input;
        std::vector<pipeline_descriptor_set> descriptor_sets;
        const shader* vertex_shader = nullptr;
        const shader* fragment_shader = nullptr;
        std::vector<vk::Format> color_attachment_formats;
        vk::Format depth_stencil_format = vk::Format::eUndefined;
    };

    class pipeline
    {
    public:
        explicit pipeline(pipeline_args);

        vk::PipelineLayout pipeline_layout() const { return *_layout; }

        vk::Pipeline get() const { return *_pipeline; }

        bool has_descriptor_set_index(uint32_t set_index) const { return _descriptor_set_layouts.contains(set_index); }

        vk::DescriptorSetLayout descriptor_set_layout(uint32_t set_index) const
        {
            CRITICAL_CHECK(
                _descriptor_set_layouts.count(set_index),
                "Attempt to get descriptor set layout with non-existing set index");
            return *_descriptor_set_layouts.at(set_index);
        }

        const std::unordered_map<uint32_t, vk::UniqueDescriptorSetLayout>& descriptor_set_layouts() const
        {
            return _descriptor_set_layouts;
        }

    private:
        pipeline_args _args;
        vk::UniquePipelineLayout _layout;
        std::unordered_map<uint32_t, vk::UniqueDescriptorSetLayout> _descriptor_set_layouts;
        vk::UniquePipeline _pipeline;
    };
} // namespace cathedral::gfx