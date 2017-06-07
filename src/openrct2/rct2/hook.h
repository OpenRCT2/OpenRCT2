#pragma region Copyright (c) 2014-2017 OpenRCT2 Developers
/*****************************************************************************
 * OpenRCT2, an open source clone of Roller Coaster Tycoon 2.
 *
 * OpenRCT2 is the work of many authors, a full list can be found in contributors.md
 * For more information, visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * A full copy of the GNU General Public License can be found in licence.txt
 *****************************************************************************/
#pragma endregion

#ifndef _HOOK_H_
#define _HOOK_H_

#ifndef NO_RCT2

#include "../common.h"

enum {
    X86_FLAG_CARRY = 1 << 0,
    X86_FLAG_PARITY = 1 << 2,

    X86_FLAG_ADJUST = 1 << 4,

    X86_FLAG_ZERO = 1 << 6,
    X86_FLAG_SIGN = 1 << 7,
};

typedef uint8 (hook_function)(registers *regs);

void addhook(uintptr_t address, hook_function *function);

#endif

#endif
