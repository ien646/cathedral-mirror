#pragma once

#include <QVulkanInstance>
#include <QWidget>
#include <QWindow>

#include <vulkan/vulkan.hpp>

namespace cathedral::editor
{
    class vulkan_widget : public QObject
    {
        Q_OBJECT

    public:
        vulkan_widget(QWindow* parent_window, QWidget* parent_widget);
        virtual ~vulkan_widget();

        vk::SurfaceKHR init_surface(vk::Instance inst);

        inline QWidget* get_widget() { return _vulkan_widget; }

        inline QWindow* get_window() { return _vulkan_window; }

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