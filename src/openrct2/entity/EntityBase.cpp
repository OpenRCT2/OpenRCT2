/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "EntityBase.h"

#include "../core/DataSerialiser.h"
#include "../interface/Viewport.h"

using namespace OpenRCT2;

// Required for GetEntity to return a default
template<>
bool EntityBase::Is<EntityBase>() const
{
    return true;
}

void EntityBase::Invalidate()
{
    if (x == kLocationNull)
        return;

    ZoomLevel maxZoom{ 0 };
    switch (Type)
    {
        case EntityType::Vehicle:
        case EntityType::Guest:
        case EntityType::Staff:
            maxZoom = ZoomLevel{ 2 };
            break;
        case EntityType::CrashedVehicleParticle:
        case EntityType::JumpingFountain:
            maxZoom = ZoomLevel{ 0 };
            break;
        case EntityType::Duck:
            maxZoom = ZoomLevel{ 1 };
            break;
        case EntityType::SteamParticle:
        case EntityType::MoneyEffect:
        case EntityType::ExplosionCloud:
        case EntityType::CrashSplash:
        case EntityType::ExplosionFlare:
        case EntityType::Balloon:
            maxZoom = ZoomLevel{ 2 };
            break;
        case EntityType::Litter:
            maxZoom = ZoomLevel{ 0 };
            break;
        default:
            break;
    }

    ViewportsInvalidate(GetLocation(), SpriteData.Width, SpriteData.HeightMin, SpriteData.HeightMax, maxZoom);
}

void EntityBase::Serialise(DataSerialiser& stream)
{
    stream << Type;
    stream << Id;
    stream << x;
    stream << y;
    stream << z;
    stream << Orientation;
}

// Exists only for signature
void EntityBase::Paint() const
{
    Guard::Assert(false, "You are not supposed to call this");
}
