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

    // highliter
    initHighliter(HighlightType::None, {});
    initHighliter(HighlightType::X86_64, QStringLiteral("x86_64"));
    initHighliter(HighlightType::X86_64_ATT, QStringLiteral("x86_64_att"));
    initHighliter(HighlightType::ARM, QStringLiteral("arm"));
    initHighliter(HighlightType::MIPS, QStringLiteral("powerpc"));
    initHighliter(HighlightType::MIPS, QStringLiteral("mips"));

    auto op = [isAsm, this]() {
        auto fmt = WingEngine::AsmFormat(ui->cbStyle->currentData().toInt());
        if (isAsm) {
            auto arch = WingEngine::KSArch(ui->cbArch->currentData().toInt());
            auto htype = getHighlightType(arch, fmt);
            auto hler = m_highlights.value(htype);
            auto co = m_completer.value(htype);
            ui->textEdit->setHighlighter(hler);
            ui->textEdit->setCompleter(co);
        } else {
            auto arch = WingEngine::CSArch(ui->cbArch->currentData().toInt());
            auto htype = getHighlightType(arch, fmt);
            auto hler = m_highlights.value(htype);
            auto co = m_completer.value(htype);
            ui->textEdit->setHighlighter(hler);
            ui->textEdit->setCompleter(co);
        }
    };

    connect(ui->cbArch, &QComboBox::currentIndexChanged, this, op);
    connect(ui->cbStyle, &QComboBox::currentIndexChanged, this, op);

    emit ui->cbArch->currentIndexChanged(ui->cbArch->currentIndex());
}

AsmWindow::~AsmWindow() { delete ui; }

void AsmWindow::initHighliter(HighlightType type, const QString &id) {
    if (id.isEmpty()) {
        m_highlights.insert(type, nullptr);
        m_completer.insert(type, nullptr);
    } else {
        auto hler = new AsmHighlighter(id, this);
        auto cp = new QCompleter(hler->keyWords(), this);
        cp->setCaseSensitivity(Qt::CaseInsensitive);
        m_highlights.insert(type, hler);
        m_completer.insert(type, cp);
    }
}

AsmWindow::HighlightType
AsmWindow::getHighlightType(WingEngine::KSArch arch,
                            WingEngine::AsmFormat fmt) {
    switch (arch) {
    case WingEngine::KSArch::ARM_V8_EB:
    case WingEngine::KSArch::THUMB_V8_EB:
    case WingEngine::KSArch::ARM_V7_EB:
    case WingEngine::KSArch::ARM_V8:
    case WingEngine::KSArch::THUMB_V8:
    case WingEngine::KSArch::ARM_V7:
    case WingEngine::KSArch::THUMB_V7:
        return HighlightType::ARM;
    case WingEngine::KSArch::MIPS:
    case WingEngine::KSArch::MIPS64:
    case WingEngine::KSArch::MIPS_EL:
    case WingEngine::KSArch::MIPS64_EL:
        return HighlightType::MIPS;
    case WingEngine::KSArch::POWER_PC32:
    case WingEngine::KSArch::POWER_PC64:
    case WingEngine::KSArch::POWER_PC32_EL:
    case WingEngine::KSArch::POWER_PC64_EL:
        return HighlightType::MIPS;
    case WingEngine::KSArch::I386:
    case WingEngine::KSArch::X86:
    case WingEngine::KSArch::X86_64:
        return fmt == WingEngine::AsmFormat::ATT ? HighlightType::X86_64_ATT
                                                 : HighlightType::X86_64;
    default:
        break;
    }
    return HighlightType::None;
}

AsmWindow::HighlightType
AsmWindow::getHighlightType(WingEngine::CSArch arch,
                            WingEngine::AsmFormat fmt) {
    switch (arch) {
    case WingEngine::CSArch::ARM_V8_EB:
    case WingEngine::CSArch::THUMB_V8_EB:
    case WingEngine::CSArch::ARM_V7_EB:
    case WingEngine::CSArch::ARM_V8:
    case WingEngine::CSArch::THUMB_V8:
    case WingEngine::CSArch::ARM_V7:
    case WingEngine::CSArch::THUMB_V7:
        return HighlightType::ARM;
    case WingEngine::CSArch::MIPS:
    case WingEngine::CSArch::MIPS64:
    case WingEngine::CSArch::MIPS_EL:
    case WingEngine::CSArch::MIPS64_EL:
        return HighlightType::MIPS;
    case WingEngine::CSArch::POWER_PC64:
    case WingEngine::CSArch::POWER_PC64_EL:
        return HighlightType::POWERPC;
    case WingEngine::CSArch::I386:
    case WingEngine::CSArch::X86:
    case WingEngine::CSArch::X86_64:
        return fmt == WingEngine::AsmFormat::ATT ? HighlightType::X86_64_ATT
                                                 : HighlightType::X86_64;
    default:
        break;
    }
    return HighlightType::None;
}

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
