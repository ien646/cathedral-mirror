#pragma once

#include <cathedral/project/asset.hpp>

namespace cathedral::project
{
    class dynamic_script_asset final : public asset
    {
    public:
        using asset::asset;

        void save() const override;
        void load() override;
        std::string relative_path() const override;
        std::string name() const override;

        const std::string& source() const { return _source; }

        void set_source(std::string source) { _source = MOVE(source); }

        constexpr const char* typestr() const override { return "dynamic_script"; }

    private:
        std::string _source;
    };
} // namespace cathedral::project