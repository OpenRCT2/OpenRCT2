/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
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
#include "../world/MapAnimation.h"
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

    if (session.DPI.zoom_level > ZoomLevel{ 2 })
    {
        return;
    }

    const bool highlightPathIssues = (session.ViewFlags & VIEWPORT_FLAG_HIGHLIGHT_PATH_ISSUES);

    for (auto* spr : EntityTileList(pos))
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

        const auto entityPos = spr->GetLocation();

        // Only paint sprites that are below the clip height and inside the clip selection.
        // Here converting from land/path/etc height scale to pixel height scale.
        // Note: peeps/scenery on slopes will be above the base
        // height of the slope element, and consequently clipped.
        if ((session.ViewFlags & VIEWPORT_FLAG_CLIP_VIEW))
        {
            if (entityPos.z > (gClipHeight * kCoordsZStep))
            {
                continue;
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

        auto screenCoords = Translate3DTo2DWithZ(session.CurrentRotation, spr->GetLocation());
        auto spriteRect = ScreenRect(
            screenCoords - ScreenCoordsXY{ spr->SpriteData.Width, spr->SpriteData.HeightMin },
            screenCoords + ScreenCoordsXY{ spr->SpriteData.Width, spr->SpriteData.HeightMax });

        const ZoomLevel zoom = session.DPI.zoom_level;
        if (session.DPI.y + session.DPI.height <= zoom.ApplyInversedTo(spriteRect.GetTop())
            || zoom.ApplyInversedTo(spriteRect.GetBottom()) <= session.DPI.y
            || session.DPI.x + session.DPI.width <= zoom.ApplyInversedTo(spriteRect.GetLeft())
            || zoom.ApplyInversedTo(spriteRect.GetRight()) <= session.DPI.x)
        {
            continue;
        }

        int32_t image_direction = session.CurrentRotation;
        image_direction <<= 3;
        image_direction += spr->Orientation;
        image_direction &= 0x1F;

        session.CurrentlyDrawnEntity = spr;
        session.SpritePosition.x = entityPos.x;
        session.SpritePosition.y = entityPos.y;
        session.InteractionType = ViewportInteractionItem::Entity;

        switch (spr->Type)
        {
            case EntityType::Vehicle:
                spr->As<Vehicle>()->Paint(session, image_direction);
                if (LightFx::ForVehiclesIsAvailable())
                {
                    LightFx::AddLightsMagicVehicle(spr->As<Vehicle>());
                }
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
