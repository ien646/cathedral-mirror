#pragma once

#include <cathedral/core.hpp>

#include <QWidget>
#include <QWindow>

#include <vulkan/vulkan.hpp>

FORWARD_CLASS_INLINE(QVulkanInstance);

namespace cathedral::editor
{
    class vulkan_widget final : public QObject
    {
        Q_OBJECT

    public:
        vulkan_widget(QWindow* parent_window, QWidget* parent_widget);

        vk::SurfaceKHR init_surface(vk::Instance inst);

        QWidget* get_widget() const { return _vulkan_widget; }

        QWindow* get_window() const { return _vulkan_window; }

        bool eventFilter(QObject* obj, QEvent* ev) override;

    signals:
        void left_click_press(QPoint pos);
        void left_click_release(QPoint pos);
        void right_click_press(QPoint pos);
        void right_click_release(QPoint pos);
        void middle_click_press(QPoint pos);
        void middle_click_release(QPoint pos);
        void mouse_move(QPoint delta);

    private:
        QWindow* _vulkan_window = nullptr;
        QWidget* _vulkan_widget = nullptr;
        QVulkanInstance* _qvulkan_instance = nullptr;
        vk::Instance _vulkan_instance;
        vk::SurfaceKHR _surface;
        QPoint _last_mouse_pos;

    signals:
        void size_changed(int w, int h);
    };
} // namespace cathedral::editor