#pragma once

#include <cathedral/project/asset.hpp>
#include <cathedral/project/serialization/enums.hpp>

#include <cathedral/gfx/shader_data_types.hpp>

#include <cathedral/engine/material_domain.hpp>
#include <cathedral/engine/shader_bindings.hpp>

#include <cathedral/glm_serializers.hpp>

#include <cereal/access.hpp>
#include <cereal/types/base_class.hpp>
#include <cereal/types/optional.hpp>
#include <cereal/types/string.hpp>
#include <cereal/types/unordered_map.hpp>
#include <cereal/types/variant.hpp>
#include <cereal/types/vector.hpp>

#include <variant>

namespace cathedral::project
{
    struct material_asset_variable_value
    {
        gfx::shader_data_type type;
        std::variant<
            uint32_t,
            int32_t,
            float,
            double,

            glm::bvec2,
            glm::bvec3,
            glm::bvec4,
            glm::ivec2,
            glm::ivec3,
            glm::ivec4,
            glm::vec2,
            glm::vec3,
            glm::vec4,
            glm::uvec2,
            glm::uvec3,
            glm::uvec4,
            glm::dvec2,
            glm::dvec3,
            glm::dvec4,

            glm::mat2x2,
            glm::mat2x3,
            glm::mat2x4,
            glm::mat3x2,
            glm::mat3x3,
            glm::mat3x4,
            glm::mat4x2,
            glm::mat4x3,
            glm::mat4x4>
            value;

        template <typename Archive>
        void CEREAL_SERIALIZE_FUNCTION_NAME(Archive& ar)
        {
            ar(type, value);
        }
    };

    class material_asset : public asset
    {
    public:
        using asset::asset;

        CATHEDRAL_ASSET_SUBCLASS_DECL

        const auto& vertex_shader_ref() const { return _vertex_shader_ref; }

        void set_vertex_shader_ref(std::string_view ref) { _vertex_shader_ref = ref; }

        const auto& fragment_shader_ref() const { return _fragment_shader_ref; }

        void set_fragment_shader_ref(std::string_view ref) { _fragment_shader_ref = ref; }

        const auto& texture_slot_refs() const { return _material_texture_slot_refs; }

        void set_texture_slot_refs(std::vector<std::string> refs) { _material_texture_slot_refs = std::move(refs); }

        const auto& material_variable_values() const { return _material_variable_values; }

        void set_variable_values(std::unordered_map<std::string, material_asset_variable_value> values)
        {
            _material_variable_values = std::move(values);
        }

        engine::material_domain domain() const { return _domain; }

        void set_domain(const engine::material_domain domain) { _domain = domain; }

        const auto& material_variable_bindings() const { return _material_variable_bindings; }

        const auto& node_variable_bindings() const { return _node_variable_bindings; }

        void set_material_variable_binding(
            const std::string& var_name,
            std::optional<engine::shader_material_uniform_binding> binding);

        void set_node_variable_binding(
            const std::string& var_name,
            std::optional<engine::shader_node_uniform_binding> binding);

        constexpr const char* typestr() const override { return "material"; };

    private:
        std::string _vertex_shader_ref;
        std::string _fragment_shader_ref;
        std::vector<std::string> _material_texture_slot_refs;
        std::unordered_map<engine::shader_material_uniform_binding,std::string> _material_variable_bindings;
        std::unordered_map<engine::shader_node_uniform_binding,std::string> _node_variable_bindings;
        std::unordered_map<std::string, material_asset_variable_value> _material_variable_values;
        engine::material_domain _domain = engine::material_domain::OPAQUE;

        template <class Archive>
        void CEREAL_SERIALIZE_FUNCTION_NAME(Archive& ar)
        {
            ar(cereal::make_nvp("asset", cereal::base_class<asset>(this)),
               cereal::make_nvp("vertex_shader_ref", _vertex_shader_ref),
               cereal::make_nvp("fragment_shader_ref", _fragment_shader_ref),
               cereal::make_nvp("material_texture_slot_references", _material_texture_slot_refs),
               cereal::make_nvp("material_variable_values", _material_variable_values),
               cereal::make_nvp("material_variable_bindings", _material_variable_bindings),
               cereal::make_nvp("node_variable_bindings", _node_variable_bindings),
               cereal::make_nvp("domain", _domain));
        }
        friend class cereal::access;
    };
} // namespace cathedral::project