#include <cathedral/script/enums.hpp>

#include <cathedral/script/init_macros.hpp>
#include <cathedral/script/state.hpp>

#include <cathedral/engine/material_domain.hpp>
#include <cathedral/engine/node_type.hpp>

#include <magic_enum.hpp>

constexpr auto ANNOTATIONS = R"lua(

---@type {  OPAQUE: 0, TRANSPARENT: 1, OVERLAY: 2  }
local material_domain = {}

---@type {  NODE: 0, MESH3D_NODE: 1, CAMERA2D_NODE: 2, CAMERA3D_NODE: 3, POINT_LIGHT: 4, DIRECTIONAL_LIGHT: 5  }
local node_type = {}

)lua";

namespace cathedral::script
{
    void enums_initializer::initialize(state& s)
    {
        AUTO_INIT_ENUM(s, engine, material_domain);
        AUTO_INIT_ENUM(s, engine, node_type);
    }

    const std::string& enums_initializer::get_annotations()
    {
        static const std::string annotations = ANNOTATIONS;
        return annotations;
    }
} // namespace cathedral::script