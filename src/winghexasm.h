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

#ifndef WINGHEXASM_H
#define WINGHEXASM_H

#include "iwingplugin.h"

#include "asmwindow.h"

class WingHexAsm final : public WingHex::IWingPlugin {
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "com.wingsummer.iwingplugin" FILE "WingHexAsm.json")
    Q_INTERFACES(WingHex::IWingPlugin)

public:
    explicit WingHexAsm();
    virtual ~WingHexAsm();

public:
    virtual int sdkVersion() const override;
    virtual const QString signature() const override;
    virtual bool init(const std::unique_ptr<QSettings> &set) override;
    virtual void unload(std::unique_ptr<QSettings> &set) override;
    virtual const QString pluginName() const override;
    virtual const QString pluginAuthor() const override;
    virtual uint pluginVersion() const override;
    virtual const QString pluginComment() const override;

    // IWingPlugin interface
public:
    virtual QList<WingHex::WingDockWidgetInfo>
    registeredDockWidgets() const override;
    virtual QMenu *registeredHexContextMenu() const override;
    virtual QHash<WingHex::SettingPage *, bool>
    registeredSettingPages() const override;

private:
    void initDockWidgets();

private slots:
    void on_asm();

    void on_disasm();

private:
    QList<WingHex::WingDockWidgetInfo> _dwinfos;
    QHash<WingHex::SettingPage *, bool> _spinfos;

    AsmWindow *_asmWin;
    AsmWindow *_disasmWin;
};

#endif // WINGHEXASM_H
