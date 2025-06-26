#include <cathedral/script/mat.hpp>

#include <cathedral/script/init_macros.hpp>

#include <glm/matrix.hpp>

constexpr auto ANNOTATIONS = R"lua(

---@class mat4
---@field public new fun(): mat4
---@field public new fun(f32: number): mat4
---@field public new fun(mat: mat4): mat4
local mat4 = {}

)lua";

namespace cathedral::script
{
    void mat_initializer::initialize(state& s)
    {
        AUTO_INIT_NEW_TYPE(s, glm, mat4);
        AUTO_CTORS(AUTO_TYPE(), AUTO_TYPE(float), AUTO_TYPE(glm::mat4));
        AUTO_MULTIPLICATION();
        AUTO_MULTIPLICATION_ARBITRARY(float, AUTO_OPERATOR_LHS* AUTO_OPERATOR_RHS);
    }

    const std::string& mat_initializer::get_annotations()
    {
        static const std::string annotations = ANNOTATIONS;
        return annotations;
    }
} // namespace cathedral::script