#pragma once

#include <magic_enum.hpp>

#define AUTO_STATE _cathedral_user_type_state

#define AUTO_INIT_NEW_TYPE(state_, namespace_, type_)                                                                       \
    auto AUTO_STATE = (state_).new_usertype<namespace_::type_>(#type_);                                                     \
    using _cathedral_user_type = namespace_::type_

#define AUTO_FUNC(name_) AUTO_STATE.set_function(#name_, &_cathedral_user_type::name_)

#define AUTO_FUNC_OVERLOAD(name_, ret_, args_)                                                                              \
    AUTO_STATE.set_function(                                                                                                \
        #name_,                                                                                                             \
        static_cast<ret_(_cathedral_user_type::*) /*NOLINT*/ args_>(&_cathedral_user_type::name_))

#define AUTO_MEMBER(member_) AUTO_STATE.set(#member_, &_cathedral_user_type::member_)

#define AUTO_INIT_ENUM(state_, namespace_, name_)                                                                           \
    {                                                                                                                       \
        auto _cathedral_state = (state_).create_table(#name_);                                                              \
        for (const auto& [enum_val, name] : magic_enum::enum_entries<namespace_::name_>())                                  \
        {                                                                                                                   \
            _cathedral_state.add(name, enum_val);                                                                           \
        }                                                                                                                   \
    }
