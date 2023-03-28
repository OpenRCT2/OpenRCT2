/*****************************************************************************
 * Copyright (c) 2014-2023 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../../paint/Paint.h"
#include "../../world/Location.hpp"
#include "../TrackPaint.h"

#include <cstdint>

struct SpriteBoundBox2
{
    uint32_t sprite_id_a;
    uint32_t sprite_id_b;
    CoordsXYZ offset;
    CoordsXYZ bb_offset;
    CoordsXYZ bb_size;
};

template<bool isClassic> ImageId WoodenRCGetTrackColour(const PaintSession& session)
{
    if (isClassic)
        return session.TrackColours[SCHEME_TRACK];
    else
        return session.TrackColours[SCHEME_TRACK].IsRemap()
            ? session.TrackColours[SCHEME_TRACK]
            : session.TrackColours[SCHEME_TRACK].WithPrimary(session.TrackColours[SCHEME_SUPPORTS].GetPrimary());
}

ImageId WoodenRCGetRailsColour(PaintSession& session);

template<bool isClassic>
PaintStruct* WoodenRCTrackPaint(
    PaintSession& session, uint8_t direction, ImageIndex imageIdTrack, ImageIndex imageIdRails, const CoordsXYZ& offset,
    const BoundBoxXYZ& boundBox)
{
    ImageId imageId = WoodenRCGetTrackColour<isClassic>(session).WithIndex(imageIdTrack);
    ImageId railsImageId = WoodenRCGetRailsColour(session).WithIndex(imageIdRails);

    PaintAddImageAsParentRotated(session, direction, imageId, offset, boundBox);
    return PaintAddImageAsChildRotated(session, direction, railsImageId, offset, boundBox);
}

template<bool isClassic> void WoodenRCTrackPaintBb(PaintSession& session, const SpriteBoundBox2* bb, int16_t height)
{
    if (bb->sprite_id_a == 0)
        return;

    ImageId imageId = WoodenRCGetTrackColour<isClassic>(session).WithIndex(bb->sprite_id_a);
    PaintAddImageAsParent(
        session, imageId, { bb->offset.x, bb->offset.y, height + bb->offset.z },
        { { bb->bb_offset.x, bb->bb_offset.y, height + bb->bb_offset.z }, bb->bb_size });
    if (bb->sprite_id_b != 0)
    {
        ImageId railsImageId = WoodenRCGetRailsColour(session).WithIndex(bb->sprite_id_b);
        PaintAddImageAsChild(
            session, railsImageId, { bb->offset.x, bb->offset.y, height + bb->offset.z },
            { { bb->bb_offset, height + bb->bb_offset.z }, bb->bb_size });
    }
}

TRACK_PAINT_FUNCTION GetTrackPaintFunctionClassicWoodenRCFallback(int32_t trackType);
