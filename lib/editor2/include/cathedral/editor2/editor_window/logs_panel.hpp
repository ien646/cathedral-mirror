#pragma once

#include <cathedral/core.hpp>

#include <unordered_set>

namespace cathedral::editor2
{
    struct error_line
    {
        std::string text;
    };

    class logs_panel
    {
    public:
        void tick();

    private:
        std::vector<log_line> _lines;
        std::unordered_set<size_t> _expanded_lines;

        bool _autoscroll = true;

        void collect_logs();
    };
} // namespace cathedral::editor2