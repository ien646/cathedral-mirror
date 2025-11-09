#pragma once

#include <cathedral/core.hpp>
#include <cathedral/ds.hpp>

namespace cathedral::editor
{
    struct error_line
    {
        std::string text;
    };

    class logs_panel
    {
    public:
        void tick();

        static constexpr auto WINDOW_ID = "Logs";

    private:
        std::vector<log_line> _lines;
        unordered_set<size_t> _expanded_lines;

        bool _autoscroll = true;

        void collect_logs();
    };
} // namespace cathedral::editor