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

#include "../paint.h"
#include "../../interface/viewport.h"
#include "../../addresses.h"
#include "map_element.h"
#include "../../drawing/drawing.h"
#include "../../ride/ride_data.h"
#include "../../ride/track_data.h"
#include "../../ride/track_paint.h"
#include "../../config.h"
#include "../../world/sprite.h"
#include "../../world/banner.h"
#include "../../world/entrance.h"
#include "../../world/footpath.h"
#include "../../world/scenery.h"
#include "../../sprites.h"
#include "../../localisation/localisation.h"
#include "../../game.h"
#include "../supports.h"

static void blank_tiles_paint(int x, int y);
static void sub_68B3FB(int x, int y);

/**
 *
 *  rct2: 0x0068B35F
 */
void map_element_paint_setup(int x, int y)
{
	rct_drawpixelinfo *dpi = RCT2_GLOBAL(0x0140E9A8, rct_drawpixelinfo*);
	if (
		x < gMapSizeUnits &&
		y < gMapSizeUnits &&
		x >= 32 &&
		y >= 32
	) {
		RCT2_GLOBAL(0x0141E9B4, uint32) = 0xFFFF;
		RCT2_GLOBAL(0x0141E9B8, uint32) = 0xFFFF;
		RCT2_GLOBAL(0x0141E9BC, uint32) = 0xFFFF;
		RCT2_GLOBAL(0x0141E9C0, uint32) = 0xFFFF;
		RCT2_GLOBAL(0x0141E9C4, uint32) = 0xFFFF;
		RCT2_GLOBAL(0x0141E9C8, uint32) = 0xFFFF;
		RCT2_GLOBAL(0x0141E9CC, uint32) = 0xFFFF;
		RCT2_GLOBAL(0x0141E9D0, uint32) = 0xFFFF;
		RCT2_GLOBAL(0x0141E9D4, uint32) = 0xFFFF;
		RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_PAINT_TILE_MAX_HEIGHT, sint16) = -1;
		RCT2_GLOBAL(0x0141E9DA, sint16) = 0;
		RCT2_GLOBAL(0x0141E9DC, uint32) = 0xFFFF;

		sub_68B3FB(x, y);
	} else {
		blank_tiles_paint(x, y);
	}
}

/**
 *
 *  rct2: 0x0068B2B7
 */
void sub_68B2B7(int x, int y)
{
	if (
		x < gMapSizeUnits &&
		y < gMapSizeUnits &&
		x >= 32 &&
		y >= 32
	) {
		RCT2_GLOBAL(0x0141E9B4, uint32) = 0xFFFF;
		RCT2_GLOBAL(0x0141E9B8, uint32) = 0xFFFF;
		RCT2_GLOBAL(0x0141E9BC, uint32) = 0xFFFF;
		RCT2_GLOBAL(0x0141E9C0, uint32) = 0xFFFF;
		RCT2_GLOBAL(0x0141E9C4, uint32) = 0xFFFF;
		RCT2_GLOBAL(0x0141E9C8, uint32) = 0xFFFF;
		RCT2_GLOBAL(0x0141E9CC, uint32) = 0xFFFF;
		RCT2_GLOBAL(0x0141E9D0, uint32) = 0xFFFF;
		RCT2_GLOBAL(0x0141E9D4, uint32) = 0xFFFF;
		RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_PAINT_TILE_MAX_HEIGHT, sint16) = -1;
		RCT2_GLOBAL(0x0141E9DA, uint16) = 0;
		RCT2_GLOBAL(0x0141E9DC, uint32) = 0xFFFF;
		RCT2_GLOBAL(0x0141E9DB, uint8) |= 2;

		sub_68B3FB(x, y);
	} else {
		blank_tiles_paint(x, y);
	}
}

/**
 *
 *  rct2: 0x0068B60E
 */
static void blank_tiles_paint(int x, int y)
{
	rct_drawpixelinfo *dpi = RCT2_GLOBAL(0x0140E9A8, rct_drawpixelinfo*);

	int dx;
	switch (get_current_rotation()) {
	case 0:
		dx = x + y;
		break;
	case 1:
		x += 32;
		dx = y - x;
		break;
	case 2:
		x += 32;
		y += 32;
		dx = -(x + y);
		break;
	case 3:
		y += 32;
		dx = x - y;
		break;
	}
	dx /= 2;
	dx -= 16;
	int bx = dx + 32;
	if (bx <= dpi->y) return;
	dx -= 20;
	dx -= dpi->height;
	if (dx >= dpi->y) return;
	RCT2_GLOBAL(0x9DE568, sint16) = x;
	RCT2_GLOBAL(0x9DE56C, sint16) = y;
	RCT2_GLOBAL(RCT2_ADDRESS_PAINT_SETUP_CURRENT_TYPE, uint8_t) = VIEWPORT_INTERACTION_ITEM_NONE;
	sub_98196C(3123, 0, 0, 32, 32, -1, 16, get_current_rotation());
}

/**
 *
 *  rct2: 0x0068B3FB
 */
static void sub_68B3FB(int x, int y)
{
	rct_drawpixelinfo *dpi = RCT2_GLOBAL(0x0140E9A8, rct_drawpixelinfo*);

	RCT2_GLOBAL(0x141F56A, uint16_t) = 0;
	RCT2_GLOBAL(0x9E3138, uint8_t) = 0xFF;
	RCT2_GLOBAL(0x9E30B6, uint8_t) = 0xFF;
	RCT2_GLOBAL(0x9E323C, uint8_t) = 0xFF;
	RCT2_GLOBAL(0x9DE56A, uint16_t) = x;
	RCT2_GLOBAL(0x9DE56E, uint16_t) = y;
	RCT2_GLOBAL(0x9DE574, uint16_t) = x;
	RCT2_GLOBAL(0x9DE576, uint16_t) = y;

	rct_map_element* map_element = map_get_first_element_at(x >> 5, y >> 5);
	uint8 rotation = get_current_rotation();

	int dx = 0;
	switch (rotation) {
	case 0:
		dx = x + y;
		break;
	case 1:
		x += 32;
		dx = y - x;
		break;
	case 2:
		x += 32;
		y += 32;
		dx = -(x + y);
		break;
	case 3:
		y += 32;
		dx = x - y;
		break;
	}
	dx >>= 1;
	// Display little yellow arrow when building footpaths?
	if ((RCT2_GLOBAL(RCT2_ADDRESS_MAP_SELECTION_FLAGS, uint16) & 4) &&
		RCT2_GLOBAL(0x9DE56A, uint16) == RCT2_GLOBAL(RCT2_ADDRESS_MAP_ARROW_X, uint16) &&
		RCT2_GLOBAL(0x9DE56E, uint16) == RCT2_GLOBAL(RCT2_ADDRESS_MAP_ARROW_Y, uint16)){
		uint8 arrowRotation =
			(rotation
			+ (RCT2_GLOBAL(RCT2_ADDRESS_MAP_ARROW_DIRECTION, uint8) & 3)) & 3;

		uint32 imageId =
			arrowRotation +
			(RCT2_GLOBAL(RCT2_ADDRESS_MAP_ARROW_DIRECTION, uint8) & 0xFC) +
			0x20900C27;
		int arrowZ = RCT2_GLOBAL(RCT2_ADDRESS_MAP_ARROW_Z, uint16);

		RCT2_GLOBAL(0x9DE568, sint16) = x;
		RCT2_GLOBAL(0x9DE56C, sint16) = y;
		RCT2_GLOBAL(RCT2_ADDRESS_PAINT_SETUP_CURRENT_TYPE, uint8) = VIEWPORT_INTERACTION_ITEM_NONE;

		sub_98197C(imageId, 0, 0, 32, 32, 0xFF, arrowZ, 0, 0, arrowZ + 18, rotation);
	}
	int bx = dx + 52;

	if (bx <= dpi->y)
		return;

	const rct_map_element* element = map_element;//push map_element

	sint16 max_height = 0;
	do{
		max_height = max(max_height, element->clearance_height);
	} while (!map_element_is_last_for_tile(element++));

	element--;

	if (map_element_get_type(element) == MAP_ELEMENT_TYPE_SURFACE &&
		(element->properties.surface.terrain & MAP_ELEMENT_WATER_HEIGHT_MASK) != 0){
		max_height = (element->properties.surface.terrain & MAP_ELEMENT_WATER_HEIGHT_MASK) << 1;
	}

	max_height *= 8;

	dx -= max_height + 32;

	element = map_element;//pop map_element
	dx -= dpi->height;
	if (dx >= dpi->y)
		return;

	RCT2_GLOBAL(0x9DE568, sint16) = x;
	RCT2_GLOBAL(0x9DE56C, sint16) = y;
	RCT2_GLOBAL(0x9DE57C, uint16) = 0;
	do {
		int direction = (map_element->type + rotation) & MAP_ELEMENT_DIRECTION_MASK;
		int height = map_element->base_height * 8;

		uint32_t dword_9DE574 = RCT2_GLOBAL(0x9DE574, uint32_t);
		RCT2_GLOBAL(0x9DE578, rct_map_element*) = map_element;
		//setup the painting of for example: the underground, signs, rides, scenery, etc.
		switch (map_element_get_type(map_element))
		{
		case MAP_ELEMENT_TYPE_SURFACE:
			surface_paint(direction, height, map_element);
			break;
		case MAP_ELEMENT_TYPE_PATH:
			path_paint(direction, height, map_element);
			break;
		case MAP_ELEMENT_TYPE_TRACK:
			track_paint(direction, height, map_element);
			break;
		case MAP_ELEMENT_TYPE_SCENERY:
			scenery_paint(direction, height, map_element);
			break;
		case MAP_ELEMENT_TYPE_ENTRANCE:
			entrance_paint(direction, height, map_element);
			break;
		case MAP_ELEMENT_TYPE_FENCE:
			fence_paint(direction, height, map_element);
			break;
		case MAP_ELEMENT_TYPE_SCENERY_MULTIPLE:
			scenery_multiple_paint(direction, height, map_element);
			break;
		case MAP_ELEMENT_TYPE_BANNER:
			banner_paint(direction, height, map_element);
			break;
		// A corrupt element inserted by OpenRCT2 itself, which skips the drawing of the next element only.
		case MAP_ELEMENT_TYPE_CORRUPT:
			if (map_element_is_last_for_tile(map_element))
				return;
			map_element++;
			break;
		default:
			// An undefined map element is most likely a corrupt element inserted by 8 cars' MOM feature to skip drawing of all elements after it.
			return;
		}
		RCT2_GLOBAL(0x9DE574, uint32_t) = dword_9DE574;
	} while (!map_element_is_last_for_tile(map_element++));
}
