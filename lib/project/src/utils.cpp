#include <cathedral/project/utils.hpp>

#include <cathedral/engine/native_script_registry.hpp>
#include <cathedral/project/project.hpp>

namespace cathedral::project
{
    script_type get_script_type(const std::string& script_name, project& pro)
    {
        if (engine::get_native_script_registry().contains(script_name))
        {
            if (pro.script_assets().contains(script_name))
            {
                return script_type::DYNAMIC_OVERRIDING;
            }
            return script_type::NATIVE;
        }

        if (pro.script_assets().contains(script_name))
        {
            return script_type::DYNAMIC;
        }

        return script_type::NOT_FOUND;
    }
} // namespace cathedral::project