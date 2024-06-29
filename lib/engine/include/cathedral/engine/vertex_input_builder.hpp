#pragma once

#include <cathedral/gfx/types.hpp>

#include <vector>

namespace cathedral::engine
{
    class vertex_input_builder
    {
    public:
        vertex_input_builder& push(gfx::vertex_data_type type);

        inline std::vector<gfx::vertex_input_attribute> build() const { return _attributes; }

    private:
        std::vector<gfx::vertex_input_attribute> _attributes;
        uint32_t _offset = 0;
    };
}