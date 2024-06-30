#pragma once

#include <cathedral/core.hpp>

#include <cathedral/engine/renderer.hpp>
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

        void tick(std::function<void()> tick_work);

        engine::renderer& renderer() { return *_renderer; }
        const gfx::swapchain& swapchain() const { return *_swapchain; }

        void initialize_vulkan();

    private:
        std::unique_ptr<gfx::vulkan_context> _vkctx;
        std::unique_ptr<gfx::swapchain> _swapchain;
        std::unique_ptr<engine::renderer> _renderer;
        QWindow* _vk_window = nullptr;
        QWidget* _vk_widget = nullptr;
        QDockWidget* _scene_dock = nullptr;
        QDockWidget* _props_dock = nullptr;
    };
} // namespace cathedral::editor