#pragma once

#include <cereal/cereal.hpp>

#include <cereal/access.hpp>

#include <cereal/types/base_class.hpp>
#include <cereal/types/optional.hpp>
#include <cereal/types/polymorphic.hpp>
#include <cereal/types/string.hpp>
#include <cereal/types/unordered_map.hpp>
#include <cereal/types/utility.hpp>
#include <cereal/types/variant.hpp>
#include <cereal/types/vector.hpp>

#include <cereal/archives/json.hpp>

#include <glm/matrix.hpp>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

#include <magic_enum.hpp>

namespace cereal
{
    template <typename Archive, typename TElem>
    void serialize(Archive& archive, glm::vec<2, TElem>& value)
    {
        archive(value.x, value.y);
    }

    template <typename Archive, typename TElem>
    void serialize(Archive& archive, glm::vec<3, TElem>& value)
    {
        archive(value.x, value.y, value.z);
    }

    template <typename Archive, typename TElem>
    void serialize(Archive& archive, glm::vec<4, TElem>& value)
    {
        archive(value.x, value.y, value.z, value.w);
    }

    template <typename Archive, typename TElem, glm::length_t X, glm::length_t Y>
    void serialize(Archive& ar, glm::mat<X, Y, TElem>& value)
    {
        for (size_t y = 0; y < Y; ++y)
        {
            for (size_t x = 0; x < X; ++x)
            {
                ar(cereal::make_nvp(std::format("[{}][{}]", x, y), value[x][y]));
            }
        }
    }

    template <typename Archive, typename Enum>
        requires(std::is_enum_v<Enum> && cereal::traits::is_text_archive<Archive>::value)
    std::string CEREAL_SAVE_MINIMAL_FUNCTION_NAME([[maybe_unused]] Archive& ar, const Enum& type)
    {
        return std::string{ magic_enum::enum_name(type) };
    }

    template <typename Archive, typename Enum>
        requires(std::is_enum_v<Enum> && cereal::traits::is_text_archive<Archive>::value)
    void CEREAL_LOAD_MINIMAL_FUNCTION_NAME([[maybe_unused]] const Archive& ar, Enum& type, const std::string& value)
    {
        auto opt = magic_enum::enum_cast<Enum>(value);
        CRITICAL_CHECK(opt.has_value());
        type = *opt;
    }
} // namespace cereal