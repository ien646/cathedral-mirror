#pragma once

#include <cathedral/core.hpp>

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

        const std::string& path() const { return _path; }

        bool is_loaded() const { return _is_loaded; }
        void mark_as_manually_loaded() { _is_loaded = true; }

        virtual void save() const = 0;
        virtual void load() = 0;
        virtual void unload() = 0;

        void move_path(const std::string& new_path);

    protected:
        project& _project;
        bool _is_loaded = false;
        std::string _path;
    };

    template <typename T>
    constexpr std::string asset_typestr() = delete;

    namespace detail
    {
        bool path_is_asset_typestr(const std::string& path, const std::string& typestr);
    }

    template <typename TAsset>
        requires(std::is_base_of_v<asset, std::remove_cvref_t<TAsset>>)
    bool path_is_asset_type(const std::string& path)
    {
        return detail::path_is_asset_typestr(path, asset_typestr<TAsset>());
    }
} // namespace cathedral::project