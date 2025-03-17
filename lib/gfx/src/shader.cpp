#include <cathedral/gfx/shader.hpp>

#include <cathedral/gfx/vulkan_context.hpp>

#include <shaderc/shaderc.hpp>

#include <SPIRV-Reflect/spirv_reflect.h>

#include <iostream>

namespace cathedral::gfx
{
    shaderc_shader_kind to_shaderc_shader_kind(shader_type type)
    {
        switch (type)
        {
        case shader_type::VERTEX:
            return shaderc_vertex_shader;
        case shader_type::FRAGMENT:
            return shaderc_fragment_shader;
        default:
            CRITICAL_ERROR("Unhandled shader type");
        }
    }

    shader::shader(const shader_args& args)
        : _source(args.source)
        , _type(args.type)

    {
    }

    std::optional<vk::ShaderModule> shader::get_module(const gfx::vulkan_context& vkctx) const
    {
        if (_module)
        {
            return **_module;
        }
        if (_spirv.empty())
        {
            return std::nullopt;
        }
        vk::ShaderModuleCreateInfo module_info;
        module_info.codeSize = sizeof(uint32_t) * _spirv.size();
        module_info.pCode = _spirv.data();

        _module = vkctx.device().createShaderModuleUnique(module_info);
        return **_module;
    }

    void shader::compile()
    {
        CRITICAL_CHECK(!_source.empty());

        shaderc::Compiler compiler;
        auto result = compiler.CompileGlslToSpv(_source, to_shaderc_shader_kind(_type), "main.glsl");
        if (result.GetCompilationStatus() != shaderc_compilation_status_success)
        {
            _message = result.GetErrorMessage();
            std::cout << _message << '\n';
            return;
        }

        const auto size = result.cend() - result.cbegin();
        _spirv.resize(size);
        std::ranges::copy(result, _spirv.begin());

        generate_reflection_data();
    }

    shader shader::from_compiled(shader_type type, std::string source, std::vector<uint32_t> spirv)
    {
        shader result = {};
        result._source = std::move(source);
        result._spirv = std::move(spirv);
        result._type = type;

        return result;
    }

    std::string shader::validate(const std::string& source, gfx::shader_type type)
    {
        shaderc::Compiler compiler;
        if (auto result = compiler.CompileGlslToSpv(source, to_shaderc_shader_kind(type), "main.glsl");
            result.GetCompilationStatus() != shaderc_compilation_status_success)
        {
            return result.GetErrorMessage();
        }
        return {};
    }

    void shader::generate_reflection_data()
    {
        const auto result_check = [](const auto result) {
            CRITICAL_CHECK(result == SpvReflectResult::SPV_REFLECT_RESULT_SUCCESS);
        };

        SpvReflectShaderModule module;
        const auto module_create_result = spvReflectCreateShaderModule(_spirv.size() * sizeof(uint32_t), _spirv.data(), &module);
        result_check(module_create_result);

        uint32_t input_variables_count = 0;
        const auto enum_input_vars_result = spvReflectEnumerateInputVariables(&module, &input_variables_count, nullptr);
        result_check(enum_input_vars_result);

        uint32_t output_variables_count = 0;
        const auto enum_output_vars_result = spvReflectEnumerateOutputVariables(&module, &output_variables_count, nullptr);
        result_check(enum_output_vars_result);

        std::vector<SpvReflectInterfaceVariable*> input_vars;
        input_vars.resize(input_variables_count);
        spvReflectEnumerateInputVariables(&module, &input_variables_count, input_vars.data());

        std::vector<SpvReflectInterfaceVariable*> output_vars;
        output_vars.resize(output_variables_count);
        spvReflectEnumerateOutputVariables(&module, &output_variables_count, output_vars.data());

        return;
    }
} // namespace cathedral::gfx