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

        virtual bool is_loaded() const = 0;
        virtual void save() const = 0;
        virtual void load() = 0;
        virtual void unload() = 0;

    protected:
        const std::string _path;
    };
} // namespace cathedral::project