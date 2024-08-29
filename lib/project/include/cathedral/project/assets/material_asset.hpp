#pragma once

#include <cathedral/project/asset.hpp>

#include <cathedral/gfx/shader_data_types.hpp>

#include <glm/matrix.hpp>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

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
    };

    class material_asset : public asset
    {
    public:
        using asset::asset;

        void save() const override;
        void load() override;
        void unload() override;
        std::string relative_path() const override;

        const auto& material_definition_ref() const { return _material_definition_ref; }

        void set_material_definition_ref(const std::string& ref) { _material_definition_ref = ref; }

        const auto& vertex_shader_ref() const { return _vertex_shader_ref; }

        void set_vertex_shader_ref(const std::string& ref) { _vertex_shader_ref = ref; }

        const auto& fragment_shader_ref() const { return _fragment_shader_ref; }

        void set_fragment_shader_ref(const std::string& ref) { _fragment_shader_ref = ref; }

        const auto& texture_slot_refs() const { return _material_texture_slot_refs; }

        void set_texture_slot_refs(std::vector<std::string> refs) { _material_texture_slot_refs = std::move(refs); }

        const auto& variable_values() const { return _material_variable_values; }

        void set_variable_values(std::vector<material_asset_variable_value> values)
        {
            _material_variable_values = std::move(values);
        }

    private:
        std::string _material_definition_ref;
        std::string _vertex_shader_ref;
        std::string _fragment_shader_ref;
        std::vector<std::string> _material_texture_slot_refs;
        std::vector<material_asset_variable_value> _material_variable_values;
    };

    template <>
    constexpr std::string asset_typestr<material_asset>()
    {
        return "material";
    }
} // namespace cathedral::project