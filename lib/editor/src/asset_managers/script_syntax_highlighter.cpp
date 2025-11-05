#include <cathedral/editor/asset_managers/script_syntax_highlighter.hpp>

namespace cathedral::editor
{
    namespace
    {
        QRegularExpression to_rx(const char* text)
        {
            return QRegularExpression(QString{ "\\b" } + QString{ text } + QString{ "\\b" });
        }

        std::vector<QRegularExpression> to_single_word_regular_expressions(std::initializer_list<const char*> patterns)
        {
            std::vector<QRegularExpression> result;
            for (const auto& pattern : patterns)
            {
                result.push_back(to_rx(pattern));
            }
            return result;
        }
    } // namespace

    script_syntax_highlighter::script_syntax_highlighter(QTextDocument* parent)
        : syntax_highlighter_base(parent)
    {
        format_rule_group script_funcs_rules = {
            .patterns = to_single_word_regular_expressions(
                { "function[ \t\n]+init", "function[ \t\n]+editor_tick", "function[ \t\n]+tick", "function[ \t\n]+teardown" })
        };
        script_funcs_rules.format.setForeground(QColor(0x880088));
        script_funcs_rules.format.setFontWeight(QFont::Bold);
        script_funcs_rules.format.setFontItalic(true);

        format_rule_group keyword_rules = { .patterns = to_single_word_regular_expressions(
                                                { "and", "break",    "do",     "else", "elseif", "end",   "false",
                                                  "for", "function", "if",     "in",   "local",  "nil",   "not",
                                                  "or",  "repeat",   "return", "then", "true",   "until", "while" }) };
        keyword_rules.format.setFontWeight(QFont::Bold);
        keyword_rules.format.setForeground(QColor(0x008800));
        keyword_rules.format.setFontItalic(false);

        format_rule_group comment_rules = { .patterns = { { QRegularExpression("--.*$") } } };

        comment_rules.format.setFontItalic(true);
        comment_rules.format.setForeground(QColor(0x447744));

        // Order matters
        _rules.push_back(MOVE(script_funcs_rules));
        _rules.push_back(MOVE(keyword_rules));
        _rules.push_back(MOVE(comment_rules));
    }
} // namespace cathedral::editor