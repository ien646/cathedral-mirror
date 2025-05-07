#pragma once

#include <cathedral/core.hpp>

#include <QWidget>

FORWARD_CLASS_INLINE(QVBoxLayout);
FORWARD_CLASS(cathedral::engine, mesh3d_node);
FORWARD_CLASS(cathedral::engine, scene);
FORWARD_CLASS(cathedral::project, project);

namespace cathedral::editor
{
    FORWARD_CLASS_INLINE(material_selector);
    FORWARD_CLASS_INLINE(mesh_selector);
    FORWARD_CLASS_INLINE(transform_widget);

    class mesh3d_properties_widget : public QWidget
    {
    public:
        mesh3d_properties_widget(
            project::project* pro,
            std::shared_ptr<engine::scene> scene,
            QWidget* parent,
            engine::mesh3d_node* node);

    private:
        project::project* _project;
        std::shared_ptr<engine::scene> _scene;

        engine::mesh3d_node* _node = nullptr;

        QVBoxLayout* _main_layout = nullptr;
        transform_widget* _transform_widget = nullptr;
        mesh_selector* _mesh_selector = nullptr;
        material_selector* _material_selector = nullptr;
        QVBoxLayout* _node_textures_layout = nullptr;
        QWidget* _stretch = nullptr;

        void init_ui();

        void update_transform_widget() const;

        void refresh_node_texture_selectors();
    };
} // namespace cathedral::editor