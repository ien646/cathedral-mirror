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

#include <QMainWindow>

#define CATHEDRAL_EDITOR_INITIALIZE() \
    Q_INIT_RESOURCE(fonts); \
    Q_INIT_RESOURCE(icons); \
    Q_INIT_RESOURCE(shaders)

FORWARD_CLASS_INLINE(QLabel);

namespace cathedral::editor
{
    FORWARD_CLASS_INLINE(material_manager);
    FORWARD_CLASS_INLINE(shader_manager);
    FORWARD_CLASS_INLINE(texture_manager);
    FORWARD_CLASS_INLINE(mesh_manager);

    class editor_window : public QMainWindow
    {
        Q_OBJECT

    public:
        editor_window(std::shared_ptr<project::project> project);

        void tick(const std::function<void(double)>& tick_work);

        engine::renderer& renderer() { return *_renderer; }

        std::shared_ptr<engine::scene> scene() const { return _scene; }

        project::project& project() { return *_project; }

        gfx::swapchain& swapchain() { return *_swapchain; }

        void initialize_vulkan();

        void set_status_text(const QString& text);

    private:
        std::unique_ptr<gfx::vulkan_context> _vkctx;
        std::unique_ptr<gfx::swapchain> _swapchain;
        std::unique_ptr<engine::renderer> _renderer;
        std::shared_ptr<engine::scene> _scene;
        std::unique_ptr<vulkan_widget> _vulkan_widget;
        std::shared_ptr<project::project> _project;

        editor_window_menubar* _menubar = nullptr;
        QLabel* _status_label = nullptr;

        scene_dock_widget* _scene_dock = nullptr;
        properties_dock_widget* _props_dock = nullptr;

        shader_manager* _shader_manager = nullptr;
        material_manager* _material_manager = nullptr;
        texture_manager* _texture_manager = nullptr;
        mesh_manager* _mesh_manager = nullptr;

        void setup_menubar_connections();

        void open_project();

        void open_material_manager();
        void open_mesh_manager();
        void open_shader_manager();
        void open_texture_manager();

        void new_scene();
        void open_scene();
        void save_scene();

        void capture_screenshot();

    signals:
        void size_changed(int w, int h);
    };
} // namespace cathedral::editor