#pragma once

#include <magic_enum.hpp>

#define CATHEDRAL_ENUM_TO_CSTR(e) std::string{ magic_enum::enum_name(e) }.c_str()