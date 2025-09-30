#include <cathedral/script/engine/drawable_node.hpp>

#include <cathedral/engine/nodes/bits/drawable_node.hpp>
#include <cathedral/script/init_macros.hpp>

namespace cathedral::script::engine
{
    namespace
    {
        const std::string annotations = "";
    }

    void drawable_node_initializer::initialize(state& s)
    {
        AUTO_INIT_NEW_TYPE(s, cathedral::engine, drawable_node);
        AUTO_BASE_CLASSES(cathedral::engine::node, cathedral::engine::scene_node);
    }

    const std::string& drawable_node_initializer::get_annotations()
    {
        return annotations;
    }
} // namespace cathedral::script::engine