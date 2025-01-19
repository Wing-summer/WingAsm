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

#include "asmwindow.h"
#include "ui_asmwindow.h"

#include "QSyntaxStyle.hpp"

#include <QFile>
#include <QMetaEnum>

AsmWindow::AsmWindow(bool isAsm, bool isDarkTheme, QWidget *parent)
    : QWidget(parent), ui(new Ui::AsmWindow) {
    ui->setupUi(this);

    if (isAsm) {
        e_arch = QMetaEnum::fromType<WingEngine::KSArch>();
    } else {
        e_arch = QMetaEnum::fromType<WingEngine::CSArch>();
    }

    for (int i = 0; i < e_arch.keyCount(); ++i) {
        ui->cbArch->addItem(e_arch.key(i), e_arch.value(i));
    }

    auto e_style = QMetaEnum::fromType<WingEngine::AsmFormat>();
    for (int i = 0; i < e_style.keyCount(); ++i) {
        ui->cbStyle->addItem(e_style.key(i), e_style.value(i));
    }

    if (isDarkTheme) {
        loadStyle(QStringLiteral(":/WingHexAsm/resources/drakula.xml"));
    }

    ui->textEdit->setReadOnly(!isAsm);

    connect(ui->pushButton, &QPushButton::clicked, this,
            &AsmWindow::onProcessClicked);
}

AsmWindow::~AsmWindow() { delete ui; }

QString AsmWindow::editorText() const { return ui->textEdit->toPlainText(); }

void AsmWindow::setEditorText(const QString &txt) {
    ui->textEdit->setText(txt);
}

WingEngine::AsmFormat AsmWindow::currentAsmFormat() const {
    return WingEngine::AsmFormat(ui->cbStyle->currentData().toInt());
}

void AsmWindow::setCurrentAsmFormat(WingEngine::AsmFormat fmt) {
    for (int i = 0; i < ui->cbStyle->count(); ++i) {
        if (ui->cbStyle->itemData(i).toInt() == int(fmt)) {
            ui->cbStyle->setCurrentIndex(i);
            break;
        }
    }
}

int AsmWindow::currentArch() const { return ui->cbArch->currentData().toInt(); }

void AsmWindow::setCurrentArch(int arch) const {
    for (int i = 0; i < ui->cbArch->count(); ++i) {
        if (ui->cbArch->itemData(i).toInt() == arch) {
            ui->cbArch->setCurrentIndex(i);
            break;
        }
    }
}

void AsmWindow::setProcessButtonEnabled(bool enabled) {
    ui->pushButton->setEnabled(enabled);
}

void AsmWindow::loadStyle(const QString &path) {
    QFile fl(path);

    if (!fl.open(QIODevice::ReadOnly)) {
        return;
    }

    auto style = new QSyntaxStyle(this);

    if (!style->load(fl.readAll())) {
        delete style;
        return;
    }

    ui->textEdit->setSyntaxStyle(style);
}
