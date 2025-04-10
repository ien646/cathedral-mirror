#pragma once

#include <cathedral/gfx/shader.hpp>

#include <cathedral/project/asset.hpp>
#include <cathedral/project/serialization/enums.hpp>

#include <cereal/types/base_class.hpp>
#include <cereal/types/string.hpp>

namespace cathedral::project
{
    class shader_asset : public asset
    {
    public:
        using asset::asset;

        CATHEDRAL_ASSET_SUBCLASS_DECL

        gfx::shader_type type() const { return _type; }

        void set_type(gfx::shader_type type) { _type = type; }

        const std::string& source() const { return _source; }

        void set_source(std::string source) { _source = std::move(source); }

        constexpr const char* typestr() const override { return "shader"; };

    private:
        gfx::shader_type _type = gfx::shader_type::UNDEFINED;
        std::string _source;

        friend class cereal::access;

        template <typename Archive>
        void CEREAL_SERIALIZE_FUNCTION_NAME(Archive& ar)
        {
            ar(cereal::make_nvp("asset", cereal::base_class<asset>(this)),
               cereal::make_nvp("type", _type),
               cereal::make_nvp("source", _source));
        }
    };
} // namespace cathedral::project