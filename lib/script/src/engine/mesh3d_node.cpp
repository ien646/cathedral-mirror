#include <cathedral/script/engine/mesh3d_node.hpp>

#include <cathedral/script/init_macros.hpp>

#include <cathedral/engine/nodes/mesh3d_node.hpp>

#define INIT_VEC_ONE(prefix, type, dimensions)                                                                              \
    AUTO_STATE.set(                                                                                                         \
        "set_node_variable_" prefix "vec" #dimensions,                                                                      \
        [](AUTO_TYPE& self, const std::string& name, const glm::vec<dimensions, type>& value) {                             \
            self.set_node_uniform_variable_value<glm::vec<dimensions, type>>(name, value);                                  \
        })

#define INIT_VEC_ALL(prefix, type)                                                                                          \
    INIT_VEC_ONE(prefix, type, 2);                                                                                          \
    INIT_VEC_ONE(prefix, type, 3);                                                                                          \
    INIT_VEC_ONE(prefix, type, 4)

namespace
{
    const std::string annotations = R"lua(

---@class mesh3d_node : node
---@field public set_mesh fun(self, mesh_name: string)
---@field public mesh_name fun(self): string
---@field public get_material fun(self): material
---@field public bind_node_texture_slot fun(self, texture_name: string, slot: number)
---@field public texture_names fun(self): string[]
---@field public set_instance_count(self, count: number)
---@field public instance_count(self): number
---@field public set_node_variable_bool fun(self, name: string, value: boolean)
---@field public set_node_variable_f32 fun(self, name: string, value: number)
---@field public set_node_variable_f64 fun(self, name: string, value: number)
---@field public set_node_variable_i32 fun(self, name: string, value: number)
---@field public set_node_variable_u32 fun(self, name: string, value: number)
---@field public set_node_variable_vec2 fun(self, name: string, value: vec2)
---@field public set_node_variable_vec3 fun(self, name: string, value: vec3)
---@field public set_node_variable_vec4 fun(self, name: string, value: vec4)
---@field public set_node_variable_bvec2 fun(self, name: string, value: bvec2)
---@field public set_node_variable_bvec3 fun(self, name: string, value: bvec3)
---@field public set_node_variable_bvec4 fun(self, name: string, value: bvec4)
---@field public set_node_variable_ivec2 fun(self, name: string, value: ivec2)
---@field public set_node_variable_ivec3 fun(self, name: string, value: ivec3)
---@field public set_node_variable_ivec4 fun(self, name: string, value: ivec4)
---@field public set_node_variable_uvec2 fun(self, name: string, value: uvec2)
---@field public set_node_variable_uvec3 fun(self, name: string, value: uvec3)
---@field public set_node_variable_uvec4 fun(self, name: string, value: uvec4)
---@field public set_node_variable_dvec2 fun(self, name: string, value: dvec2)
---@field public set_node_variable_dvec3 fun(self, name: string, value: dvec3)
---@field public set_node_variable_dvec4 fun(self, name: string, value: dvec4)
---@field public set_node_variable_mat4 fun(self, name: string, value: mat4)
mesh3d_node = {{}}

---@type fun(snode: scene_node): mesh3d_node
function as_mesh3d_node(snode) end
)lua";
}

namespace cathedral::script::engine
{
    void mesh3d_node_initializer::initialize(state& s)
    {
        AUTO_INIT_NEW_TYPE(s, cathedral::engine, mesh3d_node);
        AUTO_BASE_CLASSES(cathedral::engine::drawable_node, cathedral::engine::node, cathedral::engine::scene_node);
        // clang-format off
        AUTO_FUNC_OVERLOAD(set_mesh, void, (std::optional<std::string>));
        // clang-format on
        AUTO_FUNC(mesh_name);
        AUTO_FUNC(get_material);
        AUTO_FUNC_OVERLOAD(bind_node_texture_slot, void, (const std::string&, uint32_t));
        AUTO_FUNC(texture_names);
        AUTO_FUNC(set_instance_count);
        AUTO_FUNC(instance_count);

        AUTO_STATE.set("set_node_variable_bool", [](AUTO_TYPE& self, const std::string& name, const bool value) {
            self.set_node_uniform_variable_value<bool>(name, value);
        });

        AUTO_STATE.set("set_node_variable_f32", [](AUTO_TYPE& self, const std::string& name, const double value) {
            self.set_node_uniform_variable_value<float>(name, static_cast<float>(value));
        });

        AUTO_STATE.set("set_node_variable_f64", [](AUTO_TYPE& self, const std::string& name, const double value) {
            self.set_node_uniform_variable_value<double>(name, value);
        });

        AUTO_STATE.set("set_node_variable_i32", [](AUTO_TYPE& self, const std::string& name, const double value) {
            self.set_node_uniform_variable_value<int32_t>(name, static_cast<int32_t>(value));
        });

        AUTO_STATE.set("set_node_variable_u32", [](AUTO_TYPE& self, const std::string& name, const double value) {
            self.set_node_uniform_variable_value<uint32_t>(name, static_cast<uint32_t>(value));
        });

        INIT_VEC_ALL("b", bool);
        INIT_VEC_ALL("d", double);
        INIT_VEC_ALL("u", uint32_t);
        INIT_VEC_ALL("i", int32_t);
        INIT_VEC_ALL("", float);

        AUTO_STATE.set("set_node_variable_mat4", [](AUTO_TYPE& self, const std::string& name, const glm::mat4& value) {
            self.set_node_uniform_variable_value<glm::mat4>(name, value);
        });

        s.set_function("as_mesh3d_node", [](cathedral::engine::scene_node* node) {
            return dynamic_cast<cathedral::engine::mesh3d_node*>(node);
        });
    }

    const std::string& mesh3d_node_initializer::get_annotations()
    {
        return annotations;
    }
} // namespace cathedral::script::engine