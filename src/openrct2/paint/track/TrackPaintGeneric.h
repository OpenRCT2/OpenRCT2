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

struct Ride;

namespace OpenRCT2
{
    ImageId getPrimaryTrackColourWithSecondarySupportColour(const PaintSession& session);

    template<const bool flipXAxis, const size_t sequenceCount, const size_t spriteCount>
    constexpr uint64_t createSpriteMap(const bool array[kNumOrthogonalDirections][sequenceCount][spriteCount])
    {
        const size_t directionOffset = sequenceCount * spriteCount;
        uint64_t spriteMap = 0;
        for (size_t direction = 0; direction < kNumOrthogonalDirections; direction++)
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

    template<
        const size_t sequenceCount, const size_t mapSpriteCount, const size_t sequenceSpriteCount, const ImageIndex imageIndex,
        const uint64_t spriteMap,
        const std::array<std::array<std::array<BoundBoxXYZ, mapSpriteCount>, sequenceCount>, kNumOrthogonalDirections>&
            boundingBoxes,
        const bool woodenSupports,
        const std::array<std::array<int8_t, kNumOrthogonalDirections>, sequenceCount>& supportHeightExtras,
        const OpenRCT2::BlockedSegmentsType blockedSegmentsType, const TunnelGroup tunnelGroup,
        const int16_t trackClearanceHeight, const auto tunnelPaintFunction, const bool down, const bool trackSupportColours>
    void trackPaintGeneric(
        PaintSession& session, const Ride& ride, const uint8_t trackSequence, const Direction direction, const int32_t height,
        const OpenRCT2::TrackElement& trackElement, const SupportType supportType)
    {
        const auto& ted = OpenRCT2::TrackMetadata::GetTrackElementDescriptor(trackElement.GetTrackType());
        const auto& sequenceDescriptor = ted.sequenceData.sequences[trackSequence];

        uint8_t modifiedTrackSequence = trackSequence;
        Direction modifiedDirection = direction;
        if constexpr (down)
        {
            modifiedTrackSequence = sequenceDescriptor.reversedTrackSequence;
            modifiedDirection = (direction + ted.reversedRotationOffset) & 3;
        }

        ImageId imageId = trackSupportColours ? getPrimaryTrackColourWithSecondarySupportColour(session) : session.TrackColours;
        imageId = imageId.WithIndex(imageIndex);
        if constexpr (sequenceSpriteCount >= 1)
        {
            paintTrackSprite(
                session, modifiedDirection, sequenceCount, modifiedTrackSequence, mapSpriteCount, 0, spriteMap, imageId, height,
                &boundingBoxes[0][0][0]);
        }
        if constexpr (sequenceSpriteCount >= 2)
        {
            paintTrackSprite(
                session, modifiedDirection, sequenceCount, modifiedTrackSequence, mapSpriteCount, 1, spriteMap, imageId, height,
                &boundingBoxes[0][0][0]);
        }

        if constexpr (woodenSupports)
        {
            if (sequenceDescriptor.woodenSupports.subType != WoodenSupportSubType::null)
            {
                const Direction supportRotation = (direction + sequenceDescriptor.extraSupportRotation) & 3;
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
                const Direction supportRotation = (direction + sequenceDescriptor.extraSupportRotation) & 3;
                MetalASupportsPaintSetupRotated(
                    session, supportType.metal, sequenceDescriptor.metalSupports.place, supportRotation,
                    supportHeightExtras[modifiedTrackSequence][modifiedDirection],
                    height + sequenceDescriptor.metalSupports.height, session.SupportColours);
            }
        }

        const auto segments = sequenceDescriptor.blockedSegments[EnumValue(blockedSegmentsType)];
        PaintUtilSetSegmentSupportHeight(session, paintSegmentsRotate(segments, direction), 0xFFFF, 0);

        constexpr int16_t trackClearanceHeightCeil = Numerics::ceil2(trackClearanceHeight, kLandHeightStep);
        const auto generalSupportHeight = height + trackClearanceHeightCeil + sequenceDescriptor.generalSupportHeight;
        const bool setGeneralSupportHeight = sequenceDescriptor.generalSupportHeight
            != TrackMetadata::kDoNotSetGeneralSupportHeight;
        PaintUtilSetGeneralSupportHeight(session, generalSupportHeight * setGeneralSupportHeight);

        tunnelPaintFunction(session, modifiedTrackSequence, modifiedDirection, height, tunnelGroup);
    }
} // namespace OpenRCT2
