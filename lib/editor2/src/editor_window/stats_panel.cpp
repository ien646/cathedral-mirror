#include <cathedral/editor2/editor_window/stats_panel.hpp>

#include <cathedral/engine/scene.hpp>

#include <imgui.h>
#include <numeric>

namespace cathedral::editor2
{
    constexpr size_t HISTOGRAM_ELEMENT_LIMIT = 100;

    namespace
    {
        void clamp_histogram_elems(std::vector<float>& src, const size_t count)
        {
            if (src.size() <= count)
            {
                return;
            }

            src = std::vector(src.end() - count, src.end());
        }
    } // namespace

    void stats_panel::tick(const engine::scene& scene, std::unordered_map<std::string, std::string> additional_entries)
    {
        collect_stats(scene);

        ImGui::Begin("Stats");
        {
            const auto avg_uqu = std::ranges::fold_left(_upload_queue_usage, 0.0F, std::plus<float>()) / 1'000'000;
            const auto max_uqu = static_cast<float>(scene.get_renderer().get_upload_queue().size_in_bytes()) / 1'000'000;
            ImGui::PlotHistogram(
                std::format("UQ usage {:.1f}/{:.1f}MB", avg_uqu, max_uqu).c_str(),
                _upload_queue_usage.data(),
                _upload_queue_usage.size(),
                0,
                nullptr,
                0,
                scene.get_renderer().get_upload_queue().size_in_bytes() / 1000);

            ImGui::PlotHistogram("UQ flushes", _upload_queue_flushes.data(), _upload_queue_flushes.size());

            const auto avg_fps = std::ranges::fold_left(_framerates, 0.0F, std::plus<float>()) / _framerates.size();
            ImGui::PlotLines(
                std::format("FPS (avg:{:.0f})", avg_fps).c_str(),
                _framerates.data(),
                _framerates.size(),
                0,
                nullptr,
                0,
                std::ranges::max(_framerates));

            ImGui::PlotHistogram(
                std::format("VRAM (max: {:.1f}MB)", _total_vram).c_str(),
                _vram_usage.data(),
                _vram_usage.size(),
                0,
                nullptr,
                0,
                _total_vram);

            for (const auto& [key, value] : additional_entries)
            {
                ImGui::Text("%s", std::format("{}: {}", key, value).c_str());
            }
        }
        ImGui::End();
    }

    void stats_panel::collect_stats(const engine::scene& scene)
    {
        _upload_queue_usage.push_back(scene.get_renderer().get_upload_queue().last_cycle_usage_bytes());
        clamp_histogram_elems(_upload_queue_usage, HISTOGRAM_ELEMENT_LIMIT);

        _upload_queue_flushes.push_back(scene.get_renderer().get_upload_queue().last_cycle_forced_flushes());
        clamp_histogram_elems(_upload_queue_flushes, HISTOGRAM_ELEMENT_LIMIT);

        _framerates.push_back(1.0F / scene.last_deltatime());
        clamp_histogram_elems(_framerates, HISTOGRAM_ELEMENT_LIMIT);

        VkPhysicalDeviceMemoryBudgetPropertiesEXT budget_info = zero_struct<VkPhysicalDeviceMemoryBudgetPropertiesEXT>();
        budget_info.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MEMORY_BUDGET_PROPERTIES_EXT;

        vk::PhysicalDeviceMemoryProperties2 mem_props;
        mem_props.pNext = &budget_info;

        scene.get_renderer().vkctx().physdev().getMemoryProperties2(&mem_props);

        _total_vram = 0;
        uint32_t used = 0;
        for (uint32_t i = 0; i < mem_props.memoryProperties.memoryHeapCount; ++i)
        {
            const auto& [heap_size, heap_flags] = mem_props.memoryProperties.memoryHeaps[i];
            if ((heap_flags | vk::MemoryHeapFlagBits::eDeviceLocal) == heap_flags)
            {
                _total_vram += static_cast<float>(budget_info.heapBudget[i]) / 1'000'000;
                used += static_cast<float>(budget_info.heapUsage[i]) / 1'000'000;
            }
        }

        _vram_usage.push_back(used);
        clamp_histogram_elems(_vram_usage, HISTOGRAM_ELEMENT_LIMIT);
    }
} // namespace cathedral::editor2