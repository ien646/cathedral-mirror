#include <cathedral/script/engine/scene.hpp>

#include "cathedral/script/helpers.hpp"

extern "C"
{
#include <lauxlib.h>
}
#include <LuaBridge/LuaBridge.h>
#include <cathedral/engine/scene.hpp>
#include <cathedral/script/global_state.hpp>

namespace cathedral::script
{
    void register_scene(global_state& state)
    {
        auto class_ = luabridge::getGlobalNamespace(*state).beginClass<engine::scene>("Scene");
        register_annotated_members<engine::scene>(class_);
        class_.endClass();
    }
} // namespace cathedral::script