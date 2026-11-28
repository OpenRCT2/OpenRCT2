/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../../ride/TrackData.h"
#include "../../ride/ted/TrackElementDescriptor.h"
#include "../../world/tile_element/TrackElement.h"
#include "../Paint.h"
#include "../tile_element/Paint.TileElement.h"
#include "Support.h"

namespace OpenRCT2
{
    struct Ride;

    ImageId getPrimaryTrackColourWithSecondarySupportColour(const PaintSession& session);

    template<const bool flipXAxis, const size_t sequenceCount, const size_t spriteCount>
    constexpr uint64_t createSpriteMap(const bool array[kNumOrthogonalDirections][sequenceCount][spriteCount])
    {
        const size_t directionOffset = sequenceCount * spriteCount;
        uint64_t spriteMap = 0;
        for (Direction direction : kAllDirections)
        {
            for (size_t sequence = 0; sequence < sequenceCount; sequence++)
            {
                for (size_t sprite = 0; sprite < spriteCount; sprite++)
                {
                    if (array[direction][sequence][sprite] == true)
                    {
                        const size_t directionIndex = flipXAxis ? kNumOrthogonalDirections - 1 - direction : direction;
                        const uint64_t shift = sprite + (sequence * spriteCount) + (directionIndex * directionOffset);
                        const uint64_t bit = 0b1LL << shift;
                        spriteMap |= bit;
                    }
                }
            }
        }
        return spriteMap;
    }

    void paintTrackSprite(
        PaintSession& session, const Direction direction, const uint8_t trackSequenceCount, const uint8_t trackSequence,
        const uint8_t spriteCount, const uint8_t spriteIndex, const uint64_t spriteMap, const ImageId imageId,
        const int32_t height, const BoundBoxXYZ* const boundingBoxes);

    void paintTrackTunnel(
        PaintSession& session, const Direction direction, const int32_t height, const TrackMetadata::SequenceTunnel& tunnel,
        const TrackMetadata::SequenceTunnelType tunnelType, const TunnelGroup tunnelGroup);

    struct TrackPaintFunctionConfig
    {
        uint8_t spriteCount{};
        uint8_t tunnelCount{};
        bool woodenSupports{};
        bool reversed{};
        bool trackSupportColours{};
    };

    struct TrackPaintTrackArgs
    {
        BlockedSegmentsType blockedSegmentsType{};
        TunnelGroup tunnelGroup{};
        TrackMetadata::SequenceTunnelType tunnelType{};
        uint8_t clearanceHeight{};
    };

    struct TrackPaintElementArgs
    {
        uint64_t spriteMap{};
        const BoundBoxXYZ* boundingBoxes{};
        ImageIndex imageIndex{};
        uint8_t mapSpriteCount{};
        std::array<std::array<int8_t, kNumOrthogonalDirections>, TrackMetadata::kMaxSequencesPerPiece> supportHeights{};
    };

    template<
        const TrackPaintFunctionConfig config, const TrackPaintTrackArgs trackArgs, const TrackPaintElementArgs elementArgs>
    void trackPaintGeneric(
        PaintSession& session, const Ride& ride, const uint8_t trackSequence, const Direction direction, const int32_t height,
        const OpenRCT2::TrackElement& trackElement, const SupportType supportType)
    {
        static_assert(config.tunnelCount <= TrackMetadata::kSequenceTunnelMaxPerSequence);

        const auto& ted = OpenRCT2::TrackMetadata::GetTrackElementDescriptor(trackElement.getTrackType());
        const auto& sequenceDescriptor = ted.sequenceData.sequences[trackSequence];

        uint8_t modifiedTrackSequence = trackSequence;
        Direction modifiedDirection = direction;
        if constexpr (config.reversed)
        {
            modifiedTrackSequence = sequenceDescriptor.reversedTrackSequence;
            modifiedDirection = DirectionRotate(direction, ted.reversedRotationOffset);
        }

        ImageId imageId;
        if constexpr (config.trackSupportColours)
            imageId = getPrimaryTrackColourWithSecondarySupportColour(session);
        else
            imageId = session.TrackColours;
        imageId = imageId.WithIndex(elementArgs.imageIndex);

        if constexpr (config.spriteCount >= 1)
        {
            paintTrackSprite(
                session, modifiedDirection, ted.sequenceData.numSequences, modifiedTrackSequence, elementArgs.mapSpriteCount, 0,
                elementArgs.spriteMap, imageId, height, elementArgs.boundingBoxes);
        }
        if constexpr (config.spriteCount >= 2)
        {
            paintTrackSprite(
                session, modifiedDirection, ted.sequenceData.numSequences, modifiedTrackSequence, elementArgs.mapSpriteCount, 1,
                elementArgs.spriteMap, imageId, height, elementArgs.boundingBoxes);
        }

        if constexpr (config.woodenSupports)
        {
            if (sequenceDescriptor.woodenSupports.subType != WoodenSupportSubType::null)
            {
                const Direction supportRotation = DirectionRotate(direction, sequenceDescriptor.extraSupportRotation);
                WoodenASupportsPaintSetupRotated(
                    session, supportType.wooden, sequenceDescriptor.woodenSupports.subType, supportRotation,
                    height + sequenceDescriptor.woodenSupports.height, session.SupportColours,
                    sequenceDescriptor.woodenSupports.transitionType);
            }
        }
        else
        {
            if (sequenceDescriptor.metalSupports.place != MetalSupportPlace::none)
            {
                const Direction supportRotation = DirectionRotate(direction, sequenceDescriptor.extraSupportRotation);
                MetalASupportsPaintSetupRotated(
                    session, supportType.metal, sequenceDescriptor.metalSupports.place, supportRotation,
                    elementArgs.supportHeights[modifiedTrackSequence][modifiedDirection],
                    height + sequenceDescriptor.metalSupports.height, session.SupportColours);
            }
        }

        const auto segments = sequenceDescriptor.blockedSegments[EnumValue(trackArgs.blockedSegmentsType)];
        PaintUtilSetSegmentSupportHeight(session, paintSegmentsRotate(segments, direction), 0xFFFF, 0);

        constexpr int16_t trackClearanceHeightCeil = Numerics::ceil2(trackArgs.clearanceHeight, kLandHeightStep);
        const auto generalSupportHeight = height + trackClearanceHeightCeil + sequenceDescriptor.generalSupportHeight;
        const bool setGeneralSupportHeight = sequenceDescriptor.generalSupportHeight
            != TrackMetadata::kDoNotSetGeneralSupportHeight;
        PaintUtilSetGeneralSupportHeight(session, generalSupportHeight * setGeneralSupportHeight);

        if constexpr (config.tunnelCount >= 1)
        {
            paintTrackTunnel(
                session, direction, height, sequenceDescriptor.tunnels[0], trackArgs.tunnelType, trackArgs.tunnelGroup);
        }
        if constexpr (config.tunnelCount >= 2)
        {
            paintTrackTunnel(
                session, direction, height, sequenceDescriptor.tunnels[1], trackArgs.tunnelType, trackArgs.tunnelGroup);
        }
    }

    constexpr auto kTPFC1Sprite1TunnelMetalSupportsA = TrackPaintFunctionConfig{
        .spriteCount = 1,
        .tunnelCount = 1,
    };

    constexpr auto kTPFC1Sprite1TunnelMetalSupportsAReversed = TrackPaintFunctionConfig{
        .spriteCount = 1,
        .tunnelCount = 1,
        .reversed = true,
    };

    constexpr auto kTPFC2Sprites1TunnelMetalSupportsA = TrackPaintFunctionConfig{
        .spriteCount = 2,
        .tunnelCount = 1,
    };

    constexpr auto kTPFC2Sprites1TunnelMetalSupportsAReversed = TrackPaintFunctionConfig{
        .spriteCount = 2,
        .tunnelCount = 1,
        .reversed = true,
    };
} // namespace OpenRCT2
