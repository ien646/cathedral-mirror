#pragma once

#include <cathedral/core.hpp>
#include <unordered_map>

#include <vector>

FORWARD_CLASS(cathedral::engine, scene);

namespace cathedral::editor2
{
    class stats_panel
    {
    public:
        void tick(const engine::scene& scene, std::unordered_map<std::string, std::string> additional_entries = {});

        static constexpr auto WINDOW_ID = "Stats";

    private:
        std::vector<float> _upload_queue_usage;
        std::vector<float> _upload_queue_flushes;
        std::vector<float> _framerates;
        std::vector<float> _vram_usage;
        float _total_vram = 0;

        void collect_stats(const engine::scene& scene);
    };
}