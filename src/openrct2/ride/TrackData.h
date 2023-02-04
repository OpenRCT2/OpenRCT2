/*****************************************************************************
 * Copyright (c) 2014-2023 OpenRCT2 developers
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

#define TRACK_BLOCK_END                                                                                                        \
{                                                                                                                          \
    255, 255, 255, 255, 255, { 255, 255 }, 255                                                                             \
}

constexpr const uint32_t MaxSequencesPerPiece = 256;

// 0x009968BB, 0x009968BC, 0x009968BD, 0x009968BF, 0x009968C1, 0x009968C3

struct TrackCurveChain
{
    int32_t next;
    int32_t previous;
};

struct TrackDescriptor
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

extern const TrackDescriptor gTrackDescriptors[142];

struct DodgemsTrackSize
{
    uint8_t left;
    uint8_t top;
    uint8_t right;
    uint8_t bottom;
};

constexpr const DodgemsTrackSize GetDodgemsTrackSize(track_type_t type)
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
    TrackCoordinates Coordinates;

    PreviewTrack* Block;
    uint8_t PieceLength;
    TrackCurveChain CurveChain;
    track_type_t AlternativeType;
    // Price Modifier should be used as in the following calculation:
    // (RideTrackPrice * TED::PriceModifier) / 65536
    uint32_t PriceModifier;
    track_type_t MirrorElement;
    uint32_t HeightMarkerPositions;
    uint16_t Flags;

    std::array<uint8_t, MaxSequencesPerPiece> SequenceElementAllowedWallEdges;
    std::array<uint8_t, MaxSequencesPerPiece> SequenceProperties;

    TrackDefinition Definition;
    uint8_t SpinFunction;

    TrackComputeFunction VerticalFactor;
    TrackComputeFunction LateralFactor;

    /**
     * Retrieves the block for the given sequence. This method safely handles
     * out-of-bounds sequence indices.
     *
     * @param sequenceIndex
     * @return The track block, or nullptr if it doesn’t exist.
     */
    const PreviewTrack* GetBlockForSequence(uint8_t sequenceIndex) const;
};

namespace OpenRCT2
{
    namespace TrackMetaData
    {
        const TrackElementDescriptor& GetTrackElementDescriptor(const uint32_t type);
    } // namespace TrackMetaData
} // namespace OpenRCT2
