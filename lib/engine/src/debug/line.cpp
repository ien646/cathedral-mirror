#include <cathedral/engine/debug/line.hpp>

#include <cathedral/engine/renderer.hpp>
#include <cathedral/engine/scene.hpp>
#include <cathedral/memory.hpp>

#include <battery/embed.hpp>

#include <ranges>

namespace cathedral::engine::debug
{
    line_renderer::line_renderer(scene& scene)
        : _scene(scene)
    {
        init_shaders();
        init_pipeline();
    }

    void line_renderer::add_line(std::vector<line_vertex> vertices, double lifetime)
    {
        gfx::vertex_buffer_args buffer_args;
        buffer_args.size = vertices.size() * sizeof(line_vertex);
        buffer_args.vertex_size = sizeof(line_vertex);
        buffer_args.vkctx = &_scene.get_renderer().vkctx();

        const auto& emplace_result = _vx_buffers.emplace_back(std::make_unique<gfx::vertex_buffer>(buffer_args), lifetime);

        auto& upload_queue = _scene.get_renderer().get_upload_queue();
        upload_queue.update_buffer(*emplace_result.first, 0, std::as_bytes(std::span{ vertices }));
    }

    void line_renderer::render_tick(const double deltatime)
    {
        if (_vx_buffers.empty())
        {
            return;
        }

        const auto& cmdbuff = _scene.get_renderer().render_cmdbuff(render_domain::OVERLAY);

        cmdbuff.bindPipeline(vk::PipelineBindPoint::eGraphics, _pipeline->get());
        cmdbuff.bindDescriptorSets(
            vk::PipelineBindPoint::eGraphics,
            _pipeline->pipeline_layout(),
            0,
            _scene.descriptor_set(),
            {});

        for (const auto& vxbuff : _vx_buffers | std::views::keys)
        {
            cmdbuff.bindVertexBuffers(0, vxbuff->buffer(), { 0 });
            cmdbuff.draw(vxbuff->vertex_count(), 1, 0, 0);
        }

        for (auto& lifetime : _vx_buffers | std::views::values)
        {
            lifetime -= deltatime;
        }
    }

    void line_renderer::pre_render_tick()
    {
        std::erase_if(_vx_buffers, [](const auto& inst) {
            return inst.second <= 0;
        });
    }

    void line_renderer::init_shaders()
    {
        const auto vertex_source = b::embed<"engine/shaders/debug_line/vertex.glsl">().str();
        const auto fragment_source = b::embed<"engine/shaders/debug_line/fragment.glsl">().str();

        const std::vector<std::pair<std::string, std::string>> macros = {
            { "DIRECTIONAL_LIGHT_COUNT", std::to_string(CATHEDRAL_SCENE_MAX_DIRECTIONAL_LIGHTS) },
            { "POINT_LIGHT_COUNT", std::to_string(CATHEDRAL_SCENE_MAX_POINT_LIGHTS) }
        };

        gfx::shader_args vx_args;
        vx_args.type = gfx::shader_type::VERTEX;
        vx_args.source = vertex_source;
        vx_args.macro_definitions = macros;

        gfx::shader_args fg_args;
        fg_args.type = gfx::shader_type::FRAGMENT;
        fg_args.source = fragment_source;
        fg_args.macro_definitions = macros;

        _vertex_shader = std::make_unique<gfx::shader>(vx_args);
        _fragment_shader = std::make_unique<gfx::shader>(fg_args);

        _vertex_shader->compile();
        _fragment_shader->compile();

        if (!_vertex_shader->compilation_message().empty())
        {
            CRITICAL_ERROR("Invalid debug line vertex shader:" + _vertex_shader->compilation_message());
        }
        if (!_fragment_shader->compilation_message().empty())
        {
            CRITICAL_ERROR("Invalid debug line fragment shader:" + _fragment_shader->compilation_message());
        }
    }

    void line_renderer::init_pipeline()
    {
        const auto& renderer = _scene.get_renderer();

        gfx::vertex_input_description vertex_input_description;
        vertex_input_description.vertex_size = sizeof(line_vertex);
        vertex_input_description.attributes = {
            { gfx::vertex_input_attribute{ 0, 0, gfx::vertex_data_type::VEC4F },
              gfx::vertex_input_attribute{ 1, sizeof(glm::vec4), gfx::vertex_data_type::VEC4F } }
        };

        gfx::pipeline_args args;
        args.color_attachment_formats = { renderer.swapchain().swapchain_image_format() };
        args.color_blend_enable = true;
        args.cull_backfaces = false;
        args.depth_stencil_format = vk::Format::eD32SfloatS8Uint;
        args.descriptor_sets = { gfx::pipeline_descriptor_set{
            SCENE_DESCRIPTOR_SET_INDEX,
            gfx::descriptor_set_definition{
                { gfx::descriptor_set_entry{ SCENE_DESCRIPTOR_SET_INDEX, 0, gfx::descriptor_type::UNIFORM, 1 } } } } };
        args.enable_depth = false;
        args.enable_stencil = false;
        args.flip_front_face = false;
        args.fragment_shader = _fragment_shader.get();
        args.input_topology = vk::PrimitiveTopology::eLineList;
        args.line_width = 1.0F;
        args.msaa_sample_shading = false;
        args.msaa_samples = renderer.msaa_samples();
        args.polygon_mode = vk::PolygonMode::eLine;
        args.vertex_input = MOVE(vertex_input_description);
        args.vertex_shader = _vertex_shader.get();
        args.vkctx = &renderer.vkctx();

        _pipeline = std::make_unique<gfx::pipeline>(MOVE(args));
    }
} // namespace cathedral::engine::debug