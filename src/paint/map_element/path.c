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

#include "map_element.h"
#include "../../addresses.h"
#include "../../world/map.h"

// #3628: Until path_paint is implemented, this variable is used by scrolling_text_setup
//        to use the old string arguments array. Remove when scrolling_text_setup is no
//        longer hooked.
bool TempForScrollText = false;

void path_paint(uint8 direction, uint16 height, rct_map_element *mapElement) {
	TempForScrollText = true;
	RCT2_CALLPROC_X(0x6A3590, 0, 0, direction, height, (int)mapElement, 0, 0);
	TempForScrollText = false;
}
