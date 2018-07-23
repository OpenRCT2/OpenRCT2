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

using hook_function = uint8_t (*)(registers* regs);

void addhook(uintptr_t address, hook_function function);

#endif
