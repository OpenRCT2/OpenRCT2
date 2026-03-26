/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../../paint/track/TrackPaintGeneric.h"

namespace OpenRCT2
{
    static constexpr uint8_t kQuarterHelixSequenceCount = 7;
    static constexpr uint8_t kQuarterHelixSequenceSpriteCount = 2;

    static constexpr bool kLeftQuarterHelixLargeUpSpriteMapArray[kNumOrthogonalDirections][kQuarterHelixSequenceCount]
                                                                [kQuarterHelixSequenceSpriteCount]
        = { { { true, false },
              { false, false },
              { true, false },
              { true, false },
              { false, false },
              { true, false },
              { true, false } },
            { { true, false },
              { false, false },
              { true, false },
              { true, false },
              { false, false },
              { true, false },
              { true, false } },
            { { true, false },
              { false, false },
              { true, false },
              { true, false },
              { false, false },
              { true, false },
              { true, false } },
            { { true, false },
              { false, false },
              { true, false },
              { true, false },
              { false, false },
              { true, false },
              { true, false } } };

    static constexpr bool kLeftQuarterBankedHelixLargeUpSpriteMapArray[kNumOrthogonalDirections][kQuarterHelixSequenceCount]
                                                                      [kQuarterHelixSequenceSpriteCount]
        = { { { true, true },
              { false, false },
              { true, false },
              { true, false },
              { false, false },
              { true, false },
              { true, false } },
            { { false, true },
              { false, false },
              { false, true },
              { false, false },
              { false, false },
              { false, true },
              { false, true } },
            { { true, false },
              { false, false },
              { true, false },
              { true, false },
              { false, false },
              { true, false },
              { false, true } },
            { { true, false },
              { false, false },
              { true, false },
              { true, false },
              { false, false },
              { true, false },
              { true, false } } };

    static constexpr uint64_t kLeftQuarterHelixLargeUpSpriteMap = createSpriteMap<false>(
        kLeftQuarterHelixLargeUpSpriteMapArray);
    static constexpr uint64_t kRightQuarterHelixLargeUpSpriteMap = createSpriteMap<true>(
        kLeftQuarterHelixLargeUpSpriteMapArray);
    static constexpr uint64_t kLeftQuarterBankedHelixLargeUpSpriteMap = createSpriteMap<false>(
        kLeftQuarterBankedHelixLargeUpSpriteMapArray);
    static constexpr uint64_t kRightQuarterBankedHelixLargeUpSpriteMap = createSpriteMap<true>(
        kLeftQuarterBankedHelixLargeUpSpriteMapArray);

    typedef std::array<
        std::array<std::array<BoundBoxXYZ, kQuarterHelixSequenceSpriteCount>, kQuarterHelixSequenceCount>,
        kNumOrthogonalDirections>
        QuarterHelixBoundingBoxes;
    extern const QuarterHelixBoundingBoxes kLeftQuarterHelixLargeUpBoundingBoxes;
    extern const QuarterHelixBoundingBoxes kRightQuarterHelixLargeUpBoundingBoxes;

    extern const std::array<int8_t, kQuarterHelixSequenceCount> kQuarterHelixGeneralSupportHeights;

    void trackPaintLeftQuarterHelixTunnels(
        PaintSession& session, const uint8_t trackSequence, const Direction direction, const int32_t height,
        const TunnelGroup tunnelGroup);
    void trackPaintRightQuarterHelixTunnels(
        PaintSession& session, const uint8_t trackSequence, const Direction direction, const int32_t height,
        const TunnelGroup tunnelGroup);

    template<
        const ImageIndex imageIndex, const uint64_t spriteMap, const bool woodenSupports,
        const std::array<std::array<int8_t, kNumOrthogonalDirections>, kQuarterHelixSequenceCount>& supportHeightExtras,
        const OpenRCT2::BlockedSegmentsType blockedSegmentsType, const TunnelGroup tunnelGroup, const bool trackSupportColours>
    void trackPaintLeftQuarterHelixLargeUp(
        PaintSession& session, const Ride& ride, const uint8_t trackSequence, const Direction direction, const int32_t height,
        const OpenRCT2::TrackElement& trackElement, const SupportType supportType)
    {
        trackPaintGeneric<
            kQuarterHelixSequenceCount, kQuarterHelixSequenceSpriteCount, 1, imageIndex, spriteMap,
            kLeftQuarterHelixLargeUpBoundingBoxes, woodenSupports, supportHeightExtras, blockedSegmentsType, tunnelGroup,
            kQuarterHelixGeneralSupportHeights, trackPaintLeftQuarterHelixTunnels, false, trackSupportColours>(
            session, ride, trackSequence, direction, height, trackElement, supportType);
    }

    template<
        const ImageIndex imageIndex, const uint64_t spriteMap, const bool woodenSupports,
        const std::array<std::array<int8_t, kNumOrthogonalDirections>, kQuarterHelixSequenceCount>& supportHeightExtras,
        const OpenRCT2::BlockedSegmentsType blockedSegmentsType, const TunnelGroup tunnelGroup, const bool trackSupportColours>
    void trackPaintRightQuarterHelixLargeUp(
        PaintSession& session, const Ride& ride, const uint8_t trackSequence, const Direction direction, const int32_t height,
        const OpenRCT2::TrackElement& trackElement, const SupportType supportType)
    {
        trackPaintGeneric<
            kQuarterHelixSequenceCount, kQuarterHelixSequenceSpriteCount, 1, imageIndex, spriteMap,
            kRightQuarterHelixLargeUpBoundingBoxes, woodenSupports, supportHeightExtras, blockedSegmentsType, tunnelGroup,
            kQuarterHelixGeneralSupportHeights, trackPaintRightQuarterHelixTunnels, false, trackSupportColours>(
            session, ride, trackSequence, direction, height, trackElement, supportType);
    }

    template<
        const ImageIndex imageIndex, const uint64_t spriteMap, const bool woodenSupports,
        const std::array<std::array<int8_t, kNumOrthogonalDirections>, kQuarterHelixSequenceCount>& supportHeightExtras,
        const OpenRCT2::BlockedSegmentsType blockedSegmentsType, const TunnelGroup tunnelGroup, const bool trackSupportColours>
    void trackPaintLeftQuarterHelixLargeDown(
        PaintSession& session, const Ride& ride, const uint8_t trackSequence, const Direction direction, const int32_t height,
        const OpenRCT2::TrackElement& trackElement, const SupportType supportType)
    {
        trackPaintGeneric<
            kQuarterHelixSequenceCount, kQuarterHelixSequenceSpriteCount, 1, imageIndex, spriteMap,
            kRightQuarterHelixLargeUpBoundingBoxes, woodenSupports, supportHeightExtras, blockedSegmentsType, tunnelGroup,
            kQuarterHelixGeneralSupportHeights, trackPaintRightQuarterHelixTunnels, true, trackSupportColours>(
            session, ride, trackSequence, direction, height, trackElement, supportType);
    }

    template<
        const ImageIndex imageIndex, const uint64_t spriteMap, const bool woodenSupports,
        const std::array<std::array<int8_t, kNumOrthogonalDirections>, kQuarterHelixSequenceCount>& supportHeightExtras,
        const OpenRCT2::BlockedSegmentsType blockedSegmentsType, const TunnelGroup tunnelGroup, const bool trackSupportColours>
    void trackPaintRightQuarterHelixLargeDown(
        PaintSession& session, const Ride& ride, const uint8_t trackSequence, const Direction direction, const int32_t height,
        const OpenRCT2::TrackElement& trackElement, const SupportType supportType)
    {
        trackPaintGeneric<
            kQuarterHelixSequenceCount, kQuarterHelixSequenceSpriteCount, 1, imageIndex, spriteMap,
            kLeftQuarterHelixLargeUpBoundingBoxes, woodenSupports, supportHeightExtras, blockedSegmentsType, tunnelGroup,
            kQuarterHelixGeneralSupportHeights, trackPaintLeftQuarterHelixTunnels, true, trackSupportColours>(
            session, ride, trackSequence, direction, height, trackElement, supportType);
    }

    template<
        const ImageIndex imageIndex, const uint64_t spriteMap, const bool woodenSupports,
        const std::array<std::array<int8_t, kNumOrthogonalDirections>, kQuarterHelixSequenceCount>& supportHeightExtras,
        const OpenRCT2::BlockedSegmentsType blockedSegmentsType, const TunnelGroup tunnelGroup, const bool trackSupportColours>
    void trackPaintLeftQuarterBankedHelixLargeUp(
        PaintSession& session, const Ride& ride, const uint8_t trackSequence, const Direction direction, const int32_t height,
        const OpenRCT2::TrackElement& trackElement, const SupportType supportType)
    {
        trackPaintGeneric<
            kQuarterHelixSequenceCount, kQuarterHelixSequenceSpriteCount, 2, imageIndex, spriteMap,
            kLeftQuarterHelixLargeUpBoundingBoxes, woodenSupports, supportHeightExtras, blockedSegmentsType, tunnelGroup,
            kQuarterHelixGeneralSupportHeights, trackPaintLeftQuarterHelixTunnels, false, trackSupportColours>(
            session, ride, trackSequence, direction, height, trackElement, supportType);
    }

    template<
        const ImageIndex imageIndex, const uint64_t spriteMap, const bool woodenSupports,
        const std::array<std::array<int8_t, kNumOrthogonalDirections>, kQuarterHelixSequenceCount>& supportHeightExtras,
        const OpenRCT2::BlockedSegmentsType blockedSegmentsType, const TunnelGroup tunnelGroup, const bool trackSupportColours>
    void trackPaintRightQuarterBankedHelixLargeUp(
        PaintSession& session, const Ride& ride, const uint8_t trackSequence, const Direction direction, const int32_t height,
        const OpenRCT2::TrackElement& trackElement, const SupportType supportType)
    {
        trackPaintGeneric<
            kQuarterHelixSequenceCount, kQuarterHelixSequenceSpriteCount, 2, imageIndex, spriteMap,
            kRightQuarterHelixLargeUpBoundingBoxes, woodenSupports, supportHeightExtras, blockedSegmentsType, tunnelGroup,
            kQuarterHelixGeneralSupportHeights, trackPaintRightQuarterHelixTunnels, false, trackSupportColours>(
            session, ride, trackSequence, direction, height, trackElement, supportType);
    }

    template<
        const ImageIndex imageIndex, const uint64_t spriteMap, const bool woodenSupports,
        const std::array<std::array<int8_t, kNumOrthogonalDirections>, kQuarterHelixSequenceCount>& supportHeightExtras,
        const OpenRCT2::BlockedSegmentsType blockedSegmentsType, const TunnelGroup tunnelGroup, const bool trackSupportColours>
    void trackPaintLeftQuarterBankedHelixLargeDown(
        PaintSession& session, const Ride& ride, const uint8_t trackSequence, const Direction direction, const int32_t height,
        const OpenRCT2::TrackElement& trackElement, const SupportType supportType)
    {
        trackPaintGeneric<
            kQuarterHelixSequenceCount, kQuarterHelixSequenceSpriteCount, 2, imageIndex, spriteMap,
            kRightQuarterHelixLargeUpBoundingBoxes, woodenSupports, supportHeightExtras, blockedSegmentsType, tunnelGroup,
            kQuarterHelixGeneralSupportHeights, trackPaintRightQuarterHelixTunnels, true, trackSupportColours>(
            session, ride, trackSequence, direction, height, trackElement, supportType);
    }

    template<
        const ImageIndex imageIndex, const uint64_t spriteMap, const bool woodenSupports,
        const std::array<std::array<int8_t, kNumOrthogonalDirections>, kQuarterHelixSequenceCount>& supportHeightExtras,
        const OpenRCT2::BlockedSegmentsType blockedSegmentsType, const TunnelGroup tunnelGroup, const bool trackSupportColours>
    void trackPaintRightQuarterBankedHelixLargeDown(
        PaintSession& session, const Ride& ride, const uint8_t trackSequence, const Direction direction, const int32_t height,
        const OpenRCT2::TrackElement& trackElement, const SupportType supportType)
    {
        trackPaintGeneric<
            kQuarterHelixSequenceCount, kQuarterHelixSequenceSpriteCount, 2, imageIndex, spriteMap,
            kLeftQuarterHelixLargeUpBoundingBoxes, woodenSupports, supportHeightExtras, blockedSegmentsType, tunnelGroup,
            kQuarterHelixGeneralSupportHeights, trackPaintLeftQuarterHelixTunnels, true, trackSupportColours>(
            session, ride, trackSequence, direction, height, trackElement, supportType);
    }
} // namespace OpenRCT2
