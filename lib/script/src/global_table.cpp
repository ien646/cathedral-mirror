#include <cathedral/script/global_table.hpp>

#include <cathedral/ds.hpp>

namespace
{
    const std::string annotations = R"lua(

---@type fun(name:string): any
function global_get(name) end

---@type fun(name: string, value: any)
function global_set(name, value) end

)lua";
}

namespace cathedral::script
{
    namespace
    {
        unordered_map<std::string, sol::object> global_table;
    }

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

    const std::string& global_table_initializer::get_annotations()
    {
        return annotations;
    }
} // namespace cathedral::script
