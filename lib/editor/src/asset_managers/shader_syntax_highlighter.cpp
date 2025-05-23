#include <cathedral/editor/asset_managers/shader_syntax_highlighter.hpp>

#include <QRegularExpression>

namespace
{
    QRegularExpression operator""_rx(const char* text, [[maybe_unused]] size_t unused)
    {
        return QRegularExpression("\\b" + QRegularExpression::escape(text) + "\\b");
    }
} // namespace

namespace cathedral::editor
{
    shader_syntax_highlighter::shader_syntax_highlighter(QTextDocument* parent)
        : QSyntaxHighlighter(parent)
    {
        format_rule_group shader_layout_rules = { .patterns = { "layout"_rx, "location"_rx, "set"_rx, "binding"_rx },
                                                  .format = {} };
        shader_layout_rules.format.setForeground(QColor(0xAA0000));
        shader_layout_rules.format.setFontWeight(QFont::Bold);

        format_rule_group in_out_rules = { .patterns = { "in"_rx, "out"_rx }, .format = {} };
        in_out_rules.format.setForeground(QColor(0xAA00AA));
        in_out_rules.format.setFontWeight(QFont::Bold);

        format_rule_group type_rules = {
            .patterns = { "bool"_rx,   "int"_rx,    "uint"_rx,   "float"_rx,  "double"_rx, "bvec2"_rx,  "bvec3"_rx,
                          "bvec4"_rx,  "ivec2"_rx,  "ivec3"_rx,  "ivec4"_rx,  "uvec2"_rx,  "uvec3"_rx,  "uvec4"_rx,
                          "vec2"_rx,   "vec3"_rx,   "vec4"_rx,   "dvec2"_rx,  "dvec3"_rx,  "dvec4"_rx,  "mat2x2"_rx,
                          "mat2x3"_rx, "mat2x4"_rx, "mat3x2"_rx, "mat3x3"_rx, "mat3x4"_rx, "mat4x2"_rx, "mat4x3"_rx,
                          "mat4x4"_rx, "mat2"_rx,   "mat3"_rx,   "mat4"_rx },
            .format = {}
        };
        type_rules.format.setFontItalic(true);
        type_rules.format.setForeground(QColor(0x004000));

        format_rule_group gl_keyword_rules = { .patterns = { "gl_Position"_rx }, .format = {} };
        gl_keyword_rules.format.setUnderlineStyle(QTextCharFormat::DotLine);
        gl_keyword_rules.format.setUnderlineColor(Qt::black);

        format_rule_group ppmacro_keyword_rules = { .patterns = { "MATERIAL_TEXTURES"_rx, "NODE_TEXTURES"_rx },
                                                    .format = {} };
        ppmacro_keyword_rules.format.setForeground(QColor(0xFF00FF));
        ppmacro_keyword_rules.format.setFontWeight(QFont::Weight::Bold);

        _rules.push_back(shader_layout_rules);
        _rules.push_back(in_out_rules);
        _rules.push_back(type_rules);
        _rules.push_back(gl_keyword_rules);
        _rules.push_back(ppmacro_keyword_rules);
    }

    void shader_syntax_highlighter::highlightBlock(const QString& text)
    {
        for (const auto& [patterns, format] : _rules)
        {
            for (const auto& regex : patterns)
            {
                QRegularExpressionMatchIterator matcher = regex.globalMatch(text);
                while (matcher.hasNext())
                {
                    const QRegularExpressionMatch match = matcher.next();
                    setFormat(static_cast<int>(match.capturedStart()), static_cast<int>(match.capturedLength()), format);
                }
            }
        }
    }
} // namespace cathedral::editor