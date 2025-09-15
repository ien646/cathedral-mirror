#pragma once

#include <cathedral/core.hpp>
#include <cathedral/gfx/buffers/vertex_buffer.hpp>
#include <cathedral/gfx/pipeline.hpp>

#include <glm/vec4.hpp>

#include <memory>

FORWARD_CLASS(cathedral::engine, renderer);
FORWARD_CLASS(cathedral::engine, scene);

namespace cathedral::engine::debug
{
    struct line_vertex
    {
        glm::vec4 position = { 0, 0, 0, 0 };
        glm::vec4 color = { 1, 1, 1, 1 };
    };

    class line
    {
    public:
        explicit line(const renderer& renderer, std::vector<line_vertex> vertices);

        const gfx::vertex_buffer& vertex_buffer() const;

    private:
        std::unique_ptr<gfx::vertex_buffer> _vx_buffer;
    };

    class line_renderer
    {
    public:
        static constexpr double LIFETIME_ONE_FRAME = 0.0000001;

        explicit line_renderer(scene& scene);

        void add_line(std::vector<line_vertex> vertices, double lifetime = LIFETIME_ONE_FRAME);

        void render_tick(double deltatime);

        void pre_render_tick();

    private:
        scene& _scene;
        std::unique_ptr<gfx::shader> _vertex_shader;
        std::unique_ptr<gfx::shader> _fragment_shader;
        std::unique_ptr<gfx::pipeline> _pipeline;
        std::vector<std::pair<std::unique_ptr<gfx::vertex_buffer>, double>> _vx_buffers;

        void init_shaders();
        void init_pipeline();
    };
} // namespace cathedral::engine::debug