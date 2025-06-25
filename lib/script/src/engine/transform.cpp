#include <cathedral/script/engine/transform.hpp>

#include <cathedral/script/init_macros.hpp>

#include <cathedral/engine/transform.hpp>

constexpr auto ANNOTATIONS = R"lua(

---@class transform
---@field public new fun(): transform
---@field public position vec3
---@field public rotation vec3
---@field public scale vec3
---@field public translate fun(offset: vec3)
---@field public rotate_degrees fun(offset: vec3)
---@field public get_model_matrix fun(): mat4
local transform = {}

)lua";

namespace cathedral::script::engine
{
    void transform_initializer::initialize(state& s)
    {
        AUTO_INIT_NEW_TYPE(s, cathedral::engine, transform);
        AUTO_CTORS(AUTO_TYPE());
        AUTO_PROPERTY("position", position, set_position);
        AUTO_PROPERTY("rotation", rotation, set_rotation);
        AUTO_PROPERTY("scale", scale, set_scale);
        AUTO_FUNC(translate);
        AUTO_FUNC(rotate_degrees);
        AUTO_FUNC(get_model_matrix);
    }

    const std::string& transform_initializer::get_annotations()
    {
        return ANNOTATIONS;
    }
} // namespace cathedral::script::engine