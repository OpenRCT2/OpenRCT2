/*****************************************************************************
 * Copyright (c) 2014-2024 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../paint/support/WoodenSupports.h"
#include "Track.h"

using namespace OpenRCT2;

namespace OpenRCT2::TrackMetaData
{
    constexpr uint8_t kMaxSequencesPerPiece = 16;

    // 0x009968BB, 0x009968BC, 0x009968BD, 0x009968BF, 0x009968C1, 0x009968C3

    struct TrackCurveChain
    {
        TypeOrCurve next;
        TypeOrCurve previous;
    };

    struct TrackDescriptor
    {
        bool startsDiagonally;
        TrackPitch slopeStart;
        TrackRoll rollStart;
        TrackCurve trackCurve;
        TrackPitch slopeEnd;
        TrackRoll rollEnd;
        track_type_t trackElement;
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

    struct SequenceWoodenSupport
    {
        WoodenSupportSubType subType = WoodenSupportSubType::Null;
        WoodenSupportTransitionType transitionType = WoodenSupportTransitionType::None;
    };

    using TrackComputeFunction = int32_t (*)(const int16_t);
    struct TrackElementDescriptor
    {
        StringId description;
        TrackCoordinates coordinates;

        PreviewTrack* block;
        uint8_t pieceLength;
        TrackCurveChain curveChain;
        track_type_t alternativeType;
        // Price Modifier should be used as in the following calculation:
        // (RideTrackPrice * TED::PriceModifier) / 65536
        uint32_t priceModifier;
        track_type_t mirrorElement;
        uint32_t heightMarkerPositions;
        uint32_t flags;

        std::array<uint8_t, kMaxSequencesPerPiece> sequenceElementAllowedWallEdges;
        std::array<uint8_t, kMaxSequencesPerPiece> sequenceProperties;
        std::array<SequenceWoodenSupport, kMaxSequencesPerPiece> sequenceWoodenSupports;

        TrackDefinition definition;
        SpinFunction spinFunction;

        TrackComputeFunction verticalFactor;
        TrackComputeFunction lateralFactor;

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
