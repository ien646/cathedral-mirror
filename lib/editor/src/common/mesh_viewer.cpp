#include <cathedral/editor/common/mesh_viewer.hpp>

#include <cathedral/editor/utils.hpp>
#include <cathedral/editor/vulkan_widget.hpp>
#include <cathedral/engine/nodes/camera3d_node.hpp>
#include <cathedral/engine/nodes/mesh3d_node.hpp>
#include <cathedral/project/project.hpp>

#include <QTimer>

#include <utility>

namespace cathedral::editor
{
    constexpr auto vertex_shader_source = R"glsl(
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

    constexpr auto fragment_shader_source = R"glsl(
    $NODE_VARIABLE vec3 light_position;

    const vec3 ambient_light = vec3(0.25, 0.25, 0.25);
    const vec3 light_color = vec3(1.0, 1.0, 1.0);

    layout(location=0) in vec3 in_normal;
    layout(location=1) in vec3 in_fragpos;

    layout(location=0) out vec4 out_fragcolor;

    vec3 calculate_diffuse(vec3 light_direction, vec3 normal)
    {
        float intensity = max(dot(normal, light_direction), 0.0);
        return light_color * intensity;
    }

    void main()
    {
        vec3 light_direction = normalize(light_position - in_fragpos);
        out_fragcolor = vec4(calculate_diffuse(light_direction, in_normal) + ambient_light, 1.0);
    }
)glsl";

    mesh_viewer::mesh_viewer(QWidget* parent)
        : QWidget(parent)
    {
        setMouseTracking(true);
    }

    void mesh_viewer::initialize(project::project* project, std::optional<std::string> mesh_name)
    {
        _vulkan_widget = new vulkan_widget(this->windowHandle(), this);

        gfx::vulkan_context_args vkctx_args;
        vkctx_args.instance_extensions = get_vulkan_instance_extensions();
        vkctx_args.surface_retriever = [this](const vk::Instance inst) { return _vulkan_widget->init_surface(inst); };
        vkctx_args.surface_size_retriever = [this] {
            const auto ratio = devicePixelRatio();
            const auto size = glm::ivec2{ _vulkan_widget->get_widget()->size().width() * ratio,
                                          _vulkan_widget->get_widget()->size().height() * ratio };
            return size;
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
        material_args.vertex_shader_source = vertex_shader_source;
        material_args.fragment_shader_source = fragment_shader_source;
        material_args.domain = engine::material_domain::OPAQUE;
        material_args.material_bindings = {};
        material_args.name = "__mesh_viewer_material__";
        material_args.node_bindings = { { engine::shader_node_uniform_binding::NODE_MODEL_MATRIX, "node_model_matrix" } };

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

        _initialized = true;

        QTimer::singleShot(100, [this] { resize(size() - QSize{ 1, 0 }); });
    }

    void mesh_viewer::set_mesh(std::optional<std::string> mesh_name) const
    {
        if (!mesh_name.has_value())
        {
            _node->set_mesh(std::nullopt);
        }
        else
        {
            _node->set_mesh(*mesh_name);
        }
    }

    void mesh_viewer::tick() const
    {
        _scene->tick([]([[maybe_unused]] double deltatime) {});
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
        if (_initialized)
        {
            _renderer->vkctx().device().waitIdle();
            _vulkan_widget->get_window()->resize(size());
            _vulkan_widget->get_widget()->resize(size());
            _swapchain->recreate();
            _renderer->recreate_swapchain_dependent_resources();
        }
        QWidget::resizeEvent(event);
    }
} // namespace cathedral::editor