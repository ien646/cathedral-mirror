#include <cathedral/engine/materials/world_geometry.hpp>

#include <cathedral/engine/vertex_input_builder.hpp>

namespace cathedral::engine
{
    world_geometry_material::world_geometry_material(world_geometry_material_args args)
        : _args(args)
    {
        init_pipeline();
    }

    void world_geometry_material::init_pipeline()
    {
        const auto vertex_input_attrs =
            vertex_input_builder()
                .push(gfx::vertex_data_type::VEC3F) // 0-pos
                .push(gfx::vertex_data_type::VEC2F) // 1-uv
                .push(gfx::vertex_data_type::VEC3F) // 2-normal
                .push(gfx::vertex_data_type::VEC4F) // 3-color
                .build();

        gfx::pipeline_args args;
        args.vertex_shader = _args.vertex_shader;
        args.fragment_shader = _args.fragment_shader;
        args.color_attachment_formats = { _args.color_attachment_format };
        args.color_blend_enable = true;
        args.depth_stencil_format = _args.depth_attachment_format;
        args.enable_depth = true;
        args.enable_stencil = false;
        args.cull_backfaces = false;
        args.descriptor_sets = {};
        args.input_topology = vk::PrimitiveTopology::eTriangleList;
        args.line_width = 1.0f;
        args.polygon_mode = vk::PolygonMode::eFill;
        args.vertex_input = gfx::vertex_input_description((3 + 2 + 3 + 4) * sizeof(float), vertex_input_attrs);
        args.vkctx = _args.vkctx;

        _pipeline = std::make_unique<gfx::pipeline>(args);
    }
} // namespace cathedral::engine