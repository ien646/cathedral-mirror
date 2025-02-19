#include <cathedral/bits/error.hpp>

#include <ien/platform.hpp>

#include <cassert>
#include <iostream>

namespace cathedral
{
    [[noreturn]] void die(const std::string& message, int code)
    {
        std::cerr << message << '\n';
#ifdef NDEBUG
        std::exit(code);
#else
        throw std::logic_error(message);
#endif
    }
} // namespace cathedral