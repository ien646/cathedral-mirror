#pragma once

#ifdef SELF
#error "SELF macro is somehow already defined"
#endif

#define SELF std::remove_cvref_t<decltype(*this)>