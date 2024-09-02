#include <cathedral/engine/vertex_input_builder.hpp>

namespace cathedral::engine
{
    constexpr uint32_t get_offset_for_type(gfx::vertex_data_type type)
    {
        switch (type)
        {
        case gfx::vertex_data_type::FLOAT:
            return sizeof(float);
        case gfx::vertex_data_type::VEC2F:
            return sizeof(float) * 2;
        case gfx::vertex_data_type::VEC3F:
            return sizeof(float) * 3;
        case gfx::vertex_data_type::VEC4F:
            return sizeof(float) * 4;
        default:
            CRITICAL_ERROR("Unhandled vertex data type");
        }
    }

    vertex_input_builder& vertex_input_builder::push(gfx::vertex_data_type type)
    {
        gfx::vertex_input_attribute attr;
        attr.location = static_cast<uint32_t>(_attributes.size());
        attr.offset = _offset;
        attr.type = type;

        _attributes.push_back(attr);
        _offset += get_offset_for_type(type);

        return *this;
    }
} // namespace cathedral::engine