/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "../drawing/Drawing.h"
#include "../interface/Viewport.h"
#include "../ride/TrackPaint.h"
#include "Paint.h"

PaintStruct* paintAddImageAsParentRotated(
    PaintSession& session, const uint8_t direction, const ImageId imageId, const CoordsXYZ& offset, const BoundBoxXYZ& boundBox)
{
    if (direction & 1)
    {
        return paintAddImageAsParent(
            session, imageId, { offset.y, offset.x, offset.z },
            { { boundBox.offset.y, boundBox.offset.x, boundBox.offset.z },
              { boundBox.length.y, boundBox.length.x, boundBox.length.z } });
    }

    return paintAddImageAsParent(session, imageId, offset, boundBox);
}

PaintStruct* paintAddImageAsChildRotated(
    PaintSession& session, const uint8_t direction, const ImageId imageId, const CoordsXYZ& offset,
    const BoundBoxXYZ& boundBox)
{
    if (direction & 1)
    {
        return paintAddImageAsChild(
            session, imageId, { offset.y, offset.x, offset.z },
            { { boundBox.offset.y, boundBox.offset.x, boundBox.offset.z },
              { boundBox.length.y, boundBox.length.x, boundBox.length.z } });
    }

    return paintAddImageAsChild(session, imageId, offset, boundBox);
}
