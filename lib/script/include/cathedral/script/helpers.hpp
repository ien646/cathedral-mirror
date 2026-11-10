#pragma once

#include <cathedral/script/global_state.hpp>

#include <format>
#include <meta>
#include <string>

namespace cathedral::script
{
    using namespace std::meta;

    template <typename T, typename TLBClass>
    constexpr void register_annotated_members(TLBClass& state)
    {
        constexpr auto refl = ^^T;
        printf("%s", std::format("Registering type members: {}\n", display_string_of(refl)).c_str());
        constexpr auto context = access_context::current();
        static constexpr auto members = std::define_static_array(members_of(refl, context));
        template for (constexpr auto member : members)
        {
            if constexpr (is_function(member))
            {
                static constexpr auto annotations = std::define_static_array(annotations_of(member));
                template for (constexpr auto annotation : annotations)
                {
                    if constexpr ([:constant_of(annotation):] == export_script)
                    {
                        constexpr auto fnptr = &[:member:];
                        state.addFunction(std::string{ identifier_of(member) }.c_str(), fnptr);
                        printf("%s", std::format(" >> [{}] -> {}\n", identifier_of(member), display_string_of(member)).c_str());
                    }
                }
            }
        }
    }

    template <typename T>
    constexpr void register_class_auto(global_state& s, const char* name)
    {
        auto c = luabridge::getGlobalNamespace(*s).beginClass<T>(name);
        register_annotated_members<T>(c);
        c.endClass();
    }

    template <typename T, typename TBase0, typename... TBases>
    constexpr void register_derived_class_auto(global_state& s, const char* name)
    {
        auto c = luabridge::getGlobalNamespace(*s).deriveClass<T, TBase0, TBases...>(name);
        register_annotated_members<T>(c);
        c.endClass();
    }
} // namespace cathedral::script