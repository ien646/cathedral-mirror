#pragma once

#include <cathedral/core.hpp>

#include <nlohmann/json_fwd.hpp>

#include <string>

namespace cathedral::project
{
    class project;

    class asset : public uid_type
    {
    public:
        asset(project& pro, std::string path)
            : _project(pro)
            , _path(std::move(path))
        {
        }

        virtual ~asset() = default;

        const std::string& path() const { return _path; }

        bool is_loaded() const { return _is_loaded; }

        void mark_as_manually_loaded() { _is_loaded = true; }

        virtual void save() const = 0;
        virtual void load() = 0;
        virtual void unload() = 0;

        virtual std::string relative_path() const = 0;

        void move_path(const std::string& new_path);

    protected:
        project& _project;
        bool _is_loaded = false;
        std::string _path;

        nlohmann::json get_asset_json() const;

        void write_asset_json(const nlohmann::json& j) const;
        void write_asset_binary(const std::vector<std::byte>& data) const;
        std::string get_binpath() const;
    };

    template <typename T>
    concept AssetLike = std::is_base_of_v<asset, T>;

    template <AssetLike T>
    constexpr std::string asset_typestr() = delete;

    namespace detail
    {
        bool path_is_asset_typestr(const std::string& path, const std::string& typestr);
    }

    template <AssetLike TAsset>
    bool path_is_asset_type(const std::string& path)
    {
        return detail::path_is_asset_typestr(path, asset_typestr<TAsset>());
    }
} // namespace cathedral::project