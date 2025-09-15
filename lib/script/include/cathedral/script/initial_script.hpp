#pragma once

namespace cathedral::script
{
    constexpr auto SCRIPT_INITIAL_SOURCE = R"(
-- ---@param node scene_node
-- ---@param scn scene
-- function _Init(node, scn)
-- end

-- ---@param node scene_node
-- ---@param scn scene
-- ---@param deltatime number
-- function _Editor_Tick(node, scn, deltatime)
-- end

-- ---@param node scene_node
-- ---@param scn scene
-- ---@param deltatime number
-- function _Tick(node, scn, deltatime)
-- end

-- ---@param node scene_node
-- ---@param scn scene
-- ---@param deltatime number
-- function _Teardown(node, scn, deltatime)
-- end
)";
}