/*****************************************************************************
 * Copyright (c) 2014-2023 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "../interface/Viewport.h"
#include "../ride/TrackPaint.h"
#include "Paint.h"

PaintStruct* PaintAddImageAsParentRotated(
    PaintSession& session, const uint8_t direction, const ImageId imageId, const CoordsXYZ& offset, const BoundBoxXYZ& boundBox)
{
    if (direction & 1)
    {
        return PaintAddImageAsParent(
            session, imageId, { offset.y, offset.x, offset.z },
            { { boundBox.offset.y, boundBox.offset.x, boundBox.offset.z },
              { boundBox.length.y, boundBox.length.x, boundBox.length.z } });
    }

    return PaintAddImageAsParent(session, imageId, offset, boundBox);
}

PaintStruct* PaintAddImageAsChildRotated(
    PaintSession& session, const uint8_t direction, const ImageId image_id, const CoordsXYZ& offset,
    const BoundBoxXYZ& boundBox)
{
    if (direction & 1)
    {
        return PaintAddImageAsChild(
            session, image_id, { offset.y, offset.x, offset.z },
            { { boundBox.offset.y, boundBox.offset.x, boundBox.offset.z },
              { boundBox.length.y, boundBox.length.x, boundBox.length.z } });
    }

    return PaintAddImageAsChild(session, image_id, offset, boundBox);
}

void PaintUtilPushTunnelRotated(PaintSession& session, uint8_t direction, uint16_t height, uint8_t type)
{
    if (direction & 1)
    {
        PaintUtilPushTunnelRight(session, height, type);
    }
    else
    {
        PaintUtilPushTunnelLeft(session, height, type);
    }
}
