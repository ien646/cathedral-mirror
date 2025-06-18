#pragma once

#include <cathedral/core.hpp>

FORWARD_CLASS(cathedral::project, project)

namespace cathedral::project
{
    enum class script_type
    {
        NOT_FOUND,
        NATIVE,
        DYNAMIC,
        DYNAMIC_OVERRIDING
    };

    script_type get_script_type(const std::string& script_name, project& pro);
} // namespace cathedral::project