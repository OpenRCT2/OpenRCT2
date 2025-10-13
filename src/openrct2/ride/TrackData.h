/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../paint/support/MetalSupports.h"
#include "../paint/support/WoodenSupports.h"
#include "../paint/tile_element/Paint.Tunnel.h"
#include "../paint/tile_element/Segment.h"
#include "Track.h"

#include <optional>

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

    struct SequenceTunnel
    {
        int8_t height = 0;
        Direction direction{};
        TunnelSlope slope = TunnelSlope::flat;
        bool allowDoors = false;
    };

    constexpr uint8_t kSequenceTunnelMaxPerSequence = 2;

    using SequenceTunnelGroup = std::array<std::optional<SequenceTunnel>, kSequenceTunnelMaxPerSequence>;

    constexpr SequenceTunnelGroup kSequenceTunnelUnimplemented{};

    struct SequenceTunnels
    {
        std::array<SequenceTunnelGroup, kTunnelGroupCount> tunnelGroups{};
        std::optional<uint8_t> verticalTunnelHeight{};
    };

    constexpr SequenceTunnels SequenceTunnelsAllGroups(
        const SequenceTunnelGroup& tunnels, const std::optional<uint8_t> verticalTunnelHeight = std::nullopt)
    {
        return { { { tunnels, tunnels, tunnels } }, verticalTunnelHeight };
    }

    constexpr SequenceTunnels SequenceTunnelsReverse(SequenceTunnels sequenceTunnels)
    {
        for (auto& tunnelGroup : sequenceTunnels.tunnelGroups)
        {
            for (auto& tunnel : tunnelGroup)
            {
                if (tunnel.has_value())
                {
                    tunnel.value().direction = DirectionReverse(tunnel.value().direction);
                }
            }
        }
        return sequenceTunnels;
    }

    constexpr SequenceTunnels SequenceTunnelsPrev(SequenceTunnels sequenceTunnels)
    {
        for (auto& tunnelGroup : sequenceTunnels.tunnelGroups)
        {
            for (auto& tunnel : tunnelGroup)
            {
                if (tunnel.has_value())
                {
                    tunnel.value().direction = DirectionPrev(tunnel.value().direction);
                }
            }
        }
        return sequenceTunnels;
    }

    constexpr SequenceTunnels SequenceTunnelsFlipXAxis(SequenceTunnels sequenceTunnels)
    {
        for (auto& tunnelGroup : sequenceTunnels.tunnelGroups)
        {
            for (auto& tunnel : tunnelGroup)
            {
                if (tunnel.has_value())
                {
                    tunnel.value().direction = DirectionFlipXAxis(tunnel.value().direction);
                }
            }
        }
        return sequenceTunnels;
    }

    struct SequenceDescriptor
    {
        SequenceClearance clearance{};
        /** rct2: 0x00999A94 */
        uint8_t allowedWallEdges{};
        /** rct2: 0x0099BA64 */
        uint8_t flags{};
        SequenceWoodenSupport woodenSupports{};
        SequenceMetalSupport metalSupports{};
        int8_t extraSupportRotation = 0;
        bool invertSegmentBlocking = false;
        std::array<uint16_t, kBlockedSegmentsTypeCount> blockedSegments{ kSegmentsNone, kSegmentsNone, kSegmentsNone };
        SequenceTunnels tunnels{};
    };

    using TrackComputeFunction = int32_t (*)(const int16_t);
    struct TrackElementDescriptor
    {
        StringId description;
        TrackCoordinates coordinates;

        uint8_t pieceLength;
        TrackCurveChain curveChain;
        OpenRCT2::TrackElemType alternativeType;
        // Price Modifier should be used as in the following calculation:
        // (RideTrackPrice * TED::PriceModifier) / 65536
        uint32_t priceModifier;
        OpenRCT2::TrackElemType mirrorElement;
        uint32_t heightMarkerPositions;
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
