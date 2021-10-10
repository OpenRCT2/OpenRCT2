/*****************************************************************************
 * Copyright (c) 2014-2021 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "Paint.Entity.h"

#include "../drawing/Drawing.h"
#include "../drawing/LightFX.h"
#include "../entity/Balloon.h"
#include "../entity/Duck.h"
#include "../entity/EntityList.h"
#include "../entity/Fountain.h"
#include "../entity/Litter.h"
#include "../entity/MoneyEffect.h"
#include "../entity/Particle.h"
#include "../entity/Staff.h"
#include "../interface/Viewport.h"
#include "../profiling/Profiling.h"
#include "../ride/RideData.h"
#include "../ride/TrackDesign.h"
#include "../ride/Vehicle.h"
#include "../ride/VehiclePaint.h"
#include "../world/Climate.h"
#include "../world/MapAnimation.h"
#include "../world/Park.h"
#include "Paint.h"

/**
 * Paint Quadrant
 *  rct2: 0x0069E8B0
 */
void EntityPaintSetup(paint_session& session, const CoordsXY& pos)
{
    PROFILED_FUNCTION();

    if (!map_is_location_valid(pos))
    {
        return;
    }
    if (gTrackDesignSaveMode || (session.ViewFlags & VIEWPORT_FLAG_INVISIBLE_SPRITES))
    {
        return;
    }

    rct_drawpixelinfo* dpi = &session.DPI;
    if (dpi->zoom_level > ZoomLevel{ 2 })
    {
        return;
    }

    const bool highlightPathIssues = (session.ViewFlags & VIEWPORT_FLAG_HIGHLIGHT_PATH_ISSUES);

    for (const auto* spr : EntityTileList(pos))
    {
        if (highlightPathIssues)
        {
            const auto staff = spr->As<Staff>();
            if (staff != nullptr)
            {
                if (staff->AssignedStaffType != StaffType::Handyman)
                {
                    continue;
                }
            }
            else if (spr->Type != EntityType::Litter)
            {
                continue;
            }
        }

        if ((session.ViewFlags & VIEWPORT_FLAG_INVISIBLE_RIDES) && spr->Type == EntityType::Vehicle)
        {
            const auto veh = spr->As<Vehicle>();
            if (veh != nullptr)
            {
                auto ride = get_ride(veh->ride);
                if (ride != nullptr && !ride->ignore_invisible_flag)
                    return;
            }
        }

        const auto entityPos = spr->GetLocation();

        // Only paint sprites that are below the clip height and inside the clip selection.
        // Here converting from land/path/etc height scale to pixel height scale.
        // Note: peeps/scenery on slopes will be above the base
        // height of the slope element, and consequently clipped.
        if ((session.ViewFlags & VIEWPORT_FLAG_CLIP_VIEW))
        {
            if (entityPos.z > (gClipHeight * COORDS_Z_STEP))
            {
                continue;
            }
            if (entityPos.x < gClipSelectionA.x || entityPos.x > gClipSelectionB.x)
            {
                continue;
            }
            if (entityPos.y < gClipSelectionA.y || entityPos.y > gClipSelectionB.y)
            {
                continue;
            }
        }

        dpi = &session.DPI;

        if (dpi->y + dpi->height <= spr->SpriteRect.GetTop() || spr->SpriteRect.GetBottom() <= dpi->y
            || dpi->x + dpi->width <= spr->SpriteRect.GetLeft() || spr->SpriteRect.GetRight() <= dpi->x)
        {
            continue;
        }

        int32_t image_direction = session.CurrentRotation;
        image_direction <<= 3;
        image_direction += spr->sprite_direction;
        image_direction &= 0x1F;

        session.CurrentlyDrawnItem = spr;
        session.SpritePosition.x = entityPos.x;
        session.SpritePosition.y = entityPos.y;
        session.InteractionType = ViewportInteractionItem::Entity;

        switch (spr->Type)
        {
            case EntityType::Vehicle:
                spr->As<Vehicle>()->Paint(session, image_direction);
#ifdef __ENABLE_LIGHTFX__
                if (lightfx_for_vehicles_is_available())
                {
                    lightfx_add_lights_magic_vehicle(spr->As<Vehicle>());
                }
#endif
                break;
            case EntityType::Guest:
            case EntityType::Staff:
                spr->As<Peep>()->Paint(session, image_direction);
                break;
            case EntityType::SteamParticle:
                spr->As<SteamParticle>()->Paint(session, image_direction);
                break;
            case EntityType::MoneyEffect:
                spr->As<MoneyEffect>()->Paint(session, image_direction);
                break;
            case EntityType::CrashedVehicleParticle:
                spr->As<VehicleCrashParticle>()->Paint(session, image_direction);
                break;
            case EntityType::ExplosionCloud:
                spr->As<ExplosionCloud>()->Paint(session, image_direction);
                break;
            case EntityType::CrashSplash:
                spr->As<CrashSplashParticle>()->Paint(session, image_direction);
                break;
            case EntityType::ExplosionFlare:
                spr->As<ExplosionFlare>()->Paint(session, image_direction);
                break;
            case EntityType::JumpingFountain:
                spr->As<JumpingFountain>()->Paint(session, image_direction);
                break;
            case EntityType::Balloon:
                spr->As<Balloon>()->Paint(session, image_direction);
                break;
            case EntityType::Duck:
                spr->As<Duck>()->Paint(session, image_direction);
                break;
            case EntityType::Litter:
                spr->As<Litter>()->Paint(session, image_direction);
                break;
            default:
                assert(false);
                break;
        }
    }
}
