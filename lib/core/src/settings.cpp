#include <cathedral/settings.hpp>

#include <unordered_map>

namespace cathedral
{
    namespace
    {
        std::unordered_map<std::string, setting_value> entries;
    }

    std::optional<setting_value> settings::get(const std::string& key)
    {
        if (!entries.contains(key))
        {
            return {};
        }
        return entries.at(key);
    }

    void settings::set(const std::string& key, const std::optional<setting_value>& value)
    {
        if (!value.has_value())
        {
            erase(key);
            return;
        }
        entries.at(key) = *value;
    }

    void settings::erase(const std::string& key)
    {
        entries.erase(key);
    }

    const std::unordered_map<std::string, setting_value>& settings::all_entries()
    {
        return entries;
    }
} // namespace cathedral