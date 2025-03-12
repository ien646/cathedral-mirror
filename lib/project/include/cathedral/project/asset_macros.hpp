#pragma once

#include <cereal/archives/json.hpp>

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
