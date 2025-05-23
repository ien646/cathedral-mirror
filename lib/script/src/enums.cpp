#include <cathedral/script/enums.hpp>

#include <cathedral/script/init_macros.hpp>
#include <cathedral/script/state.hpp>

#include <cathedral/engine/material_domain.hpp>
#include <cathedral/engine/node_type.hpp>

#include <magic_enum.hpp>

namespace cathedral::script
{
    void enums_initializer::initialize(state& s)
    {
        AUTO_INIT_ENUM(s, engine, material_domain);
        AUTO_INIT_ENUM(s, engine, node_type);
    }
} // namespace cathedral::script