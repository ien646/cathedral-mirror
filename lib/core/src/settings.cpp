#include <cathedral/settings.hpp>

#include <unordered_map>

namespace cathedral
{
    std::optional<setting_value> settings::get(const std::string& key)
    {
        if (!_entries.contains(key))
        {
            return {};
        }
        return _entries.at(key);
    }

    void settings::set(const std::string& key, const std::optional<setting_value>& value)
    {
        if (!value.has_value())
        {
            erase(key);
            return;
        }
        _entries[key] = *value;

        if (_subscriptions.contains(key))
        {
            for (auto& subscription : _subscriptions[key])
            {
                subscription(*value);
            }
        }
    }

    void settings::erase(const std::string& key)
    {
        _entries.erase(key);
    }

    const std::unordered_map<std::string, setting_value>& settings::all_entries() const
    {
        return _entries;
    }
} // namespace cathedral