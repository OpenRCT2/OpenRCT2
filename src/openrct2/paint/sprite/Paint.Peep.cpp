/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "../../config/Config.h"
#include "../../drawing/LightFX.h"
#include "../../entity/Guest.h"
#include "../../interface/Viewport.h"
#include "../Paint.h"
#include "Paint.Sprite.h"

/**
 *
 *  rct2: 0x0068F0FB
 */
template<> void PaintEntity(paint_session* session, const Peep* peep, int32_t imageDirection)
{
#ifdef __ENABLE_LIGHTFX__
    if (lightfx_is_available())
    {
        if (peep->Is<Staff>())
        {
            auto loc = peep->GetLocation();
            switch (peep->sprite_direction)
            {
                case 0:
                    loc.x -= 10;
                    break;
                case 8:
                    loc.y += 10;
                    break;
                case 16:
                    loc.x += 10;
                    break;
                case 24:
                    loc.y -= 10;
                    break;
                default:
                    return;
            }

            LightfxAdd3DLight(*peep, 0, loc, LightType::Spot1);
        }
    }
#endif

    rct_drawpixelinfo* dpi = &session->DPI;
    if (dpi->zoom_level > 2)
    {
        return;
    }

    if (session->ViewFlags & VIEWPORT_FLAG_INVISIBLE_PEEPS)
    {
        return;
    }

    PeepActionSpriteType actionSpriteType = peep->ActionSpriteType;
    uint8_t imageOffset = peep->ActionSpriteImageOffset;

    if (peep->Action == PeepActionType::Idle)
    {
        actionSpriteType = peep->NextActionSpriteType;
        imageOffset = 0;
    }

    // In the following 4 calls to PaintAddImageAsParent/PaintAddImageAsChild, we add 5 (instead of 3) to the
    //  bound_box_offset_z to make sure peeps are drawn on top of railways
    uint32_t baseImageId = (imageDirection >> 3) + GetPeepAnimation(peep->SpriteType, actionSpriteType).base_image
        + imageOffset * 4;
    uint32_t imageId = baseImageId | peep->TshirtColour << 19 | peep->TrousersColour << 24 | IMAGE_TYPE_REMAP
        | IMAGE_TYPE_REMAP_2_PLUS;
    PaintAddImageAsParent(session, imageId, { 0, 0, peep->z }, { 1, 1, 11 }, { 0, 0, peep->z + 5 });
    auto* guest = peep->As<Guest>();
    if (guest != nullptr)
    {
        if (baseImageId >= 10717 && baseImageId < 10749)
        {
            imageId = (baseImageId + 32) | guest->HatColour << 19 | IMAGE_TYPE_REMAP;
            PaintAddImageAsChild(session, imageId, { 0, 0, peep->z }, { 1, 1, 11 }, { 0, 0, peep->z + 5 });
            return;
        }

        if (baseImageId >= 10781 && baseImageId < 10813)
        {
            imageId = (baseImageId + 32) | guest->BalloonColour << 19 | IMAGE_TYPE_REMAP;
            PaintAddImageAsChild(session, imageId, { 0, 0, peep->z }, { 1, 1, 11 }, { 0, 0, peep->z + 5 });
            return;
        }

        if (baseImageId >= 11197 && baseImageId < 11229)
        {
            imageId = (baseImageId + 32) | guest->UmbrellaColour << 19 | IMAGE_TYPE_REMAP;
            PaintAddImageAsChild(session, imageId, { 0, 0, peep->z }, { 1, 1, 11 }, { 0, 0, peep->z + 5 });
            return;
        }
    }
}
