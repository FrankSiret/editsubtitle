#ifndef SUBSYNTAXHIGHLIGTHER_H
#define SUBSYNTAXHIGHLIGTHER_H

#include <QSyntaxHighlighter>
#include <QRegExp>

class SubSyntaxHighligther : public QSyntaxHighlighter
{
public:
    explicit SubSyntaxHighligther(QTextDocument *parent = nullptr);

protected:
    virtual void highlightBlock(const QString &text);

private:
    struct HighlightingRule
    {
        QRegExp pattern;
        QTextCharFormat format;
    };
    QVector<HighlightingRule> highlightingRules;

    QTextCharFormat tagFormat;
    QTextCharFormat attributeFormat;
};


#endif // SUBSYNTAXHIGHLIGTHER_H
