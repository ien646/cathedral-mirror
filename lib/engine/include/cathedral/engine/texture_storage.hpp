#pragma once

#include <cathedral/engine/texture.hpp>

namespace cathedral::engine
{
    class texture_storage
    {
    public:

    private:
        std::unordered_map<std::string, std::weak_ptr<texture>> _textures;
    };
}