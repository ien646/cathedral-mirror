#include <cathedral/gfx/shader.hpp>

#include <shaderc/shaderc.hpp>

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
        }
        CRITICAL_ERROR("Unhandled shader type");
        return static_cast<shaderc_shader_kind>(0);
    }

    shader::shader(shader_args args)
        : _type(args.type)
    {
        CRITICAL_CHECK_NOTNULL(args.vkctx);
        CRITICAL_CHECK(!args.source.empty());

        shaderc::Compiler compiler;
        auto result = compiler.CompileGlslToSpv(args.source, to_shaderc_shader_kind(args.type), "");
        if (result.GetCompilationStatus() != shaderc_compilation_status_success)
        {
            _message = result.GetErrorMessage();
            std::cout << _message << std::endl;
            return;
        }

        const auto size = result.cend() - result.cbegin();
        std::vector<uint32_t> spirv;
        spirv.resize(size);
        std::copy(result.cbegin(), result.cend(), spirv.begin());

        if (args.store_spirv)
        {
            _spirv.resize(size);
            std::copy(result.cbegin(), result.cend(), _spirv.begin());
        }

        vk::ShaderModuleCreateInfo module_info;
        module_info.codeSize = sizeof(uint32_t) * spirv.size();
        module_info.pCode = spirv.data();

        _module = args.vkctx->device().createShaderModuleUnique(module_info);
    }
} // namespace cathedral::gfx