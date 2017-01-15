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

#include "../common.h"
#include "../interface/window.h"

typedef enum tile_inspector_page
{
	PAGE_DEFAULT,
	PAGE_SURFACE,
	PAGE_PATH,
	PAGE_TRACK,
	PAGE_SCENERY,
	PAGE_ENTRANCE,
	PAGE_FENCE,
	PAGE_LARGE_SCENERY,
	PAGE_BANNER,
	PAGE_CORRUPT
} tile_inspector_page;

extern sint32 windowTileInspectorElementCount;

void window_tile_inspector_set_page(rct_window *w, const tile_inspector_page page);
void window_tile_inspector_auto_set_buttons(rct_window *w);
