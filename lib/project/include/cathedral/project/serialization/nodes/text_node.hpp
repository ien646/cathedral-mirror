#pragma once

#include <cathedral/engine/nodes/text_node.hpp>

#include <cathedral/project/serialization/enums.hpp>
#include <cathedral/project/serialization/transform.hpp>

#include <cathedral/glm_serializers.hpp>

#include <cereal/types/memory.hpp>
#include <cereal/types/polymorphic.hpp>
#include <cereal/types/string.hpp>
#include <cereal/types/vector.hpp>

#include <ranges>

namespace cereal
{
    template <typename Archive>
    void CEREAL_SAVE_FUNCTION_NAME(Archive& ar, const cathedral::engine::text_node& node)
    {
        std::vector<uint32_t> text;
        std::ranges::copy(node.text(), std::back_inserter(text));

        ar(make_nvp("node", cereal::base_class<cathedral::engine::node>(&node)),
           make_nvp("font_name", node.font_name()),
           make_nvp("text", text),
           make_nvp("text_color", node.text_color()),
           make_nvp("horizontal_spacing", node.horizontal_spacing()),
           make_nvp("mode", node.mode()));
    }

    template <typename Archive>
    void CEREAL_LOAD_FUNCTION_NAME(Archive& ar, cathedral::engine::text_node& node)
    {
        std::optional<std::string> font_name;
        std::vector<uint32_t> text;
        glm::vec3 text_color;
        float horizontal_spacing;
        cathedral::engine::font_mode mode;

        ar(cereal::base_class<cathedral::engine::node>(&node), font_name, text, text_color, horizontal_spacing, mode);

        std::u32string u32text;
        std::ranges::copy(node.text(), std::back_inserter(u32text));

        node.set_font_name(font_name);
        node.set_text(u32text);
        node.set_text_color(text_color);
        node.set_horizontal_spacing(horizontal_spacing);
        node.set_mode(mode);
    }
} // namespace cereal

CEREAL_REGISTER_TYPE(cathedral::engine::text_node);
CEREAL_REGISTER_POLYMORPHIC_RELATION(cathedral::engine::node, cathedral::engine::text_node);