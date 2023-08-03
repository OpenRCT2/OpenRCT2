/*****************************************************************************
 * Copyright (c) 2014-2023 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "EntityBase.h"

#include "../core/DataSerialiser.h"

// Required for GetEntity to return a default
template<> bool EntityBase::Is<EntityBase>() const
{
    return true;
}

CoordsXYZ EntityBase::GetLocation() const
{
    return { x, y, z };
}

void EntityBase::SetLocation(const CoordsXYZ& newLocation)
{
    x = newLocation.x;
    y = newLocation.y;
    z = newLocation.z;
}

void EntityBase::Invalidate()
{
    if (x == LOCATION_NULL)
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

    ViewportsInvalidate(SpriteData.SpriteRect, maxZoom);
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

static_assert(static_cast<uint8_t>(EntityType::Count) == 13, "Add new entity type to GetEntityTypeName");
std::string GetEntityTypeName(EntityType type)
{
    switch (type)
    {
        case EntityType::Vehicle:
            return "car";
        case EntityType::Guest:
            return "guest";
        case EntityType::Staff:
            return "staff";
        case EntityType::SteamParticle:
            return "steam_particle";
        case EntityType::MoneyEffect:
            return "money_effect";
        case EntityType::CrashedVehicleParticle:
            return "crashed_vehicle_particle";
        case EntityType::ExplosionCloud:
            return "explosion_cloud";
        case EntityType::CrashSplash:
            return "crash_splash";
        case EntityType::ExplosionFlare:
            return "explosion_flare";
        case EntityType::Balloon:
            return "balloon";
        case EntityType::Duck:
            return "duck";
        case EntityType::JumpingFountain:
            return "jumping_fountain";
        case EntityType::Litter:
            return "litter";
        default:
            return "unknown";
    }
}
