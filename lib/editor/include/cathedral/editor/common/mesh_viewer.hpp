#pragma once

#include <cathedral/core.hpp>

#include <cathedral/engine/renderer.hpp>
#include <cathedral/engine/scene.hpp>

#include <QWidget>

FORWARD_CLASS(cathedral::engine, mesh);
FORWARD_CLASS(cathedral::engine, mesh3d_node);
FORWARD_CLASS(cathedral::engine, point_light_node);
FORWARD_CLASS(cathedral::editor, vulkan_widget);
FORWARD_CLASS(cathedral::project, project);

namespace cathedral::editor
{
    class mesh_viewer final : public QWidget
    {
    public:
        explicit mesh_viewer(QWidget* parent);

        void initialize(project::project* project, std::optional<std::string> mesh_name = std::nullopt);

        void set_mesh(const std::optional<std::string>& mesh_name) const;

    private:
        bool _initialized = false;
        QTimer* _update_timer = nullptr;
        project::project* _project = nullptr;
        vulkan_widget* _vulkan_widget = nullptr;
        std::unique_ptr<gfx::vulkan_context> _vkctx;
        std::unique_ptr<gfx::swapchain> _swapchain;
        std::unique_ptr<engine::renderer> _renderer;
        std::unique_ptr<engine::scene> _scene;
        engine::mesh3d_node* _node = nullptr;
        engine::point_light_node* _light_node = nullptr;
        bool _left_click = false;
        bool _right_click = false;
        bool _middle_click = false;
        QPoint _click_pos;

        void tick() const;

        void handle_left_click();
        void handle_right_click();
        void handle_middle_click();
        void handle_mouse_move(QPoint delta) const;

        void closeEvent(QCloseEvent* event) override;
        void resizeEvent(QResizeEvent* event) override;
        void wheelEvent(QWheelEvent* event) override;
        void showEvent(QShowEvent* event) override;
    };
} // namespace cathedral::editor