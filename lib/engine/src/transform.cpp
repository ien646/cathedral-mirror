#include <cathedral/engine/transform.hpp>

#include <glm/ext/matrix_transform.hpp>

namespace cathedral::engine
{
    void transform::set_position(const glm::vec3 p)
    {
        _position = p;
        _model_needs_regen = true;
    }

    void transform::set_rotation(const glm::vec3 r)
    {
        _rotation = r;
        clamp_rotation();
        _model_needs_regen = true;
    }

    void transform::set_scale(const glm::vec3 s)
    {
        _scale = s;
        _model_needs_regen = true;
    }

    void transform::translate(const glm::vec3 translation)
    {
        _position += translation;
        _model_needs_regen = true;
    }

    void transform::rotate_degrees(const glm::vec3 degrees)
    {
        _rotation += degrees;
        clamp_rotation();
        _model_needs_regen = true;
    }

    const glm::mat4& transform::get_model_matrix() const
    {
        if (_model_needs_regen)
        {
            const glm::vec3 euler_radians = glm::radians(_rotation);

            const glm::mat4 translation = glm::translate(glm::mat4(1.0F), _position);

            glm::mat4 rotation = glm::rotate(glm::mat4(1.0F), euler_radians.x, { 1, 0, 0 });
            rotation = glm::rotate(rotation, euler_radians.y, { 0, 1, 0 });
            rotation = glm::rotate(rotation, euler_radians.z, { 0, 0, 1 });

            const glm::mat4 scale = glm::scale(glm::mat4(1.0F), _scale);

            _model_matrix = translation * rotation * scale;
            _model_needs_regen = false;
        }
        return _model_matrix;
    }

    void transform::clamp_rotation()
    {
        //const auto clamp_value = [](auto& value) {
        //    if (value < -360.0F)
        //    {
        //        value = 360.0F + (std::fmod(value, 360.0F));
        //    }
        //    else if (value > 360.0F)
        //    {
        //        value = -360.0F + (std::fmod(value, 360.0F));
        //    }
        //};

        const auto clamp_value = [](auto& value) {
            while (value > 360.0F)
            {
                value -= 360.0;
            }
            while (value < -360.0F)
            {
                value += 360.0;
            }
        };

        clamp_value(_rotation.x);
        clamp_value(_rotation.y);
        clamp_value(_rotation.z);
    }
} // namespace cathedral::engine