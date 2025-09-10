#include <cathedral/script/mat.hpp>

#include <cathedral/script/init_macros.hpp>

#include <glm/matrix.hpp>

namespace
{
    const std::string annotations = R"lua(

---@class mat4
mat4 = {}

---@return mat4
function mat4.new() end

---@param f number
---@return mat4
function mat4.new(f) end

---@param mat mat4
---@return mat4
function mat4.new(mat) end

)lua";
}

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
        return annotations;
    }
} // namespace cathedral::script