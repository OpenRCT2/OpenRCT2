/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../common.h"
#include "Track.h"
#include "TrackPaint.h"
#include "VehicleSubpositionData.h"

constexpr const uint8_t MaxSequencesPerPiece = 16;

#define TRACK_BLOCK_END                                                                                                        \
    {                                                                                                                          \
        255, 255, 255, 255, 255, { 255, 255 }, 255                                                                             \
    }


struct track_curve_chain
{
    uint16_t next;
    uint16_t previous;
};

struct dodgems_track_size
{
    uint8_t left;
    uint8_t top;
    uint8_t right;
    uint8_t bottom;
};
struct track_descriptor
{
    bool starts_diagonal;
    uint8_t slope_start;
    uint8_t bank_start;
    uint8_t track_curve;
    uint8_t slope_end;
    uint8_t bank_end;
    uint8_t track_element;
};

struct TrackTypeDescriptor
{
    track_type_t AlternateTrackType;
    track_type_t MirroredTrackType;
    uint8_t Length;
    rct_trackdefinition TrackDefinition;
    rct_track_coordinates TrackCoordinates;
    money32 TrackPrice;
    uint32_t TrackHeightMarkerPositions;
    dodgems_track_size DodgemsTrackSize;
    std::array<uint8_t, 16> AllowedWallEdges;
    std::vector<rct_preview_track> TrackBlocks;
    rct_vehicle_info_list SubpositionData;
};

#ifdef _WIN32
#    define SET_FIELD(fieldname, ...) __VA_ARGS__
#else
#    define SET_FIELD(fieldname, ...) .fieldname = __VA_ARGS__
#endif

// 0x009968BB, 0x009968BC, 0x009968BD, 0x009968BF, 0x009968C1, 0x009968C3
extern const rct_track_coordinates TrackCoordinates[TrackElemType::Count];
// 0x009972BB, 0x009972BC, 0x009972BD, 0x009972BF, 0x009972C1, 0x009972C3
extern const rct_track_coordinates FlatTrackCoordinates[TrackElemType::Count];

extern const uint8_t TrackSequenceProperties[TrackElemType::Count][MaxSequencesPerPiece];
extern const uint8_t FlatRideTrackSequenceProperties[TrackElemType::Count][MaxSequencesPerPiece];

extern const rct_preview_track* TrackBlocks[TrackElemType::Count];
extern const rct_preview_track* FlatRideTrackBlocks[TrackElemType::Count];

extern const uint8_t TrackPieceLengths[TrackElemType::Count];

extern const track_curve_chain gTrackCurveChain[TrackElemType::Count];
extern const track_curve_chain gFlatRideTrackCurveChain[TrackElemType::Count];

extern const track_descriptor gTrackDescriptors[142];

extern const int16_t AlternativeTrackTypes[256];

extern const money32 TrackPricing[256];
extern const money32 FlatRideTrackPricing[256];

extern const dodgems_track_size DodgemsTrackSize[256];

extern const uint8_t TrackElementMirrorMap[256];

extern const uint32_t TrackHeightMarkerPositions[256];

extern const uint8_t TrackSequenceElementAllowedWallEdges[256][16];
extern const uint8_t FlatRideTrackSequenceElementAllowedWallEdges[256][16];

extern const uint16_t FlatTrackFlags[256];
extern const uint16_t TrackFlags[256];

// clang-format off
constexpr const TrackTypeDescriptor DummyTTD = {
    SET_FIELD(AlternateTrackType, -1),
    SET_FIELD(MirroredTrackType, -1),
    SET_FIELD(Length, 0),
    SET_FIELD(TrackDefinition, {0, 0, 0, 0, 0, 0, 0}),
    SET_FIELD(TrackCoordinates, {0, 0, 0, 0, 0, 0}),
    SET_FIELD(TrackPrice, 0),
    SET_FIELD(TrackHeightMarkerPositions, 1),
    SET_FIELD(DodgemsTrackSize, {0, 0, 0, 0}),
    SET_FIELD(AllowedWallEdges, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0),
    SET_FIELD(TrackBlocks, TRACK_BLOCK_END),
    SET_FIELD(SubpositionData,0)
};
// clang-format on
