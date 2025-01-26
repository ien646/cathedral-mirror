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

        float _aspect_ratio = 0.0F;
        std::shared_ptr<engine::mesh> _mesh;
        std::vector<float> _vertex_data;
        std::vector<uint32_t> _index_data;
        engine::perspective_camera _camera;
        engine::transform _object_transform;
        QOpenGLVertexArrayObject _vao;
        QOpenGLBuffer _vertex_buffer, _index_buffer;
        GLuint _vertex_shader, _fragment_shader;
        GLuint _program;

    private:
        void check_error();
    };
} // namespace cathedral::editor