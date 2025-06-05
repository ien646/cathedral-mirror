#pragma once

#include <cathedral/editor/asset_managers/syntax_highlighter_base.hpp>

namespace cathedral::editor
{
    class script_syntax_highlighter final : public syntax_highlighter_base
    {
    public:
        explicit script_syntax_highlighter(QTextDocument* parent);
    };
} // namespace cathedral::editor