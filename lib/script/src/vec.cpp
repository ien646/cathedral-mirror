#include <cathedral/script/vec.hpp>

#include <cathedral/script/init_macros.hpp>

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

constexpr auto ANNOTATION_FORMAT = R"(

---@class {0}vec2
---@field public x number
---@field public y number
{0}vec2 = {{}}

---@return vec2
function {0}vec2.new() end

---@param x number
---@param y number
---@return vec2
function {0}vec2.new(x, y) end

---@class {0}vec3
---@field public x number
---@field public y number
---@field public z number
{0}vec3 = {{}}

---@return vec3
function {0}vec3.new() end

---@param x number
---@param y number
---@param z number
---@return vec3
function {0}vec3.new(x, y, z) end

---@class {0}vec4
---@field public x number
---@field public y number
---@field public z number
---@field public w number
{0}vec4 = {{}}

---@return vec4
function {0}vec4.new() end

---@param x number
---@param y number
---@param z number
---@param w number
---@return vec4
function {0}vec4.new(x, y, z, w) end

)";

#define INIT_VEC_TYPE_INTEGRAL(prefix, itype)                                                                               \
    {                                                                                                                       \
        AUTO_INIT_NEW_TYPE(s, glm, prefix##vec2);                                                                           \
        AUTO_CTORS(AUTO_TYPE(), AUTO_TYPE(itype, itype));                                                                   \
        AUTO_MEMBER(x);                                                                                                     \
        AUTO_MEMBER(y);                                                                                                     \
        AUTO_ADDITION();                                                                                                    \
        AUTO_SUBSTRACTION();                                                                                                \
        AUTO_MULTIPLICATION();                                                                                              \
        AUTO_DIVISION();                                                                                                    \
        AUTO_INDEX();                                                                                                       \
    }                                                                                                                       \
    {                                                                                                                       \
        AUTO_INIT_NEW_TYPE(s, glm, prefix##vec3);                                                                           \
        AUTO_CTORS(AUTO_TYPE(), AUTO_TYPE(itype, itype, itype));                                                            \
        AUTO_MEMBER(x);                                                                                                     \
        AUTO_MEMBER(y);                                                                                                     \
        AUTO_MEMBER(z);                                                                                                     \
        AUTO_ADDITION();                                                                                                    \
        AUTO_SUBSTRACTION();                                                                                                \
        AUTO_MULTIPLICATION();                                                                                              \
        AUTO_DIVISION();                                                                                                    \
        AUTO_INDEX();                                                                                                       \
    }                                                                                                                       \
    {                                                                                                                       \
        AUTO_INIT_NEW_TYPE(s, glm, prefix##vec4);                                                                           \
        AUTO_CTORS(AUTO_TYPE(), AUTO_TYPE(itype, itype, itype, itype));                                                     \
        AUTO_MEMBER(x);                                                                                                     \
        AUTO_MEMBER(y);                                                                                                     \
        AUTO_MEMBER(z);                                                                                                     \
        AUTO_MEMBER(w);                                                                                                     \
        AUTO_ADDITION();                                                                                                    \
        AUTO_SUBSTRACTION();                                                                                                \
        AUTO_MULTIPLICATION();                                                                                              \
        AUTO_DIVISION();                                                                                                    \
        AUTO_INDEX();                                                                                                       \
    }

#define INIT_VEC_TYPE_FLOAT(prefix, ftype)                                                                                  \
    {                                                                                                                       \
        AUTO_INIT_NEW_TYPE(s, glm, prefix##vec2);                                                                           \
        AUTO_CTORS(AUTO_TYPE(), AUTO_TYPE(ftype, ftype));                                                                   \
        AUTO_MEMBER(x);                                                                                                     \
        AUTO_MEMBER(y);                                                                                                     \
        AUTO_ADDITION();                                                                                                    \
        AUTO_SUBSTRACTION();                                                                                                \
        AUTO_MULTIPLICATION();                                                                                              \
        AUTO_DIVISION();                                                                                                    \
        AUTO_INDEX();                                                                                                       \
        AUTO_MULTIPLICATION_ARBITRARY(ftype, AUTO_OPERATOR_LHS * AUTO_OPERATOR_RHS);                                        \
        AUTO_DIVISION_ARBITRARY(ftype, AUTO_OPERATOR_LHS / AUTO_OPERATOR_RHS);                                              \
    }                                                                                                                       \
    {                                                                                                                       \
        AUTO_INIT_NEW_TYPE(s, glm, prefix##vec3);                                                                           \
        AUTO_CTORS(AUTO_TYPE(), AUTO_TYPE(ftype, ftype, ftype));                                                            \
        AUTO_MEMBER(x);                                                                                                     \
        AUTO_MEMBER(y);                                                                                                     \
        AUTO_MEMBER(z);                                                                                                     \
        AUTO_ADDITION();                                                                                                    \
        AUTO_SUBSTRACTION();                                                                                                \
        AUTO_MULTIPLICATION();                                                                                              \
        AUTO_DIVISION();                                                                                                    \
        AUTO_INDEX();                                                                                                       \
        AUTO_MULTIPLICATION_ARBITRARY(ftype, AUTO_OPERATOR_LHS * AUTO_OPERATOR_RHS);                                        \
        AUTO_DIVISION_ARBITRARY(ftype, AUTO_OPERATOR_LHS / AUTO_OPERATOR_RHS);                                              \
    }                                                                                                                       \
    {                                                                                                                       \
        AUTO_INIT_NEW_TYPE(s, glm, prefix##vec4);                                                                           \
        AUTO_CTORS(AUTO_TYPE(), AUTO_TYPE(ftype, ftype, ftype, ftype));                                                     \
        AUTO_MEMBER(x);                                                                                                     \
        AUTO_MEMBER(y);                                                                                                     \
        AUTO_MEMBER(z);                                                                                                     \
        AUTO_MEMBER(w);                                                                                                     \
        AUTO_ADDITION();                                                                                                    \
        AUTO_SUBSTRACTION();                                                                                                \
        AUTO_MULTIPLICATION();                                                                                              \
        AUTO_DIVISION();                                                                                                    \
        AUTO_INDEX();                                                                                                       \
        AUTO_MULTIPLICATION_ARBITRARY(ftype, AUTO_OPERATOR_LHS * AUTO_OPERATOR_RHS);                                        \
        AUTO_DIVISION_ARBITRARY(ftype, AUTO_OPERATOR_LHS / AUTO_OPERATOR_RHS);                                              \
    }

namespace cathedral::script
{
    void vec_initializer::initialize(state& s)
    {
        INIT_VEC_TYPE_FLOAT(, float);
        INIT_VEC_TYPE_INTEGRAL(i, int);
        INIT_VEC_TYPE_INTEGRAL(u, unsigned int);
        INIT_VEC_TYPE_FLOAT(d, double);

        {
            AUTO_INIT_NEW_TYPE(s, glm, bvec2);
            AUTO_CTORS(AUTO_TYPE(), AUTO_TYPE(bool, bool));
            AUTO_MEMBER(x);
            AUTO_MEMBER(y);
            AUTO_INDEX();
        }
        {
            AUTO_INIT_NEW_TYPE(s, glm, bvec3);
            AUTO_CTORS(AUTO_TYPE(), AUTO_TYPE(bool, bool, bool));
            AUTO_MEMBER(x);
            AUTO_MEMBER(y);
            AUTO_MEMBER(z);
            AUTO_INDEX();
        }
        {
            AUTO_INIT_NEW_TYPE(s, glm, bvec4);
            AUTO_CTORS(AUTO_TYPE(), AUTO_TYPE(bool, bool, bool, bool));
            AUTO_MEMBER(x);
            AUTO_MEMBER(y);
            AUTO_MEMBER(z);
            AUTO_MEMBER(w);
            AUTO_INDEX();
        }
    }

    const std::string& vec_initializer::get_annotations()
    {
        static const std::string annotations = std::format(ANNOTATION_FORMAT, "") + std::format(ANNOTATION_FORMAT, "b") +
                                               std::format(ANNOTATION_FORMAT, "d") + std::format(ANNOTATION_FORMAT, "i") +
                                               std::format(ANNOTATION_FORMAT, "u");

        return annotations;
    }
} // namespace cathedral::script