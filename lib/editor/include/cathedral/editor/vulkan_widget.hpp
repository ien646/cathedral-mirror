#pragma once

#include <cathedral/core.hpp>

#include <QWindow>

#include <vulkan/vulkan.hpp>

FORWARD_CLASS_INLINE(QVulkanInstance);
FORWARD_CLASS_INLINE(QWidget);

namespace cathedral::editor
{
    class vulkan_widget : public QObject
    {
        Q_OBJECT

    public:
        vulkan_widget(QWindow* parent_window, QWidget* parent_widget);
        ~vulkan_widget() override;

        vk::SurfaceKHR init_surface(vk::Instance inst);

        QWidget* get_widget() { return _vulkan_widget; }

        QWindow* get_window() { return _vulkan_window; }

    private:
        QWindow* _vulkan_window = nullptr;
        QWidget* _vulkan_widget = nullptr;
        QVulkanInstance* _qvulkan_instance = nullptr;
        vk::Instance _vulkan_instance;
        vk::SurfaceKHR _surface;

    signals:
        void size_changed(int w, int h);
    };
} // namespace cathedral::editor