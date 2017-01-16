#pragma region Copyright (c) 2014-2016 OpenRCT2 Developers
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

#pragma once

// Structures shared between both RCT1 and RCT2.

#include "common.h"

#define RCT12_MAX_AWARDS        4

#pragma pack(push, 1)

typedef struct rct12_award
{
    uint16 time;
    uint16 type;
} rct12_award;
assert_struct_size(rct12_award, 4);

#pragma pack(pop)
