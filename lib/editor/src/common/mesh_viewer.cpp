#include "cathedral/engine/nodes/point_light_node.hpp"

#include <cathedral/editor/common/mesh_viewer.hpp>

#include <cathedral/editor/utils.hpp>
#include <cathedral/editor/vulkan_widget.hpp>
#include <cathedral/engine/nodes/camera3d_node.hpp>
#include <cathedral/engine/nodes/mesh3d_node.hpp>
#include <cathedral/project/project.hpp>

#include <QTimer>
#include <QVBoxLayout>
#include <QWheelEvent>

#include <utility>

namespace cathedral::editor
{
    constexpr auto VERTEX_SHADER_SOURCE = R"glsl(
    $NODE_VARIABLE mat4 node_model_matrix;

    layout (location=0) out vec3 out_normal;
    layout (location=1) out vec3 out_fragpos;

    void main()
    {
        gl_Position = PROJECTION_3D * VIEW_3D * node_model_matrix * vec4(VERTEX_POSITION, 1.0);

        out_normal = normalize(mat3(transpose(inverse(node_model_matrix))) * VERTEX_NORMAL);

        out_fragpos = vec3(node_model_matrix * vec4(VERTEX_POSITION, 1.0));
    }
)glsl";

    constexpr auto FRAGMENT_SHADER_SOURCE = R"glsl(
    layout(location=0) in vec3 in_normal;
    layout(location=1) in vec3 in_fragpos;

    layout(location=0) out vec4 out_fragcolor;

    vec3 diffuse(vec3 frag_world_pos, vec3 frag_world_normal)
    {
	    vec3 result = vec3(0, 0, 0);
	    for(int i = 0; i < ENABLED_POINT_LIGHTS; ++i)
	    {
		    const vec3 light_dir = normalize(POINT_LIGHTS[i].position - frag_world_pos);
		    const float incidence = max(dot(frag_world_normal, light_dir), 0.0);

		    const float distance = abs(distance(frag_world_pos, POINT_LIGHTS[i].position));

		    float range_value = max((POINT_LIGHTS[i].range - distance) / POINT_LIGHTS[i].range, 0.0);
		    range_value = pow(range_value, POINT_LIGHTS[i].falloff_coefficient);

		    float applicable_intensity = range_value * POINT_LIGHTS[i].intensity * incidence;

		    result += POINT_LIGHTS[i].color * applicable_intensity;
	    }
	    return result;
    }

    void main()
    {
        out_fragcolor = vec4(diffuse(in_fragpos, in_normal) + AMBIENT_LIGHT, 1.0);
    }
)glsl";

    mesh_viewer::mesh_viewer(QWidget* parent)
        : QWidget(parent)
    {
        setMouseTracking(true);
        auto* layout = new QVBoxLayout;
        layout->setContentsMargins(0, 0, 0, 0);
        setLayout(layout);
    }

    void mesh_viewer::initialize(project::project* project, std::optional<std::string> mesh_name)
    {
        _vulkan_widget = new vulkan_widget(this->windowHandle(), this);

        layout()->addWidget(_vulkan_widget->get_widget());

        gfx::vulkan_context_args vkctx_args;
        vkctx_args.instance_extensions = get_vulkan_instance_extensions();
        vkctx_args.surface_retriever = [this](const vk::Instance inst) { return _vulkan_widget->init_surface(inst); };
        vkctx_args.surface_size_retriever = [this] {
            const auto& [x, y] = _vulkan_widget->get_window()->size().toSizeF() * devicePixelRatio();
            return glm::ivec2{ std::round(x), std::round(y) };
        };
        vkctx_args.validation_layers = is_debug_build();

        _vkctx = std::make_unique<gfx::vulkan_context>(vkctx_args);
        _swapchain = std::make_unique<gfx::swapchain>(*_vkctx, vk::PresentModeKHR::eFifo);

        engine::renderer_args renderer_args;
        renderer_args.swapchain = &*_swapchain;
        _renderer = std::make_unique<engine::renderer>(renderer_args);

        engine::scene_args scene_args;
        scene_args.name = "new scene";
        scene_args.prenderer = _renderer.get();
        scene_args.loaders = project->get_loader_funcs();
        _scene = std::make_unique<engine::scene>(std::move(scene_args));

        _node = _scene->add_root_node<engine::mesh3d_node>("root");

        engine::material_args material_args;
        material_args.vertex_shader_source = VERTEX_SHADER_SOURCE;
        material_args.fragment_shader_source = FRAGMENT_SHADER_SOURCE;
        material_args.domain = engine::material_domain::OPAQUE;
        material_args.material_uniform_bindings = {};
        material_args.name = "__mesh_viewer_material__";
        material_args.node_uniform_bindings = { { "node_model_matrix", engine::shader_node_uniform_binding::NODE_MODEL_MATRIX } };

        std::ignore = _renderer->create_material(material_args);
        _node->set_material(material_args.name);

        set_mesh(std::move(mesh_name));

        _update_timer = new QTimer(this);
        _update_timer->setSingleShot(false);
        _update_timer->setInterval(10);
        connect(_update_timer, &QTimer::timeout, [this] { tick(); });
        _update_timer->start();

        auto camera_node = _scene->add_root_node<engine::camera3d_node>("camera");
        camera_node->set_main_camera(true);
        camera_node->set_local_position({ 0.0F, 0.0F, -5.0F });

        _light_node = camera_node->add_child_node<engine::point_light_node>("light");
        _light_node->set_local_position({ 0.0F, 1.0F, 0.0F });
        _light_node->set_range(10.0F);

        _initialized = true;

        //QTimer::singleShot(100, [this] { resize(size() + QSize{ 0, 1 }); });

        connect(_vulkan_widget, &vulkan_widget::left_click_press, [this] { _left_click = true; });
        connect(_vulkan_widget, &vulkan_widget::right_click_press, [this] { _right_click = true; });
        connect(_vulkan_widget, &vulkan_widget::middle_click_press, [this] { _middle_click = true; });
        connect(_vulkan_widget, &vulkan_widget::left_click_release, [this] { _left_click = false; });
        connect(_vulkan_widget, &vulkan_widget::right_click_release, [this] { _right_click = false; });
        connect(_vulkan_widget, &vulkan_widget::middle_click_release, [this] { _middle_click = false; });
        connect(_vulkan_widget, &vulkan_widget::mouse_move, [this](const QPoint delta) { handle_mouse_move(delta); });
    }

    void mesh_viewer::set_mesh(const std::optional<std::string>& mesh_name) const
    {
        if (!mesh_name.has_value())
        {
            _node->set_mesh(std::nullopt);
        }
        else
        {
            if (_node)
            {
                _node->set_mesh(mesh_name);
            }
        }
    }

    void mesh_viewer::tick() const
    {
        _scene->tick([]([[maybe_unused]] double deltatime) {});
    }

    void mesh_viewer::handle_left_click()
    {
        if (_left_click || _right_click || _middle_click)
        {
            return;
        }
        _left_click = true;
    }

    void mesh_viewer::handle_right_click()
    {
        if (_left_click || _right_click || _middle_click)
        {
            return;
        }
        _right_click = true;
    }

    void mesh_viewer::handle_middle_click()
    {
        if (_left_click || _right_click || _middle_click)
        {
            return;
        }
        _middle_click = true;
    }

    void mesh_viewer::handle_mouse_move(const QPoint delta) const
    {
        if (_left_click)
        {
            constexpr auto DELTA_TO_DEGREES = 0.25F;
            _node->rotate_degrees(
                { DELTA_TO_DEGREES * static_cast<float>(-delta.y()),
                  DELTA_TO_DEGREES * static_cast<float>(-delta.x()),
                  0.0F });
        }
        else if (_right_click)
        {
            constexpr auto DELTA_TO_POSITION = 0.05F;
            _light_node->translate(
                { DELTA_TO_POSITION * static_cast<float>(delta.x()),
                  DELTA_TO_POSITION * static_cast<float>(-delta.y()),
                  0.0F });
        }
        else if (_middle_click)
        {
            constexpr auto DELTA_TO_INTENSITY = 0.05F;
            _light_node->set_intensity(_light_node->intensity() + (DELTA_TO_INTENSITY * delta.x()));
            _light_node->set_range(_light_node->range() + (DELTA_TO_INTENSITY * -delta.y()));
        }
    }

    void mesh_viewer::closeEvent(QCloseEvent* event)
    {
        _update_timer->stop();

        _renderer->vkctx().device().waitIdle();
        _swapchain.reset();
        _renderer.reset();

        QWidget::closeEvent(event);
    }

    void mesh_viewer::resizeEvent(QResizeEvent* event)
    {
        QWidget::resizeEvent(event);
        if (_initialized)
        {
            _renderer->vkctx().device().waitIdle();
            _vulkan_widget->get_window()->resize(size());
            _vulkan_widget->get_widget()->resize(size());
            _swapchain->recreate();
            _renderer->recreate_swapchain_dependent_resources();
        }
    }

    void mesh_viewer::wheelEvent(QWheelEvent* event)
    {
        QWidget::wheelEvent(event);
        if (!_initialized)
        {
            return;
        }

        const auto delta = static_cast<float>(event->angleDelta().y()) / 500.0F;
        _node->translate({ 0.0F, 0.0F, delta });
    }

    void mesh_viewer::showEvent(QShowEvent* event)
    {
        QWidget::showEvent(event);
        if (_initialized)
        {
            _renderer->vkctx().device().waitIdle();
            _vulkan_widget->get_window()->resize(size());
            _vulkan_widget->get_widget()->resize(size());
            _swapchain->recreate();
            _renderer->recreate_swapchain_dependent_resources();
        }
    }
} // namespace cathedral::editor