/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#ifndef NO_RCT2

#    include <openrct2/common.h>

enum
{
    X86_FLAG_CARRY = 1 << 0,
    X86_FLAG_PARITY = 1 << 2,

    X86_FLAG_ADJUST = 1 << 4,

    X86_FLAG_ZERO = 1 << 6,
    X86_FLAG_SIGN = 1 << 7,
};

/**
 * x86 register structure, only used for easy interop to RCT2 code.
 */
#    pragma pack(push, 1)
struct registers
{
    union
    {
        int32_t eax;
        int16_t ax;
        struct
        {
            char al;
            char ah;
        };
    };
    union
    {
        int32_t ebx;
        int16_t bx;
        struct
        {
            char bl;
            char bh;
        };
    };
    union
    {
        int32_t ecx;
        int16_t cx;
        struct
        {
            char cl;
            char ch;
        };
    };
    union
    {
        int32_t edx;
        int16_t dx;
        struct
        {
            char dl;
            char dh;
        };
    };
    union
    {
        int32_t esi;
        int16_t si;
    };
    union
    {
        int32_t edi;
        int16_t di;
    };
    union
    {
        int32_t ebp;
        int16_t bp;
    };
};
assert_struct_size(registers, 7 * 4);
#    pragma pack(pop)

using hook_function = uint8_t (*)(registers* regs);

void addhook(uintptr_t address, hook_function function);

#endif
