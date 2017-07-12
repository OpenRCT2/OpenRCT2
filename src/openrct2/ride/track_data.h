#pragma region Copyright (c) 2014-2017 OpenRCT2 Developers
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
#include "track.h"
#include "track_paint.h"

// 0x009968BB, 0x009968BC, 0x009968BD, 0x009968BF, 0x009968C1, 0x009968C3
extern const rct_track_coordinates TrackCoordinates[256];
// 0x009972BB, 0x009972BC, 0x009972BD, 0x009972BF, 0x009972C1, 0x009972C3
extern const rct_track_coordinates FlatTrackCoordinates[253];

extern const uint8 TrackSequenceProperties[256][16];
extern const uint8 FlatRideTrackSequenceProperties[256][16];

extern const uint64 RideTypePossibleTrackConfigurations[RIDE_TYPE_COUNT];
extern const rct_preview_track *TrackBlocks[256];
extern const rct_preview_track *FlatRideTrackBlocks[256];

extern const uint8 TrackPieceLengths[256];

typedef struct track_curve_chain {
    uint16 next;
    uint16 previous;
} track_curve_chain;

extern const track_curve_chain gTrackCurveChain[256];
extern const track_curve_chain gFlatRideTrackCurveChain[256];

extern const TRACK_PAINT_FUNCTION_GETTER RideTypeTrackPaintFunctions[RIDE_TYPE_COUNT];
extern const uint32 * RideTypeTrackPaintFunctionsOld[RIDE_TYPE_COUNT];

typedef struct track_descriptor {
    bool starts_diagonal;
    uint8 slope_start;
    uint8 bank_start;
    uint8 track_curve;
    uint8 slope_end;
    uint8 bank_end;
    uint8 track_element;
} track_descriptor;

extern const track_descriptor gTrackDescriptors[142];

typedef struct {
    uint16 size;
    const rct_vehicle_info info[];
} rct_vehicle_info_list;

extern const rct_vehicle_info_list **gTrackVehicleInfo[17];

extern const sint16 AlternativeTrackTypes[256];

extern const money32 TrackPricing[256];
extern const money32 FlatRideTrackPricing[256];

typedef struct dodgems_track_size {
    uint8 left;
    uint8 top;
    uint8 right;
    uint8 bottom;
} dodgems_track_size;

extern const dodgems_track_size DodgemsTrackSize[256];

extern const uint8 TrackElementMirrorMap[256];

extern const uint32 TrackHeightMarkerPositions[256];

extern const uint8 TrackSequenceElementAllowedWallEdges[256][16];
extern const uint8 FlatRideTrackSequenceElementAllowedWallEdges[256][16];

extern const uint16 FlatTrackFlags[256];
extern const uint16 TrackFlags[256];
