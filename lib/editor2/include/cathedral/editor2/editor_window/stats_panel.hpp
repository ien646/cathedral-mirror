#pragma once

#include <cathedral/core.hpp>

#include <vector>

FORWARD_CLASS(cathedral::engine, scene);

namespace cathedral::editor2
{
    class stats_panel
    {
    public:
        void tick(const engine::scene& scene);

    private:
        std::vector<float> _upload_queue_usage;
        std::vector<float> _upload_queue_flushes;
        std::vector<float> _framerates;

        void collect_stats(const engine::scene& scene);
    };
}