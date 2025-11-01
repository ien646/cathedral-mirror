#pragma once

#include <string>
#include <vector>

namespace cathedral::editor2
{
    class resource_filter
    {
    public:
        void tick(const std::vector<std::string>& available, std::vector<const std::string*>& filtered);

    private:
        one_time_flag _first_tick = one_time_flag(true);
        std::string _filter_text = std::string(256, '\0');
    };
} // namespace cathedral::editor2