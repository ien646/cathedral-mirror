#include <cathedral/editor/vulkan_widget.hpp>

#include <vulkan/vulkan.h>

namespace cathedral::editor
{
    vulkan_widget::vulkan_widget(QWindow* parent_window, QWidget* parent_widget)
    {
        _vulkan_window = new QWindow(parent_window);
        _vulkan_window->setFlags(Qt::WindowTransparentForInput);
        _vulkan_window->setSurfaceType(QWindow::SurfaceType::VulkanSurface);
        _vulkan_window->show();

        _vulkan_widget = QWidget::createWindowContainer(_vulkan_window, parent_widget, Qt::WindowDoesNotAcceptFocus);
        _vulkan_widget->setMouseTracking(true);
        _vulkan_widget->setMinimumSize(200, 200);

        connect(_vulkan_window, &QWindow::widthChanged, this, [this]([[maybe_unused]] int w) {
            emit size_changed(_vulkan_window->width(), _vulkan_window->height());
        });

        connect(_vulkan_window, &QWindow::heightChanged, this, [this]([[maybe_unused]] int h) {
            emit size_changed(_vulkan_window->width(), _vulkan_window->height());
        });
    }

    vulkan_widget::~vulkan_widget()
    {
        if (_vulkan_instance)
        {
            _vulkan_instance.destroySurfaceKHR(_surface);
        }
    }

    vk::SurfaceKHR vulkan_widget::init_surface(vk::Instance inst)
    {
        if (_vulkan_instance)
        {
            return _surface;
        }

        _vulkan_instance = inst;

        _qvulkan_instance = new QVulkanInstance();
        _qvulkan_instance->setVkInstance(inst);
        _qvulkan_instance->create();

        _vulkan_window->setVulkanInstance(_qvulkan_instance);
        _surface = QVulkanInstance::surfaceForWindow(_vulkan_window);

        return _surface;
    }
} // namespace cathedral::editor