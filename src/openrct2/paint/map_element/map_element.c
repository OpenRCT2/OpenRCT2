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
#include "map_element.h"
#include "../../drawing/drawing.h"
#include "../../ride/ride_data.h"
#include "../../ride/track_data.h"
#include "../../ride/track_paint.h"
#include "../../config/Config.h"
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
uint8 g141E9DB;
uint16 gUnk141E9DC;
rct_xy16 gPaintMapPosition;
bool gDidPassSurface;
rct_map_element * gSurfaceElement;
tunnel_entry gLeftTunnels[TUNNEL_MAX_COUNT];
uint8 gLeftTunnelCount;
tunnel_entry gRightTunnels[TUNNEL_MAX_COUNT];
uint8 gRightTunnelCount;
uint8 gVerticalTunnelHeight;
#endif

#ifdef __TESTPAINT__
uint16 testPaintVerticalTunnelHeight;
#endif

static void blank_tiles_paint(sint32 x, sint32 y);
static void sub_68B3FB(sint32 x, sint32 y);

const sint32 SEGMENTS_ALL = SEGMENT_B4 | SEGMENT_B8 | SEGMENT_BC | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4;

/**
 *
 *  rct2: 0x0068B35F
 */
void map_element_paint_setup(sint32 x, sint32 y)
{
	if (
		x < gMapSizeUnits &&
		y < gMapSizeUnits &&
		x >= 32 &&
		y >= 32
	) {
		paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0);
		paint_util_force_set_general_support_height(-1, 0);
		g141E9DB = 0;
		gUnk141E9DC = 0xFFFF;

		sub_68B3FB(x, y);
	} else {
		blank_tiles_paint(x, y);
	}
}

/**
 *
 *  rct2: 0x0068B2B7
 */
void sub_68B2B7(sint32 x, sint32 y)
{
	if (
		x < gMapSizeUnits &&
		y < gMapSizeUnits &&
		x >= 32 &&
		y >= 32
	) {
		paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0);
		paint_util_force_set_general_support_height(-1, 0);
		gUnk141E9DC = 0xFFFF;
		g141E9DB = G141E9DB_FLAG_2;

		sub_68B3FB(x, y);
	} else {
		blank_tiles_paint(x, y);
	}
}

/**
 *
 *  rct2: 0x0068B60E
 */
static void blank_tiles_paint(sint32 x, sint32 y)
{
	rct_drawpixelinfo *dpi = unk_140E9A8;

	sint32 dx = 0;
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
	sint32 bx = dx + 32;
	if (bx <= dpi->y) return;
	dx -= 20;
	dx -= dpi->height;
	if (dx >= dpi->y) return;
	gUnk9DE568 = x;
	gUnk9DE56C = y;
	gPaintInteractionType = VIEWPORT_INTERACTION_ITEM_NONE;
	sub_98196C(3123, 0, 0, 32, 32, -1, 16, get_current_rotation());
}

bool gShowSupportSegmentHeights = false;

/**
 *
 *  rct2: 0x0068B3FB
 */
static void sub_68B3FB(sint32 x, sint32 y)
{
	rct_drawpixelinfo *dpi = unk_140E9A8;

	gLeftTunnelCount = 0;
	gRightTunnelCount = 0;
	gLeftTunnels[0] = (tunnel_entry){0xFF, 0xFF};
	gRightTunnels[0] = (tunnel_entry){0xFF, 0xFF};

	gVerticalTunnelHeight = 0xFF;

#ifndef NO_RCT2
	RCT2_GLOBAL(0x009DE56A, uint16) = x;
	RCT2_GLOBAL(0x009DE56E, uint16) = y;
#endif
	gPaintMapPosition.x = x;
	gPaintMapPosition.y = y;

	rct_map_element* map_element = map_get_first_element_at(x >> 5, y >> 5);
	uint8 rotation = get_current_rotation();

	/* Check if the first (lowest) map_element is below the clip
	 * height. */
	if ((gCurrentViewportFlags & VIEWPORT_FLAG_PAINT_CLIP_TO_HEIGHT) && (map_element->base_height > gClipHeight)) {
		blank_tiles_paint(x, y);
		return;
	}

	sint32 dx = 0;
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
		gPaintMapPosition.x == gMapSelectArrowPosition.x &&
		gPaintMapPosition.y == gMapSelectArrowPosition.y
	) {
		uint8 arrowRotation =
			(rotation
			+ (gMapSelectArrowDirection & 3)) & 3;

		uint32 imageId =
			arrowRotation +
			(gMapSelectArrowDirection & 0xFC) +
			0x20900C27;
		sint32 arrowZ = gMapSelectArrowPosition.z;

		gUnk9DE568 = x;
		gUnk9DE56C = y;
		gPaintInteractionType = VIEWPORT_INTERACTION_ITEM_NONE;

		sub_98197C(imageId, 0, 0, 32, 32, 0xFF, arrowZ, 0, 0, arrowZ + 18, rotation);
	}
	sint32 bx = dx + 52;

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

	gUnk9DE568 = x;
	gUnk9DE56C = y;
	gDidPassSurface = false;
	do {
		// Only paint map_elements below the clip height.
		if ((gCurrentViewportFlags & VIEWPORT_FLAG_PAINT_CLIP_TO_HEIGHT) && (map_element->base_height > gClipHeight)) break;

		sint32 direction = (map_element->type + rotation) & MAP_ELEMENT_DIRECTION_MASK;
		sint32 height = map_element->base_height * 8;

		rct_xy16 dword_9DE574 = gPaintMapPosition;
		g_currently_drawn_item = map_element;
		// Setup the painting of for example: the underground, signs, rides, scenery, etc.
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
		case MAP_ELEMENT_TYPE_WALL:
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
		gPaintMapPosition = dword_9DE574;
	} while (!map_element_is_last_for_tile(map_element++));

	if (!gShowSupportSegmentHeights) {
		return;
	}

	if (map_element_get_type(map_element - 1) == MAP_ELEMENT_TYPE_SURFACE) {
		return;
	}

	static const sint32 segmentPositions[][3] = {
		{0, 6, 2},
		{5, 4, 8},
		{1, 7, 3},
	};

	for (sint32 sy = 0; sy < 3; sy++) {
		for (sint32 sx = 0; sx < 3; sx++) {
			uint16 segmentHeight = gSupportSegments[segmentPositions[sy][sx]].height;
			sint32 imageColourFlats = 0b101111 << 19 | 0x40000000;
			if (segmentHeight == 0xFFFF) {
				segmentHeight = gSupport.height;
				// white: 0b101101
				imageColourFlats = 0b111011 << 19 | 0x40000000;
			}

			// Only draw supports below the clipping height.
			if ((gCurrentViewportFlags & VIEWPORT_FLAG_PAINT_CLIP_TO_HEIGHT) && (segmentHeight > gClipHeight)) continue;

			sint32 xOffset = sy * 10;
			sint32 yOffset = -22 + sx * 10;
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

void paint_util_set_vertical_tunnel(uint16 height)
{
#ifdef __TESTPAINT__
	testPaintVerticalTunnelHeight = height;
#endif
	gVerticalTunnelHeight = height / 16;
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

void paint_util_set_segment_support_height(sint32 segments, uint16 height, uint8 slope)
{
	for (sint32 s = 0; s < 9; s++) {
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
