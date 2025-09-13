#pragma once

#include <cathedral/core.hpp>
#include <cathedral/gfx/pipeline.hpp>

#include <glm/vec4.hpp>

#include <memory>

FORWARD_CLASS(cathedral::engine, scene);

namespace cathedral::engine::debug
{
    struct line_vertex
    {
        glm::vec4 position = { 0, 0, 0, 0 };
        glm::vec4 color = { 1, 1, 1, 1 };
    };

    class line_renderer
    {
    public:
        explicit line_renderer(scene& scene);

    private:
        scene& _scene;
        std::unique_ptr<gfx::shader> _vertex_shader;
        std::unique_ptr<gfx::shader> _fragment_shader;
        std::unique_ptr<gfx::pipeline> _pipeline;

        void init_shaders();
        void init_pipeline();
    };
} // namespace cathedral::engine::debug_line