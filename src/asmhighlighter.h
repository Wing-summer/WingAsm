#ifndef ASMHIGHLIGHTER_H
#define ASMHIGHLIGHTER_H

#include "3rdparty/QCodeEditor/include/QHighlightRule.hpp"
#include "3rdparty/QCodeEditor/include/QStyleSyntaxHighlighter.hpp"

class AsmHighlighter : public QStyleSyntaxHighlighter {
    Q_OBJECT
public:
    explicit AsmHighlighter(QTextDocument *document = nullptr);

protected:
    void highlightBlock(const QString &text) override;

private:
    QVector<QHighlightRule> m_highlightRules;
};

#endif // ASMHIGHLIGHTER_H
