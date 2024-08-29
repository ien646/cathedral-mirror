#pragma once

#include <cathedral/core.hpp>

#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>

namespace cathedral::engine
{
    class transform
    {
    public:
        transform(){};

        inline glm::vec3 position() const { return _position; }
        // Rotation, in degrees (pitch, yaw, roll)
        inline glm::vec3 rotation() const { return _rotation; }
        inline glm::vec3 scale() const { return _scale; }

        void set_position(glm::vec3);
        void set_rotation(glm::vec3);
        void set_scale(glm::vec3);

        void translate(glm::vec3);
        void rotate_degrees(glm::vec3);

        const glm::mat4& get_model_matrix() const;

    private:
        glm::vec3 _position = { 0, 0, 0 };
        glm::vec3 _rotation = { 0, 0, 0 };
        glm::vec3 _scale = { 1, 1, 1 };

        mutable glm::mat4 _model_matrix = glm::mat4(1.0F);
        mutable bool _model_needs_regen = true;
        void clamp_rotation();
    };
} // namespace cathedral::engine