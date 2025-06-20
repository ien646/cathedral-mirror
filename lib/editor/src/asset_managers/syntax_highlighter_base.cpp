#include <cathedral/editor/asset_managers/syntax_highlighter_base.hpp>

#include <QRegularExpression>
#include <regex>

namespace cathedral::editor
{
    syntax_highlighter_base::syntax_highlighter_base(QTextDocument* parent)
        : QSyntaxHighlighter(parent)
    {
    }

    void syntax_highlighter_base::highlightBlock(const QString& text)
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