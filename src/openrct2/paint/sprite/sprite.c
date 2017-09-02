#pragma region Copyright (c) 2014-2017 OpenRCT2 Developers
/*****************************************************************************
 * OpenRCT2, an open source clone of Roller Coaster Tycoon 2.
 *
 * OpenRCT2 is the work of many authors, a full list can be found in contributors.md
 * For more information, visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * A full copy of the GNU General Public License can be found in licence.txt
 *****************************************************************************/
#pragma endregion

#include "sprite.h"
#include "../paint.h"
#include "../../drawing/drawing.h"
#include "../../world/sprite.h"
#include "../../ride/ride_data.h"
#include "../../interface/viewport.h"
#include "../../ride/vehicle_paint.h"

/**
 * Paint Quadrant
 *  rct2: 0x0069E8B0
 */
void sprite_paint_setup(paint_session * session, const uint16 eax, const uint16 ecx)
{
    rct_drawpixelinfo* dpi;

    if ((eax & 0xe000) | (ecx & 0xe000)) return;

    uint16 sprite_idx = sprite_get_first_in_quadrant(eax, ecx);
    if (sprite_idx == SPRITE_INDEX_NULL) return;

    if (gTrackDesignSaveMode) return;

    if (gCurrentViewportFlags & VIEWPORT_FLAG_INVISIBLE_SPRITES) return;

    dpi = session->Unk140E9A8;
    if (dpi->zoom_level > 2) return;


    for (rct_sprite* spr = get_sprite(sprite_idx); sprite_idx != SPRITE_INDEX_NULL; sprite_idx = spr->unknown.next_in_quadrant) {
        spr = get_sprite(sprite_idx);

        // Only paint sprites that are below the clip height.
        // Here converting from land/path/etc height scale to pixel height scale.
        // Note: peeps/scenery on slopes will be above the base
        // height of the slope element, and consequently clipped.
        if ((gCurrentViewportFlags & VIEWPORT_FLAG_PAINT_CLIP_TO_HEIGHT) && (spr->unknown.z > (gClipHeight * 8) )) continue;

        dpi = session->Unk140E9A8;

        if (dpi->y + dpi->height <= spr->unknown.sprite_top) continue;
        if (spr->unknown.sprite_bottom <= dpi->y)continue;
        if (dpi->x + dpi->width <= spr->unknown.sprite_left)continue;
        if (spr->unknown.sprite_right <= dpi->x)continue;

        sint32 image_direction = get_current_rotation();
        image_direction <<= 3;
        image_direction += spr->unknown.sprite_direction;
        image_direction &= 0x1F;

        session->CurrentlyDrawnItem = spr;
        session->SpritePosition.x = spr->unknown.x;
        session->SpritePosition.y = spr->unknown.y;
        session->InteractionType = VIEWPORT_INTERACTION_ITEM_SPRITE;

        switch (spr->unknown.sprite_identifier) {
        case SPRITE_IDENTIFIER_VEHICLE:
            vehicle_paint(session, (rct_vehicle*)spr, image_direction);
            break;
        case SPRITE_IDENTIFIER_PEEP:
            peep_paint(session, (rct_peep*)spr, image_direction);
            break;
        case SPRITE_IDENTIFIER_MISC:
            misc_paint(session, spr, image_direction);
            break;
        case SPRITE_IDENTIFIER_LITTER:
            litter_paint(session, (rct_litter*)spr, image_direction);
            break;
        default:
            assert(false);
            break;
        }
    }
}
