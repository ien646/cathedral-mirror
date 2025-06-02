#pragma once

#include <cathedral/project/asset.hpp>

namespace cathedral::project
{
    class dynamic_script_asset final : public asset
    {
    public:
        using asset::asset;

        CATHEDRAL_ASSET_SUBCLASS_DECL

        const std::string& source() const { return _source; }

        void set_source(std::string source) { _source = std::move(source); }

        constexpr const char* typestr() const override { return "dynamic_script"; }

    private:
        std::string _source;

        friend class cereal::access;

        template <typename Archive>
        void CEREAL_SERIALIZE_FUNCTION_NAME(Archive& ar)
        {
            ar(cereal::make_nvp("asset", cereal::base_class<asset>(this)), cereal::make_nvp("source", _source));
        }
    };
} // namespace cathedral::project