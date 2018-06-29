/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "Paint.Sprite.h"
#include "../Paint.h"
#include "../../drawing/Drawing.h"
#include "../../world/Sprite.h"
#include "../../ride/RideData.h"
#include "../../interface/Viewport.h"
#include "../../peep/Staff.h"
#include "../../ride/VehiclePaint.h"
#include "../../ride/TrackDesign.h"

/**
 * Paint Quadrant
 *  rct2: 0x0069E8B0
 */
void sprite_paint_setup(paint_session * session, const uint16_t x, const uint16_t y)
{
    if ((x & 0xe000) | (y & 0xe000))
    {
        return;
    }

    if (gTrackDesignSaveMode || (gCurrentViewportFlags & VIEWPORT_FLAG_INVISIBLE_SPRITES))
    {
        return;
    }

    uint16_t sprite_idx = sprite_get_first_in_quadrant(x, y);
    if (sprite_idx == SPRITE_INDEX_NULL)
    {
        return;
    }

    rct_drawpixelinfo* dpi = session->DPI;
    if (dpi->zoom_level > 2)
    {
        return;
    }

    const bool highlightPathIssues = (gCurrentViewportFlags & VIEWPORT_FLAG_HIGHLIGHT_PATH_ISSUES);

    for (const rct_sprite* spr = get_sprite(sprite_idx); sprite_idx != SPRITE_INDEX_NULL; sprite_idx = spr->unknown.next_in_quadrant)
    {
        spr = get_sprite(sprite_idx);

        if (highlightPathIssues)
        {
            if (spr->unknown.sprite_identifier == SPRITE_IDENTIFIER_PEEP)
            {
                rct_peep * peep = (rct_peep*)spr;
                if (!(peep->type == PEEP_TYPE_STAFF && peep->staff_type == STAFF_TYPE_HANDYMAN))
                {
                    continue;
                }
            }
            else if (spr->unknown.sprite_identifier != SPRITE_IDENTIFIER_LITTER)
            {
                continue;
            }
        }

        // Only paint sprites that are below the clip height and inside the clip selection.
        // Here converting from land/path/etc height scale to pixel height scale.
        // Note: peeps/scenery on slopes will be above the base
        // height of the slope element, and consequently clipped.
        if ((gCurrentViewportFlags & VIEWPORT_FLAG_CLIP_VIEW))
        {
            if (spr->unknown.z > (gClipHeight * 8))
            {
                continue;
            }
            if (spr->unknown.x / 32 < gClipSelectionA.x || spr->unknown.x / 32 > gClipSelectionB.x)
            {
                continue;
            }
            if (spr->unknown.y / 32 < gClipSelectionA.y || spr->unknown.y / 32 > gClipSelectionB.y)
            {
                continue;
            }
        }

        dpi = session->DPI;

        if (dpi->y + dpi->height <= spr->unknown.sprite_top || spr->unknown.sprite_bottom <= dpi->y
            || dpi->x + dpi->width <= spr->unknown.sprite_left || spr->unknown.sprite_right <= dpi->x)
        {
            continue;
        }

        int32_t image_direction = session->CurrentRotation;
        image_direction <<= 3;
        image_direction += spr->unknown.sprite_direction;
        image_direction &= 0x1F;

        session->CurrentlyDrawnItem = spr;
        session->SpritePosition.x = spr->unknown.x;
        session->SpritePosition.y = spr->unknown.y;
        session->InteractionType = VIEWPORT_INTERACTION_ITEM_SPRITE;

        switch (spr->unknown.sprite_identifier)
        {
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
