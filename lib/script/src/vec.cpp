#include <cathedral/script/vec.hpp>

#include <cathedral/script/init_macros.hpp>

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

#define INIT_VEC_TYPE(prefix)                                                                                               \
    {                                                                                                                       \
        AUTO_INIT_NEW_TYPE(s, glm, prefix##vec2);                                                                           \
        AUTO_MEMBER(x);                                                                                                     \
        AUTO_MEMBER(y);                                                                                                     \
    }                                                                                                                       \
    {                                                                                                                       \
        AUTO_INIT_NEW_TYPE(s, glm, prefix##vec3);                                                                           \
        AUTO_MEMBER(x);                                                                                                     \
        AUTO_MEMBER(y);                                                                                                     \
        AUTO_MEMBER(z);                                                                                                     \
    }                                                                                                                       \
    {                                                                                                                       \
        AUTO_INIT_NEW_TYPE(s, glm, prefix##vec4);                                                                           \
        AUTO_MEMBER(x);                                                                                                     \
        AUTO_MEMBER(y);                                                                                                     \
        AUTO_MEMBER(z);                                                                                                     \
        AUTO_MEMBER(w);                                                                                                     \
    }

namespace cathedral::script
{
    void vec_initializer::initialize(state& s)
    {
        INIT_VEC_TYPE();
        INIT_VEC_TYPE(i);
        INIT_VEC_TYPE(u);
        INIT_VEC_TYPE(d);
    }
} // namespace cathedral::script