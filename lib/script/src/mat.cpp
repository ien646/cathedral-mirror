#include <cathedral/script/mat.hpp>

#include <cathedral/script/init_macros.hpp>

#include <glm/matrix.hpp>

namespace cathedral::script
{
    void mat_initializer::initialize(state& s)
    {
        AUTO_INIT_NEW_TYPE(s, glm, mat4);
        AUTO_MULTIPLICATION();
        AUTO_MULTIPLICATION_ARBITRARY(float, AUTO_OPERATOR_LHS * AUTO_OPERATOR_RHS);
    }
} // namespace cathedral::script