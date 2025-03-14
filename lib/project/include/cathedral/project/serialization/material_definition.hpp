#pragma once

#include <cathedral/engine/material_definition.hpp>

#include <cathedral/project/serialization/shader_variable.hpp>

#include <magic_enum.hpp>

namespace cereal
{
    template <typename Archive>
    void CEREAL_SAVE_FUNCTION_NAME(Archive& ar, const cathedral::engine::material_definition& def)
    {
        auto domain = std::string{magic_enum::enum_name(def.domain())};
        auto material_texture_slot_count = def.material_texture_slot_count();
        auto node_texture_slot_count = def.node_texture_slot_count();
        auto material_variables = def.material_variables();
        auto node_variables = def.node_variables();
        ar(CEREAL_NVP(domain),
           CEREAL_NVP(material_texture_slot_count),
           CEREAL_NVP(node_texture_slot_count),
           CEREAL_NVP(material_variables),
           CEREAL_NVP(node_variables));
    }

    template <typename Archive>
    void CEREAL_LOAD_FUNCTION_NAME(Archive& ar, cathedral::engine::material_definition& def)
    {
        std::string domain;
        uint32_t mat_slot_count;
        uint32_t node_slot_count;
        std::vector<cathedral::engine::shader_variable> material_variables;
        std::vector<cathedral::engine::shader_variable> node_variables;

        ar(domain, mat_slot_count, node_slot_count, material_variables, node_variables);
        def.set_domain(*magic_enum::enum_cast<cathedral::engine::material_definition_domain>(domain));
        def.set_material_texture_slot_count(mat_slot_count);
        def.set_node_texture_slot_count(node_slot_count);
        for (const auto& mat_var : material_variables)
        {
            def.add_material_variable(mat_var);
        }
        for (const auto& node_var : node_variables)
        {
            def.add_node_variable(node_var);
        }
    }
} // namespace cereal