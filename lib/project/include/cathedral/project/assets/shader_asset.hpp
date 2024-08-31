#pragma once

#include <cathedral/gfx/shader.hpp>
#include <cathedral/project/asset.hpp>

namespace cathedral::project
{
    class shader_asset : public asset
    {
    public:
        shader_asset(std::string path)
            : asset(std::move(path))
        {
        }

        bool is_loaded() const override;
        void load() override;
        void save() const override;
        void unload() override;

        gfx::shader_type type() const { return _type; }
        const std::string& source() const { return _source; }
        const std::vector<uint32_t> spirv() const { return _spirv; }

    private:
        gfx::shader_type _type = gfx::shader_type::UNDEFINED;
        std::string _source;
        std::vector<uint32_t> _spirv;
    };
} // namespace cathedral::project