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

QByteArray WingEngine::doAsm(const QString &code, KSArch arch, AsmFormat format,
                             ErrorKSEngine &errcode) {
    if (code.isEmpty()) {
        errcode = ErrorKSEngine::ERR_OK;
        return {};
    }

    ks_engine *ks;
    size_t count;
    unsigned char *encode;
    size_t size;

    ks_err err = ks_open(ksArch(arch), ksArch2ksMode(arch), &ks);
    if (err != KS_ERR_OK) {
        errcode = ErrorKSEngine(err);
        return {};
    }

    ks_option(ks, KS_OPT_SYNTAX, asmFmt2KsOptFmt(format));

    auto c = code.toUtf8();
    if (ks_asm(ks, c.constData(), 0, &encode, &size, &count)) {
        errcode = ErrorKSEngine(ks_errno(ks));
    }

    auto data = QByteArray(reinterpret_cast<char *>(encode), size);
    ks_free(encode);
    ks_close(ks);

    errcode = ErrorKSEngine::ERR_OK;
    return data;
}

QString WingEngine::doDisasm(const QByteArray &code, CSArch arch,
                             AsmFormat format, ErrorCSEngine &errcode) {
    if (code.isEmpty()) {
        errcode = ErrorCSEngine::ERR_OK;
        return {};
    }

    csh handle;
    QStringList asms;

    cs_err err = cs_open(csArch(arch), csArch2csMode(arch), &handle);

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
            asms << (QStringLiteral("%1 %2;").arg(QString(insn[j].mnemonic),
                                                  QString(insn[j].op_str)))
                        .trimmed();
        }
    } else {
        errcode = ErrorCSEngine(cs_errno(handle));
    }

    cs_free(insn, count);
    cs_close(&handle);

    errcode = ErrorCSEngine::ERR_OK;
    return asms.join('\n');
}

ks_arch WingEngine::ksArch(KSArch arch) {
    switch (arch) {
    case KSArch::ARM_V8_EB:
    case KSArch::THUMB_V8_EB:
    case KSArch::ARM_V7_EB:
    case KSArch::ARM_V8:
    case KSArch::THUMB_V8:
    case KSArch::ARM_V7:
    case KSArch::THUMB_V7:
        return KS_ARCH_ARM;
    case KSArch::AARCH64:
        return KS_ARCH_ARM64;
    case KSArch::HEXAGON:
        return KS_ARCH_HEXAGON;
    case KSArch::S390X:
        return KS_ARCH_SYSTEMZ;
    case KSArch::SPARC64:
    case KSArch::SPARC:
    case KSArch::SPARC_EL:
    case KSArch::SPARC64_V9:
    case KSArch::SPARC_V9:
    case KSArch::SPARC_EL_V9:
        return KS_ARCH_SPARC;
    case KSArch::MIPS:
    case KSArch::MIPS64:
    case KSArch::MIPS_EL:
    case KSArch::MIPS64_EL:
        return KS_ARCH_MIPS;
    case KSArch::POWER_PC32:
    case KSArch::POWER_PC64:
    case KSArch::POWER_PC32_EL:
    case KSArch::POWER_PC64_EL:
        return KS_ARCH_PPC;
    case KSArch::I386:
    case KSArch::X86:
    case KSArch::X86_64:
        return KS_ARCH_X86;
    }
    return KS_ARCH_MAX;
}

cs_arch WingEngine::csArch(CSArch arch) {
    switch (arch) {
    case CSArch::ARM_V8_EB:
    case CSArch::THUMB_V8_EB:
    case CSArch::ARM_V7_EB:
    case CSArch::ARM_V8:
    case CSArch::THUMB_V8:
    case CSArch::ARM_V7:
    case CSArch::THUMB_V7:
        return CS_ARCH_ARM;
    case CSArch::AARCH64:
        return CS_ARCH_AARCH64;
    case CSArch::S390X:
        return CS_ARCH_SYSTEMZ;
    case CSArch::MIPS:
    case CSArch::MIPS64:
    case CSArch::MIPS_EL:
    case CSArch::MIPS64_EL:
        return CS_ARCH_MIPS;
    case CSArch::POWER_PC64:
    case CSArch::POWER_PC64_EL:
        return CS_ARCH_PPC;
    case CSArch::I386:
    case CSArch::X86:
    case CSArch::X86_64:
        return CS_ARCH_X86;
    case CSArch::RISCV32:
    case CSArch::RISCV64:
        return CS_ARCH_RISCV;
    case CSArch::LOONG32:
    case CSArch::LOONG64:
        return CS_ARCH_LOONGARCH;
    }
    return CS_ARCH_MAX;
}

ks_mode WingEngine::ksArch2ksMode(KSArch arch) {
    switch (arch) {
    case KSArch::ARM_V8_EB:
        return ks_mode(KS_MODE_BIG_ENDIAN | KS_MODE_V8 | KS_MODE_ARM);
    case KSArch::THUMB_V8_EB:
        return ks_mode(KS_MODE_BIG_ENDIAN | KS_MODE_V8 | KS_MODE_THUMB);
    case KSArch::ARM_V7_EB:
        return ks_mode(KS_MODE_BIG_ENDIAN | KS_MODE_ARM);
    case KSArch::ARM_V8:
        return ks_mode(KS_MODE_LITTLE_ENDIAN | KS_MODE_V8 | KS_MODE_ARM);
    case KSArch::THUMB_V8:
        return ks_mode(KS_MODE_LITTLE_ENDIAN | KS_MODE_V8 | KS_MODE_THUMB);
    case KSArch::ARM_V7:
        return ks_mode(KS_MODE_LITTLE_ENDIAN | KS_MODE_ARM);
    case KSArch::THUMB_V7:
        return ks_mode(KS_MODE_LITTLE_ENDIAN | KS_MODE_THUMB);
    case KSArch::AARCH64:
        return ks_mode(KS_MODE_LITTLE_ENDIAN);
    case KSArch::HEXAGON:
        return ks_mode(KS_MODE_BIG_ENDIAN);
    case KSArch::S390X:
        return ks_mode(KS_MODE_BIG_ENDIAN);
    case KSArch::SPARC64:
        return ks_mode(KS_MODE_BIG_ENDIAN | KS_MODE_SPARC64);
    case KSArch::SPARC:
        return ks_mode(KS_MODE_BIG_ENDIAN | KS_MODE_SPARC32);
    case KSArch::SPARC_EL:
        return ks_mode(KS_MODE_LITTLE_ENDIAN | KS_MODE_SPARC32);
    case KSArch::SPARC64_V9:
        return ks_mode(KS_MODE_BIG_ENDIAN | KS_MODE_SPARC64 | KS_MODE_V9);
    case KSArch::SPARC_V9:
        return ks_mode(KS_MODE_BIG_ENDIAN | KS_MODE_SPARC32 | KS_MODE_V9);
    case KSArch::SPARC_EL_V9:
        return ks_mode(KS_MODE_LITTLE_ENDIAN | KS_MODE_SPARC32 | KS_MODE_V9);
    case KSArch::MIPS:
        return ks_mode(KS_MODE_BIG_ENDIAN | KS_MODE_MIPS32);
    case KSArch::MIPS64:
        return ks_mode(KS_MODE_BIG_ENDIAN | KS_MODE_MIPS64);
    case KSArch::MIPS_EL:
        return ks_mode(KS_MODE_LITTLE_ENDIAN | KS_MODE_MIPS32);
    case KSArch::MIPS64_EL:
        return ks_mode(KS_MODE_LITTLE_ENDIAN | KS_MODE_MIPS64);
    case KSArch::POWER_PC32:
        return ks_mode(KS_MODE_BIG_ENDIAN | KS_MODE_PPC32);
    case KSArch::POWER_PC64:
        return ks_mode(KS_MODE_BIG_ENDIAN | KS_MODE_PPC64);
    case KSArch::POWER_PC32_EL:
        return ks_mode(KS_MODE_LITTLE_ENDIAN | KS_MODE_PPC32);
    case KSArch::POWER_PC64_EL:
        return ks_mode(KS_MODE_LITTLE_ENDIAN | KS_MODE_PPC64);
    case KSArch::I386:
        return ks_mode(KS_MODE_LITTLE_ENDIAN | KS_MODE_16);
    case KSArch::X86:
        return ks_mode(KS_MODE_LITTLE_ENDIAN | KS_MODE_32);
    case KSArch::X86_64:
        return ks_mode(KS_MODE_LITTLE_ENDIAN | KS_MODE_64);
    }
    return ks_mode(0);
}

cs_mode WingEngine::csArch2csMode(CSArch arch) {
    switch (arch) {
    case CSArch::ARM_V8_EB:
        return cs_mode(CS_MODE_BIG_ENDIAN | CS_MODE_V8 | CS_MODE_ARM);
    case CSArch::THUMB_V8_EB:
        return cs_mode(CS_MODE_BIG_ENDIAN | CS_MODE_V8 | CS_MODE_THUMB);
    case CSArch::ARM_V7_EB:
        return cs_mode(CS_MODE_BIG_ENDIAN | CS_MODE_ARM);
    case CSArch::ARM_V8:
        return cs_mode(CS_MODE_LITTLE_ENDIAN | CS_MODE_V8 | CS_MODE_ARM);
    case CSArch::THUMB_V8:
        return cs_mode(CS_MODE_LITTLE_ENDIAN | CS_MODE_V8 | CS_MODE_THUMB);
    case CSArch::ARM_V7:
        return cs_mode(CS_MODE_LITTLE_ENDIAN | CS_MODE_ARM);
    case CSArch::THUMB_V7:
        return cs_mode(CS_MODE_LITTLE_ENDIAN | CS_MODE_THUMB);
    case CSArch::AARCH64:
        return cs_mode(CS_MODE_LITTLE_ENDIAN);
    case CSArch::S390X:
        return cs_mode(CS_MODE_BIG_ENDIAN);
    case CSArch::MIPS:
        return cs_mode(CS_MODE_BIG_ENDIAN | CS_MODE_MIPS32);
    case CSArch::MIPS64:
        return cs_mode(CS_MODE_BIG_ENDIAN | CS_MODE_MIPS64);
    case CSArch::MIPS_EL:
        return cs_mode(CS_MODE_LITTLE_ENDIAN | CS_MODE_MIPS32);
    case CSArch::MIPS64_EL:
        return cs_mode(CS_MODE_LITTLE_ENDIAN | CS_MODE_MIPS64);
    case CSArch::POWER_PC64:
        return cs_mode(CS_MODE_BIG_ENDIAN | CS_MODE_64);
    case CSArch::POWER_PC64_EL:
        return cs_mode(CS_MODE_LITTLE_ENDIAN | CS_MODE_64);
    case CSArch::I386:
        return cs_mode(CS_MODE_LITTLE_ENDIAN | CS_MODE_16);
    case CSArch::X86:
        return cs_mode(CS_MODE_LITTLE_ENDIAN | CS_MODE_32);
    case CSArch::X86_64:
        return cs_mode(CS_MODE_LITTLE_ENDIAN | CS_MODE_64);
    case CSArch::RISCV32:
        return cs_mode(CS_MODE_LITTLE_ENDIAN | CS_MODE_RISCV32);
    case CSArch::RISCV64:
        return cs_mode(CS_MODE_LITTLE_ENDIAN | CS_MODE_RISCV64);
    case CSArch::LOONG32:
        return cs_mode(CS_MODE_LITTLE_ENDIAN | CS_MODE_LOONGARCH32);
    case CSArch::LOONG64:
        return cs_mode(CS_MODE_LITTLE_ENDIAN | CS_MODE_LOONGARCH64);
    }
    return cs_mode(0);
}

const char *WingEngine::getErrorString(ErrorKSEngine error) {
    return ks_strerror(ks_err(error));
}

const char *WingEngine::getErrorString(ErrorCSEngine error) {
    return cs_strerror(cs_err(error));
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
