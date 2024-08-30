#include <cathedral/editor/editor_window.hpp>

#include <cathedral/editor/asset_managers/shader_manager.hpp>
#include <cathedral/editor/common/message.hpp>

#include <QFileDialog>
#include <QMessageBox>
#include <QMouseEvent>
#include <QVulkanInstance>
#include <QWindow>

#include <ien/fs_utils.hpp>
#include <ien/platform.hpp>

// clang-format off
#if defined(IEN_OS_WIN)
    #include <vulkan/vulkan_win32.h>
    const std::vector<const char*> get_instance_extensions()
    {
        return { VK_KHR_WIN32_SURFACE_EXTENSION_NAME };
    };
#elif defined(IEN_OS_LINUX)
    #include <vulkan/vulkan_wayland.h>
    #include <xcb/xcb.h>
    #include <vulkan/vulkan_xcb.h>
    
    const std::vector<const char*> get_instance_extensions() 
    {
        if(qgetenv("QT_QPA_PLATFORM") == "xcb")
        {
            return {VK_KHR_XCB_SURFACE_EXTENSION_NAME};
        }
        else
        {
            return {VK_KHR_WAYLAND_SURFACE_EXTENSION_NAME};
        }
    };
#endif
// clang-format on

namespace cathedral::editor
{
    editor_window::editor_window()
    {
        _menubar = new editor_window_menubar(this);
        setMenuBar(_menubar);

        _scene_dock = new scene_dock_widget(this);
        _scene_dock->setAllowedAreas(Qt::DockWidgetArea::LeftDockWidgetArea);
        _scene_dock->setFeatures(QDockWidget::DockWidgetFeature::NoDockWidgetFeatures);
        _scene_dock->setMinimumWidth(200);
        addDockWidget(Qt::DockWidgetArea::LeftDockWidgetArea, _scene_dock);

        _props_dock = new properties_dock_widget(this);
        _props_dock->setAllowedAreas(Qt::DockWidgetArea::RightDockWidgetArea);
        _props_dock->setFeatures(QDockWidget::DockWidgetFeature::NoDockWidgetFeatures);
        _props_dock->setMinimumWidth(200);
        addDockWidget(Qt::DockWidgetArea::RightDockWidgetArea, _props_dock);

        connect(_scene_dock, &scene_dock_widget::node_selected, this, [this](engine::scene_node* node) {
            if (auto* mesh = dynamic_cast<engine::mesh3d_node*>(node))
            {
                _props_dock->set_node(mesh);
            }
            else if (auto* camera = dynamic_cast<engine::camera3d_node*>(node))
            {
                _props_dock->set_node(camera);
            }
            else
            {
                _props_dock->clear_node();
            }
        });

        setup_menubar_connections();

        _project = std::make_unique<project::project>();
    }

    void editor_window::tick(std::function<void(double)> tick_work)
    {
        _scene->tick(tick_work);
        update();
    }

    void editor_window::initialize_vulkan()
    {
        _vulkan_widget = std::make_unique<vulkan_widget>(QGuiApplication::topLevelWindows().at(0), this);

        setCentralWidget(_vulkan_widget->get_widget());

        cathedral::gfx::vulkan_context_args vkctx_args;
        vkctx_args.instance_extensions = get_instance_extensions();
        vkctx_args.surface_retriever = [&](vk::Instance inst) { return _vulkan_widget->init_surface(inst); };
        vkctx_args.surface_size_retriever = [&]() {
            const auto* widget = _vulkan_widget->get_widget();
            return glm::ivec2{ widget->width() * devicePixelRatio(), widget->height() * devicePixelRatio() };
        };
        vkctx_args.validation_layers = is_debug_build();

        _vkctx = std::make_unique<cathedral::gfx::vulkan_context>(vkctx_args);
        _swapchain = std::make_unique<cathedral::gfx::swapchain>(*_vkctx, vk::PresentModeKHR::eFifo);

        engine::renderer_args renderer_args;
        renderer_args.swapchain = &*_swapchain;
        _renderer = std::make_unique<engine::renderer>(renderer_args);

        _scene = std::make_unique<engine::scene>(*_renderer);

        _scene_dock->set_scene(_scene.get());

        auto* timer = new QTimer(this);
        timer->setSingleShot(true);
        timer->setInterval(500);
        timer->start();
        connect(timer, &QTimer::timeout, this, [this, timer] {
            resize(800, 600);
            timer->deleteLater();
        });

        connect(_vulkan_widget.get(), &vulkan_widget::size_changed, this, [this](int w, int h) {
            _swapchain->recreate();
            _renderer->recreate_swapchain_dependent_resources();
        });
    }

    void editor_window::setup_menubar_connections()
    {
        connect(_menubar, &editor_window_menubar::close_clicked, this, &editor_window::close);

        connect(_menubar, &editor_window_menubar::open_project_clicked, this, [this] {
            const QString dir = QFileDialog::getExistingDirectory(
                this,
                "Select project directory",
                QString::fromStdString(ien::get_current_user_homedir()));

            if (dir.isEmpty())
            {
                return;
            }

            auto status = _project->load_project(dir.toStdString());
            if (status != project::load_project_status::OK)
            {
                show_error_message("Failure loading project");
            }
        });

        connect(_menubar, &editor_window_menubar::shader_manager_clicked, this, [this]{
            if(_shader_manager)
            {
                delete _shader_manager;
            }
            _shader_manager = new shader_manager(*_project);
            _shader_manager->show();
        });
    }
} // namespace cathedral::editor