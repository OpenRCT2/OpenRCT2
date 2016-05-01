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

#include "../../world/sprite.h"
#include "../../addresses.h"

/**
 *
 *  rct2: 0x00672AC9
 */
void misc_paint(rct_sprite *misc, int imageDirection)
{
	RCT2_CALLPROC_X(0x00672AC9, misc->unknown.x, imageDirection, misc->unknown.y, misc->unknown.z, (int)misc, 0, 0);
}