#pragma once

#include <cathedral/core.hpp>

namespace cathedral::editor2
{
    class texture_selector
    {
    public:
        void set_texture_list(std::vector<std::string> names);

        void open();

        void tick();

    private:
        one_time_flag _open_flag;
        std::vector<std::string> _texture_list;
    };
} // namespace cathedral::editor2