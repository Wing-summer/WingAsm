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

WingHexAsm::WingHexAsm() : WingHex::IWingPlugin() {}

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

    _dwinfos.append(info);

    win = new AsmWindow(false, isDark);
    info.widgetName = QStringLiteral("WingDisAsm");
    info.displayName = tr("WingDisAsm");
    info.area = Qt::BottomDockWidgetArea;
    info.widget = win;
    connect(win, &AsmWindow::onProcessClicked, this, &WingHexAsm::on_disasm);
    _dwinfos.append(info);
}

void WingHexAsm::on_asm() {}

void WingHexAsm::on_disasm() {}
