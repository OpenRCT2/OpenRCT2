/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "Paint.Guest.h"

#include "../../drawing/ImageId.hpp"
#include "../../entity/Guest.h"
#include "../../peep/PeepSpriteIds.h"
#include "../../profiling/Profiling.h"
#include "../../profiling/ProfilingMacros.hpp"
#include "../Paint.h"
#include "Paint.Peep.h"

using namespace OpenRCT2;

void PaintGuest(PaintSession& session, const Guest& guest, int32_t orientation)
{
    PROFILED_FUNCTION();

    if (session.rt.zoom_level > ZoomLevel{ 2 })
    {
        return;
    }

    Direction direction = (orientation >> 3);

    auto baseImageData = PaintPeepGetBaseImageAndOffset(guest, direction);
    auto imageId = ImageId(baseImageData.baseImageId, guest.TshirtColour, guest.TrousersColour);

    // In the following 4 calls to PaintAddImageAsParent/PaintAddImageAsChild, we add 5 (instead of 3) to the
    // bound_box_offset_z to make sure peeps are drawn on top of railways
    auto bb = kPaintPeepBoundBox(guest.z);
    PaintAddImageAsParent(session, imageId, { 0, 0, guest.z }, bb);

    PaintGuestAccesories(session, guest, baseImageData.offset, baseImageData.actionAnimationGroup, direction);
}

void PaintGuestAccesories(
    PaintSession& session, const Guest& guest, uint8_t imageOffset, PeepAnimationType actionAnimationGroup, Direction direction)
{
    // Can't display any accessories whilst drowning or clapping
    if (guest.Action == PeepActionType::drowning || guest.Action == PeepActionType::clap)
        return;

    // There are only 6 walking frames available for each item,
    // as well as 1 sprite for sitting and 1 for standing still.
    auto itemFrame = imageOffset % 6;
    if (actionAnimationGroup == PeepAnimationType::watchRide)
        itemFrame = 6;
    else if (actionAnimationGroup == PeepAnimationType::sittingIdle)
        itemFrame = 7;

    if (guest.AnimationGroup == PeepAnimationGroup::hat)
    {
        auto itemOffset = kPeepSpriteHatItemStart;
        auto imageId = ImageId(itemOffset + direction + itemFrame * 4, guest.HatColour);
        PaintAddImageAsChild(session, imageId, kPaintPeepOffset(guest.z), kPaintPeepBoundBox(guest.z));
    }
    else if (guest.AnimationGroup == PeepAnimationGroup::balloon)
    {
        auto itemOffset = kPeepSpriteBalloonItemStart;
        auto imageId = ImageId(itemOffset + direction + itemFrame * 4, guest.BalloonColour);
        PaintAddImageAsChild(session, imageId, kPaintPeepOffset(guest.z), kPaintPeepBoundBox(guest.z));
    }
    else if (guest.AnimationGroup == PeepAnimationGroup::umbrella)
    {
        auto itemOffset = kPeepSpriteUmbrellaItemStart;
        auto imageId = ImageId(itemOffset + direction + itemFrame * 4, guest.UmbrellaColour);
        PaintAddImageAsChild(session, imageId, kPaintPeepOffset(guest.z), kPaintPeepBoundBox(guest.z));
    }
}
