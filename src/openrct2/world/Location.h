#pragma region Copyright (c) 2017 OpenRCT2 Developers
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

#include "../common.h"

#define LOCATION_NULL     ((sint16)(uint16)0x8000)
#define RCT_XY8_UNDEFINED 0xFFFF
#define MakeXY16(x, y)    {(sint16)(x), (sint16)(y)}

#pragma pack(push, 1)
typedef struct LocationXY8 {
    union {
        struct {
            uint8 x, y;
        };
        uint16 xy;
    };
} LocationXY8;
assert_struct_size(LocationXY8, 2);

typedef struct LocationXYZ8 {
    uint8 x, y, z;
} LocationXYZ8;
assert_struct_size(LocationXYZ8, 3);

typedef struct LocationXYZD8 {
    uint8 x, y, z, direction;
} LocationXYZD8;
assert_struct_size(LocationXYZD8, 4);

typedef struct LocationXY16 {
    sint16 x, y;
} LocationXY16;
assert_struct_size(LocationXY16, 4);


typedef struct LocationXYZ16 {
    sint16 x, y, z;
} LocationXYZ16;
assert_struct_size(LocationXYZ16, 6);

typedef struct LocationXYZD16 {
    sint16 x, y, z;
    uint8 direction;
} LocationXYZD16;
assert_struct_size(LocationXYZD16, 7);

typedef struct LocationXY32 {
    sint32 x, y;
} LocationXY32;

typedef struct LocationXYZ32 {
    sint32 x, y, z;
} LocationXYZ32;

#pragma pack(pop)