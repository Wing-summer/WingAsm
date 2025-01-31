// QCodeEditor
#include "QStyleSyntaxHighlighter.hpp"

QStyleSyntaxHighlighter::QStyleSyntaxHighlighter(QObject *parent)
    : QSyntaxHighlighter(parent), m_syntaxStyle(nullptr) {}

QStyleSyntaxHighlighter::QStyleSyntaxHighlighter(QTextDocument *document)
    : QSyntaxHighlighter(document), m_syntaxStyle(nullptr) {}

void QStyleSyntaxHighlighter::setSyntaxStyle(QSyntaxStyle *style) {
    m_syntaxStyle = style;
}

QSyntaxStyle *QStyleSyntaxHighlighter::syntaxStyle() const {
    return m_syntaxStyle;
}
