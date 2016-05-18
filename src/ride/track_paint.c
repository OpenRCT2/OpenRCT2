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

#include "../addresses.h"
#include "../config.h"
#include "../drawing/drawing.h"
#include "../paint/supports.h"
#include "../interface/viewport.h"
#include "../paint/paint.h"
#include "../interface/window.h"
#include "../localisation/localisation.h"
#include "../sprites.h"
#include "../world/map.h"
#include "../world/sprite.h"
#include "ride_data.h"
#include "track_data.h"
#include "track_paint.h"
#include "../paint/map_element/map_element.h"

/* rct2: 0x007667AC */
static rct_xy16 loc_7667AC[] = {
	{.x = -1, .y = 0},
	{.x = 0, .y = -1},
	{.x = 1, .y = 0},
	{.x = 0, .y = 1},
};

/* rct2: 0x007667AE */
static rct_xy16 loc_7667AE[] = {
	{.x = 0, .y = -1},
	{.x = 1, .y = 0},
	{.x = 0, .y = 1},
	{.x = -1, .y = 0},
};

const uint8 track_map_2x2[][4] = {
	{0, 1, 2, 3},
	{1, 3, 0, 2},
	{3, 2, 1, 0},
	{2, 0, 3, 1}
};

const uint8 edges_2x2[] = {
	EDGE_NE | EDGE_NW,
	EDGE_NE | EDGE_SE,
	EDGE_SW | EDGE_NW,
	EDGE_SW | EDGE_SE,
};

const uint8 track_map_3x3[][9] = {
	{0, 1, 2, 3, 4, 5, 6, 7, 8},
	{0, 3, 5, 7, 2, 8, 1, 6, 4},
	{0, 7, 8, 6, 5, 4, 3, 1, 2},
	{0, 6, 4, 1, 8, 2, 7, 3, 5}
};

const uint8 edges_3x3[] = {
	0,
	EDGE_NE | EDGE_NW,
	EDGE_NE,
	EDGE_NE | EDGE_SE,
	EDGE_NW,
	EDGE_SE,
	EDGE_SW | EDGE_NW,
	EDGE_SW | EDGE_SE,
	EDGE_SW,
};

const uint8 track_map_4x4[][16] = {
	{0,  1,  2,  3,  4,  5,  6,  7,  8,  9,  10, 11, 12, 13, 14, 15},
	{3,  7,  11, 15, 2,  6,  10, 14, 1,  5,  9,  13, 0,  4,  8,  12},
	{15, 14, 13, 12, 11, 10, 9,  8,  7,  6,  5,  4,  3,  2,  1,  0},
	{12, 8,  4,  0,  13, 9,  5,  1,  14, 10, 6,  2,  15, 11, 7,  3},
};

const uint8 edges_4x4[] = {
	EDGE_NE | EDGE_NW,
	EDGE_NE,
	EDGE_NE,
	EDGE_NE | EDGE_SE,
	EDGE_NW,
	0,
	0,
	EDGE_SE,
	EDGE_NW,
	0,
	0,
	EDGE_SE,
	EDGE_NW | EDGE_SW,
	EDGE_SW,
	EDGE_SW,
	EDGE_SW | EDGE_SE
};

const uint32 floorSpritesCork[] = {
	SPR_FLOOR_CORK_SE_SW,
	SPR_FLOOR_CORK_SW,
	SPR_FLOOR_CORK_SE,
	SPR_FLOOR_CORK
};

const uint32 fenceSpritesRope[] = {
	SPR_FENCE_ROPE_NE,
	SPR_FENCE_ROPE_SE,
	SPR_FENCE_ROPE_SW,
	SPR_FENCE_ROPE_NW
};

enum
{
	SPR_STATION_COVER_OFFSET_NE_SW_BACK_0 = 0,
	SPR_STATION_COVER_OFFSET_NE_SW_BACK_1,
	SPR_STATION_COVER_OFFSET_NE_SW_FRONT,
	SPR_STATION_COVER_OFFSET_SE_NW_BACK_0,
	SPR_STATION_COVER_OFFSET_SE_NW_BACK_1,
	SPR_STATION_COVER_OFFSET_SE_NW_FRONT,
	SPR_STATION_COVER_OFFSET_HIGH
};

bool track_paint_util_has_fence(enum edge edge, rct_xy16 position, rct_map_element * mapElement, rct_ride * ride, uint8 rotation)
{
	rct_xy16 offset;
	switch (edge) {
		case EDGE_NE:
			offset = loc_7667AC[rotation];
			break;
		case EDGE_SE:
			offset = loc_7667AE[(rotation + 2) & 3];
			break;
		case EDGE_SW:
			offset = loc_7667AC[(rotation + 2) & 3];
			break;
		case EDGE_NW:
			offset = loc_7667AE[rotation];
			break;
	}

	uint16 entranceLoc =
		((position.x / 32) + offset.x) |
		(((position.y / 32) + offset.y) << 8);

	int entranceId = map_get_station(mapElement);

	return (ride->entrances[entranceId] != entranceLoc && ride->exits[entranceId] != entranceLoc);
}

void track_paint_util_paint_floor(uint8 edges, uint32 colourFlags, uint16 height, const uint32 floorSprites[4], uint8 rotation)
{
	uint32 imageId;

	if (edges & EDGE_SW && edges & EDGE_SE) {
		imageId = floorSprites[0];
	} else if (edges & EDGE_SW) {
		imageId = floorSprites[1];
	} else if (edges & EDGE_SE) {
		imageId = floorSprites[2];
	} else {
		imageId = floorSprites[3];
	}

	sub_98197C(imageId | colourFlags, 0, 0, 32, 32, 1, height, 0, 0, height, rotation);
}

void track_paint_util_paint_fences(uint8 edges, rct_xy16 position, rct_map_element * mapElement, rct_ride * ride, uint32 colourFlags, uint16 height, const uint32 fenceSprites[4], uint8 rotation)
{
	uint32 imageId;

	if (edges & EDGE_NW && track_paint_util_has_fence(EDGE_NW, position, mapElement, ride, rotation)) {
		imageId = fenceSprites[3] | colourFlags;
		sub_98199C(imageId, 0, 0, 32, 1, 7, height, 0, 2, height + 2, rotation);
	}
	if (edges & EDGE_NE && track_paint_util_has_fence(EDGE_NE, position, mapElement, ride, rotation)) {
		imageId = fenceSprites[0] | colourFlags;
		sub_98199C(imageId, 0, 0, 1, 32, 7, height, 2, 0, height + 2, rotation);
	}
	if (edges & EDGE_SE && track_paint_util_has_fence(EDGE_SE, position, mapElement, ride, rotation)) {
		imageId = fenceSprites[1] | colourFlags;
		sub_98197C(imageId, 0, 0, 32, 1, 7, height, 0, 30, height + 2, rotation);
	}
	if (edges & EDGE_SW && track_paint_util_has_fence(EDGE_SW, position, mapElement, ride, rotation)) {
		imageId = fenceSprites[2] | colourFlags;
		sub_98197C(imageId, 0, 0, 1, 32, 7, height, 30, 0, height + 2, rotation);
	}
}

/* Supports are only placed every 2 tiles for flat pieces*/
bool track_paint_util_should_paint_supports(rct_xy16 position)
{
	if ((position.x & (1 << 5)) == (position.y & (1 << 5)))
		return true;

	if ((!(position.x & (1 << 5))) && (!(position.y & (1 << 5))))
		return true;

	return false;
}

bool track_paint_util_draw_station_covers(enum edge edge, bool hasFence, const rct_ride_entrance_definition * entranceStyle, uint8 direction, uint16 height)
{
	if (RCT2_GLOBAL(0x0141E9DB, uint8) & 3) {
		return false;
	}

	uint32 imageId;
	uint32 baseImageId = entranceStyle->base_image_id;
	int imageOffset;
	rct_xyz16 offset, bounds, boundsOffset;

	offset = (rct_xyz16) {0, 0, height};
	switch (edge) {
		case EDGE_NE:
			bounds = (rct_xyz16) {1, 30, 0};
			boundsOffset = (rct_xyz16) {0, 1, height + 1};
			imageOffset = hasFence ? SPR_STATION_COVER_OFFSET_SE_NW_BACK_1 : SPR_STATION_COVER_OFFSET_SE_NW_BACK_0;
			break;
		case EDGE_SE:
			bounds = (rct_xyz16) {32, 32, 0};
			boundsOffset = (rct_xyz16) {1, 0, height + 31};
			imageOffset = SPR_STATION_COVER_OFFSET_NE_SW_FRONT;
			break;
		case EDGE_SW:
			bounds = (rct_xyz16) {32, 32, 0};
			boundsOffset = (rct_xyz16) {0, 0, height + 31};
			imageOffset = SPR_STATION_COVER_OFFSET_SE_NW_FRONT;
			break;
		case EDGE_NW:
			bounds = (rct_xyz16) {30, 1, 30};
			boundsOffset = (rct_xyz16) {1, 0, height + 1};
			imageOffset = hasFence ? SPR_STATION_COVER_OFFSET_NE_SW_BACK_1 : SPR_STATION_COVER_OFFSET_NE_SW_BACK_0;
			break;
	}

	if (RCT2_GLOBAL(0x00F441A0, uint32) != 0x20000000) {
		baseImageId &= 0x7FFFF;
	}

	if (baseImageId <= 0x20) {
		return false;
	}

	if (baseImageId & 0x40000000) {
		imageId = (baseImageId & 0xBFFFFFFF) + imageOffset;
		sub_98197C(imageId, (sint8)offset.x, (sint8)offset.y, bounds.x, bounds.y, (sint8)bounds.z, offset.z, boundsOffset.x, boundsOffset.y, boundsOffset.z, get_current_rotation());

		uint32 edi = RCT2_GLOBAL(0x00F44198, uint32) & (0b11111 << 19);

		// weird jump
		imageId = (baseImageId | edi) + 0x3800000 + imageOffset + 12;
		sub_98199C(imageId, (sint8)offset.x, (sint8)offset.y, bounds.x, bounds.y, (sint8)bounds.z, offset.z, boundsOffset.x, boundsOffset.y, boundsOffset.z, get_current_rotation());
		return true;
	}

	imageId = (baseImageId + imageOffset) | RCT2_GLOBAL(0x00F44198, uint32);
	sub_98197C(imageId, (sint8)offset.x, (sint8)offset.y, bounds.x, bounds.y, (sint8)bounds.z, offset.z, boundsOffset.x, boundsOffset.y, boundsOffset.z, get_current_rotation());
	return true;
}

/**
 *
 *  rct2: 0x006C4794
 */
void track_paint(uint8 direction, int height, rct_map_element *mapElement)
{
	rct_drawpixelinfo *dpi = RCT2_GLOBAL(0x0140E9A8, rct_drawpixelinfo*);
	rct_ride *ride;
	int rideIndex, trackType, trackColourScheme, trackSequence;

	rideIndex = mapElement->properties.track.ride_index;
	ride = get_ride(rideIndex);
	if (ride->type == RIDE_TYPE_NULL) {
		log_error("Attempted to paint invalid ride: %d", rideIndex);
		return;
	}

	// HACK Set entrance style to plain if none to stop glitch until entrance track piece is implemented
	bool isEntranceStyleNone = false;
	if (ride->entrance_style == RIDE_ENTRANCE_STYLE_NONE) {
		isEntranceStyleNone = true;
		ride->entrance_style = RIDE_ENTRANCE_STYLE_PLAIN;
	}

	if (!(RCT2_GLOBAL(0x009DEA6F, uint8) & 1) || rideIndex == RCT2_GLOBAL(0x00F64DE8, uint8)) {
		trackType = mapElement->properties.track.type;
		trackSequence = mapElement->properties.track.sequence & 0x0F;
		trackColourScheme = mapElement->properties.track.colour & 3;

		if ((gCurrentViewportFlags & VIEWPORT_FLAG_TRACK_HEIGHTS) && dpi->zoom_level == 0) {
			RCT2_GLOBAL(RCT2_ADDRESS_PAINT_SETUP_CURRENT_TYPE, uint8) = 0;
			if (RCT2_ADDRESS(0x00999694, uint32)[trackType] & (1 << trackSequence)) {
				uint16 ax = RideData5[ride->type].z_offset;
				uint32 ebx = 0x20381689 + (height + 8) / 16;
				ebx += RCT2_GLOBAL(RCT2_ADDRESS_CONFIG_HEIGHT_MARKERS, uint16);
				ebx -= RCT2_GLOBAL(0x01359208, uint16);
				sub_98197C(ebx, 16, 16, 1, 1, 0, height + ax + 3, 1000, 1000, 2047, get_current_rotation());
			}
		}

		RCT2_GLOBAL(RCT2_ADDRESS_PAINT_SETUP_CURRENT_TYPE, uint8) = 3;
		RCT2_GLOBAL(0x00F44198, uint32) = (ride->track_colour_main[trackColourScheme] << 19) | (ride->track_colour_additional[trackColourScheme] << 24) | 0xA0000000;
		RCT2_GLOBAL(0x00F441A0, uint32) = 0x20000000;
		RCT2_GLOBAL(0x00F441A4, uint32) = 0x20C00000;
		RCT2_GLOBAL(0x00F4419C, uint32) = (ride->track_colour_supports[trackColourScheme] << 19) | 0x20000000;
		if (mapElement->type & 0x40) {
			RCT2_GLOBAL(0x00F44198, uint32) = 0x21600000;
			RCT2_GLOBAL(0x00F4419C, uint32) = 0x21600000;
			RCT2_GLOBAL(0x00F441A0, uint32) = 0x21600000;
			RCT2_GLOBAL(0x00F441A4, uint32) = 0x21600000;
		}
		if (mapElement->flags & MAP_ELEMENT_FLAG_GHOST) {
			uint32 ghost_id = construction_markers[gConfigGeneral.construction_marker_colour];
			RCT2_GLOBAL(RCT2_ADDRESS_PAINT_SETUP_CURRENT_TYPE, uint8) = 0;
			RCT2_GLOBAL(0x00F44198, uint32) = ghost_id;
			RCT2_GLOBAL(0x00F4419C, uint32) = ghost_id;
			RCT2_GLOBAL(0x00F441A0, uint32) = ghost_id;
			RCT2_GLOBAL(0x00F441A4, uint32) = ghost_id;
		}

		int rideType = ride->type;
		if (rideType == RIDE_TYPE_JUNIOR_ROLLER_COASTER) {
			switch (trackType) {
			case TRACK_ELEM_60_DEG_UP:
			case TRACK_ELEM_25_DEG_UP_TO_60_DEG_UP:
			case TRACK_ELEM_60_DEG_UP_TO_25_DEG_UP:
			case TRACK_ELEM_60_DEG_DOWN:
			case TRACK_ELEM_25_DEG_DOWN_TO_60_DEG_DOWN:
			case TRACK_ELEM_60_DEG_DOWN_TO_25_DEG_DOWN:
				rideType = RIDE_TYPE_WATER_COASTER;
				break;

			case TRACK_ELEM_FLAT_TO_60_DEG_UP:
			case TRACK_ELEM_60_DEG_UP_TO_FLAT:
			case TRACK_ELEM_FLAT_TO_60_DEG_DOWN:
			case TRACK_ELEM_60_DEG_DOWN_TO_FLAT:
				return;
			}
		}

		TRACK_PAINT_FUNCTION **trackTypeList = (TRACK_PAINT_FUNCTION**)RideTypeTrackPaintFunctionsOld[rideType];
		if (trackTypeList == NULL) {
			TRACK_PAINT_FUNCTION_GETTER paintFunctionGetter = RideTypeTrackPaintFunctions[rideType];
			TRACK_PAINT_FUNCTION paintFunction = paintFunctionGetter(trackType, direction);
			if (paintFunction != NULL) {
				paintFunction(rideIndex, trackSequence, direction, height, mapElement);
			}
		}
		else {
			uint32 *trackDirectionList = (uint32*)trackTypeList[trackType];

			// Have to call from this point as it pushes esi and expects callee to pop it
			RCT2_CALLPROC_X(
				0x006C4934,
				rideType,
				(int)trackDirectionList,
				direction,
				height,
				(int)mapElement,
				rideIndex * sizeof(rct_ride),
				trackSequence
				);
		}
	}

	if (isEntranceStyleNone) {
		ride->entrance_style = RIDE_ENTRANCE_STYLE_NONE;
	}
}
