/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../core/FlagHolder.hpp"
#include "../paint/support/MetalSupports.h"
#include "../paint/support/WoodenSupports.h"
#include "../paint/tile_element/Segment.h"
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
        OpenRCT2::TrackElemType trackElement;
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

    constexpr DodgemsTrackSize GetDodgemsTrackSize(OpenRCT2::TrackElemType type)
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
        WoodenSupportSubType subType = WoodenSupportSubType::null;
        WoodenSupportTransitionType transitionType = WoodenSupportTransitionType::none;
    };

    struct SequenceMetalSupport
    {
        MetalSupportPlace place = MetalSupportPlace::none;
        uint8_t alternates = false;
    };

    using SequenceFlags = FlagHolder<uint8_t, SequenceFlag>;

    struct SequenceDescriptor
    {
        SequenceClearance clearance{};
        /** rct2: 0x00999A94 */
        uint8_t allowedWallEdges{};
        /** rct2: 0x0099BA64 */
        SequenceFlags flags{};
        SequenceWoodenSupport woodenSupports{};
        SequenceMetalSupport metalSupports{};
        int8_t extraSupportRotation = 0;
        bool invertSegmentBlocking = false;
        std::array<uint16_t, kBlockedSegmentsTypeCount> blockedSegments{ kSegmentsNone, kSegmentsNone, kSegmentsNone };

        constexpr uint8_t getEntranceConnectionSides() const
        {
            return flags.holder & 0xF;
        }
    };

    using TrackComputeFunction = int32_t (*)(const int16_t);
    struct TrackElementDescriptor
    {
        StringId description;
        TrackCoordinates coordinates;

        // Used to estimate the ride length for number of powered vehicle trains
        uint8_t pieceLength;
        // Piece the ride construction window automatically selects next
        TrackCurveChain curveChain;
        // Track element to build when building "covered"/"splashdown" track
        OpenRCT2::TrackElemType alternativeType;
        // Price Modifier should be used as in the following calculation:
        // (RideTrackPrice * TED::PriceModifier) / 65536
        uint32_t priceModifier;
        OpenRCT2::TrackElemType mirrorElement;
        uint32_t flags;

        uint8_t numSequences{};
        std::array<SequenceDescriptor, kMaxSequencesPerPiece> sequences;

        TrackDefinition definition;
        SpinFunction spinFunction;

        TrackComputeFunction verticalFactor;
        TrackComputeFunction lateralFactor;
    };

    const TrackElementDescriptor& GetTrackElementDescriptor(OpenRCT2::TrackElemType type);
} // namespace OpenRCT2::TrackMetaData
