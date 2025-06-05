#pragma once

#include <QRegularExpression>
#include <QSyntaxHighlighter>

#include <vector>

namespace cathedral::editor
{
    class syntax_highlighter_base : public QSyntaxHighlighter
    {
    public:
        explicit syntax_highlighter_base(QTextDocument* parent);

    protected:
        void highlightBlock(const QString& text) override;

        struct format_rule_group
        {
            std::vector<QRegularExpression> patterns;
            QTextCharFormat format;
        };

        std::vector<format_rule_group> _rules;
    };
} // namespace cathedral::editor