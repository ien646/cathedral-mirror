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
        struct FormatRuleGroup
        {
            std::vector<QRegularExpression> patterns;
            QTextCharFormat format;
        };

        std::vector<FormatRuleGroup> _rules;
    };
} // namespace cathedral::editor