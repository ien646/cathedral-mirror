#include <cathedral/script/engine/material.hpp>

#include <cathedral/engine/material.hpp>
#include <cathedral/script/init_macros.hpp>

#define INIT_VEC_(prefix, type, dimensions)                                                                                 \
    AUTO_STATE.set(                                                                                                         \
        "set_material_variable_" prefix "vec" #dimensions,                                                                  \
        [](AUTO_TYPE& self, const std::string& name, const glm::vec<dimensions, type>& value) {                             \
            self.set_material_variable_value<glm::vec<dimensions, type>>(name, value);                                      \
        })

#define INIT_VEC_ALL(prefix, type)                                                                                          \
    INIT_VEC_(prefix, type, 2);                                                                                             \
    INIT_VEC_(prefix, type, 3);                                                                                             \
    INIT_VEC_(prefix, type, 4)

constexpr auto ANNOTATIONS = R"lua(

---@class material
---@field public name fun(): string
---@field public domain fun(): string
---@field public bind_material_texture_slot fun(name: string, slot: number)
---@field public set_material_variable_bool fun(name: string, value: boolean)
---@field public set_material_variable_f32 fun(name: string, value: number)
---@field public set_material_variable_f64 fun(name: string, value: number)
---@field public set_material_variable_i32 fun(name: string, value: number)
---@field public set_material_variable_u32 fun(name: string, value: number)
---@field public set_material_variable_vec2 fun(name: string, value: vec2)
---@field public set_material_variable_vec3 fun(name: string, value: vec3)
---@field public set_material_variable_vec4 fun(name: string, value: vec4)
---@field public set_material_variable_bvec2 fun(name: string, value: bvec2)
---@field public set_material_variable_bvec3 fun(name: string, value: bvec3)
---@field public set_material_variable_bvec4 fun(name: string, value: bvec4)
---@field public set_material_variable_ivec2 fun(name: string, value: ivec2)
---@field public set_material_variable_ivec3 fun(name: string, value: ivec3)
---@field public set_material_variable_ivec4 fun(name: string, value: ivec4)
---@field public set_material_variable_uvec2 fun(name: string, value: uvec2)
---@field public set_material_variable_uvec3 fun(name: string, value: uvec3)
---@field public set_material_variable_uvec4 fun(name: string, value: uvec4)
---@field public set_material_variable_dvec2 fun(name: string, value: dvec2)
---@field public set_material_variable_dvec3 fun(name: string, value: dvec3)
---@field public set_material_variable_dvec4 fun(name: string, value: dvec4)
---@field public set_material_variable_mat4 fun(name: string, value: mat4)
local material = {}

)lua";

namespace cathedral::script
{
    void engine::material_initializer::initialize(state& s)
    {
        AUTO_INIT_NEW_TYPE(s, cathedral::engine, material);
        AUTO_FUNC(name);
        AUTO_FUNC(domain);
        AUTO_FUNC_OVERLOAD(bind_material_texture_slot, void, (const std::string&, uint32_t));

        AUTO_STATE.set("set_material_variable_bool", [](AUTO_TYPE& self, const std::string& name, const bool value) {
            self.set_material_variable_value<bool>(name, value);
        });

        AUTO_STATE.set("set_material_variable_f32", [](AUTO_TYPE& self, const std::string& name, const double value) {
            self.set_material_variable_value<float>(name, static_cast<float>(value));
        });

        AUTO_STATE.set("set_material_variable_f64", [](AUTO_TYPE& self, const std::string& name, const double value) {
            self.set_material_variable_value<double>(name, value);
        });

        AUTO_STATE.set("set_material_variable_i32", [](AUTO_TYPE& self, const std::string& name, const double value) {
            self.set_material_variable_value<int32_t>(name, static_cast<int32_t>(value));
        });

        AUTO_STATE.set("set_material_variable_u32", [](AUTO_TYPE& self, const std::string& name, const double value) {
            self.set_material_variable_value<uint32_t>(name, static_cast<uint32_t>(value));
        });

        INIT_VEC_ALL("b", bool);
        INIT_VEC_ALL("d", double);
        INIT_VEC_ALL("u", uint32_t);
        INIT_VEC_ALL("i", int32_t);
        INIT_VEC_ALL("", float);

        AUTO_STATE.set("set_material_variable_mat4", [](AUTO_TYPE& self, const std::string& name, const glm::mat4& value) {
            self.set_material_variable_value<glm::mat4>(name, value);
        });
    }

    const std::string& engine::material_initializer::get_annotations()
    {
        return {};
    }
} // namespace cathedral::script