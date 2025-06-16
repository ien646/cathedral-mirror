#include <cathedral/script/vec.hpp>

#include <cathedral/script/init_macros.hpp>

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

#define INIT_OPERATORS()                                                                                                    \
    AUTO_ADDITION();                                                                                                        \
    AUTO_SUBSTRACTION();                                                                                                    \
    AUTO_MULTIPLICATION();                                                                                                  \
    AUTO_DIVISION();                                                                                                        \
    AUTO_INDEX();

#define INIT_OPERATORS_MULDIV_BY_FLOAT(ftype)                                                                               \
    INIT_OPERATORS();                                                                                                       \
    AUTO_MULTIPLICATION_ARBITRARY(ftype, AUTO_OPERATOR_LHS* AUTO_OPERATOR_RHS);                                             \
    AUTO_DIVISION_ARBITRARY(ftype, AUTO_OPERATOR_LHS / AUTO_OPERATOR_RHS);

#define INIT_VEC_TYPE_INTEGRAL(prefix, itype)                                                                               \
    {                                                                                                                       \
        AUTO_INIT_NEW_TYPE(s, glm, prefix##vec2);                                                                           \
        AUTO_CTORS(AUTO_TYPE(), AUTO_TYPE(itype, itype));                                                                   \
        AUTO_MEMBER(x);                                                                                                     \
        AUTO_MEMBER(y);                                                                                                     \
        INIT_OPERATORS();                                                                                                   \
    }                                                                                                                       \
    {                                                                                                                       \
        AUTO_INIT_NEW_TYPE(s, glm, prefix##vec3);                                                                           \
        AUTO_CTORS(AUTO_TYPE(), AUTO_TYPE(itype, itype, itype));                                                            \
        AUTO_MEMBER(x);                                                                                                     \
        AUTO_MEMBER(y);                                                                                                     \
        AUTO_MEMBER(z);                                                                                                     \
        INIT_OPERATORS();                                                                                                   \
    }                                                                                                                       \
    {                                                                                                                       \
        AUTO_INIT_NEW_TYPE(s, glm, prefix##vec4);                                                                           \
        AUTO_CTORS(AUTO_TYPE(), AUTO_TYPE(itype, itype, itype, itype));                                                     \
        AUTO_MEMBER(x);                                                                                                     \
        AUTO_MEMBER(y);                                                                                                     \
        AUTO_MEMBER(z);                                                                                                     \
        AUTO_MEMBER(w);                                                                                                     \
        INIT_OPERATORS();                                                                                                   \
    }

#define INIT_VEC_TYPE_FLOAT(prefix, ftype)                                                                                  \
    {                                                                                                                       \
        AUTO_INIT_NEW_TYPE(s, glm, prefix##vec2);                                                                           \
        AUTO_CTORS(AUTO_TYPE(), AUTO_TYPE(ftype, ftype));                                                                   \
        AUTO_MEMBER(x);                                                                                                     \
        AUTO_MEMBER(y);                                                                                                     \
        INIT_OPERATORS_MULDIV_BY_FLOAT(ftype);                                                                              \
    }                                                                                                                       \
    {                                                                                                                       \
        AUTO_INIT_NEW_TYPE(s, glm, prefix##vec3);                                                                           \
        AUTO_CTORS(AUTO_TYPE(), AUTO_TYPE(ftype, ftype, ftype));                                                            \
        AUTO_MEMBER(x);                                                                                                     \
        AUTO_MEMBER(y);                                                                                                     \
        AUTO_MEMBER(z);                                                                                                     \
        INIT_OPERATORS_MULDIV_BY_FLOAT(ftype);                                                                              \
    }                                                                                                                       \
    {                                                                                                                       \
        AUTO_INIT_NEW_TYPE(s, glm, prefix##vec4);                                                                           \
        AUTO_CTORS(AUTO_TYPE(), AUTO_TYPE(ftype, ftype, ftype, ftype));                                                     \
        AUTO_MEMBER(x);                                                                                                     \
        AUTO_MEMBER(y);                                                                                                     \
        AUTO_MEMBER(z);                                                                                                     \
        AUTO_MEMBER(w);                                                                                                     \
        INIT_OPERATORS_MULDIV_BY_FLOAT(ftype);                                                                              \
    }

namespace cathedral::script
{
    void vec_initializer::initialize(state& s)
    {
        INIT_VEC_TYPE_FLOAT(, float);
        INIT_VEC_TYPE_INTEGRAL(i, int);
        INIT_VEC_TYPE_INTEGRAL(u, unsigned int);
        INIT_VEC_TYPE_FLOAT(d, double);
    }
} // namespace cathedral::script