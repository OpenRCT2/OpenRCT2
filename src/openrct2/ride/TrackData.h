/*****************************************************************************
 * Copyright (c) 2014-2024 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "Track.h"

namespace OpenRCT2::TrackMetaData
{
    constexpr uint8_t MaxSequencesPerPiece = 16;

    // 0x009968BB, 0x009968BC, 0x009968BD, 0x009968BF, 0x009968C1, 0x009968C3

    struct TrackCurveChain
    {
        int32_t next;
        int32_t previous;
    };

    struct TrackDescriptor
    {
        bool starts_diagonal;
        TrackPitch slope_start;
        TrackRoll RollStart;
        TrackCurve track_curve;
        TrackPitch slope_end;
        TrackRoll RollEnd;
        track_type_t track_element;
    };

    enum class SpinFunction : uint8_t
    {
        None,
        L8,
        R8,
        LR,
        RL,
        L7,
        R7,
        L5,
        R5,
        RC, // Rotation Control Spin
        SP, // Special rapids Spin
        L9,
        R9
    };

    extern const TrackDescriptor gTrackDescriptors[186];

    struct DodgemsTrackSize
    {
        uint8_t left;
        uint8_t top;
        uint8_t right;
        uint8_t bottom;
    };

    constexpr DodgemsTrackSize GetDodgemsTrackSize(track_type_t type)
    {
        if (type == OpenRCT2::TrackElemType::FlatTrack2x2)
            return { 4, 4, 59, 59 };
        if (type == OpenRCT2::TrackElemType::FlatTrack4x4)
            return { 4, 4, 123, 123 };
        if (type == OpenRCT2::TrackElemType::FlatTrack2x4)
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
        uint32_t Flags;

        std::array<uint8_t, MaxSequencesPerPiece> SequenceElementAllowedWallEdges;
        std::array<uint8_t, MaxSequencesPerPiece> SequenceProperties;

        TrackDefinition Definition;
        SpinFunction spinFunction;

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

    const TrackElementDescriptor& GetTrackElementDescriptor(const uint32_t type);
} // namespace OpenRCT2::TrackMetaData
