/*****************************************************************************
 * Copyright (c) 2014-2022 OpenRCT2 developers
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

struct track_curve_chain
{
    int32_t next;
    int32_t previous;
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

enum
{
    NO_SPIN,
    L8_SPIN,
    R8_SPIN,
    LR_SPIN,
    RL_SPIN,
    L7_SPIN,
    R7_SPIN,
    L5_SPIN,
    R5_SPIN,
    RC_SPIN, // Rotation Control Spin
    SP_SPIN, // Special rapids Spin
    L9_SPIN,
    R9_SPIN
};

extern const track_descriptor gTrackDescriptors[142];

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

using TrackComputeFunction = int32_t (*)(const int16_t);
struct TrackElementDescriptor
{
    StringId Description;
    rct_track_coordinates Coordinates;

    rct_preview_track* Block;
    uint8_t PieceLength;
    track_curve_chain CurveChain;
    track_type_t AlternativeType;
    // Price Modifier should be used as in the following calculation:
    // (RideTrackPrice * TED::PriceModifier) / 65536
    uint32_t PriceModifier;
    track_type_t MirrorElement;
    uint32_t HeightMarkerPositions;
    uint16_t Flags;

    std::array<uint8_t, MaxSequencesPerPiece> SequenceElementAllowedWallEdges;
    std::array<uint8_t, MaxSequencesPerPiece> SequenceProperties;

    rct_trackdefinition Definition;
    uint8_t SpinFunction;

    TrackComputeFunction VerticalFactor;
    TrackComputeFunction LateralFactor;
};

namespace OpenRCT2
{
    namespace TrackMetaData
    {
        const TrackElementDescriptor& GetTrackElementDescriptor(const uint32_t type);
    } // namespace TrackMetaData
} // namespace OpenRCT2
