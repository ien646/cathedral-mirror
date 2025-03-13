#pragma once

#include <QSyntaxHighlighter>

#include <vector>

namespace cathedral::editor
{
    class shader_syntax_highlighter : public QSyntaxHighlighter
    {
    public:
        shader_syntax_highlighter(QTextDocument* parent);

    protected:
        void highlightBlock(const QString& text) override;

    private:
        struct format_rule_group
        {
            std::vector<QRegularExpression> patterns;
            QTextCharFormat format;
        };

        std::vector<format_rule_group> _rules;
    };
} // namespace cathedral::editor