#include <cathedral/engine/materials/definitions.hpp>

#include <cathedral/engine/vertex_input_builder.hpp>

namespace cathedral::engine
{
    gfx::vertex_input_description standard_vertex_input_description()
    {
        gfx::vertex_input_description result;
        result.vertex_size = (3 + 2 + 3 + 4) * sizeof(float);
        result.attributes =
            vertex_input_builder()
                .push(gfx::vertex_data_type::VEC3F)
                .push(gfx::vertex_data_type::VEC2F)
                .push(gfx::vertex_data_type::VEC3F)
                .push(gfx::vertex_data_type::VEC4F)
                .build();

        return result;
    }
} // namespace cathedral::engine