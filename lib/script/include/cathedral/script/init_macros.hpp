#pragma once

#include <magic_enum.hpp>

#define AUTO_STATE _cathedral_user_type_state
#define AUTO_TYPE _cathedral_user_type

#define AUTO_CTORS(...) AUTO_STATE.set("new", sol::constructors<__VA_ARGS__>())

#define AUTO_INIT_NEW_TYPE(state_, namespace_, type_)                                                                       \
    auto AUTO_STATE = (state_).new_usertype<namespace_::type_>(#type_);                                                     \
    using AUTO_TYPE = namespace_::type_

#define AUTO_FUNC(name_) AUTO_STATE.set_function(#name_, &AUTO_TYPE::name_)

#define AUTO_FUNC_OVERLOAD(name_, ret_, args_)                                                                              \
    AUTO_STATE.set_function(#name_, static_cast<ret_(AUTO_TYPE::*) /*NOLINT*/ args_>(&AUTO_TYPE::name_))

#define AUTO_MEMBER(member_) AUTO_STATE.set(#member_, &AUTO_TYPE::member_)

#define AUTO_INIT_ENUM(state_, namespace_, name_)                                                                           \
    {                                                                                                                       \
        auto AUTO_STATE = (state_).create_table(#name_);                                                                    \
        for (const auto& [enum_val, name] : magic_enum::enum_entries<namespace_::name_>())                                  \
        {                                                                                                                   \
            AUTO_STATE.add(name, enum_val);                                                                                 \
        }                                                                                                                   \
    }

#define AUTO_BASE_CLASS(base_) AUTO_STATE.set(sol::base_classes, sol::bases<base_>())

#define AUTO_PROPERTY(name_, getter_, setter_) AUTO_STATE.set(name_, sol::property(&AUTO_TYPE::getter_, &AUTO_TYPE::setter_))

#define AUTO_PROPERTY_ADVANCED(name_, getter_, setter_) AUTO_STATE.set(name_, sol::property((getter_), (setter_)))

#define AUTO_PROPERTY_READONLY(name_, getter_) AUTO_STATE.set(name_, sol::property(&AUTO_TYPE::getter_))

#define AUTO_OPERATOR_LHS _cathedral_operator_lhs
#define AUTO_OPERATOR_RHS _cathedral_operator_rhs

#define AUTO_ADDITION()                                                                                                       \
    AUTO_STATE.set(sol::meta_function::addition, [](const AUTO_TYPE& AUTO_OPERATOR_LHS, const AUTO_TYPE& AUTO_OPERATOR_RHS) { \
        return AUTO_OPERATOR_LHS + AUTO_OPERATOR_RHS;                                                                         \
    })
#define AUTO_SUBSTRACTION()                                                                                                 \
    AUTO_STATE.set(                                                                                                         \
        sol::meta_function::subtraction,                                                                                    \
        [](const AUTO_TYPE& AUTO_OPERATOR_LHS, const AUTO_TYPE& AUTO_OPERATOR_RHS) {                                        \
            return AUTO_OPERATOR_LHS - AUTO_OPERATOR_RHS;                                                                   \
        })
#define AUTO_MULTIPLICATION()                                                                                               \
    AUTO_STATE.set(                                                                                                         \
        sol::meta_function::multiplication,                                                                                 \
        [](const AUTO_TYPE& AUTO_OPERATOR_LHS, const AUTO_TYPE& AUTO_OPERATOR_RHS) {                                        \
            return AUTO_OPERATOR_LHS * AUTO_OPERATOR_RHS;                                                                   \
        })

#define AUTO_DIVISION()                                                                                                       \
    AUTO_STATE.set(sol::meta_function::division, [](const AUTO_TYPE& AUTO_OPERATOR_LHS, const AUTO_TYPE& AUTO_OPERATOR_RHS) { \
        return AUTO_OPERATOR_LHS / AUTO_OPERATOR_RHS;                                                                         \
    })

#define AUTO_INDEX() AUTO_STATE.set(sol::meta_function::index, [](AUTO_TYPE& v, int index) { return v[index - 1]; })

#define AUTO_ADDITION_ARBITRARY(rhs_type_, func)                                                                            \
    AUTO_STATE.set(                                                                                                         \
        sol::meta_function::addition,                                                                                       \
        [](const AUTO_TYPE& AUTO_OPERATOR_LHS, const rhs_type_& AUTO_OPERATOR_RHS) { return func; })

#define AUTO_SUBTRACTION_ARBITRARY(rhs_type_, func)                                                                         \
    AUTO_STATE.set(                                                                                                         \
        sol::meta_function::subtraction,                                                                                    \
        [](const AUTO_TYPE& AUTO_OPERATOR_LHS, const rhs_type_& AUTO_OPERATOR_RHS) { return func; })

#define AUTO_MULTIPLICATION_ARBITRARY(rhs_type_, func)                                                                      \
    AUTO_STATE.set(                                                                                                         \
        sol::meta_function::multiplication,                                                                                 \
        [](const AUTO_TYPE& AUTO_OPERATOR_LHS, const rhs_type_& AUTO_OPERATOR_RHS) { return func; })

#define AUTO_DIVISION_ARBITRARY(rhs_type_, func)                                                                            \
    AUTO_STATE.set(                                                                                                         \
        sol::meta_function::division,                                                                                       \
        [](const AUTO_TYPE& AUTO_OPERATOR_LHS, const rhs_type_& AUTO_OPERATOR_RHS) { return func; })
