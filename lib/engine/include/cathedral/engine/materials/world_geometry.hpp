#pragma once

#include <cathedral/gfx/pipeline.hpp>

namespace cathedral::engine
{
    struct world_geometry_material_args
    {
        const gfx::vulkan_context* vkctx = nullptr;
        const gfx::shader* vertex_shader = nullptr;
        const gfx::shader* fragment_shader = nullptr;
        vk::Format color_attachment_format = vk::Format::eUndefined;
        vk::Format depth_attachment_format = vk::Format::eUndefined;
    };

    class world_geometry_material
    {
    public:
        world_geometry_material(world_geometry_material_args args);

        inline const gfx::pipeline& pipeline() const { return *_pipeline; }

    private:
        world_geometry_material_args _args;
        std::unique_ptr<gfx::pipeline> _pipeline;

        void init_pipeline();
    };
}