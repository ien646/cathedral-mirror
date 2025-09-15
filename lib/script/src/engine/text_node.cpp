#include <cathedral/script/engine/text_node.hpp>

#include <cathedral/engine/nodes/text_node.hpp>
#include <cathedral/script/engine/node.hpp>
#include <cathedral/script/init_macros.hpp>

namespace
{
    const std::string annotations = R"lua(

---@class text_node : node
---@field public set_text fun(self, text: string)
---@field public text fun(self): string
---@field public set_font fun(self, name: string)
---@field public font fun(self): string
---@field public set_mode fun(self, mode: font_mode)
---@field public mode fun(self): font_mode
text_node = {{}}

)lua";
}

namespace cathedral::script::engine
{
    void text_node_initializer::initialize(state& s)
    {
        AUTO_INIT_NEW_TYPE(s, cathedral::engine, text_node);
        AUTO_BASE_CLASSES(cathedral::engine::drawable_node, cathedral::engine::node, cathedral::engine::scene_node);

        AUTO_FUNC(set_text);
        AUTO_FUNC(text);

        AUTO_FUNC_NAMED(set_font, set_font_name);
        AUTO_FUNC_NAMED(font, font_name);

        AUTO_FUNC(set_mode);
        AUTO_FUNC(mode);
    }

    const std::string& text_node_initializer::get_annotations()
    {
        return annotations;
    }
} // namespace cathedral::script::engine