#ifndef ASMHIGHLIGHTER_H
#define ASMHIGHLIGHTER_H

#include "3rdparty/QCodeEditor/include/QHighlightRule.hpp"
#include "3rdparty/QCodeEditor/include/QStyleSyntaxHighlighter.hpp"

class AsmHighlighter : public QStyleSyntaxHighlighter {
    Q_OBJECT
public:
    explicit AsmHighlighter(const QString &id, QObject *parent = nullptr);

    QStringList keyWords() const;

protected:
    void highlightBlock(const QString &text) override;

private:
    QVector<QHighlightRule> m_highlightRules;
    QStringList m_keyWords;
};

#endif // ASMHIGHLIGHTER_H
