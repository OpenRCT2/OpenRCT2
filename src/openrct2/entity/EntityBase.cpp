/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "EntityBase.h"

#include "../core/DataSerialiser.h"
#include "../core/Guard.hpp"
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
        using enum EntityType;
        case vehicle:
        case guest:
        case staff:
            maxZoom = ZoomLevel{ 2 };
            break;
        case crashedVehicleParticle:
        case jumpingFountain:
            maxZoom = ZoomLevel{ 0 };
            break;
        case duck:
            maxZoom = ZoomLevel{ 1 };
            break;
        case steamParticle:
        case moneyEffect:
        case explosionCloud:
        case crashSplash:
        case explosionFlare:
        case balloon:
            maxZoom = ZoomLevel{ 2 };
            break;
        case litter:
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
