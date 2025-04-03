#include <cathedral/engine/shader_validation.hpp>

#include <cathedral/gfx/shader.hpp>
#include <cathedral/gfx/shader_reflection.hpp>

#define CHECK_AND_RETURN(v)                                                                                                 \
    {                                                                                                                       \
        const auto result = v;                                                                                              \
        if (result.has_value())                                                                                             \
        {                                                                                                                   \
            return result;                                                                                                  \
        }                                                                                                                   \
    }

namespace cathedral::engine
{
    std::optional<std::string> validate_vertex_input_layout(const gfx::shader_reflection_info& refl)
    {
        // -- Check vertex shader input layout --
        const auto check_contains_vx_input =
            [&refl](uint32_t location, gfx::shader_data_type type) -> std::optional<std::string> {
            auto it =
                std::ranges::find_if(refl.inputs, [location, type](const gfx::shader_reflection_inout_variable& input) {
                    return input.location == location && input.type == type;
                });
            if (it != refl.inputs.end())
            {
                return std::format("Vertex shader input location {} not found", location);
            }
            return {};
        };

        CHECK_AND_RETURN(check_contains_vx_input(0, gfx::shader_data_type::VEC3)); // pos
        CHECK_AND_RETURN(check_contains_vx_input(1, gfx::shader_data_type::VEC2)); // uv
        CHECK_AND_RETURN(check_contains_vx_input(2, gfx::shader_data_type::VEC3)); // nrm
        CHECK_AND_RETURN(check_contains_vx_input(3, gfx::shader_data_type::VEC4)); // rgba

        return {};
    }

    std::optional<std::string> validate_descriptor_sets(const gfx::shader_reflection_info& refl)
    {
        for (const auto& dset : refl.descriptor_sets)
        {
            if (dset.set <= 2)
            {
                return "Illegal descriptor set set-index";
            }
            switch (dset.binding)
            {
            case 0:
                if (dset.descriptor_type != gfx::descriptor_type::UNIFORM)
                {
                    return "Descriptor set binding-index 0 is reserved for uniforms";
                }
            case 1:
                if (dset.descriptor_type != gfx::descriptor_type::SAMPLER)
                {
                    return "Descriptor set binding-index 1 is reserved for combined image samplers";
                }
            case 2:
                if (dset.descriptor_type != gfx::descriptor_type::STORAGE)
                {
                    return "Descriptor set binding-index 2 is reserved for storage buffers";
                }
            default:
                return std::format("Invalid descriptor set binding-index '{}'", dset.binding);
            }
        }
        return {};
    }

    std::optional<std::string> validate_shader(const gfx::shader& shader)
    {
        const auto refl = gfx::get_shader_reflection_info(shader);
        if (shader.type() == gfx::shader_type::VERTEX)
        {
            CHECK_AND_RETURN(validate_vertex_input_layout(refl));
        }
        CHECK_AND_RETURN(validate_descriptor_sets(refl));
        return {};
    }
} // namespace cathedral::engine