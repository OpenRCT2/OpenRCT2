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

#include "../../addresses.h"
#include "../paint.h"
#include "../../interface/viewport.h"
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

#ifdef NO_RCT2
tunnel_entry gLeftTunnels[65];
uint8 gLeftTunnelCount;
tunnel_entry gRightTunnels[65];
uint8 gRightTunnelCount;
#endif

static void blank_tiles_paint(int x, int y);
static void sub_68B3FB(int x, int y);

const int SEGMENTS_ALL = SEGMENT_B4 | SEGMENT_B8 | SEGMENT_BC | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4;

/**
 *
 *  rct2: 0x0068B35F
 */
void map_element_paint_setup(int x, int y)
{
	if (
		x < gMapSizeUnits &&
		y < gMapSizeUnits &&
		x >= 32 &&
		y >= 32
	) {
		paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0);
		paint_util_force_set_general_support_height(-1, 0);
		RCT2_GLOBAL(0x0141E9DB, uint8) = 0;
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
		paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0);
		paint_util_force_set_general_support_height(-1, 0);
		RCT2_GLOBAL(0x0141E9DC, uint32) = 0xFFFF;
		RCT2_GLOBAL(0x0141E9DB, uint8) = 2;

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
	rct_drawpixelinfo *dpi = unk_140E9A8;

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
	gPaintInteractionType = VIEWPORT_INTERACTION_ITEM_NONE;
	sub_98196C(3123, 0, 0, 32, 32, -1, 16, get_current_rotation());
}

bool gShowSupportSegmentHeights = false;

/**
 *
 *  rct2: 0x0068B3FB
 */
static void sub_68B3FB(int x, int y)
{
	rct_drawpixelinfo *dpi = unk_140E9A8;

	gLeftTunnelCount = 0;
	gRightTunnelCount = 0;
	gLeftTunnels[0] = (tunnel_entry){0xFF, 0xFF};
	gRightTunnels[0] = (tunnel_entry){0xFF, 0xFF};

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
	if ((gMapSelectFlags & MAP_SELECT_FLAG_ENABLE_ARROW) &&
		RCT2_GLOBAL(0x9DE56A, uint16) == gMapSelectArrowPosition.x &&
		RCT2_GLOBAL(0x9DE56E, uint16) == gMapSelectArrowPosition.y
	) {
		uint8 arrowRotation =
			(rotation
			+ (gMapSelectArrowDirection & 3)) & 3;

		uint32 imageId =
			arrowRotation +
			(gMapSelectArrowDirection & 0xFC) +
			0x20900C27;
		int arrowZ = gMapSelectArrowPosition.z;

		RCT2_GLOBAL(0x9DE568, sint16) = x;
		RCT2_GLOBAL(0x9DE56C, sint16) = y;
		gPaintInteractionType = VIEWPORT_INTERACTION_ITEM_NONE;

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
		g_currently_drawn_item = map_element;
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

	if (!gShowSupportSegmentHeights) {
		return;
	}

	if (map_element_get_type(map_element - 1) == MAP_ELEMENT_TYPE_SURFACE) {
		return;
	}

	static const int segmentPositions[][3] = {
		{0, 6, 2},
		{5, 4, 8},
		{1, 7, 3},
	};

	for (int y = 0; y < 3; y++) {
		for (int x = 0; x < 3; x++) {
			uint16 segmentHeight = gSupportSegments[segmentPositions[y][x]].height;
			int imageColourFlats = 0b101111 << 19 | 0x40000000;
			if (segmentHeight == 0xFFFF) {
				segmentHeight = gSupport.height;
				// white: 0b101101
				imageColourFlats = 0b111011 << 19 | 0x40000000;
			}

			int xOffset = y * 10;
			int yOffset = -22 + x * 10;
			paint_struct * ps = sub_98197C(5504 | imageColourFlats, xOffset, yOffset, 10, 10, 1, segmentHeight, xOffset + 1, yOffset + 16, segmentHeight, get_current_rotation());
			if (ps != NULL) {
				ps->flags &= PAINT_STRUCT_FLAG_IS_MASKED;
				ps->colour_image_id = COLOUR_BORDEAUX_RED;
			}

		}
	}
}

void paint_util_push_tunnel_left(uint16 height, uint8 type)
{
	gLeftTunnels[gLeftTunnelCount] = (tunnel_entry){.height = (height / 16), .type = type};
	gLeftTunnels[gLeftTunnelCount + 1] = (tunnel_entry){0xFF, 0xFF};
	gLeftTunnelCount++;
}

void paint_util_push_tunnel_right(uint16 height, uint8 type)
{
	gRightTunnels[gRightTunnelCount] = (tunnel_entry){.height = (height / 16), .type = type};
	gRightTunnels[gRightTunnelCount + 1] = (tunnel_entry){0xFF, 0xFF};
	gRightTunnelCount++;
}

void paint_util_set_general_support_height(sint16 height, uint8 slope)
{
	if (gSupport.height >= height) {
		return;
	}

	paint_util_force_set_general_support_height(height, slope);
}

void paint_util_force_set_general_support_height(sint16 height, uint8 slope)
{
	gSupport.height = height;
	gSupport.slope = slope;
}

const uint16 segment_offsets[9] = {
	SEGMENT_B4,
	SEGMENT_B8,
	SEGMENT_BC,
	SEGMENT_C0,
	SEGMENT_C4,
	SEGMENT_C8,
	SEGMENT_CC,
	SEGMENT_D0,
	SEGMENT_D4
};

void paint_util_set_segment_support_height(int segments, uint16 height, uint8 slope)
{
	for (int s = 0; s < 9; s++) {
		if (segments & segment_offsets[s]) {
			gSupportSegments[s].height = height;
			if (height != 0xFFFF) {
				gSupportSegments[s].slope = slope;
			}
		}
	}
}

uint16 paint_util_rotate_segments(uint16 segments, uint8 rotation)
{
	uint8 temp = segments & 0xFF;
	temp = rol8(temp, rotation * 2);

	return (segments & 0xFF00) | temp;
}
