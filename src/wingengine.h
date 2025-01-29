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

#ifndef WINGENGINE_H
#define WINGENGINE_H

#include <QObject>
#include <QString>

#include "3rdparty/keystone/include/keystone/keystone.h"
#include "capstone/capstone.h"

/**
 * @brief A simple wrapper QT class for capstone and keystone
 */
class WingEngine {
    Q_GADGET

public:
    // same as cs_err, but exported to QT meta-systems
    enum class ErrorCSEngine {
        ERR_OK = 0,
        ERR_MEM,
        ERR_ARCH,
        ERR_HANDLE,
        ERR_CSH,
        ERR_MODE,
        ERR_OPTION,
        ERR_DETAIL,
        ERR_MEMSETUP,
        ERR_VERSION,
        ERR_DIET,
        ERR_SKIPDATA,
        ERR_X86_ATT,
        ERR_X86_INTEL,
        ERR_X86_MASM,
    };
    Q_ENUM(ErrorCSEngine)

    // same as ks_err, but exported to QT meta-systems
    enum class ErrorKSEngine {
        ERR_OK = 0,
        ERR_NOMEM,
        ERR_ARCH,
        ERR_HANDLE,
        ERR_MODE,
        ERR_VERSION,
        ERR_OPT_INVALID,

        // generic input assembly errors - parser specific
        ERR_ASM_EXPR_TOKEN = KS_ERR_ASM,
        ERR_ASM_DIRECTIVE_VALUE_RANGE,
        ERR_ASM_DIRECTIVE_ID,
        ERR_ASM_DIRECTIVE_TOKEN,
        ERR_ASM_DIRECTIVE_STR,
        ERR_ASM_DIRECTIVE_COMMA,
        ERR_ASM_DIRECTIVE_RELOC_NAME,
        ERR_ASM_DIRECTIVE_RELOC_TOKEN,
        ERR_ASM_DIRECTIVE_FPOINT,
        ERR_ASM_DIRECTIVE_UNKNOWN,
        ERR_ASM_DIRECTIVE_EQU,
        ERR_ASM_DIRECTIVE_INVALID,
        ERR_ASM_VARIANT_INVALID,
        ERR_ASM_EXPR_BRACKET,
        ERR_ASM_SYMBOL_MODIFIER,
        ERR_ASM_SYMBOL_REDEFINED,
        ERR_ASM_SYMBOL_MISSING,
        ERR_ASM_RPAREN,
        ERR_ASM_STAT_TOKEN,
        ERR_ASM_UNSUPPORTED,
        ERR_ASM_MACRO_TOKEN,
        ERR_ASM_MACRO_PAREN,
        ERR_ASM_MACRO_EQU,
        ERR_ASM_MACRO_ARGS,
        ERR_ASM_MACRO_LEVELS_EXCEED,
        ERR_ASM_MACRO_STR,
        ERR_ASM_MACRO_INVALID,
        ERR_ASM_ESC_BACKSLASH,
        ERR_ASM_ESC_OCTAL,
        ERR_ASM_ESC_SEQUENCE,
        ERR_ASM_ESC_STR,
        ERR_ASM_TOKEN_INVALID,
        ERR_ASM_INSN_UNSUPPORTED,
        ERR_ASM_FIXUP_INVALID,
        ERR_ASM_LABEL_INVALID,
        ERR_ASM_FRAGMENT_INVALID,

        // generic input assembly errors - architecture specific
        ERR_ASM_INVALIDOPERAND = KS_ERR_ASM_ARCH,
        ERR_ASM_MISSINGFEATURE,
        ERR_ASM_MNEMONICFAIL,
    };
    Q_ENUM(ErrorKSEngine)

public:
    enum class CSArch : int {
        ARM_V8_EB,
        THUMB_V8_EB,
        ARM_V7_EB,
        ARM_V8,
        THUMB_V8,
        ARM_V7,
        THUMB_V7,
        AARCH64,
        S390X,
        MIPS,
        MIPS64,
        MIPS_EL,
        MIPS64_EL,
        POWER_PC64,
        POWER_PC64_EL,
        I386,
        X86,
        X86_64,
        RISCV32,
        RISCV64,
        LOONG32,
        LOONG64
    };
    Q_ENUM(CSArch)

    enum class KSArch {
        ARM_V8_EB,
        THUMB_V8_EB,
        ARM_V7_EB,
        ARM_V8,
        THUMB_V8,
        ARM_V7,
        THUMB_V7,
        AARCH64,
        HEXAGON,
        S390X,
        SPARC64,
        SPARC,
        SPARC_EL,
        SPARC64_V9,
        SPARC_V9,
        SPARC_EL_V9,
        MIPS,
        MIPS64,
        MIPS_EL,
        MIPS64_EL,
        POWER_PC32,
        POWER_PC64,
        POWER_PC32_EL,
        POWER_PC64_EL,
        I386,
        X86,
        X86_64
    };
    Q_ENUM(KSArch)

    enum class AsmFormat { INTEL, ATT, MASM };
    Q_ENUM(AsmFormat)

public:
    static QByteArray doAsm(const QString &code, KSArch arch, AsmFormat format,
                            ErrorKSEngine &errcode);
    static QString doDisasm(const QByteArray &code, CSArch arch,
                            AsmFormat format, ErrorCSEngine &errcode);

    static const char *getErrorString(ErrorKSEngine error);
    static const char *getErrorString(ErrorCSEngine error);

private:
    static ks_arch ksArch(KSArch arch);
    static cs_arch csArch(CSArch arch);

    static ks_mode ksArch2ksMode(KSArch arch);
    static cs_mode csArch2csMode(CSArch arch);

private:
    static ks_opt_value asmFmt2KsOptFmt(AsmFormat fmt);
    static cs_opt_value asmFmt2CsOptFmt(AsmFormat fmt);
};

#endif // WINGENGINE_H
