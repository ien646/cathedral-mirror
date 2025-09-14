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
        explicit line_renderer(scene& scene);

        void draw(const line& ln) const;

    private:
        scene& _scene;
        std::unique_ptr<gfx::shader> _vertex_shader;
        std::unique_ptr<gfx::shader> _fragment_shader;
        std::unique_ptr<gfx::pipeline> _pipeline;

        void init_shaders();
        void init_pipeline();
        void init_descriptor_set();
    };
} // namespace cathedral::engine::debug