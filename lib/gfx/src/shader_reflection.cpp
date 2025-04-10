#include <cathedral/gfx/shader_reflection.hpp>

#include <cathedral/gfx/shader_data_types.hpp>

#include <cathedral/core.hpp>

#if __has_include(<SPIRV-Reflect/spirv_reflect.h>)
    #include <SPIRV-Reflect/spirv_reflect.h>
#elif __has_include(<spirv_reflect.h>)
    #include <spirv_reflect.h>
#endif

namespace cathedral::gfx
{
    constexpr gfx::shader_data_type spv_format_to_gfx(SpvReflectFormat format)
    {
        switch (format)
        {
        case SPV_REFLECT_FORMAT_R32_SFLOAT:
            return shader_data_type::FLOAT;
        case SPV_REFLECT_FORMAT_R32_SINT:
            return shader_data_type::INT;
        case SPV_REFLECT_FORMAT_R32_UINT:
            return shader_data_type::UINT;
        case SPV_REFLECT_FORMAT_R64_SFLOAT:
            return shader_data_type::DOUBLE;

        case SPV_REFLECT_FORMAT_R32G32_SFLOAT:
            return shader_data_type::VEC2;
        case SPV_REFLECT_FORMAT_R32G32_SINT:
            return shader_data_type::IVEC2;
        case SPV_REFLECT_FORMAT_R32G32_UINT:
            return shader_data_type::UVEC2;

        case SPV_REFLECT_FORMAT_R32G32B32_SFLOAT:
            return shader_data_type::VEC3;
        case SPV_REFLECT_FORMAT_R32G32B32_SINT:
            return shader_data_type::IVEC3;
        case SPV_REFLECT_FORMAT_R32G32B32_UINT:
            return shader_data_type::UVEC3;

        case SPV_REFLECT_FORMAT_R32G32B32A32_SFLOAT:
            return shader_data_type::VEC4;
        case SPV_REFLECT_FORMAT_R32G32B32A32_SINT:
            return shader_data_type::IVEC4;
        case SPV_REFLECT_FORMAT_R32G32B32A32_UINT:
            return shader_data_type::UVEC4;

        case SPV_REFLECT_FORMAT_R64G64_SFLOAT:
            return shader_data_type::DVEC2;
        case SPV_REFLECT_FORMAT_R64G64B64_SFLOAT:
            return shader_data_type::DVEC3;
        case SPV_REFLECT_FORMAT_R64G64B64A64_SFLOAT:
            return shader_data_type::DVEC4;

        default:
            CRITICAL_ERROR("Unhandled SpvReflectFormat");
        }
    }

    constexpr gfx::descriptor_type spv_descriptor_type_to_gfx(SpvReflectDescriptorType type)
    {
        switch (type)
        {
        case SPV_REFLECT_DESCRIPTOR_TYPE_UNIFORM_BUFFER:
            return gfx::descriptor_type::UNIFORM;
        case SPV_REFLECT_DESCRIPTOR_TYPE_STORAGE_BUFFER:
            return gfx::descriptor_type::STORAGE;
        case SPV_REFLECT_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER:
            return gfx::descriptor_type::SAMPLER;

        default:
            CRITICAL_ERROR("Unhandled SpvReflectDescriptorType");
        }
    }

    shader_reflection_info get_shader_reflection_info(const gfx::shader& shader)
    {
        const auto& spirv = shader.spirv();

        const auto result_check = [](const auto result) {
            CRITICAL_CHECK(result == SpvReflectResult::SPV_REFLECT_RESULT_SUCCESS, "Failure obtaining shader reflection");
        };

        SpvReflectShaderModule module;
        const auto module_create_result = spvReflectCreateShaderModule2(
            SpvReflectModuleFlagBits::SPV_REFLECT_MODULE_FLAG_NO_COPY,
            spirv.size() * sizeof(uint32_t),
            spirv.data(),
            &module);
        result_check(module_create_result);

        const auto reflect_enum_count = [&module, &result_check](const auto call) -> uint32_t {
            uint32_t count;
            const auto call_result = call(&module, &count, nullptr);
            result_check(call_result);
            return count;
        };

        const auto reflect_fill_vars = [&module, &result_check, &reflect_enum_count]<typename TElem>(
                                           const auto call,
                                           std::vector<TElem>& result) -> void {
            uint32_t count = reflect_enum_count(call);

            result.resize(count);

            const auto call_result = call(&module, &count, result.data());
            result_check(call_result);
        };

        std::vector<SpvReflectInterfaceVariable*> input_vars;
        std::vector<SpvReflectInterfaceVariable*> output_vars;
        reflect_fill_vars(spvReflectEnumerateInputVariables, input_vars);
        reflect_fill_vars(spvReflectEnumerateOutputVariables, output_vars);

        std::vector<SpvReflectDescriptorBinding*> descriptor_bindings;
        reflect_fill_vars(spvReflectEnumerateDescriptorBindings, descriptor_bindings);

        shader_reflection_info info;

        for (const auto& in_var : input_vars)
        {
            shader_reflection_inout_variable var;
            var.location = in_var->location;
            var.type = spv_format_to_gfx(in_var->format);
            var.name = in_var->name;

            info.inputs.push_back(std::move(var));
        }

        for (const auto& out_var : output_vars)
        {
            if (std::strlen(out_var->name) == 0)
            {
                continue;
            }
            shader_reflection_inout_variable var;
            var.location = out_var->location;
            var.type = spv_format_to_gfx(out_var->format);
            var.name = out_var->name;

            info.outputs.push_back(std::move(var));
        }

        for (const auto& desc : descriptor_bindings)
        {
            shader_reflection_descriptor_set dset;
            dset.name = desc->name;
            dset.binding = desc->binding;
            dset.set = desc->set;
            dset.count = desc->count;
            dset.descriptor_type = spv_descriptor_type_to_gfx(desc->descriptor_type);
            dset.size = desc->block.size;

            info.descriptor_sets.push_back(std::move(dset));
        }

        spvReflectDestroyShaderModule(&module);

        return info;
    }
} // namespace cathedral::gfx