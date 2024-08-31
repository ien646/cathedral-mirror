#pragma once

#include <cathedral/engine/material.hpp>
#include <cathedral/project/asset.hpp>
#include <cathedral/project/assets/shader_asset.hpp>

namespace cathedral::project
{
    class material_asset : public asset
    {
    public:
        material_asset(project& pro, std::string path)
            : asset(pro, std::move(path))
        {
        }

        auto material_domain() const { return _material_domain; }
        void set_material_domain(engine::material_domain domain) { _material_domain = domain; }

        auto vertex_shader() { return _vertex_shader; }
        void set_vertex_shader(std::shared_ptr<shader_asset> asset) { _vertex_shader = std::move(asset); }

        auto fragment_shader() { return _fragment_shader; }
        void set_fragment_shader(std::shared_ptr<shader_asset> asset) { _fragment_shader = std::move(asset); }

        void load() override;
        void save() const override;
        void unload() override;

    private:
        engine::material_domain _material_domain;
        std::shared_ptr<shader_asset> _vertex_shader;
        std::shared_ptr<shader_asset> _fragment_shader;
    };

    template <>
    constexpr std::string asset_typestr<material_asset>()
    {
        return "material";
    }
} // namespace cathedral::project