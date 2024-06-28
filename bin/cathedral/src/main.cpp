#include <SDL2/SDL.h>

#include <QApplication>
#include <QStyle>
#include <QStyleFactory>
#include <QVulkanInstance>
#include <QWindow>

#include <ien/platform.hpp>

#include <cathedral/editor/editor_window.hpp>
#include <cathedral/engine/materials/world_geometry.hpp>
#include <cathedral/gfx/vulkan_context.hpp>

using namespace cathedral;

const std::string vertex_shader_source = R"glsl(
    #version 440

    layout(location = 0) in vec3 vx_pos;
    layout(location = 1) in vec2 vx_uv;
    layout(location = 2) in vec3 vx_normal;
    layout(location = 3) in vec4 vx_color;

    layout(location = 0) out vec4 out_color;

    void main()
    {
        gl_Position = vec4(vx_pos, 1.0);
        out_color = vx_color;
    }

)glsl";

const std::string fragment_shader_source = R"glsl(
    #version 440

    layout(location = 0) in vec4 in_color;

    layout(location = 0) out vec4 out_color;

    void main()
    {
        out_color = in_color;
    }

)glsl";

// clang-format off
const std::vector<float> triangle_vertices = {
    -0.5f, -0.5f, 1.0f,     0.0f, 0.0f,     0.0f, 0.0f, 0.0f,       1.0f, 0.0f, 0.0f, 1.0f,
     0.5f, -0.5f, 1.0f,     0.0f, 0.0f,     0.0f, 0.0f, 0.0f,       0.0f, 1.0f, 0.0f, 1.0f,
     0.0f,  0.5f, 1.0f,     0.0f, 0.0f,     0.0f, 0.0f, 0.0f,       0.0f, 0.0f, 1.0f, 1.0f,
};
// clang-format on

int main(int argc, char** argv)
{
    QApplication qapp(argc, argv);
    editor::editor_window* win = new editor::editor_window();
    win->show();

    auto& renderer = win->renderer();
    auto render_cmdbuff = renderer.render_cmdbuff();

    gfx::shader vertex_shader = renderer.create_vertex_shader(vertex_shader_source);
    gfx::shader fragment_shader = renderer.create_fragment_shader(fragment_shader_source);

    engine::world_geometry_material_args mat_args;
    mat_args.color_attachment_format = win->swapchain().swapchain_image_format();
    mat_args.depth_attachment_format = renderer.depthstencil_attachment().format();
    mat_args.fragment_shader = &fragment_shader;
    mat_args.vertex_shader = &vertex_shader;
    mat_args.vkctx = &renderer.vkctx();

    engine::world_geometry_material material(mat_args);

    gfx::vertex_buffer_args vxbuff_args;
    vxbuff_args.vertex_size = (3 + 2 + 3 + 4) * sizeof(float);
    vxbuff_args.size = triangle_vertices.size() * sizeof(float);
    vxbuff_args.vkctx = &renderer.vkctx();

    gfx::vertex_buffer vxbuff(vxbuff_args);

    renderer.get_upload_queue()
        .update_buffer(vxbuff, 0, triangle_vertices.data(), triangle_vertices.size() * sizeof(float));

    while (true)
    {
        QApplication::processEvents();
        if (!win->isVisible())
        {
            win->renderer().vkctx().device().waitIdle();
            break;
        }
        win->tick([&] {
            render_cmdbuff.bindPipeline(vk::PipelineBindPoint::eGraphics, material.pipeline().get());
            render_cmdbuff.bindVertexBuffers(0, vxbuff.buffer(), { 0 });
            render_cmdbuff.draw(3, 1, 0, 0);
        });
    }
}