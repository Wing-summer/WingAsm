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

#include "wingengine.h"

#include <QStringList>

QString WingEngine::doAsm(const QByteArray &code, KSArch arch, KSModes mode,
                          AsmFormat format, ErrorKSEngine &errcode) {
    if (code.isEmpty()) {
        errcode = ErrorKSEngine::ERR_OK;
        return {};
    }

    ks_engine *ks;
    size_t count;
    unsigned char *encode;
    size_t size;

    ks_err err = ks_open(ks_arch(arch), decltype(mode)::Int(mode), &ks);
    if (err != KS_ERR_OK) {
        errcode = ErrorKSEngine(err);
        return {};
    }

    ks_option(ks, KS_OPT_SYNTAX, asmFmt2KsOptFmt(format));

    if (ks_asm(ks, code.constData(), 0, &encode, &size, &count)) {
        errcode = ErrorKSEngine(ks_errno(ks));
    }

    ks_free(encode);
    ks_close(ks);

    return QString::fromLatin1(reinterpret_cast<char *>(encode), size);
}

QString WingEngine::doDisasm(const QByteArray &code, CSArch arch, CSModes mode,
                             AsmFormat format, ErrorCSEngine &errcode) {
    if (code.isEmpty()) {
        errcode = ErrorCSEngine::ERR_OK;
        return {};
    }

    csh handle;
    QStringList asms;

    cs_err err =
        cs_open(cs_arch(arch), cs_mode(decltype(mode)::Int(mode)), &handle);

    if (err != CS_ERR_OK) {
        errcode = ErrorCSEngine(err);
        return {};
    }

    cs_insn *insn;
    cs_option(handle, CS_OPT_DETAIL, CS_OPT_ON);
    cs_option(handle, CS_OPT_SKIPDATA, CS_OPT_ON);
    cs_option(handle, CS_OPT_SYNTAX, asmFmt2CsOptFmt(format));

    auto *t = code.data();
    size_t count = cs_disasm(handle, reinterpret_cast<const uint8_t *>(t),
                             size_t(code.size()), 0, 0, &insn);

    if (count > 0) {
        size_t j;
        for (j = 0; j < count; j++) {
            asms << QStringLiteral("%1 %2;").arg(QString(insn[j].mnemonic),
                                                 QString(insn[j].op_str));
        }
    } else {
        errcode = ErrorCSEngine(cs_errno(handle));
    }

    cs_free(insn, count);
    cs_close(&handle);

    return asms.join('\n');
}

QList<WingEngine::KSMode> WingEngine::getAvalibleModes(KSArch arch) {
    switch (arch) {
    case KSArch::ARM:
    case KSArch::ARM64:
    case KSArch::MIPS:
    case KSArch::X86:
    case KSArch::PPC:
    case KSArch::SPARC:
    case KSArch::SYSTEMZ:
    case KSArch::HEXAGON:
    case KSArch::EVM:
        break;
    }
    return {};
}

QList<WingEngine::CSMode> WingEngine::getAvalibleModes(CSArch arch) {
    switch (arch) {
    case CSArch::ARM:
    case CSArch::AARCH64:
    case CSArch::SYSTEMZ:
    case CSArch::MIPS:
    case CSArch::X86:
    case CSArch::PPC:
    case CSArch::SPARC:
    case CSArch::XCORE:
    case CSArch::M68K:
    case CSArch::TMS320C64X:
    case CSArch::M680X:
    case CSArch::EVM:
    case CSArch::MOS65XX:
    case CSArch::WASM:
    case CSArch::BPF:
    case CSArch::RISCV:
    case CSArch::SH:
    case CSArch::TRICORE:
    case CSArch::ALPHA:
    case CSArch::HPPA:
    case CSArch::LOONGARCH:
    case CSArch::XTENSA:
        break;
    }
    return {};
}

ks_opt_value WingEngine::asmFmt2KsOptFmt(AsmFormat fmt) {
    switch (fmt) {
    case AsmFormat::INTEL:
        return KS_OPT_SYNTAX_INTEL;
    case AsmFormat::ATT:
        return KS_OPT_SYNTAX_ATT;
    case AsmFormat::MASM:
        return KS_OPT_SYNTAX_NASM;
    }
    return KS_OPT_SYNTAX_INTEL; // fallback to Intel
}

cs_opt_value WingEngine::asmFmt2CsOptFmt(AsmFormat fmt) {
    switch (fmt) {
    case AsmFormat::INTEL:
        return CS_OPT_SYNTAX_INTEL;
    case AsmFormat::ATT:
        return CS_OPT_SYNTAX_ATT;
    case AsmFormat::MASM:
        return CS_OPT_SYNTAX_MASM;
    }
    return CS_OPT_SYNTAX_INTEL; // fallback to Intel
}
