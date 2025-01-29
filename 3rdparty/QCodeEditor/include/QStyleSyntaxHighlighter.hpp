#pragma once

// Qt
#include <QObject>
#include <QSyntaxHighlighter> // Required for inheritance

class QSyntaxStyle;

/**
 * @brief Class, that descrubes highlighter with
 * syntax style.
 */
class QStyleSyntaxHighlighter : public QSyntaxHighlighter {
    Q_OBJECT

public:
    explicit QStyleSyntaxHighlighter(QObject *parent = nullptr);

    /**
     * @brief Constructor.
     * @param document Pointer to text document.
     */
    explicit QStyleSyntaxHighlighter(QTextDocument *document = nullptr);

    // Disable copying
    QStyleSyntaxHighlighter(const QStyleSyntaxHighlighter &) = delete;
    QStyleSyntaxHighlighter &
    operator=(const QStyleSyntaxHighlighter &) = delete;

    /**
     * @brief Method for setting syntax style.
     * @param style Pointer to syntax style.
     */
    void setSyntaxStyle(QSyntaxStyle *style);

    /**
     * @brief Method for getting syntax style.
     * @return Pointer to syntax style. May be nullptr.
     */
    QSyntaxStyle *syntaxStyle() const;

private:
    QSyntaxStyle *m_syntaxStyle;
};
