#pragma once

#include <cathedral/core.hpp>

#include <functional>

namespace cathedral::editor2
{
    class texture_selector
    {
    public:
        void set_texture_list(std::vector<std::string> names);

        void open();

        void tick();

        struct
        {
            std::function<void(const std::string&)> selected;
        } callbacks;

    private:
        one_time_flag _open_flag;
        std::vector<std::string> _texture_list;
        std::string _filter;
        std::string _selected;
    };
} // namespace cathedral::editor2