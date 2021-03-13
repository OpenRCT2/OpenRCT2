/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "Paint.Sprite.h"

#include "../../drawing/Drawing.h"
#include "../../drawing/LightFX.h"
#include "../../interface/Viewport.h"
#include "../../peep/Staff.h"
#include "../../ride/RideData.h"
#include "../../ride/TrackDesign.h"
#include "../../ride/VehiclePaint.h"
#include "../../world/EntityList.h"
#include "../../world/Sprite.h"
#include "../Paint.h"

/**
 * Paint Quadrant
 *  rct2: 0x0069E8B0
 */
void sprite_paint_setup(paint_session* session, const uint16_t x, const uint16_t y)
{
    if ((x & 0xe000) | (y & 0xe000))
    {
        return;
    }

    if (gTrackDesignSaveMode || (session->ViewFlags & VIEWPORT_FLAG_INVISIBLE_SPRITES))
    {
        return;
    }

    rct_drawpixelinfo* dpi = &session->DPI;
    if (dpi->zoom_level > 2)
    {
        return;
    }

    const bool highlightPathIssues = (session->ViewFlags & VIEWPORT_FLAG_HIGHLIGHT_PATH_ISSUES);

    for (const auto* spr : EntityTileList({ x, y }))
    {
        if (highlightPathIssues)
        {
            const auto peep = spr->As<Peep>();
            if (peep != nullptr)
            {
                if (!(peep->AssignedPeepType == PeepType::Staff && peep->AssignedStaffType == StaffType::Handyman))
                {
                    continue;
                }
            }
            else if (spr->sprite_identifier != SpriteIdentifier::Litter)
            {
                continue;
            }
        }

        // Only paint sprites that are below the clip height and inside the clip selection.
        // Here converting from land/path/etc height scale to pixel height scale.
        // Note: peeps/scenery on slopes will be above the base
        // height of the slope element, and consequently clipped.
        if ((session->ViewFlags & VIEWPORT_FLAG_CLIP_VIEW))
        {
            if (spr->z > (gClipHeight * COORDS_Z_STEP))
            {
                continue;
            }
            if (spr->x < gClipSelectionA.x || spr->x > gClipSelectionB.x)
            {
                continue;
            }
            if (spr->y < gClipSelectionA.y || spr->y > gClipSelectionB.y)
            {
                continue;
            }
        }

        dpi = &session->DPI;

        if (dpi->y + dpi->height <= spr->sprite_top || spr->sprite_bottom <= dpi->y || dpi->x + dpi->width <= spr->sprite_left
            || spr->sprite_right <= dpi->x)
        {
            continue;
        }

        int32_t image_direction = session->CurrentRotation;
        image_direction <<= 3;
        image_direction += spr->sprite_direction;
        image_direction &= 0x1F;

        session->CurrentlyDrawnItem = spr;
        session->SpritePosition.x = spr->x;
        session->SpritePosition.y = spr->y;
        session->InteractionType = ViewportInteractionItem::Entity;

        switch (spr->sprite_identifier)
        {
            case SpriteIdentifier::Vehicle:
                vehicle_paint(session, spr->As<Vehicle>(), image_direction);
#ifdef __ENABLE_LIGHTFX__
                if (lightfx_for_vehicles_is_available())
                {
                    lightfx_add_lights_magic_vehicle(spr->As<Vehicle>());
                }
#endif
                break;
            case SpriteIdentifier::Peep:
                peep_paint(session, spr->As<Peep>(), image_direction);
                break;
            case SpriteIdentifier::Misc:
                // TODO: Update misc_paint to take a specific sprite type
                misc_paint(session, spr->As<MiscEntity>(), image_direction);
                break;
            case SpriteIdentifier::Litter:
                litter_paint(session, spr->As<Litter>(), image_direction);
                break;
            default:
                assert(false);
                break;
        }
    }
}
