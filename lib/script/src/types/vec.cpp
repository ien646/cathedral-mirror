#include <cathedral/script/types/vec.hpp>

#include <cathedral/script/global_state.hpp>

extern "C"
{
#include <lauxlib.h>
}
#include <LuaBridge/LuaBridge.h>

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

namespace cathedral::script
{
    template <typename TVec>
    void register_vec(global_state& s, const std::string& class_name)
    {
        using value_t = TVec::value_type;
        constexpr auto length = TVec::length();

        auto class_ = luabridge::getGlobalNamespace(s.get()).beginClass<TVec>(class_name.c_str());

        if constexpr (length == 2)
        {
            class_.template addConstructor<void(), void(value_t), void(value_t, value_t)>();
        }
        if constexpr (length == 3)
        {
            class_.template addConstructor<void(), void(value_t), void(value_t, value_t, value_t)>();
        }
        if constexpr (length == 4)
        {
            class_.template addConstructor<void(), void(value_t), void(value_t, value_t, value_t, value_t)>();
        }

        class_.addProperty("x", &TVec::x);
        class_.addProperty("y", &TVec::y);
        if constexpr (length >= 3)
        {
            class_.addProperty("z", &TVec::z);
        }
        if constexpr (length == 4)
        {
            class_.addProperty("w", &TVec::w);
        }

        class_.addFunction("__add", [](const TVec& a, const TVec& b) { return a + b; })
            .addFunction("__sub", [](const TVec& a, const TVec& b) { return a - b; })
            .addFunction("__mul", [](const TVec& a, const value_t b) { return a * b; })
            .addFunction("__div", [](const TVec& a, const value_t b) { return a / b; })
            .addFunction("__eq", [](const TVec& a, const TVec& b) { return a == b; })
            .endClass();
    }

    void register_vec_types(global_state& s)
    {
        static constexpr std::array dims = { 2, 3, 4 };
        template for (constexpr int dim : dims)
        {
            register_vec<glm::vec<dim, float>>(s, std::format("Vec{}", dim));
            register_vec<glm::vec<dim, double>>(s, std::format("Vec{}D", dim));
            register_vec<glm::vec<dim, uint32_t>>(s, std::format("Vec{}U", dim));
            register_vec<glm::vec<dim, int32_t>>(s, std::format("Vec{}I", dim));
            register_vec<glm::vec<dim, bool>>(s, std::format("Vec{}B", dim));
        }
    }
} // namespace cathedral::script