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

#include "winghexasm.h"

Q_GLOBAL_STATIC_WITH_ARGS(QString, ASM_ASM_FMT, ("asm.asmfmt"))
Q_GLOBAL_STATIC_WITH_ARGS(QString, ASM_ASM_ARCH, ("asm.asmarch"))
Q_GLOBAL_STATIC_WITH_ARGS(QString, DISASM_ASM_FMT, ("disasm.asmfmt"))
Q_GLOBAL_STATIC_WITH_ARGS(QString, DISASM_ASM_ARCH, ("disasm.asmfmt"))

WingHexAsm::WingHexAsm() : WingHex::IWingPlugin() {
    _pixicon = QPixmap(":/WingHexAsm/images/icon.png");
}

WingHexAsm::~WingHexAsm() {}

int WingHexAsm::sdkVersion() const { return WingHex::SDKVERSION; }

const QString WingHexAsm::signature() const { return WingHex::WINGSUMMER; }

bool WingHexAsm::init(const std::unique_ptr<QSettings> &set) {
    initDockWidgets();

    auto fmt = WingEngine::AsmFormat(
        set->value(*ASM_ASM_FMT, int(WingEngine::AsmFormat::INTEL)).toInt());
    auto arch =
        set->value(*ASM_ASM_ARCH, int(WingEngine::KSArch::X86_64)).toInt();
    _asmWin->setCurrentAsmFormat(fmt);
    _asmWin->setCurrentArch(arch);

    fmt = WingEngine::AsmFormat(
        set->value(*DISASM_ASM_FMT, int(WingEngine::AsmFormat::INTEL)).toInt());
    arch =
        set->value(*DISASM_ASM_ARCH, int(WingEngine::CSArch::X86_64)).toInt();
    _disasmWin->setCurrentAsmFormat(fmt);
    _disasmWin->setCurrentArch(arch);

    return true;
}

void WingHexAsm::unload(std::unique_ptr<QSettings> &set) {
    auto fmt = int(_asmWin->currentAsmFormat());
    auto arch = _asmWin->currentArch();
    set->setValue(*ASM_ASM_FMT, fmt);
    set->setValue(*ASM_ASM_ARCH, arch);

    fmt = int(_disasmWin->currentAsmFormat());
    arch = _disasmWin->currentArch();
    set->setValue(*DISASM_ASM_FMT, fmt);
    set->setValue(*DISASM_ASM_ARCH, arch);
}

const QString WingHexAsm::pluginName() const { return tr("WingHexAsm"); }

QIcon WingHexAsm::pluginIcon() const {
    return QIcon(":/WingHexAsm/images/icon.png");
}

const QString WingHexAsm::pluginComment() const {
    return tr("Assembler/Disassembler plugin for WingHexExplorer2");
}

QList<WingHex::WingDockWidgetInfo> WingHexAsm::registeredDockWidgets() const {
    return _dwinfos;
}

QMenu *WingHexAsm::registeredHexContextMenu() const { return nullptr; }

QHash<WingHex::SettingPage *, bool> WingHexAsm::registeredSettingPages() const {
    return _spinfos;
}

void WingHexAsm::initDockWidgets() {
    auto theme = emit currentAppTheme();
    auto isDark = theme == WingHex::AppTheme::Dark;

    WingHex::WingDockWidgetInfo info;

    auto win = new AsmWindow(true, isDark);
    info.widgetName = QStringLiteral("WingAsm");
    info.displayName = tr("WingAsm");
    info.area = Qt::BottomDockWidgetArea;
    info.widget = win;
    connect(win, &AsmWindow::onProcessClicked, this, &WingHexAsm::on_asm);
    _asmWin = win;
    _dwinfos.append(info);

    win = new AsmWindow(false, isDark);
    info.widgetName = QStringLiteral("WingDisAsm");
    info.displayName = tr("WingDisAsm");
    info.area = Qt::BottomDockWidgetArea;
    info.widget = win;
    connect(win, &AsmWindow::onProcessClicked, this, &WingHexAsm::on_disasm);
    _disasmWin = win;
    _dwinfos.append(info);
}

QByteArray WingHexAsm::doAsm(const QByteArray &code, int arch, int format) {
    WingEngine::ErrorKSEngine err;
    return WingEngine::doAsm(code, WingEngine::KSArch(arch),
                             WingEngine::AsmFormat(format), err);
}

QString WingHexAsm::doDisasm(const QByteArray &code, int arch, int format) {
    WingEngine::ErrorCSEngine err;
    return WingEngine::doDisasm(code, WingEngine::CSArch(arch),
                                WingEngine::AsmFormat(format), err);
}

void WingHexAsm::on_asm() {
    auto txt = _asmWin->editorText();
    if (txt.isEmpty()) {
        emit toast(_pixicon, tr("NoInput"));
        return;
    }

    if (!emit reader.isCurrentDocEditing()) {
        emit toast(_pixicon, tr("NoCurrentFileEditing"));
        return;
    }

    auto arch = _asmWin->currentArch();
    auto fmt = _asmWin->currentAsmFormat();
    WingEngine::ErrorKSEngine err;

    auto buffer =
        WingEngine::doAsm(txt.toUtf8(), WingEngine::KSArch(arch), fmt, err);
    if (err == WingEngine::ErrorKSEngine::ERR_OK) {
        auto isInsertion = emit reader.isInsertionMode();
        auto pos = emit reader.currentOffset();
        if (isInsertion) {
            auto ret = emit controller.insertBytes(pos, buffer);
            if (!ret) {
                emit toast(_pixicon, tr("AsmWriteFailed"));
            }
        } else {
            auto ret = emit controller.writeBytes(pos, buffer);
            if (!ret) {
                emit toast(_pixicon, tr("AsmWriteFailed"));
            }
        }
    } else {
        auto e = QMetaEnum::fromType<WingEngine::ErrorKSEngine>();
        auto errCode = e.valueToKey(int(err));
        auto errStr = WingEngine::getErrorString(err);

        emit toast(_pixicon, tr("AsmErrorSeeLog"));
        emit this->error(errStr);
    }
}

void WingHexAsm::on_disasm() {
    _disasmWin->setEditorText({});

    if (!emit reader.isCurrentDocEditing()) {
        emit toast(_pixicon, tr("NoCurrentFileEditing"));
        return;
    }

    auto selCount = emit reader.selectionCount();
    if (selCount != 1) {
        emit toast(_pixicon, tr("OnlyOneSelSupport"));
        return;
    }

    auto arch = _disasmWin->currentArch();
    auto fmt = _disasmWin->currentAsmFormat();

    auto sel = emit reader.selectedBytes(0);

    WingEngine::ErrorCSEngine err;
    auto txt = WingEngine::doDisasm(sel, WingEngine::CSArch(arch), fmt, err);
    if (err == WingEngine::ErrorCSEngine::ERR_OK) {
        _disasmWin->setEditorText(txt);
    } else {
        auto e = QMetaEnum::fromType<WingEngine::ErrorCSEngine>();
        auto errCode = e.valueToKey(int(err));
        auto errStr = WingEngine::getErrorString(err);

        emit toast(_pixicon, tr("DisAsmErrorSeeLog"));
        emit this->error(errStr);
    }
}
