#pragma once

#include <cathedral/core.hpp>
#include <cathedral/gfx/vulkan_context.hpp>

#include <vulkan/vulkan.hpp>

namespace cathedral::gfx
{
    enum class shader_type
    {
        VERTEX,
        FRAGMENT
    };

    struct shader_args
    {
        const vulkan_context* vkctx = nullptr;
        shader_type type;
        std::string source;
        bool store_spirv = is_debug_build();
    };

    class shader
    {
    public:
        shader(shader_args);

        inline vk::ShaderModule module() const { return **_module; }
        shader_type type() const { return _type; }
        bool valid() const { return _module.has_value(); }
        inline const std::string& compilation_message() const { return _message; }

    private:
        std::optional<vk::UniqueShaderModule> _module;
        shader_type _type;
        std::vector<uint32_t> _spirv;
        std::string _message;
    };
}