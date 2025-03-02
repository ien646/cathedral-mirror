#pragma once

#include <cathedral/core.hpp>

#include <nlohmann/json_fwd.hpp>

#include <fstream>
#include <string>

#include <cathedral/cereal_serializers.hpp>

#define CATHEDRAL_ASSET_SUBCLASS_DECL                                                                                       \
    void save() const override;                                                                                             \
    void load() override;                                                                                                   \
    std::string relative_path() const override;                                                                             \
    std::string name() const override;

#define CATHEDRAL_ASSET_SUBCLASS_IMPL(_class)                                                                               \
    void _class::save() const                                                                                               \
    {                                                                                                                       \
        std::stringstream sstr;                                                                                             \
        {                                                                                                                   \
            cereal::JSONOutputArchive archive(sstr);                                                                        \
            archive(*this);                                                                                                 \
        }                                                                                                                   \
        ien::write_file_text(_path, sstr.str());                                                                            \
    }                                                                                                                       \
                                                                                                                            \
    void _class::load()                                                                                                     \
    {                                                                                                                       \
        std::ifstream ifs(_path);                                                                                           \
        cereal::JSONInputArchive input(ifs);                                                                                \
        input(*this);                                                                                                       \
    }                                                                                                                       \
                                                                                                                            \
    std::string _class::relative_path() const                                                                               \
    {                                                                                                                       \
        return _project->abspath_to_relpath<_class>(_path);                                                                 \
    }                                                                                                                       \
                                                                                                                            \
    std::string _class::name() const                                                                                        \
    {                                                                                                                       \
        return _project->abspath_to_name<_class>(_path);                                                                    \
    }

namespace cathedral::project
{
    class project;

    class asset : public uid_type
    {
    public:
        asset(project* pro, std::string path)
            : _project(pro)
            , _path(std::move(path))
        {
        }

        virtual ~asset() = default;

        const std::string& absolute_path() const { return _path; }

        bool is_loaded() const { return _is_loaded; }

        void mark_as_manually_loaded() { _is_loaded = true; }

        virtual void save() const = 0;
        virtual void load() = 0;

        virtual std::string relative_path() const = 0;
        virtual std::string name() const = 0;

        virtual constexpr const char* typestr() const = 0;

        void move_path(const std::string& new_path);

        std::string binpath() const;

    protected:
        asset() = default;

        project* _project;
        bool _is_loaded = false;
        std::string _path;

        nlohmann::json get_asset_json() const;

        void set_path_by_relpath(const std::string& relpath);

        void write_asset_json(const nlohmann::json& j) const;
        void write_asset_binary(const std::vector<std::byte>& data) const;       

        friend class cereal::access;

        template <typename Archive>
        void CEREAL_SAVE_FUNCTION_NAME(Archive& ar) const
        {
            ar(cereal::make_nvp("type", std::string{ typestr() }));
        }

        template <typename Archive>
        void CEREAL_LOAD_FUNCTION_NAME(Archive& ar)
        {
            std::string type;
            ar(type);

            CRITICAL_CHECK(type == typestr());
        }

        template <typename TAsset>
        friend constexpr const char* get_asset_typestr();
    };

    template <typename T>
    concept AssetLike = std::is_base_of_v<asset, T>;

    template <AssetLike TAsset>
    constexpr const char* get_asset_typestr()
    {
        return TAsset{}.typestr();
    }
} // namespace cathedral::project