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

constexpr const uint8_t MaxSequencesPerPiece = 16;

// 0x009968BB, 0x009968BC, 0x009968BD, 0x009968BF, 0x009968C1, 0x009968C3
extern const rct_track_coordinates TrackCoordinates[TrackElemType::Count];

extern const uint8_t TrackSequenceProperties[TrackElemType::Count][MaxSequencesPerPiece];

extern const rct_preview_track* TrackBlocks[TrackElemType::Count];

extern const uint8_t TrackPieceLengths[TrackElemType::Count];

struct track_curve_chain
{
    int32_t next;
    int32_t previous;
};

extern const track_curve_chain gTrackCurveChain[TrackElemType::Count];

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

extern const track_descriptor gTrackDescriptors[142];

extern const track_type_t AlternativeTrackTypes[TrackElemType::Count];

extern const money32 TrackPricing[TrackElemType::Count];

struct dodgems_track_size
{
    uint8_t left;
    uint8_t top;
    uint8_t right;
    uint8_t bottom;
};

constexpr const dodgems_track_size DodgemsTrackSize(track_type_t type)
{
    if (type == TrackElemType::FlatTrack2x2)
        return { 4, 4, 59, 59 };
    if (type == TrackElemType::FlatTrack4x4)
        return { 4, 4, 123, 123 };
    if (type == TrackElemType::FlatTrack2x4)
        return { 4, 4, 59, 123 };
    return { 0, 0, 0, 0 };
}
extern const track_type_t TrackElementMirrorMap[TrackElemType::Count];

extern const uint32_t TrackHeightMarkerPositions[TrackElemType::Count];

extern const uint8_t TrackSequenceElementAllowedWallEdges[TrackElemType::Count][16];

extern const uint16_t TrackFlags[TrackElemType::Count];
