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

#include "WingPlugin/iwingplugin.h"

#include "asmwindow.h"

class WingHexAsm final : public WingHex::IWingPlugin {
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "com.wingsummer.iwingplugin" FILE "WingHexAsm.json")
    Q_INTERFACES(WingHex::IWingPlugin)

public:
    explicit WingHexAsm();
    virtual ~WingHexAsm();

public:
    virtual bool init(const std::unique_ptr<QSettings> &set) override;
    virtual void unload(std::unique_ptr<QSettings> &set) override;
    virtual const QString pluginName() const override;
    virtual QIcon pluginIcon() const override;
    virtual const QString pluginComment() const override;

    virtual QString retranslate(const QString &str) override;

    // IWingPlugin interface
public:
    virtual QList<WingHex::WingDockWidgetInfo>
    registeredDockWidgets() const override;
    virtual QMenu *registeredHexContextMenu() const override;
    virtual QList<WingHex::SettingPage *>
    registeredSettingPages() const override;

    virtual void onRegisterScriptObj(WingHex::IWingAngel *o) override;

private:
    void initDockWidgets();

private:
    WING_SERVICE QByteArray doAsm(const WingHex::SenderInfo &sender,
                                  const QString &code, int arch, int format);
    WING_SERVICE QString doDisasm(const WingHex::SenderInfo &sender,
                                  const QByteArray &code, int arch, int format);
    WING_SERVICE int getLastAsmError(const WingHex::SenderInfo &sender);
    WING_SERVICE int getLastDisasmError(const WingHex::SenderInfo &sender);

private:
    QVariant doAsm(const QVariantList &params);
    QVariant doDisasm(const QVariantList &params);

private slots:
    void on_asm();

    void on_disasm();

private:
    QList<WingHex::WingDockWidgetInfo> _dwinfos;
    QList<WingHex::SettingPage *> _spinfos;

    AsmWindow *_asmWin;
    AsmWindow *_disasmWin;

    QHash<QString, WingEngine::ErrorKSEngine> _lastKSErr;
    QHash<QString, WingEngine::ErrorCSEngine> _lastCSErr;

    QPixmap _pixicon;
};

#endif // WINGHEXASM_H
