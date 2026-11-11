#pragma once

#include <cathedral/script/global_state.hpp>

#include <format>
#include <meta>
#include <print>
#include <string>

namespace cathedral::script
{
    using namespace std::meta;

    template <typename T, typename TLBClass>
        requires(std::is_class_v<T>)
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
                        std::println(" >> [{}] -> {}", identifier_of(member), display_string_of(member));
                    }
                }
            }
        }
    }

    template <typename T>
        requires(std::is_enum_v<T>)
    constexpr void register_enum(luabridge::Namespace& ns)
    {
        static constexpr auto enumerators = std::define_static_array(enumerators_of(^^T));
        template for (constexpr auto e : enumerators)
        {
            const auto enum_id = std::string{ identifier_of(e) };
            constexpr auto enum_value = static_cast<std::underlying_type_t<T>>([:e:]);
            ns.addProperty(enum_id.c_str(), +[] { return [:e:]; });
            std::println(" >> [{}] = {}", enum_id, enum_value);
        }
    }

    template <typename T>
        requires(std::is_enum_v<T>)
    constexpr void register_global_enum(global_state& s, const char* name)
    {
        std::println("Registering global enum table: {}", name);
        auto ns = luabridge::getGlobalNamespace(*s).beginNamespace(name);
        register_enum<T>(ns);
        ns.endNamespace();
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