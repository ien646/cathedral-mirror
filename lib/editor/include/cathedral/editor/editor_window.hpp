#pragma once

#include <cathedral/core.hpp>
#include <cathedral/gfx/swapchain.hpp>
#include <cathedral/gfx/vulkan_context.hpp>

#include <QDockWidget>
#include <QMainWindow>
#include <QMenuBar>
#include <QVBoxLayout>
#include <QVulkanInstance>

#include <cathedral/editor/editor_window_menubar.hpp>

namespace cathedral::editor
{
    class editor_window : public QMainWindow
    {
        Q_OBJECT

    public:
        editor_window();

        void tick();

    private:
        std::unique_ptr<cathedral::gfx::vulkan_context> _vkctx;
        std::unique_ptr<cathedral::gfx::swapchain> _swapchain;
        QWindow* _vk_window = nullptr;
        QWidget* _vk_widget = nullptr;
        QDockWidget* _scene_dock = nullptr;
        QDockWidget* _props_dock = nullptr;
    };
} // namespace cathedral::editor