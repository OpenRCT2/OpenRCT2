/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "TrackPaintGeneric.h"

namespace OpenRCT2
{
    ImageId getPrimaryTrackColourWithSecondarySupportColour(const PaintSession& session)
    {
        if (session.TrackColours == ConstructionMarker)
            return session.TrackColours;
        else
            return session.TrackColours.WithSecondary(session.SupportColours.GetPrimary());
    }

    void paintTrackSprite(
        PaintSession& session, const Direction direction, const uint8_t trackSequenceCount, const uint8_t trackSequence,
        const uint8_t spriteCount, const uint8_t spriteIndex, const uint64_t spriteMap, const ImageId imageId,
        const int32_t height, const BoundBoxXYZ* const boundingBoxes)
    {
        const uint64_t sequenceIndex = (direction * trackSequenceCount * spriteCount) + (trackSequence * spriteCount)
            + spriteIndex;
        const uint64_t sequenceIndexBit = 1uLL << sequenceIndex;
        if (spriteMap & sequenceIndexBit)
        {
            const ImageIndex imageIndexOffset = std::popcount(spriteMap & (sequenceIndexBit - 1));
            PaintAddImageAsParentHeight(
                session, imageId.WithIndexOffset(imageIndexOffset), height, { 0, 0, 0 }, boundingBoxes[sequenceIndex]);
        }
    }
} // namespace OpenRCT2
