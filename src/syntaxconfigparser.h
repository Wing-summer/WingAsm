#ifndef SYNTAXCONFIGPARSER_H
#define SYNTAXCONFIGPARSER_H

#include <QFile>
#include <QStringList>

class SyntaxConfigParser : QFile {
    Q_OBJECT
public:
    explicit SyntaxConfigParser(const QString &name, QObject *parent = nullptr);
    virtual ~SyntaxConfigParser() = default;

    QStringList parse();
};

#endif // SYNTAXCONFIGPARSER_H
