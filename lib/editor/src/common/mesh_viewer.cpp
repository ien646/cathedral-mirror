#include <cathedral/editor/common/mesh_viewer.hpp>

#include <cathedral/engine/mesh.hpp>

#include <QMouseEvent>
#include <QTimer>
#include <QWheelEvent>

#include <print>

#include <glm/gtc/type_ptr.hpp>

const std::string vertex_shader_source = R"glsl(
    #version 410
    precision highp float;

    layout(location=0) in vec3 vx_position;
    layout(location=1) in vec2 vx_uv;
    layout(location=2) in vec3 vx_normal;
    layout(location=3) in vec4 vx_color;

    uniform mat4 model;
    uniform mat4 view;
    uniform mat4 projection;

    varying vec3 vnormal;
    varying vec3 vfragpos;

    void main()
    {
        gl_Position = projection * view * model * vec4(vx_position, 1.0);

        vnormal = normalize(mat3(transpose(inverse(model))) * vx_normal);

        vfragpos = vec3(model * vec4(vx_position, 1.0));
    }

)glsl";

const std::string fragment_shader_source = R"glsl(
    #version 410
    precision highp float;

    uniform vec3 light_position;
    const vec3 ambient_light = vec3(0.25, 0.25, 0.25);
    const vec3 light_color = vec3(1.0, 1.0, 1.0);

    varying vec3 vnormal;
    varying vec3 vfragpos;

    vec3 calculate_diffuse(vec3 light_direction, vec3 normal)
    {
        float intensity = max(dot(normal, light_direction), 0.0);
        return light_color * intensity;
    }

    void main()
    {
        vec3 light_direction = normalize(light_position - vfragpos);
        gl_FragColor = vec4(calculate_diffuse(light_direction, vnormal) + ambient_light, 1.0);
    }

)glsl";

namespace cathedral::editor
{
    mesh_viewer::mesh_viewer(QWidget* parent, std::shared_ptr<engine::mesh> mesh)
        : QOpenGLWidget(parent)
        , _mesh(std::move(mesh))
        , _camera(90, 1.0F, 0.1F, 100.0F, glm::vec3{ 0, 0, -5 }, glm::vec3{ 0, 0, 180 })
    {
        setMouseTracking(true);

        QSurfaceFormat format;
        format.setVersion(4, 1);
        format.setDepthBufferSize(24);
        format.setStencilBufferSize(8);
        format.setProfile(QSurfaceFormat::CoreProfile);
        setFormat(format);

        _light_offset = glm::vec3{ -1, 1, 0 };
    }

    void mesh_viewer::set_mesh(std::shared_ptr<engine::mesh> mesh)
    {
        _mesh = std::move(mesh);
        if (_mesh)
        {
            _vertex_data = _mesh->get_packed_data();
            _index_data = _mesh->indices();

            _vertex_buffer.bind();
            _vertex_buffer.allocate(_vertex_data.size() * sizeof(float));
            _vertex_buffer.write(0, _vertex_data.data(), _vertex_data.size() * sizeof(float));

            _index_buffer.bind();
            _index_buffer.allocate(_index_data.size() * sizeof(uint32_t));
            _index_buffer.write(0, _index_data.data(), _index_data.size() * sizeof(uint32_t));
        }
    }

    void mesh_viewer::initializeGL()
    {
        initializeOpenGLFunctions();

        _vao.create();
        _vao.bind();

        _vertex_buffer.create();
        _index_buffer.create();

        _vertex_shader = glCreateShader(GL_VERTEX_SHADER);
        _fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);

        std::string shader_log(1024, 0);
        GLsizei dummy;

        const auto* vx_data_ptr = vertex_shader_source.data();
        const GLint vx_size = vertex_shader_source.size();
        glShaderSource(_vertex_shader, 1, &vx_data_ptr, &vx_size);
        glCompileShader(_vertex_shader);
        glGetShaderInfoLog(_vertex_shader, shader_log.size(), &dummy, shader_log.data());
        std::print("{}", shader_log);

        const auto* fg_data_ptr = fragment_shader_source.data();
        const GLint fg_size = fragment_shader_source.size();
        glShaderSource(_fragment_shader, 1, &fg_data_ptr, &fg_size);
        glCompileShader(_fragment_shader);
        glGetShaderInfoLog(_fragment_shader, shader_log.size(), &dummy, shader_log.data());
        std::print("{}", shader_log);

        _program = glCreateProgram();
        glAttachShader(_program, _vertex_shader);
        glAttachShader(_program, _fragment_shader);
        glLinkProgram(_program);
        glUseProgram(_program);

        glDeleteShader(_vertex_shader);
        glDeleteShader(_fragment_shader);

        glClearColor(0, 0, 0, 1);
        glClearDepthf(1.0F);

        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LESS);

        auto* timer = new QTimer(this);
        timer->setInterval(10);
        connect(timer, &QTimer::timeout, this, [this] { update(); });
        timer->start();
    }

    void mesh_viewer::resizeGL(int w, int h)
    {
        _aspect_ratio = static_cast<float>(w) / static_cast<float>(h);
        _camera.set_aspect_ratio(_aspect_ratio);
    }

    void mesh_viewer::paintGL()
    {
        glClear(GL_COLOR_BUFFER_BIT);
        glClear(GL_DEPTH_BUFFER_BIT);

        if (_mesh == nullptr)
        {
            return;
        }

        const auto model = _object_transform.get_model_matrix();
        const auto view = _camera.get_view_matrix();
        const auto projection = _camera.get_projection_matrix();

        glUseProgram(_program);
        _vao.bind();

        const auto model_location = glGetUniformLocation(_program, "model");
        const auto view_location = glGetUniformLocation(_program, "view");
        const auto projection_location = glGetUniformLocation(_program, "projection");

        glUniformMatrix4fv(model_location, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(view_location, 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(projection_location, 1, GL_FALSE, glm::value_ptr(projection));

        const glm::vec3 camera_position = _camera.position();
        const glm::vec3 light_position = camera_position + _light_offset;

        const auto ligth_position_location = glGetUniformLocation(_program, "light_position");
        glUniform3f(ligth_position_location, light_position.x, light_position.y, light_position.z);

        glBindBuffer(GL_ARRAY_BUFFER, _vertex_buffer.bufferId());

        uint32_t offset = 0;
        glVertexAttribPointer(
            0,
            3,
            GL_FLOAT,
            GL_FALSE,
            engine::mesh::vertex_size_bytes(),
            reinterpret_cast<void*>(offset)); // NOLINT
        glEnableVertexAttribArray(0);
        offset += 3 * sizeof(float);

        glVertexAttribPointer(
            1,
            2,
            GL_FLOAT,
            GL_FALSE,
            engine::mesh::vertex_size_bytes(),
            reinterpret_cast<void*>(offset)); // NOLINT
        glEnableVertexAttribArray(1);
        offset += 2 * sizeof(float);

        glVertexAttribPointer(
            2,
            3,
            GL_FLOAT,
            GL_FALSE,
            engine::mesh::vertex_size_bytes(),
            reinterpret_cast<void*>(offset)); // NOLINT
        glEnableVertexAttribArray(2);
        offset += 3 * sizeof(float);

        glVertexAttribPointer(
            3,
            4,
            GL_FLOAT,
            GL_FALSE,
            engine::mesh::vertex_size_bytes(),
            reinterpret_cast<void*>(offset)); // NOLINT
        glEnableVertexAttribArray(3);
        // offset += 4  * sizeof(float);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _index_buffer.bufferId());
        glDrawElements(GL_TRIANGLES, _index_data.size(), GL_UNSIGNED_INT, nullptr);
    }

    void mesh_viewer::mousePressEvent(QMouseEvent* ev)
    {
        if (ev->button() == Qt::MouseButton::LeftButton)
        {
            _hold_click = true;
            _previous_pos = ev->pos();
        }
        else if (ev->button() == Qt::MouseButton::MiddleButton)
        {
            _hold_middle_click = true;
            _previous_middle_pos = ev->pos();
        }
    }

    void mesh_viewer::mouseReleaseEvent(QMouseEvent* ev)
    {
        if (ev->button() == Qt::MouseButton::LeftButton)
        {
            _hold_click = false;
        }
        else if (ev->button() == Qt::MouseButton::MiddleButton)
        {
            _hold_middle_click = false;
        }
    }

    void mesh_viewer::mouseMoveEvent(QMouseEvent* ev)
    {
        if (_hold_click)
        {
            const auto delta = ev->pos() - _previous_pos;
            _object_transform.rotate_degrees(glm::vec3{ -delta.y(), delta.x(), 0 });
            _previous_pos = ev->pos();
        }
        else if (_hold_middle_click)
        {
            const auto delta = ev->pos() - _previous_pos;
            _light_offset +=
                glm::vec3{ static_cast<float>(-delta.x()) / 10000.0F, static_cast<float>(-delta.y()) / 10000.0F, 0.0F };
            _light_offset = glm::clamp(_light_offset, glm::vec3{ -3, -3, 0 }, glm::vec3{ 3, 3, 0 });
        }
    }

    void mesh_viewer::wheelEvent(QWheelEvent* ev)
    {
        const auto delta = ev->angleDelta();
        _camera.translate(glm::vec3{ 0, 0, static_cast<float>(delta.y()) / 1000 });
    }

    void mesh_viewer::check_error()
    {
        auto err = glGetError();
        if (err != GL_NO_ERROR)
        {
            std::print("OGL-ERROR: '{}'", err);
        }
    }
} // namespace cathedral::editor