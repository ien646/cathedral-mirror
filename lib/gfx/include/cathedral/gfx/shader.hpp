#pragma once

#include <cathedral/core.hpp>
#include <cathedral/gfx/types.hpp>
#include <cathedral/gfx/vulkan_context.hpp>

#include <vulkan/vulkan.hpp>

namespace cathedral::gfx
{
    struct shader_args
    {
        shader_type type = shader_type::UNDEFINED;
        std::string source;
    };

    class shader
    {
    public:
        explicit shader(const shader_args&);

        std::optional<vk::ShaderModule> get_module(const gfx::vulkan_context& vkctx) const;

        void compile();

        shader_type type() const { return _type; }

        bool valid() const { return _module.has_value(); }

        const std::string& compilation_message() const { return _message; }

        const std::string& source() const { return _source; }

        const std::vector<uint32_t>& spirv() const { return _spirv; }

        static shader from_compiled(shader_type type, std::string source, std::vector<uint32_t> spirv);

        static std::string validate(const std::string& source, gfx::shader_type type);

    private:
        mutable std::optional<vk::UniqueShaderModule> _module;
        std::string _source;
        shader_type _type = shader_type::UNDEFINED;
        std::vector<uint32_t> _spirv;
        std::string _message;

        shader() = default;
    };
} // namespace cathedral::gfx