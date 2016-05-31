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
#include "../../world/scenery.h"
#include "../../drawing/lightfx.h"
#include "../../world/footpath.h"

// #3628: Until path_paint is implemented, this variable is used by scrolling_text_setup
//        to use the old string arguments array. Remove when scrolling_text_setup is no
//        longer hooked.
bool TempForScrollText = false;

void path_paint(uint8 direction, uint16 height, rct_map_element *mapElement) {
	TempForScrollText = true;
	RCT2_CALLPROC_X(0x6A3590, 0, 0, direction, height, (int)mapElement, 0, 0);
	TempForScrollText = false;

#ifdef STOUT_EXPANDED_RENDERING_LIGHT

	if (footpath_element_has_path_scenery(mapElement) && !(mapElement->flags & MAP_ELEMENT_FLAG_BROKEN)) {
		rct_scenery_entry *sceneryEntry;

		sceneryEntry = get_footpath_item_entry(footpath_element_get_path_scenery_index(mapElement));

		if (sceneryEntry->path_bit.flags & PATH_BIT_FLAG_LAMP) {
			if (!(mapElement->properties.path.edges & (1 << 0))) {
				lightfx_add_3d_light_magic_from_drawing_tile(-16, 0, height + 23, LIGHTFX_LIGHT_TYPE_LANTERN_3);
			}
			if (!(mapElement->properties.path.edges & (1 << 1))) {
				lightfx_add_3d_light_magic_from_drawing_tile(0, 16, height + 23, LIGHTFX_LIGHT_TYPE_LANTERN_3);
			}
			if (!(mapElement->properties.path.edges & (1 << 2))) {
				lightfx_add_3d_light_magic_from_drawing_tile(16, 0, height + 23, LIGHTFX_LIGHT_TYPE_LANTERN_3);
			}
			if (!(mapElement->properties.path.edges & (1 << 3))) {
				lightfx_add_3d_light_magic_from_drawing_tile(0, -16, height + 23, LIGHTFX_LIGHT_TYPE_LANTERN_3);
			}
		}
	}
#endif

}	
