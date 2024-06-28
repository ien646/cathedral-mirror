#include <cathedral/editor/editor_window.hpp>

#include <QMouseEvent>
#include <QVulkanInstance>
#include <QWindow>

#include <ien/platform.hpp>

#include <thread>

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
        editor_window_menubar* menubar = new editor_window_menubar(this);
        setMenuBar(menubar);

        _scene_dock = new QDockWidget("SceneTree", this);
        _scene_dock->setAllowedAreas(Qt::DockWidgetArea::LeftDockWidgetArea);
        _scene_dock->setFeatures(QDockWidget::DockWidgetFeature::NoDockWidgetFeatures);
        _scene_dock->setMinimumWidth(200);
        addDockWidget(Qt::DockWidgetArea::LeftDockWidgetArea, _scene_dock);

        _props_dock = new QDockWidget("Properties", this);
        _props_dock->setAllowedAreas(Qt::DockWidgetArea::RightDockWidgetArea);
        _props_dock->setFeatures(QDockWidget::DockWidgetFeature::NoDockWidgetFeatures);
        _props_dock->setMinimumWidth(200);
        addDockWidget(Qt::DockWidgetArea::RightDockWidgetArea, _props_dock);
    }

    void editor_window::tick(std::function<void()> tick_work)
    {
        _renderer->begin_frame();
        tick_work();
        _renderer->end_frame();
    }

    void editor_window::initialize_vulkan()
    {
        _vk_window = new QWindow(QGuiApplication::topLevelWindows().at(0));
        _vk_window->setFlags(Qt::WindowTransparentForInput);
        _vk_window->setSurfaceType(QWindow::SurfaceType::VulkanSurface);
        _vk_window->show();

        _vk_widget = QWidget::createWindowContainer(_vk_window, this, Qt::WindowDoesNotAcceptFocus);
        _vk_widget->setMouseTracking(false);
        _vk_widget->setMinimumSize(200, 200);

        setCentralWidget(_vk_widget);

        cathedral::gfx::vulkan_context_args vkctx_args;
        vkctx_args.instance_extensions = get_instance_extensions();
        vkctx_args.surface_retriever = [&](vk::Instance inst) {
            QVulkanInstance* vkinst = new QVulkanInstance();
            vkinst->setVkInstance(inst);
            vkinst->create();
            _vk_window->setVulkanInstance(vkinst);
            return QVulkanInstance::surfaceForWindow(_vk_window);
        };
        vkctx_args.surface_size_retriever = [&]() {
            return glm::ivec2{ _vk_widget->width() * devicePixelRatio(), _vk_widget->height() * devicePixelRatio() };
        };
        vkctx_args.validation_layers = true;

        _vkctx = std::make_unique<cathedral::gfx::vulkan_context>(vkctx_args);
        _swapchain = std::make_unique<cathedral::gfx::swapchain>(*_vkctx, vk::PresentModeKHR::eFifo);

        engine::renderer_args renderer_args;
        renderer_args.swapchain = &*_swapchain;
        _renderer = std::make_unique<engine::renderer>(renderer_args);

        // On some platforms, the widget/window has no position until the first
        // render/present is submitted (i.e. Wayland). Delay the initial resize
        // so that the widget has correct geometry on start
        std::thread delayed_resize([this] {
            std::this_thread::sleep_for(std::chrono::milliseconds(200));
            QMetaObject::invokeMethod(this, [this] { resize(800, 600); });
        });
        delayed_resize.detach();

        connect(_vk_window, &QWindow::widthChanged, this, [this](int w) {
            _swapchain->recreate();
            _renderer->recreate_swapchain_dependent_resources();
        });

        connect(_vk_window, &QWindow::heightChanged, this, [this](int h) {
            _swapchain->recreate();
            _renderer->recreate_swapchain_dependent_resources();
        });
    }
} // namespace cathedral::editor