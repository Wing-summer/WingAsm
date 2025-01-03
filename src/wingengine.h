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
    // based on cs_arch, but exported to QT meta-systems
    enum class CSArch : int {
        ARM = 0,
#ifdef CAPSTONE_AARCH64_COMPAT_HEADER
        ARM64 = 1,
#else
        AARCH64 = 1,
#endif
#ifdef CAPSTONE_SYSTEMZ_COMPAT_HEADER
        SYSZ = 2,
#else
        SYSTEMZ = 2,
#endif
        MIPS,
        X86,
        PPC,
        SPARC,
        XCORE,
        M68K,
        TMS320C64X,
        M680X,
        EVM,
        MOS65XX,
        WASM,
        BPF,
        RISCV,
        SH,
        TRICORE,
        ALPHA,
        HPPA,
        LOONGARCH,
        XTENSA,
    };
    Q_ENUM(CSArch)

    // based on ks_arch, but exported to QT meta-systems
    enum class KSArch : int {
        ARM = 1,
        ARM64,
        MIPS,
        X86,
        PPC,
        SPARC,
        SYSTEMZ,
        HEXAGON,
        EVM
    };
    Q_ENUM(KSArch)

public:
    // based on ks_mode, but exported to QT meta-systems
    enum class KSMode : uint {
        MODE_LITTLE_ENDIAN = 0,
        MODE_BIG_ENDIAN = 1 << 30,
        // arm / arm64
        MODE_ARM = 1 << 0,
        MODE_THUMB = 1 << 4,
        MODE_V8 = 1 << 6,
        // mips
        MODE_MICRO = 1 << 4,
        MODE_MIPS3 = 1 << 5,
        MODE_MIPS32R6 = 1 << 6,
        MODE_MIPS32 = 1 << 2,
        MODE_MIPS64 = 1 << 3,
        // x86 / x64
        MODE_16 = 1 << 1,
        MODE_32 = 1 << 2,
        MODE_64 = 1 << 3,
        // ppc
        MODE_PPC32 = 1 << 2,
        MODE_PPC64 = 1 << 3,
        MODE_QPX = 1 << 4,
        // sparc
        MODE_SPARC32 = 1 << 2,
        MODE_SPARC64 = 1 << 3,
        MODE_V9 = 1 << 4
    };
    Q_ENUM(KSMode)
    Q_DECLARE_FLAGS(KSModes, KSMode)

    // based on cs_mode, but exported to QT meta-systems
    enum class CSMode : uint {
        MODE_LITTLE_ENDIAN = 0,
        MODE_ARM = 0,
        MODE_16 = 1 << 1,
        MODE_32 = 1 << 2,
        MODE_64 = 1 << 3,
        MODE_THUMB = 1 << 4,
        MODE_MCLASS = 1 << 5,
        MODE_V8 = 1 << 6,
        MODE_V9 = 1 << 4,
        MODE_QPX = 1 << 4,
        MODE_SPE = 1 << 5,
        MODE_BOOKE = 1 << 6,
        MODE_PS = 1 << 7,
        MODE_AIX_OS = 1 << 8,
        MODE_PWR7 = 1 << 9,
        MODE_PWR8 = 1 << 10,
        MODE_PWR9 = 1 << 11,
        MODE_PWR10 = 1 << 12,
        MODE_PPC_ISA_FUTURE = 1 << 13,
        MODE_MODERN_AIX_AS = 1 << 14,
        MODE_MSYNC = 1 << 15,

        MODE_M68K_000 = 1 << 1,
        MODE_M68K_010 = 1 << 2,
        MODE_M68K_020 = 1 << 3,
        MODE_M68K_030 = 1 << 4,
        MODE_M68K_040 = 1 << 5,
        MODE_M68K_060 = 1 << 6,
        MODE_BIG_ENDIAN = 1U << 31,
        MODE_MIPS16 = MODE_16,
        MODE_MIPS32 = MODE_32,
        MODE_MIPS64 = MODE_64,
        MODE_MICRO = 1 << 4,
        MODE_MIPS1 = 1 << 5,
        MODE_MIPS2 = 1 << 6,
        MODE_MIPS32R2 = 1 << 7,
        MODE_MIPS32R3 = 1 << 8,
        MODE_MIPS32R5 = 1 << 9,
        MODE_MIPS32R6 = 1 << 10,
        MODE_MIPS3 = 1 << 11,
        MODE_MIPS4 = 1 << 12,
        MODE_MIPS5 = 1 << 13,
        MODE_MIPS64R2 = 1 << 14,
        MODE_MIPS64R3 = 1 << 15,
        MODE_MIPS64R5 = 1 << 16,
        MODE_MIPS64R6 = 1 << 17,
        MODE_OCTEON = 1 << 18,
        MODE_OCTEONP = 1 << 19,
        MODE_NANOMIPS = 1 << 20,
        MODE_NMS1 = ((1 << 21) | MODE_NANOMIPS),
        MODE_I7200 = ((1 << 22) | MODE_NANOMIPS),
        MODE_MIPS_NOFLOAT = 1 << 23,
        MODE_MIPS_PTR64 = 1 << 24,
        MODE_MICRO32R3 = (MODE_MICRO | MODE_MIPS32R3),
        MODE_MICRO32R6 = (MODE_MICRO | MODE_MIPS32R6),
        MODE_M680X_6301 = 1 << 1,
        MODE_M680X_6309 = 1 << 2,
        MODE_M680X_6800 = 1 << 3,
        MODE_M680X_6801 = 1 << 4,
        MODE_M680X_6805 = 1 << 5,
        MODE_M680X_6808 = 1 << 6,
        MODE_M680X_6809 = 1 << 7,
        MODE_M680X_6811 = 1 << 8,
        MODE_M680X_CPU12 = 1 << 9,

        MODE_M680X_HCS08 = 1 << 10,
        MODE_BPF_CLASSIC = 0,
        MODE_BPF_EXTENDED = 1 << 0,
        MODE_RISCV32 = 1 << 0,
        MODE_RISCV64 = 1 << 1,
        MODE_RISCVC = 1 << 2,
        MODE_MOS65XX_6502 = 1 << 1,
        MODE_MOS65XX_65C02 = 1 << 2,
        MODE_MOS65XX_W65C02 = 1 << 3,
        MODE_MOS65XX_65816 = 1 << 4,
        MODE_MOS65XX_65816_LONG_M = (1 << 5),
        MODE_MOS65XX_65816_LONG_X = (1 << 6),
        MODE_MOS65XX_65816_LONG_MX =
            MODE_MOS65XX_65816_LONG_M | MODE_MOS65XX_65816_LONG_X,
        MODE_SH2 = 1 << 1,
        MODE_SH2A = 1 << 2,
        MODE_SH3 = 1 << 3,
        MODE_SH4 = 1 << 4,
        MODE_SH4A = 1 << 5,
        MODE_SHFPU = 1 << 6,
        MODE_SHDSP = 1 << 7,
        MODE_TRICORE_110 = 1 << 1,
        MODE_TRICORE_120 = 1 << 2,
        MODE_TRICORE_130 = 1 << 3,
        MODE_TRICORE_131 = 1 << 4,
        MODE_TRICORE_160 = 1 << 5,
        MODE_TRICORE_161 = 1 << 6,
        MODE_TRICORE_162 = 1 << 7,
        MODE_HPPA_11 = 1 << 1,
        MODE_HPPA_20 = 1 << 2,
        MODE_HPPA_20W = MODE_HPPA_20 | (1 << 3),
        MODE_LOONGARCH32 = 1 << 0,
        MODE_LOONGARCH64 = 1 << 1,
        MODE_SYSTEMZ_ARCH8 = 1 << 1,
        MODE_SYSTEMZ_ARCH9 = 1 << 2,
        MODE_SYSTEMZ_ARCH10 = 1 << 3,
        MODE_SYSTEMZ_ARCH11 = 1 << 4,
        MODE_SYSTEMZ_ARCH12 = 1 << 5,
        MODE_SYSTEMZ_ARCH13 = 1 << 6,
        MODE_SYSTEMZ_ARCH14 = 1 << 7,
        MODE_SYSTEMZ_Z10 = 1 << 8,
        MODE_SYSTEMZ_Z196 = 1 << 9,
        MODE_SYSTEMZ_ZEC12 = 1 << 10,
        MODE_SYSTEMZ_Z13 = 1 << 11,
        MODE_SYSTEMZ_Z14 = 1 << 12,
        MODE_SYSTEMZ_Z15 = 1 << 13,
        MODE_SYSTEMZ_Z16 = 1 << 14,
        MODE_SYSTEMZ_GENERIC = 1 << 15,
        MODE_XTENSA_ESP32 = 1 << 1,
        MODE_XTENSA_ESP32S2 = 1 << 2,
        MODE_XTENSA_ESP8266 = 1 << 3
    };
    Q_ENUM(CSMode)
    Q_DECLARE_FLAGS(CSModes, CSMode)

public:
    enum class AsmFormat { INTEL, ATT, MASM };
    Q_ENUM(AsmFormat)

public:
    static QString doAsm(const QByteArray &code, KSArch arch, KSModes mode,
                         AsmFormat format, ErrorKSEngine &errcode);
    static QString doDisasm(const QByteArray &code, CSArch arch, CSModes mode,
                            AsmFormat format, ErrorCSEngine &errcode);

    static QList<KSMode> getAvalibleModes(KSArch arch);
    static QList<CSMode> getAvalibleModes(CSArch arch);

private:
    static ks_opt_value asmFmt2KsOptFmt(AsmFormat fmt);
    static cs_opt_value asmFmt2CsOptFmt(AsmFormat fmt);
};

#endif // WINGENGINE_H
