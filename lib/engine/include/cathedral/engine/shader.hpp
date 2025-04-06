#pragma once

#include <cathedral/gfx/shader.hpp>

#include <cathedral/engine/shader_preprocess.hpp>

#include <memory>

namespace cathedral::engine
{
    class shader
    {
    public:
        shader(std::shared_ptr<gfx::shader> shader, shader_preprocess_data pp_data)
            : _shader(std::move(shader))
            , _pp_data(std::move(pp_data))
        {
        }

        const auto& gfx_shader() const { return *_shader; }

        const auto& preprocess_data() const { return _pp_data; }

    private:
        std::shared_ptr<gfx::shader> _shader;
        shader_preprocess_data _pp_data;
    };
} // namespace cathedral::engine