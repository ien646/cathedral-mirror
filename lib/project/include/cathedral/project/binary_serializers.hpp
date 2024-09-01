#pragma once

#include <ien/serialization.hpp>

#include <glm/matrix.hpp>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

namespace ien
{
    template <typename TElem, int Dim>
    struct value_serializer<glm::vec<Dim, TElem>>
    {
        void serialize(const glm::vec<Dim, TElem>& value, serializer_inserter& inserter) const
        {
            for (int i = 0; i < Dim; ++i)
            {
                ien::value_serializer<TElem>{}.serialize(value[i], inserter);
            }
        }
    };

    template <typename TElem, int Dim>
    struct value_deserializer<glm::vec<Dim, TElem>>
    {
        glm::vec<Dim, TElem> deserialize(deserializer_iterator& iterator) const
        {
            glm::vec<Dim, TElem> result;
            for (int i = 0; i < Dim; ++i)
            {
                result[i] = ien::value_deserializer<TElem>{}.deserialize(iterator);
            }
            return result;
        }
    };
} // namespace ien