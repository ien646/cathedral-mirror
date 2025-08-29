#pragma once

#include <cereal/archives/json.hpp>

#include <ien/io_utils.hpp>

#include <magic_enum.hpp>

#include <fstream>

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
            archive(cereal::make_nvp(this->typestr(), *this));                                                              \
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

#define CATHEDRAL_SERIALIZE_ENUM_AUTO(in_class)                                                                             \
    namespace cereal                                                                                                        \
    {                                                                                                                       \
        template <typename Archive>                                                                                         \
        std::string CEREAL_SAVE_MINIMAL_FUNCTION_NAME([[maybe_unused]] const Archive& ar, const in_class& binding)          \
        {                                                                                                                   \
            return std::string{ magic_enum::enum_name(binding) };                                                           \
        }                                                                                                                   \
                                                                                                                            \
        template <typename Archive>                                                                                         \
        void CEREAL_LOAD_MINIMAL_FUNCTION_NAME(                                                                             \
            [[maybe_unused]] const Archive& ar,                                                                             \
            in_class& binding,                                                                                              \
            const std::string& value)                                                                                       \
        {                                                                                                                   \
            const auto opt = magic_enum::enum_cast<in_class>(value);                                                        \
            CRITICAL_CHECK(opt.has_value(), "Invalid enum value");                                                          \
            binding = *opt;                                                                                                 \
        }                                                                                                                   \
    }
