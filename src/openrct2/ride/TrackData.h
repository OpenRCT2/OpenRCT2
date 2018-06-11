/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3
 *****************************************************************************/

#pragma once

#include "../common.h"
#include "Track.h"
#include "TrackPaint.h"

// 0x009968BB, 0x009968BC, 0x009968BD, 0x009968BF, 0x009968C1, 0x009968C3
extern const rct_track_coordinates TrackCoordinates[256];
// 0x009972BB, 0x009972BC, 0x009972BD, 0x009972BF, 0x009972C1, 0x009972C3
extern const rct_track_coordinates FlatTrackCoordinates[253];

extern const uint8 TrackSequenceProperties[256][16];
extern const uint8 FlatRideTrackSequenceProperties[256][16];

extern const uint64              RideTypePossibleTrackConfigurations[RIDE_TYPE_COUNT];
extern const rct_preview_track * TrackBlocks[256];
extern const rct_preview_track * FlatRideTrackBlocks[256];

extern const uint8 TrackPieceLengths[256];

struct track_curve_chain
{
    uint16 next;
    uint16 previous;
};

extern const track_curve_chain gTrackCurveChain[256];
extern const track_curve_chain gFlatRideTrackCurveChain[256];

extern const TRACK_PAINT_FUNCTION_GETTER RideTypeTrackPaintFunctions[RIDE_TYPE_COUNT];
extern const uint32 *                    RideTypeTrackPaintFunctionsOld[RIDE_TYPE_COUNT];

struct track_descriptor
{
    bool  starts_diagonal;
    uint8 slope_start;
    uint8 bank_start;
    uint8 track_curve;
    uint8 slope_end;
    uint8 bank_end;
    uint8 track_element;
};

extern const track_descriptor gTrackDescriptors[142];

struct rct_vehicle_info_list
{
    uint16                   size;
    const rct_vehicle_info * info;
};

extern const rct_vehicle_info_list * const * const gTrackVehicleInfo[17];

extern const sint16 AlternativeTrackTypes[256];

extern const money32 TrackPricing[256];
extern const money32 FlatRideTrackPricing[256];

struct dodgems_track_size
{
    uint8 left;
    uint8 top;
    uint8 right;
    uint8 bottom;
};

extern const dodgems_track_size DodgemsTrackSize[256];

extern const uint8 TrackElementMirrorMap[256];

extern const uint32 TrackHeightMarkerPositions[256];

extern const uint8 TrackSequenceElementAllowedWallEdges[256][16];
extern const uint8 FlatRideTrackSequenceElementAllowedWallEdges[256][16];

extern const uint16 FlatTrackFlags[256];
extern const uint16 TrackFlags[256];

