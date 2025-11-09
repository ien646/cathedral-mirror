#pragma once

#include <cathedral/core.hpp>
#include <cathedral/ds.hpp>

#include <vector>

FORWARD_CLASS(cathedral::engine, scene);

namespace cathedral::editor
{
    class stats_panel
    {
    public:
        void tick(const engine::scene& scene, const unordered_map<std::string, std::string>& additional_entries = {});

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