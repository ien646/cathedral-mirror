#include <cathedral/engine/transform.hpp>

#include <glm/ext/matrix_transform.hpp>

#include <cathedral/json_serializers.hpp>
#include <magic_enum.hpp>
#include <nlohmann/json.hpp>

namespace cathedral::engine
{
    void transform::set_position(glm::vec3 p)
    {
        _position = p;
        _model_needs_regen = true;
    }

    void transform::set_rotation(glm::vec3 r)
    {
        _rotation = r;
        clamp_rotation();
        _model_needs_regen = true;
    }

    void transform::set_scale(glm::vec3 s)
    {
        _scale = s;
        _model_needs_regen = true;
    }

    void transform::translate(glm::vec3 translation)
    {
        _position += translation;
        _model_needs_regen = true;
    }

    void transform::rotate_degrees(glm::vec3 degrees)
    {
        _rotation += degrees;
        clamp_rotation();
        _model_needs_regen = true;
    }

    const glm::mat4& transform::get_model_matrix() const
    {
        if (_model_needs_regen)
        {
            glm::vec3 euler_radians = glm::radians(_rotation);

            _model_matrix = glm::scale(glm::mat4(1.0F), _scale);
            _model_matrix = glm::rotate(_model_matrix, euler_radians.x, { 1, 0, 0 });
            _model_matrix = glm::rotate(_model_matrix, euler_radians.y, { 0, 1, 0 });
            _model_matrix = glm::rotate(_model_matrix, euler_radians.z, { 0, 0, 1 });
            _model_matrix = glm::translate(_model_matrix, _position);

            _model_needs_regen = false;
        }
        return _model_matrix;
    }

    void transform::clamp_rotation()
    {
        const auto clamp_value = [](auto& value) {
            if (value < -360.0F)
            {
                value = 360.0F + (std::fmod(value, 360.0F));
            }
            else if (value > 360.0F)
            {
                value = -360.0F + (std::fmod(value, 360.0F));
            }
        };

        clamp_value(_rotation.x);
        clamp_value(_rotation.y);
        clamp_value(_rotation.z);
    }
} // namespace cathedral::engine