#include <cathedral/script/global_table.hpp>

namespace cathedral::script
{
    std::unordered_map<std::string, sol::object> global_table;

    void global_table_initializer::initialize(state& s)
    {
        s.set_function("global_get", [](const std::string& name) -> sol::object {
            if (!global_table.contains(name))
            {
                return sol::nil;
            }
            return global_table.at(name);
        });

        s.set_function("global_set", [](const std::string& name, const sol::object& value) { global_table[name] = value; });
    }
} // namespace cathedral::script