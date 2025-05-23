#include <cathedral/script/engine/transform.hpp>

#include <cathedral/script/init_macros.hpp>

#include <cathedral/engine/transform.hpp>

namespace cathedral::script::engine
{
    void transform_initializer::initialize(state& s)
    {
        AUTO_INIT_NEW_TYPE(s, cathedral::engine, transform);
        AUTO_PROPERTY("position", position, set_position);
        AUTO_PROPERTY("rotation", rotation, set_rotation);
        AUTO_PROPERTY("scale", scale, set_scale);
        AUTO_FUNC(translate);
        AUTO_FUNC(rotate_degrees);
        AUTO_FUNC(get_model_matrix);
    }
} // namespace cathedral::script::engine