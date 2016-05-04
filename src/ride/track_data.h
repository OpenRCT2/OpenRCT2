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

#include "../common.h"
#include "track.h"
#include "track_paint.h"

// 0x009968BB, 0x009968BC, 0x009968BD, 0x009968BF, 0x009968C1, 0x009968C3
extern const rct_track_coordinates TrackCoordinates[256];
// 0x009972BB, 0x009972BC, 0x009972BD, 0x009972BF, 0x009972C1, 0x009972C3
extern const rct_track_coordinates* FlatTrackCoordinates;

extern const uint64 RideTypePossibleTrackConfigurations[91];
extern const rct_preview_track *TrackBlocks[256];
extern const rct_preview_track *FlatRideTrackBlocks[255];

extern const uint8 TrackPieceLengths[256];

typedef struct {
	uint16 next;
	uint16 previous;
} track_curve_chain;

extern const track_curve_chain gTrackCurveChain[256];
extern const track_curve_chain gFlatRideTrackCurveChain[256];

extern const TRACK_PAINT_FUNCTION_GETTER RideTypeTrackPaintFunctions[91];
extern const uint32 RideTypeTrackPaintFunctionsOld[91];

typedef struct {
	bool starts_diagonal;
	uint8 slope_start;
	uint8 bank_start;
	uint8 track_curve;
	uint8 slope_end;
	uint8 bank_end;
	uint8 track_element;
} track_descriptor;

extern const track_descriptor gTrackDescriptors[142];
