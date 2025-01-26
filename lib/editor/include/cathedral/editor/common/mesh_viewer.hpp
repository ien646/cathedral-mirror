#pragma once

#define Q_ENABLE_OPENGL_FUNCTIONS_DEBUG

#include <cathedral/core.hpp>
#include <cathedral/engine/camera.hpp>
#include <cathedral/engine/transform.hpp>

#include <QOpenGLBuffer>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLFunctions>
#include <QtOpenGLWidgets/QOpenGLWidget>

FORWARD_CLASS(cathedral::engine, mesh);

namespace cathedral::editor
{
    class mesh_viewer
        : public QOpenGLWidget
        , protected QOpenGLFunctions
    {
    public:
        mesh_viewer(QWidget* parent, std::shared_ptr<engine::mesh> mesh = nullptr);

        void set_mesh(std::shared_ptr<engine::mesh> mesh);

    protected:
        void initializeGL() override;
        void resizeGL(int w, int h) override;
        void paintGL() override;

        void mousePressEvent(QMouseEvent* ev) override;
        void mouseReleaseEvent(QMouseEvent* ev) override;
        void mouseMoveEvent(QMouseEvent* ev) override;
        void wheelEvent(QWheelEvent* ev) override;

        float _aspect_ratio = 0.0F;
        std::shared_ptr<engine::mesh> _mesh;
        std::vector<float> _vertex_data;
        std::vector<uint32_t> _index_data;
        engine::perspective_camera _camera;
        engine::transform _object_transform;
        glm::vec3 _light_offset = {};
        QOpenGLVertexArrayObject _vao;
        QOpenGLBuffer _vertex_buffer, _index_buffer;
        GLuint _vertex_shader, _fragment_shader;
        GLuint _program;

        bool _hold_click = false;
        bool _hold_middle_click = false;
        QPoint _previous_pos;
        QPoint _previous_middle_pos;

    private:
        void check_error();
    };
} // namespace cathedral::editor