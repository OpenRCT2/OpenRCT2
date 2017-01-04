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

#ifndef _WATER_H_
#define _WATER_H_

#include "../common.h"

#pragma pack(push, 1)
typedef struct rct_water_type {
	rct_string_id string_idx;	// 0x00
	uint32 image_id;			// 0x02
	uint32 var_06;
	uint32 var_0A;
	uint16 var_0E;
} rct_water_type;
assert_struct_size(rct_water_type, 16);
#pragma pack(pop)

#endif
