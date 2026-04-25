/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../../core/FlagHolder.hpp"
#include "../../localisation/StringIdType.h"
#include "../../paint/support/MetalSupports.h"
#include "../../paint/support/WoodenSupports.h"
#include "../../paint/tile_element/Paint.Tunnel.h"
#include "../../paint/tile_element/Segment.h"
#include "../../world/QuarterTile.h"
#include "PitchAndRoll.h"
#include "TrackElemType.h"
#include "TrackGroup.h"

#include <array>
#include <cstdint>

namespace OpenRCT2::TrackMetadata
{
    constexpr uint8_t kMaxSequencesPerPiece = 16;

    using TrackComputeFunction = int32_t (*)(int16_t);

    template<int32_t TConstant>
    static int32_t EvaluatorConst(const int16_t)
    {
        return TConstant;
    }

    enum class TrackElementFlag : uint8_t
    {
        onlyUnderwater,
        turnLeft,
        turnRight,
        turnBanked,
        turnSloped,
        down,
        up,
        normalToInversion,
        isGolfHole = normalToInversion,
        startsAtHalfHeight,
        onlyAboveGround,

        // Used to allow steep backwards lifts on roller coasters that do not allow steep
        // forward lift hills
        isSteepUp,
        helix,
        allowLiftHill,
        curveAllowsLift,
        inversionToNormal,

        // Also set on Spinning Tunnel and Log Flume reverser, probably to save a flag.
        banked,
        canBePartlyUnderground,
    };
    using TrackFlags = FlagHolder<uint32_t, TrackElementFlag>;

    enum class TrackCurve : uint8_t
    {
        leftVerySmall = 5,
        leftSmall = 3,
        left = 1,
        leftLarge = 7,
        none = 0,
        rightLarge = 8,
        right = 2,
        rightSmall = 4,
        rightVerySmall = 6
    };
    constexpr uint8_t kHighestCurveValue = 8;

    enum class SequenceFlag : uint8_t
    {
        entranceConnectionNE, // Ride Entrances and path connect to front
        entranceConnectionSE, // connect to right
        entranceConnectionSW, // connect to back
        entranceConnectionNW, // connect to left
        trackOrigin,
        connectsToPath,  // The sides of this sequence block flagged with entrance connections can connect directly to paths
        disallowDoors,   // Doors cannot be built on any side of this track element (only valid on first sequence of track
                         // element)
        hasHeightMarker, // Displays a height marker on this sequence block
    };
    using SequenceFlags = FlagHolder<uint8_t, SequenceFlag>;

    enum class SpinFunction : uint8_t
    {
        none,
        l8,
        r8,
        lr,
        rl,
        l7,
        r7,
        l5,
        r5,
        rc, // Rotation Control Spin
        sp, // Special rapids Spin
        l9,
        r9
    };

    /** TrackCoordinates
     * rotationBegin: Start direction of the track piece; 0 (cardinal) or 4 (45 degrees clockwise)
     * rotationEnd: End direction of the track peice, measured clockwise
     * zBegin: how high above the base level of the lowest track block the spline starts
     * zEnd: how high above the base level of the lowest track block the spline ends
     * x: tile offset to the end track block; negative is along the original direction of travel
     * Next track piece is found by moving one tile after end of the track piece, so vertical track has an x value of 32
     * y: tile offset to the end track block; negative is to the left of the original direction of travel
     */
    struct TrackCoordinates
    {
        int8_t rotationBegin; // 0x00
        int8_t rotationEnd;   // 0x01
        int16_t zBegin;       // 0x02
        int16_t zEnd;         // 0x04
        int16_t x;            // 0x06
        int16_t y;            // 0x08
    };

    struct TypeOrCurve
    {
        bool isTrackType = true; // true if a OpenRCT2::TrackElemType is selected, false if a TrackCurve is selected;
        union
        {
            OpenRCT2::TrackElemType trackType = OpenRCT2::TrackElemType::none;
            TrackCurve curve;
        };

        constexpr TypeOrCurve() noexcept
        {
            isTrackType = false;
            curve = TrackCurve::none;
        }

        constexpr bool operator==(OpenRCT2::TrackElemType rhs) const
        {
            return isTrackType && (trackType == rhs);
        }

        constexpr bool operator==(TrackCurve rhs) const
        {
            return !isTrackType && (curve == rhs);
        }

        constexpr TypeOrCurve(OpenRCT2::TrackElemType _type) noexcept
        {
            isTrackType = true;
            trackType = _type;
        }

        constexpr TypeOrCurve& operator=(OpenRCT2::TrackElemType rhs) noexcept
        {
            isTrackType = true;
            trackType = rhs;
            return *this;
        }

        constexpr TypeOrCurve(TrackCurve _curve) noexcept
        {
            isTrackType = false;
            curve = _curve;
        }

        constexpr TypeOrCurve& operator=(TrackCurve rhs) noexcept
        {
            isTrackType = false;
            curve = rhs;
            return *this;
        }
    };

    struct TrackCurveChain
    {
        TypeOrCurve next;
        TypeOrCurve previous;
    };

    enum class ClearanceFlag : uint8_t
    {
        flag0,
        flag1,
        isVertical,
    };
    using ClearanceFlags = FlagHolder<uint8_t, ClearanceFlag>;

    struct SequenceClearance
    {
        int16_t x{};
        int16_t y{};
        int16_t z{};
        uint8_t clearanceZ{};
        QuarterTile quarterTile = { 0, 0 };
        ClearanceFlags flags{};
    };

    struct SequenceWoodenSupport
    {
        WoodenSupportSubType subType = WoodenSupportSubType::null;
        WoodenSupportTransitionType transitionType = WoodenSupportTransitionType::none;
        int8_t height = 0;
    };

    struct SequenceMetalSupport
    {
        MetalSupportPlace place = MetalSupportPlace::none;
        uint8_t alternates = false;
        int8_t height = 0;
    };

    using BlockedSegmentsPerType = std::array<uint16_t, kBlockedSegmentsTypeCount>;
    constexpr BlockedSegmentsPerType kFlatStraightBlockedSegments = { {
        EnumsToFlags(PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomLeft), // narrow
        EnumsToFlags(PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomLeft), // inverted
        kSegmentsAll,                                                                         // wide
    } };

    static constexpr int16_t kDoNotSetGeneralSupportHeight = std::numeric_limits<int16_t>::min();

    constexpr int16_t calculateGeneralSupportHeight(
        const SequenceClearance& sequenceClearance, const int32_t clearanceOffset, bool startsAtHalfHeight)
    {
        const int16_t trackHeight = sequenceClearance.z + (startsAtHalfHeight * kCoordsZStep);
        startsAtHalfHeight = trackHeight % kLandHeightStep == kCoordsZStep;

        int32_t trackClearance = 0;
        if (!startsAtHalfHeight)
        {
            trackClearance = Numerics::ceil2(sequenceClearance.clearanceZ, kLandHeightStep);
        }
        else
        {
            trackClearance = Numerics::ceil2(sequenceClearance.clearanceZ, kCoordsZStep);
            trackClearance += (trackClearance + kCoordsZStep) % kLandHeightStep;
        }

        return trackClearance + (clearanceOffset * kLandHeightStep);
    };

    enum class SequenceTunnelType : uint8_t
    {
        upright,
        inverted,
        invertedFlying,
    };
    constexpr size_t kSequenceTunnelTypeCount = 3;

    struct SequenceTunnelInfo
    {
        int8_t height = 0;
        TunnelSubType type = TunnelSubType::Flat;
    };

    struct SequenceTunnel
    {
        Direction direction = kInvalidDirection;
        std::array<SequenceTunnelInfo, kSequenceTunnelTypeCount> tunnelInfo{};
    };

    constexpr uint8_t kSequenceTunnelMaxPerSequence = 2;

    constexpr SequenceTunnel sequenceTunnelAllTypes(const Direction direction, const int8_t height, const TunnelSubType type)
    {
        const SequenceTunnelInfo tunnelInfo{ height, type };
        return SequenceTunnel{ direction, { tunnelInfo, tunnelInfo, tunnelInfo } };
    }

    using SequenceTunnels = std::array<SequenceTunnel, kSequenceTunnelMaxPerSequence>;

    template<Direction (&directionFunction)(Direction)>
    constexpr SequenceTunnels sequenceTunnelsModify(SequenceTunnels sequenceTunnels)
    {
        for (auto& tunnel : sequenceTunnels)
        {
            tunnel.direction = directionFunction(tunnel.direction);
        }
        return sequenceTunnels;
    }

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
        BlockedSegmentsPerType blockedSegments{ kSegmentsNone, kSegmentsNone, kSegmentsNone };
        int16_t generalSupportHeight = kDoNotSetGeneralSupportHeight;
        SequenceTunnels tunnels{};
        uint8_t reversedTrackSequence = 0;

        constexpr uint8_t getEntranceConnectionSides() const
        {
            return flags.holder & 0xF;
        }
    };

    struct TrackDefinition
    {
        TrackGroup group;
        TrackPitch pitchEnd;
        TrackPitch pitchStart;
        TrackRoll rollEnd;
        TrackRoll rollStart;
        int8_t previewZOffset;
    };

    struct SequenceTable
    {
        uint8_t numSequences{};
        std::array<SequenceDescriptor, kMaxSequencesPerPiece> sequences;
    };

    struct TrackElementDescriptor
    {
        StringId description = kStringIdEmpty;
        TrackCoordinates coordinates;

        // Used to estimate the ride length for number of powered vehicle trains
        uint8_t pieceLength;
        // Piece the ride construction window automatically selects next
        TrackCurveChain curveChain;
        // Track element to build when building "covered"/"splashdown" track
        TrackElemType alternativeType = TrackElemType::none;
        // Price Modifier should be used as in the following calculation:
        // (RideTrackPrice * TED::PriceModifier) / 65536
        uint32_t priceModifier;
        TrackElemType mirrorElement;
        TrackFlags flags;

        TrackDefinition definition;
        SpinFunction spinFunction = SpinFunction::none;

        TrackComputeFunction verticalFactor = EvaluatorConst<0>;
        TrackComputeFunction lateralFactor = EvaluatorConst<0>;

        int8_t reversedRotationOffset = 0;

        SequenceTable sequenceData;
    };
} // namespace OpenRCT2::TrackMetadata
