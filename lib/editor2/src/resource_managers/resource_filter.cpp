#include <cathedral/editor2/resource_managers/resource_filter.hpp>

#include <boost/regex.hpp>

#include <imgui.h>

#include <ranges>

namespace cathedral::editor2
{
    void resource_filter::tick(const std::vector<std::string>& available, std::vector<const std::string*>& filtered)
    {
        if (_first_tick.get_and_reset())
        {
            filtered = available
                       | std::views::transform([](const auto& str) { return &str; })
                       | std::ranges::to<std::vector>();
        }

        const auto filter_text_size = ImGui::CalcTextSize("Filter");

        ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x - filter_text_size.x);
        if (ImGui::InputText("Filter", _filter_text.data(), _filter_text.size()))
        {
            if (!_filter_text.empty())
            {
                filtered.clear();

                const boost::regex re(_filter_text.c_str(), boost::regex::icase);
                boost::smatch match;
                for (const auto& name : available)
                {
                    if (boost::regex_search(name, match, re))
                    {
                        filtered.push_back(&name);
                    }
                }
            }
            else
            {
                filtered = available
                           | std::views::transform([](const auto& str) { return &str; })
                           | std::ranges::to<std::vector>();
            }
        }
    }
} // namespace cathedral::editor2