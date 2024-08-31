#pragma once

#include <cathedral/core.hpp>

#include <cathedral/editor/editor_window_menubar.hpp>
#include <cathedral/editor/properties_dock_widget.hpp>
#include <cathedral/editor/scene_dock_widget.hpp>
#include <cathedral/editor/vulkan_widget.hpp>

#include <cathedral/gfx/swapchain.hpp>
#include <cathedral/gfx/vulkan_context.hpp>

#include <cathedral/engine/renderer.hpp>
#include <cathedral/engine/scene.hpp>

#include <cathedral/project/project.hpp>

#include <QDockWidget>
#include <QMainWindow>
#include <QMenuBar>
#include <QVBoxLayout>
#include <QVulkanInstance>

namespace cathedral::editor
{
    class material_definition_manager;
    class shader_manager;

    class editor_window : public QMainWindow
    {
        Q_OBJECT

    public:
        editor_window();

        void tick(std::function<void(double)> tick_work);

        inline engine::renderer& renderer() { return *_renderer; }
        inline engine::scene& scene() { return *_scene; }
        inline project::project& project() { return *_project; }
        inline gfx::swapchain& swapchain() { return *_swapchain; }

        void initialize_vulkan();

    private:
        std::unique_ptr<gfx::vulkan_context> _vkctx;
        std::unique_ptr<gfx::swapchain> _swapchain;
        std::unique_ptr<engine::renderer> _renderer;
        std::unique_ptr<engine::scene> _scene;
        std::unique_ptr<vulkan_widget> _vulkan_widget;
        std::unique_ptr<project::project> _project;

        editor_window_menubar* _menubar = nullptr;

        scene_dock_widget* _scene_dock = nullptr;
        properties_dock_widget* _props_dock = nullptr;

        shader_manager* _shader_manager = nullptr;
        material_definition_manager* _material_definition_manager = nullptr;

        void setup_menubar_connections();

    signals:
        void size_changed(int w, int h);
    };
} // namespace cathedral::editor