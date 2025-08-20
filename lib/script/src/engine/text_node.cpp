#include <cathedral/script/engine/text_node.hpp>

#include <cathedral/engine/nodes/text_node.hpp>
#include <cathedral/script/engine/node.hpp>
#include <cathedral/script/init_macros.hpp>

#define INIT_VEC_(prefix, type, dimensions)                                                                                 \
    AUTO_STATE.set(                                                                                                         \
        "set_node_variable_" prefix "vec" #dimensions,                                                                      \
        [](AUTO_TYPE& self, const std::string& name, const glm::vec<dimensions, type>& value) {                             \
            self.set_node_uniform_variable_value<glm::vec<dimensions, type>>(name, value);                                  \
        })

#define INIT_VEC_ALL(prefix, type)                                                                                          \
    INIT_VEC_(prefix, type, 2);                                                                                             \
    INIT_VEC_(prefix, type, 3);                                                                                             \
    INIT_VEC_(prefix, type, 4)

namespace cathedral::script::engine
{
    constexpr auto ANNOTATIONS_FORMAT = R"lua(

---@class text_node
{0}
---@field public set_text fun(self, text: string)
---@field public text fun(self): string
---@field public set_font fun(self, name: string)
---@field public font fun(self): string
---@field public set_mode fun(self, mode: font_mode)
---@field public mode fun(self): font_mode
text_node = {{}}
)lua";

    void text_node_initializer::initialize(state& s)
    {
        AUTO_INIT_NEW_TYPE(s, cathedral::engine, text_node);
        AUTO_BASE_CLASS(cathedral::engine::node);

        AUTO_FUNC(set_text);
        AUTO_FUNC(text);

        AUTO_FUNC_NAMED(set_font, set_font_name);
        AUTO_FUNC_NAMED(font, font_name);

        AUTO_FUNC(set_mode);
        AUTO_FUNC(mode);
    }

    const std::string& text_node_initializer::get_annotations()
    {
        static const std::string annotations =
            std::format(ANNOTATIONS_FORMAT, node_initializer{}.get_inheritable_annotations());
        return annotations;
    }
} // namespace cathedral::script::engine