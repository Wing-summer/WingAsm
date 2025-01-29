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

#include <QApplication>

Q_GLOBAL_STATIC_WITH_ARGS(QString, ASM_ASM_FMT, ("asm.asmfmt"))
Q_GLOBAL_STATIC_WITH_ARGS(QString, ASM_ASM_ARCH, ("asm.asmarch"))
Q_GLOBAL_STATIC_WITH_ARGS(QString, DISASM_ASM_FMT, ("disasm.asmfmt"))
Q_GLOBAL_STATIC_WITH_ARGS(QString, DISASM_ASM_ARCH, ("disasm.asmfmt"))

WingHexAsm::WingHexAsm() : WingHex::IWingPlugin() {
    _pixicon = QPixmap(":/WingHexAsm/images/icon.png");

    // enums
    {
        QList<QPair<QString, int>> values;
        auto e = QMetaEnum::fromType<WingEngine::KSArch>();
        for (int i = 0; i < e.keyCount(); ++i) {
            values.append(qMakePair(e.key(i), e.value(i)));
        }
        _scriptEnums.insert(QStringLiteral("AsmArch"), values);
    }

    {
        QList<QPair<QString, int>> values;
        auto e = QMetaEnum::fromType<WingEngine::CSArch>();
        for (int i = 0; i < e.keyCount(); ++i) {
            values.append(qMakePair(e.key(i), e.value(i)));
        }
        _scriptEnums.insert(QStringLiteral("DisasmArch"), values);
    }

    {
        QList<QPair<QString, int>> values;
        auto e = QMetaEnum::fromType<WingEngine::AsmFormat>();
        for (int i = 0; i < e.keyCount(); ++i) {
            values.append(qMakePair(e.key(i), e.value(i)));
        }
        _scriptEnums.insert(QStringLiteral("AsmFormat"), values);
    }

    // functions
    {
        WingHex::IWingPlugin::ScriptFnInfo info;
        info.fn =
            std::bind(QOverload<const QVariantList &>::of(&WingHexAsm::doAsm),
                      this, std::placeholders::_1);
        info.ret = MetaType(MetaType::Array | MetaType::Byte);

        info.params.append(qMakePair(MetaType::String, QStringLiteral("code")));
        info.params.append(qMakePair(MetaType::Int, QStringLiteral("arch")));
        info.params.append(qMakePair(MetaType::Int, QStringLiteral("format")));

        _scriptInfo.insert(QStringLiteral("doAsm"), info);
    }

    {
        WingHex::IWingPlugin::ScriptFnInfo info;
        info.fn = std::bind(
            QOverload<const QVariantList &>::of(&WingHexAsm::doDisasm), this,
            std::placeholders::_1);
        info.ret = MetaType::String;

        info.params.append(qMakePair(MetaType(MetaType::Array | MetaType::Byte),
                                     QStringLiteral("code")));
        info.params.append(qMakePair(MetaType::Int, QStringLiteral("arch")));
        info.params.append(qMakePair(MetaType::Int, QStringLiteral("format")));

        _scriptInfo.insert(QStringLiteral("doDisasm"), info);
    }
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

QString WingHexAsm::retranslate(const QString &str) {
    return QApplication::tr(str.toLatin1());
}

QList<WingHex::WingDockWidgetInfo> WingHexAsm::registeredDockWidgets() const {
    return _dwinfos;
}

QMenu *WingHexAsm::registeredHexContextMenu() const { return nullptr; }

QHash<WingHex::SettingPage *, bool> WingHexAsm::registeredSettingPages() const {
    return _spinfos;
}

QHash<QString, QList<QPair<QString, int>>>
WingHexAsm::registeredScriptEnums() const {
    return _scriptEnums;
}

QHash<QString, WingHex::IWingPlugin::ScriptFnInfo>
WingHexAsm::registeredScriptFns() const {
    return _scriptInfo;
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

QPair<QByteArray, int> WingHexAsm::doAsm(const QString &code, int arch,
                                         int format) {
    WingEngine::ErrorKSEngine err;
    return qMakePair(WingEngine::doAsm(code, WingEngine::KSArch(arch),
                                       WingEngine::AsmFormat(format), err),
                     int(err));
}

QPair<QString, int> WingHexAsm::doDisasm(const QByteArray &code, int arch,
                                         int format) {
    WingEngine::ErrorCSEngine err;
    return qMakePair(WingEngine::doDisasm(code, WingEngine::CSArch(arch),
                                          WingEngine::AsmFormat(format), err),
                     int(err));
}

QVariant WingHexAsm::doAsm(const QVariantList &params) {
    if (params.size() != 3) {
        return {};
    }

    auto var_code = params.at(0);
    auto var_arch = params.at(1);
    auto var_fmt = params.at(2);

    if (!var_code.canConvert<QString>()) {
        return {};
    }

    if (!var_arch.canConvert<int>()) {
        return {};
    }

    if (!var_fmt.canConvert<int>()) {
        return {};
    }

    WingEngine::ErrorKSEngine err;
    auto code = var_code.toString();
    auto arch = var_arch.toInt();
    auto format = var_fmt.toInt();

    return WingEngine::doAsm(code, WingEngine::KSArch(arch),
                             WingEngine::AsmFormat(format), err);
}

QVariant WingHexAsm::doDisasm(const QVariantList &params) {
    if (params.size() != 3) {
        return {};
    }

    auto var_code = params.at(0);
    auto var_arch = params.at(1);
    auto var_fmt = params.at(2);

    if (!var_code.canConvert<QByteArray>()) {
        return {};
    }

    if (!var_arch.canConvert<int>()) {
        return {};
    }

    if (!var_fmt.canConvert<int>()) {
        return {};
    }

    WingEngine::ErrorCSEngine err;
    auto code = var_code.toByteArray();
    auto arch = var_arch.toInt();
    auto format = var_fmt.toInt();

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
