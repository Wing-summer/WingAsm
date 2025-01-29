/*==============================================================================
** Copyright (C) 2024-2027 WingSummer
**
** This program is free software: you can redistribute it and/or modify it under
** the terms of the GNU Affero General Public License as published by the Free
** Software Foundation, version 3.
**
** This program is distributed in the hope that it will be useful, but WITHOUT
** ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
** FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more
** details.
**
** You should have received a copy of the GNU Affero General Public License
** along with this program. If not, see <https://www.gnu.org/licenses/>.
** =============================================================================
*/

#ifndef ASMWINDOW_H
#define ASMWINDOW_H

#include "wingengine.h"

#include "asmhighlighter.h"

#include <QCompleter>
#include <QMap>
#include <QMetaEnum>
#include <QWidget>

namespace Ui {
class AsmWindow;
}

class AsmWindow : public QWidget {
    Q_OBJECT

public:
    explicit AsmWindow(bool isAsm, bool isDarkTheme, QWidget *parent = nullptr);
    virtual ~AsmWindow();

private:
    enum class HighlightType { None, X86_64, X86_64_ATT, ARM, MIPS, POWERPC };

private:
    HighlightType getHighlightType(WingEngine::CSArch arch,
                                   WingEngine::AsmFormat fmt);
    HighlightType getHighlightType(WingEngine::KSArch arch,
                                   WingEngine::AsmFormat fmt);

    void initHighliter(HighlightType type, const QString &id);

public:
    QString editorText() const;

    void setEditorText(const QString &txt);

    WingEngine::AsmFormat currentAsmFormat() const;

    void setCurrentAsmFormat(WingEngine::AsmFormat fmt);

    int currentArch() const;

    void setCurrentArch(int arch) const;

    void setProcessButtonEnabled(bool enabled);

signals:
    void onProcessClicked();

private:
    void loadStyle(const QString &path);

private:
    Ui::AsmWindow *ui;

    QMetaEnum e_arch;
    QMap<HighlightType, AsmHighlighter *> m_highlights;
    QMap<HighlightType, QCompleter *> m_completer;
};

#endif // ASMWINDOW_H
