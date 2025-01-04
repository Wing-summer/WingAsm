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

WingHexAsm::WingHexAsm() : WingHex::IWingPlugin() {
    _pixicon = QPixmap(":/WingHexAsm/images/icon.png");
}

WingHexAsm::~WingHexAsm() {}

int WingHexAsm::sdkVersion() const { return WingHex::SDKVERSION; }

const QString WingHexAsm::signature() const { return WingHex::WINGSUMMER; }

bool WingHexAsm::init(const std::unique_ptr<QSettings> &set) {
    initDockWidgets();
    return true;
}

void WingHexAsm::unload(std::unique_ptr<QSettings> &set) {
    // Your unloading codes here
}

const QString WingHexAsm::pluginName() const { return tr("WingHexAsm"); }

QIcon WingHexAsm::pluginIcon() const {
    return QIcon(":/WingHexAsm/images/icon.png");
}

const QString WingHexAsm::pluginAuthor() const { return WingHex::WINGSUMMER; }

uint WingHexAsm::pluginVersion() const { return 1; }

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

void WingHexAsm::on_asm() {
    auto txt = _asmWin->editorText();

    auto arch = _asmWin->currentArch();
    auto fmt = _asmWin->currentAsmFormat();
    WingEngine::ErrorKSEngine err;

    auto buffer =
        WingEngine::doAsm(txt.toUtf8(), WingEngine::KSArch(arch), fmt, err);
    if (err == WingEngine::ErrorKSEngine::ERR_OK) {

    } else {
        auto e = QMetaEnum::fromType<WingEngine::ErrorKSEngine>();
        auto errCode = e.valueToKey(int(err));
        auto errStr = WingEngine::getErrorString(err);
    }
}

void WingHexAsm::on_disasm() {
    auto selCount = emit reader.selectionCount();
    if (selCount != 1) {
        emit toast(_pixicon, tr(""));
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

        emit this->error(errStr);
    }
}
