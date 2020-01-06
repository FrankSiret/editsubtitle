#include "subsyntaxhighligther.h"

SubSyntaxHighligther::SubSyntaxHighligther(QTextDocument *parent) : QSyntaxHighlighter(parent)
{
    HighlightingRule rule;

    // number format
    tagFormat.setForeground(Qt::red);
    tagFormat.setFontWeight(QFont::Bold);

    // time format
    attributeFormat.setForeground(Qt::blue);
    attributeFormat.setFontWeight(QFont::Bold);

    //2
    //00:01:21,827 --> 00:01:24,159

    // number format
    rule.pattern = QRegExp("(^[ ]*[0-9]+[ ]*$)");
    rule.format = tagFormat;
    highlightingRules.append(rule);

    // time format
    rule.pattern = QRegExp("(^[ ]*[0-9]+:[0-9]+:[0-9]+,[0-9]+[ ]*-->[ ]*[0-9]+:[0-9]+:[0-9]+,[0-9]+[ ]*$)");
    rule.format = attributeFormat;
    highlightingRules.append(rule);
}

void SubSyntaxHighligther::highlightBlock(const QString &text)
{
    for (const HighlightingRule &rule : qAsConst(highlightingRules)) {
        QRegExp expression(rule.pattern);
        int index = text.indexOf(expression);
        while (index >= 0) {
            int length = expression.matchedLength();
            setFormat(index, length, rule.format);
            index = text.indexOf(expression, index + length);
        }
    }
    setCurrentBlockState(0);
}

