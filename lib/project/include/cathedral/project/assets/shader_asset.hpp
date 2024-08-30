#pragma once

#include <cathedral/gfx/shader.hpp>
#include <cathedral/project/asset.hpp>

namespace cathedral::project
{
    class shader_asset : public asset
    {
    public:
        shader_asset(project& pro, std::string path)
            : asset(pro, std::move(path))
        {
        }

        void load() override;
        void save() const override;
        void unload() override;

        gfx::shader_type type() const { return _type; }
        void set_type(gfx::shader_type type) { _type = type; }

        const std::string& source() const { return _source; }
        void set_source(std::string source) { _source = std::move(source); }

    private:
        gfx::shader_type _type = gfx::shader_type::UNDEFINED;
        std::string _source;
    };

    template <>
    constexpr std::string asset_typestr<shader_asset>()
    {
        return "shader";
    }
} // namespace cathedral::project