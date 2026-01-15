/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
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
#include "../world/Climate.h"
#include "../world/Map.h"
#include "../world/Park.h"
#include "Paint.h"
#include "vehicle/VehiclePaint.h"

#include <cassert>

using namespace OpenRCT2;
using namespace OpenRCT2::Drawing;

/**
 * Paint Quadrant
 *  rct2: 0x0069E8B0
 */
void EntityPaintSetup(PaintSession& session, const CoordsXY& pos)
{
    PROFILED_FUNCTION();

    if (!MapIsLocationValid(pos))
    {
        return;
    }
    if (gTrackDesignSaveMode || (session.ViewFlags & VIEWPORT_FLAG_HIDE_ENTITIES))
    {
        return;
    }

    if (session.rt.zoom_level > ZoomLevel{ 2 })
    {
        return;
    }

    const bool highlightPathIssues = (session.ViewFlags & VIEWPORT_FLAG_HIGHLIGHT_PATH_ISSUES);

    for (auto* entity : EntityTileList(pos))
    {
        if (highlightPathIssues)
        {
            const auto staff = entity->As<Staff>();
            if (staff != nullptr)
            {
                if (staff->AssignedStaffType != StaffType::handyman)
                {
                    continue;
                }
            }
            else if (entity->Type != EntityType::litter)
            {
                continue;
            }
        }

        const auto entityPos = entity->GetLocation();

        // Only paint sprites that are below the clip height and inside the clip selection.
        // Here converting from land/path/etc height scale to pixel height scale.
        // Note: peeps/scenery on slopes will be above the base
        // height of the slope element, and consequently clipped.
        if (session.ViewFlags & VIEWPORT_FLAG_CLIP_VIEW)
        {
            if (entityPos.z > (gClipHeight * kCoordsZStep))
            {
                // see-through off: don't paint this entity at all
                // see-through on: paint this entity as partial or hidden later on
                if ((session.ViewFlags & VIEWPORT_FLAG_CLIP_VIEW_SEE_THROUGH) == 0)
                {
                    continue;
                }
            }
            if (entityPos.x < gClipSelectionA.x || entityPos.x > (gClipSelectionB.x + kCoordsXYStep - 1))
            {
                continue;
            }
            if (entityPos.y < gClipSelectionA.y || entityPos.y > (gClipSelectionB.y + kCoordsXYStep - 1))
            {
                continue;
            }
        }

        auto screenCoords = Translate3DTo2DWithZ(session.CurrentRotation, entity->GetLocation());
        auto spriteRect = ScreenRect(
            screenCoords - ScreenCoordsXY{ entity->SpriteData.Width, entity->SpriteData.HeightMin },
            screenCoords + ScreenCoordsXY{ entity->SpriteData.Width, entity->SpriteData.HeightMax });

        const ZoomLevel zoom = session.rt.zoom_level;
        if (session.rt.y + session.rt.height <= zoom.ApplyInversedTo(spriteRect.GetTop())
            || zoom.ApplyInversedTo(spriteRect.GetBottom()) <= session.rt.y
            || session.rt.x + session.rt.width <= zoom.ApplyInversedTo(spriteRect.GetLeft())
            || zoom.ApplyInversedTo(spriteRect.GetRight()) <= session.rt.x)
        {
            continue;
        }

        int32_t image_direction = session.CurrentRotation;
        image_direction <<= 3;
        image_direction += entity->Orientation;
        image_direction &= 0x1F;

        session.CurrentlyDrawnEntity = entity;
        session.SpritePosition.x = entityPos.x;
        session.SpritePosition.y = entityPos.y;
        session.InteractionType = ViewportInteractionItem::entity;

        switch (entity->Type)
        {
            case EntityType::vehicle:
                entity->cast<Vehicle>()->Paint(session, image_direction);
                if (LightFx::ForVehiclesIsAvailable())
                {
                    LightFx::AddLightsMagicVehicle(entity->cast<Vehicle>());
                }
                break;
            case EntityType::guest:
            case EntityType::staff:
                entity->cast<Peep>()->Paint(session, image_direction);
                break;
            case EntityType::steamParticle:
                entity->cast<SteamParticle>()->Paint(session, image_direction);
                break;
            case EntityType::moneyEffect:
                entity->cast<MoneyEffect>()->Paint(session, image_direction);
                break;
            case EntityType::crashedVehicleParticle:
                entity->cast<VehicleCrashParticle>()->Paint(session, image_direction);
                break;
            case EntityType::explosionCloud:
                entity->cast<ExplosionCloud>()->Paint(session, image_direction);
                break;
            case EntityType::crashSplash:
                entity->cast<CrashSplashParticle>()->Paint(session, image_direction);
                break;
            case EntityType::explosionFlare:
                entity->cast<ExplosionFlare>()->Paint(session, image_direction);
                break;
            case EntityType::jumpingFountain:
                entity->cast<JumpingFountain>()->Paint(session, image_direction);
                break;
            case EntityType::balloon:
                entity->cast<Balloon>()->Paint(session, image_direction);
                break;
            case EntityType::duck:
                entity->cast<Duck>()->Paint(session, image_direction);
                break;
            case EntityType::litter:
                entity->cast<Litter>()->Paint(session, image_direction);
                break;
            default:
                assert(false);
                break;
        }
    }
}
