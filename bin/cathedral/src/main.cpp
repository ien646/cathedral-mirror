#include <QApplication>
#include <QStyle>
#include <QStyleFactory>
#include <QStyleHints>

#include <ien/platform.hpp>

#include <cathedral/editor/editor_window.hpp>
#include <cathedral/editor/styling.hpp>
#include <cathedral/editor/welcome_dialog.hpp>

#include <cathedral/engine/material.hpp>

#include <cathedral/engine/nodes/camera3d_node.hpp>
#include <cathedral/engine/nodes/mesh3d_node.hpp>

#include <cathedral/engine/scene.hpp>

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

    layout(set=2, binding=0) uniform node_uniform_data_t
    {
        mat4 model;
    } node_uniform_data;

    layout(location = 0) out vec4 out_color;
    layout(location = 1) out vec2 out_uv;

    void main()
    {
        gl_Position = scene_uniform_data.projection3d * scene_uniform_data.view3d * node_uniform_data.model * vec4(vx_pos, 1.0);
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

    vec4 colors[8] = vec4[8](
        vec4(1.0, 1.0, 1.0, 1.0),
        vec4(1.0, 0.0, 0.0, 1.0),
        vec4(0.0, 1.0, 0.0, 1.0),
        vec4(0.0, 0.0, 1.0, 1.0),
        vec4(1.0, 1.0, 0.0, 1.0),
        vec4(1.0, 0.0, 1.0, 1.0),
        vec4(0.0, 1.0, 1.0, 1.0),
        vec4(0.5, 0.5, 0.5, 1.0)
    );

    void main()
    {
        out_color = in_color * texture(tex, in_uv);
    }

)glsl";

int main(int argc, char** argv)
{
#ifdef IEN_OS_WIN
    std::string project_path = "C:\\Users\\Ien\\Documents\\cathedral\\test-project";
#else
    std::string project_path = "/home/ien/Projects/cathedral/test-project";
#endif

#ifndef IEN_OS_WIN
    qputenv("QT_QPA_PLATFORM", "xcb");
#endif

    CATHEDRAL_EDITOR_INITIALIZE();

    QApplication qapp(argc, argv);

    QApplication::setPalette(editor::get_editor_palette());
    QApplication::setStyle(editor::get_editor_style());
    qapp.setStyleSheet(editor::get_editor_stylesheet());

    QApplication::setFont(QFont("monospace", 8));

    auto* welcome_window = new editor::welcome_dialog();
    if (welcome_window->exec() == 0)
    {
        return 0;
    }

    auto* win = new editor::editor_window(welcome_window->project());
    win->show();

    QApplication::processEvents();
    win->initialize_vulkan();

    auto& renderer = win->renderer();
    auto& scene = win->scene();

    win->swapchain().set_present_mode(vk::PresentModeKHR::eMailbox);

    QApplication::processEvents();
    while (true)
    {
        QApplication::processEvents();

        if (!win->isVisible())
        {
            return 0;
        }
        scene.tick([&](double deltatime) {});
    }
}