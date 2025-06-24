#include <cathedral/script/engine/material.hpp>

#include <cathedral/engine/material.hpp>
#include <cathedral/script/init_macros.hpp>

#define INIT_VEC_(prefix, type, dimensions)                                                                                 \
    AUTO_STATE.set(                                                                                                         \
        "set_material_variable_" prefix "vec" #dimensions,                                                                  \
        [](AUTO_TYPE& self, const std::string& name, const glm::vec<dimensions, type>& value) {           \
            self.set_material_variable_value<glm::vec<dimensions, type>>(name, value);                                      \
        })

#define INIT_VEC_ALL(prefix, type)                                                                                          \
    INIT_VEC_(prefix, type, 2);                                                                                             \
    INIT_VEC_(prefix, type, 3);                                                                                             \
    INIT_VEC_(prefix, type, 4)

namespace cathedral::script
{
    void engine::material_initializer::initialize(state& s)
    {
        AUTO_INIT_NEW_TYPE(s, cathedral::engine, material);

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
} // namespace cathedral::script