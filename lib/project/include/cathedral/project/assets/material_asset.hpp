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
    struct material_asset_uniform_value
    {
        gfx::shader_data_type type;
        gfx::shader_data_value value;

        template <typename Archive>
        void CEREAL_SERIALIZE_FUNCTION_NAME(Archive& ar)
        {
            ar(type, value);
        }
    };

    class material_asset final : public asset
    {
    public:
        using asset::asset;

        CATHEDRAL_ASSET_SUBCLASS_DECL

        const auto& vertex_shader_ref() const { return _vertex_shader_ref; }

        void set_vertex_shader_ref(const std::string_view ref) { _vertex_shader_ref = ref; }

        const auto& fragment_shader_ref() const { return _fragment_shader_ref; }

        void set_fragment_shader_ref(const std::string_view ref) { _fragment_shader_ref = ref; }

        const auto& texture_slot_refs() const { return _material_texture_slot_refs; }

        void set_texture_slot_refs(std::vector<std::string> refs) { _material_texture_slot_refs = std::move(refs); }

        const auto& material_variable_values() const { return _material_uniform_values; }

        void set_variable_values(std::unordered_map<std::string, material_asset_uniform_value> values)
        {
            _material_uniform_values = std::move(values);
        }

        engine::material_domain domain() const { return _domain; }

        void set_domain(const engine::material_domain domain) { _domain = domain; }

        const auto& material_variable_bindings() const { return _material_uniform_bindings; }

        const auto& node_variable_bindings() const { return _node_uniform_bindings; }

        void set_material_uniform_binding(
            const std::string& var_name,
            std::optional<engine::shader_material_uniform_binding> binding);

        void set_node_uniform_binding(const std::string& var_name, std::optional<engine::shader_node_uniform_binding> binding);

        void set_material_texture_binding(
            const std::string& var_name,
            std::optional<engine::shader_material_texture_binding> binding);

        void set_node_texture_binding(const std::string& var_name, std::optional<engine::shader_node_texture_binding> binding);

        void set_material_buffer_binding(
            const std::string& var_name,
            std::optional<engine::shader_material_buffer_binding> binding);

        void set_node_buffer_binding(const std::string& var_name, std::optional<engine::shader_node_buffer_binding> binding);

        bool cull_backfaces() const { return _cull_backfaces; }

        void set_cull_backfaces(const bool value) { _cull_backfaces = value; }

        bool wireframe() const { return _wireframe; }

        void set_wireframe(const bool value) { _wireframe = value; }

        bool flip_front_faces() const { return _flip_front_faces; }

        void set_flip_front_faces(const bool value) { _flip_front_faces = value; }

        constexpr const char* typestr() const override { return "material"; }

    private:
        std::string _vertex_shader_ref;
        std::string _fragment_shader_ref;

        std::unordered_map<std::string, engine::shader_material_uniform_binding> _material_uniform_bindings;
        std::unordered_map<std::string, engine::shader_node_uniform_binding> _node_uniform_bindings;
        std::unordered_map<std::string, material_asset_uniform_value> _material_uniform_values;

        std::unordered_map<std::string, engine::shader_material_texture_binding> _material_texture_bindings;
        std::unordered_map<std::string, engine::shader_node_texture_binding> _node_texture_bindings;
        std::vector<std::string> _material_texture_slot_refs;

        std::unordered_map<std::string, engine::shader_material_buffer_binding> _material_buffer_bindings;
        std::unordered_map<std::string, engine::shader_node_buffer_binding> _node_buffer_bindings;

        engine::material_domain _domain = engine::material_domain::OPAQUE;
        bool _cull_backfaces = false;
        bool _wireframe = false;
        bool _flip_front_faces = false;

        template <class Archive>
        void CEREAL_SERIALIZE_FUNCTION_NAME(Archive& ar)
        {
            ar(cereal::make_nvp("asset", cereal::base_class<asset>(this)),
               cereal::make_nvp("vertex_shader_ref", _vertex_shader_ref),
               cereal::make_nvp("fragment_shader_ref", _fragment_shader_ref),
               cereal::make_nvp("material_texture_slot_references", _material_texture_slot_refs),
               cereal::make_nvp("material_uniform_values", _material_uniform_values),
               cereal::make_nvp("material_uniform_bindings", _material_uniform_bindings),
               cereal::make_nvp("node_uniform_bindings", _node_uniform_bindings),
               cereal::make_nvp("material_texture_bindings", _material_texture_bindings),
               cereal::make_nvp("node_texture_bindings", _node_texture_bindings),
               cereal::make_nvp("material_buffer_bindings", _material_buffer_bindings),
               cereal::make_nvp("node_buffer_bindings", _node_buffer_bindings),
               cereal::make_nvp("domain", _domain),
               cereal::make_nvp("cull_backfaces", _cull_backfaces),
               cereal::make_nvp("wireframe", _wireframe),
               cereal::make_nvp("flip_front_faces", _flip_front_faces));
        }
        friend class cereal::access;
    };
} // namespace cathedral::project