#include <cathedral/gfx/shader.hpp>

#include <cathedral/gfx/vulkan_context.hpp>

#include <shaderc/shaderc.hpp>

#include <iostream>

namespace cathedral::gfx
{
    namespace
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
    } // namespace

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
        CRITICAL_CHECK(!_source.empty(), "Shader source is empty");

        const shaderc::Compiler compiler;
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
        const shaderc::Compiler compiler;
        if (const auto result = compiler.CompileGlslToSpv(source, to_shaderc_shader_kind(type), "main.glsl");
            result.GetCompilationStatus() != shaderc_compilation_status_success)
        {
            return result.GetErrorMessage();
        }
        return {};
    }
} // namespace cathedral::gfx