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

#ifndef _ENTRANCE_H_
#define _ENTRANCE_H_

#include "../common.h"

#pragma pack(push, 1)
typedef struct rct_entrance_type {
	rct_string_id string_idx;	// 0x00
	uint32 image_id;			// 0x02
	uint8 scrolling_mode;		// 0x06
	uint8 text_height;			// 0x07
} rct_entrance_type;
assert_struct_size(rct_entrance_type, 8);
#pragma pack(pop)

#endif
