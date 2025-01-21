#include "asmhighlighter.h"
#include "QSyntaxStyle.hpp"

AsmHighlighter::AsmHighlighter(QTextDocument *document)
    : QStyleSyntaxHighlighter(document) {
    // Numbers
    m_highlightRules.append(
        {QRegularExpression(
             R"((?<=\b|\s|^)(?i)(?:(?:(?:(?:(?:\d+(?:'\d+)*)?\.(?:\d+(?:'\d+)*)(?:e[+-]?(?:\d+(?:'\d+)*))?)|(?:(?:\d+(?:'\d+)*)\.(?:e[+-]?(?:\d+(?:'\d+)*))?)|(?:(?:\d+(?:'\d+)*)(?:e[+-]?(?:\d+(?:'\d+)*)))|(?:0x(?:[0-9a-f]+(?:'[0-9a-f]+)*)?\.(?:[0-9a-f]+(?:'[0-9a-f]+)*)(?:p[+-]?(?:\d+(?:'\d+)*)))|(?:0x(?:[0-9a-f]+(?:'[0-9a-f]+)*)\.?(?:p[+-]?(?:\d+(?:'\d+)*))))[lf]?)|(?:(?:(?:[1-9]\d*(?:'\d+)*)|(?:0[0-7]*(?:'[0-7]+)*)|(?:0x[0-9a-f]+(?:'[0-9a-f]+)*)|(?:0b[01]+(?:'[01]+)*))(?:u?l{0,2}|l{0,2}u?)))(?=\b|\s|$))"),
         "Number"});

    // Strings
    m_highlightRules.append({QRegularExpression(R"("[^\n"]*")"), "String"});
}

void AsmHighlighter::highlightBlock(const QString &text) {
    for (auto &rule : m_highlightRules) {
        auto matchIterator = rule.pattern.globalMatch(text);

        while (matchIterator.hasNext()) {
            auto match = matchIterator.next();

            setFormat(match.capturedStart(), match.capturedLength(),
                      syntaxStyle()->getFormat(rule.formatName));
        }
    }
}
