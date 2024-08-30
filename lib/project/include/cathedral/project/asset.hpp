#pragma once

#include <cathedral/core.hpp>

#include <string>

namespace cathedral::project
{
    class asset : public uid_type
    {
    public:
        asset(std::string path)
            : _path(std::move(path))
        {
        }

        const std::string& path() const { return _path; }

        bool is_loaded() const { return _is_loaded; }
        void mark_as_manually_loaded() { _is_loaded = true; }
        
        virtual void save() const = 0;
        virtual void load() = 0;
        virtual void unload() = 0;
        virtual constexpr std::string typestr() const = 0;

        void move_path(const std::string& new_path);

    protected:
        bool _is_loaded = false;
        std::string _path;
    };
} // namespace cathedral::project