#include <cathedral/bits/error.hpp>

#include <iostream>

namespace cathedral
{
    [[noreturn]] void die(const std::string& message, int code)
    {
        std::cerr << message << std::endl;
        std::exit(code);
    }
}