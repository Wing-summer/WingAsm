#include "syntaxconfigparser.h"

SyntaxConfigParser::SyntaxConfigParser(const QString &name, QObject *parent)
    : QFile(name, parent) {}

QStringList SyntaxConfigParser::parse() {
    if (fileName().isEmpty()) {
        return {};
    }

    if (!open(QFile::ReadOnly | QFile::Text)) {
        return {};
    }

    QStringList ret;

    while (!atEnd()) {
        auto line = QString::fromUtf8(readLine()).trimmed();
        if (line.isEmpty() || line.startsWith('#')) {
            continue;
        }
        ret.append(line);
    }

    close();
    return ret;
}
