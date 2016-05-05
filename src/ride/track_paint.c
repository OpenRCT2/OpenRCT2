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
			uint32 ghost_id = RCT2_ADDRESS(0x00993CC4, uint32)[gConfigGeneral.construction_marker_colour];
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
