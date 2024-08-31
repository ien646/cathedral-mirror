#include <QApplication>
#include <QStyle>
#include <QStyleFactory>
#include <QVulkanInstance>
#include <QWindow>

#include <ien/platform.hpp>

#include <cathedral/editor/editor_window.hpp>

#include <cathedral/engine/materials/world_geometry.hpp>

#include <cathedral/engine/nodes/camera3d_node.hpp>
#include <cathedral/engine/nodes/mesh3d_node.hpp>

#include <cathedral/engine/scene.hpp>

#include <cathedral/gfx/vulkan_context.hpp>

using namespace cathedral;

const std::string vertex_shader_source = R"glsl(
    #version 450

    layout(location = 0) in vec3 vx_pos;
    layout(location = 1) in vec2 vx_uv;
    layout(location = 2) in vec3 vx_normal;
    layout(location = 3) in vec4 vx_color;

    layout(set=0, binding=0) uniform scene_uniform_data_t
    {
        float deltatime;
        uint frame_index;
        mat4 projection3d;
        mat4 view3d;
    } scene_uniform_data;

    layout(set=1, binding=0) uniform material_uniform_data_t
    {
        vec4 tint;
    } material_uniform_data;

    layout(set=2, binding=0) uniform drawable_uniform_data_t
    {
        mat4 model;
    } drawable_uniform_data;

    layout(location = 0) out vec4 out_color;
    layout(location = 1) out vec2 out_uv;

    void main()
    {
        gl_Position = scene_uniform_data.projection3d * scene_uniform_data.view3d * drawable_uniform_data.model * vec4(vx_pos, 1.0);
        out_color = vx_color * material_uniform_data.tint;
        out_uv = vx_uv;
    }

)glsl";

const std::string fragment_shader_source = R"glsl(
    #version 450

    layout(location = 0) in vec4 in_color;
    layout(location = 1) in vec2 in_uv;

    layout(location = 0) out vec4 out_color;

    layout(set = 1, binding = 1) uniform sampler2D tex;

    void main()
    {
        out_color = in_color * texture(tex, in_uv);
    }

)glsl";

int main(int argc, char** argv)
{
    qputenv("QT_QPA_PLATFORM", "xcb");
    QApplication qapp(argc, argv);
    qapp.setStyle("windows");
    qapp.setFont(QFont("monospace", 8));
    auto* win = new editor::editor_window();
    win->show();

    QApplication::processEvents();
    win->initialize_vulkan();

    auto& renderer = win->renderer();
    auto& scene = win->scene();

    win->swapchain().set_present_mode(vk::PresentModeKHR::eMailbox);

    gfx::shader vertex_shader = renderer.create_vertex_shader(vertex_shader_source);
    gfx::shader fragment_shader = renderer.create_fragment_shader(fragment_shader_source);

    ien::image funny("/home/ien/Desktop/memes/unknown.png");
    engine::texture_args tex_args;
    tex_args.image_aspect_flags = vk::ImageAspectFlagBits::eColor;
    tex_args.pimage = &funny;
    tex_args.sampler_args.vkctx = &renderer.vkctx();
    tex_args.mipmap_generation_filter = vk::Filter::eLinear;
    tex_args.mipmap_levels = 4;
    engine::texture tex(tex_args, renderer.get_upload_queue());

    auto& material = renderer.create_world_geometry_material("mat", vertex_shader, fragment_shader, 1);
    material.bind_material_texture_slot(tex, 0);

    auto mesh_buffers = scene.get_mesh_buffers("rsc/meshes/cube.ply");

    auto node0 = scene.add_root_node<engine::mesh3d_node>("test0");
    node0->set_mesh(mesh_buffers);
    node0->set_material(&material);
    node0->set_local_scale({ 1.0f, 1.0f, 1.0f });
    node0->set_local_position({ 0.0f, 0.0f, 1.0f });

    auto node1 = scene.add_root_node<engine::mesh3d_node>("test1");
    node1->set_mesh(mesh_buffers);
    node1->set_material(&material);
    node1->set_local_scale({ 1.0f, 1.0f, 1.0f });
    node1->set_local_position({ -1.0f, 0.0f, 1.0f });

    auto test_child = node1->add_child_node<engine::node>("test_child");

    auto camera = scene.add_root_node<engine::camera3d_node>("camera");
    camera->set_main_camera(true);
    camera->set_local_position({ 0.0f, 0.0f, -10.0f });
    camera->set_local_rotation({ 0.0f, 180.0f, 0.0f });

    QApplication::processEvents();
    while (true)
    {
        switch (renderer.current_frame() % 2)
        {
        case 0:
            QApplication::processEvents(QEventLoop::ProcessEventsFlag::ExcludeUserInputEvents);
            break;
        case 1:
            QApplication::processEvents();
            break;
        default:
            break;
        }

        if (!win->isVisible())
        {
            return 0;
        }
        scene.tick([&](double deltatime) {
            node0->set_local_rotation(node0->local_rotation() + glm::vec3{ 0.0f, deltatime * 20, 0.0f });
        });
    }
}