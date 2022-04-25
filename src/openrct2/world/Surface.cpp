/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "Surface.h"

#include "../Context.h"
#include "../object/ObjectManager.h"
#include "../object/TerrainEdgeObject.h"
#include "../object/TerrainSurfaceObject.h"
#include "../scenario/Scenario.h"
#include "Location.hpp"
#include "Map.h"

uint32_t SurfaceElement::GetSurfaceStyle() const
{
    return SurfaceStyle;
}

TerrainSurfaceObject* SurfaceElement::GetSurfaceStyleObject() const
{
    auto& objManager = OpenRCT2::GetContext()->GetObjectManager();
    return static_cast<TerrainSurfaceObject*>(objManager.GetLoadedObject(ObjectType::TerrainSurface, GetSurfaceStyle()));
}

uint32_t SurfaceElement::GetEdgeStyle() const
{
    return EdgeStyle;
}

TerrainEdgeObject* SurfaceElement::GetEdgeStyleObject() const
{
    auto& objManager = OpenRCT2::GetContext()->GetObjectManager();
    return static_cast<TerrainEdgeObject*>(objManager.GetLoadedObject(ObjectType::TerrainEdge, GetEdgeStyle()));
}

void SurfaceElement::SetSurfaceStyle(uint32_t newStyle)
{
    SurfaceStyle = newStyle;
}

void SurfaceElement::SetEdgeStyle(uint32_t newStyle)
{
    EdgeStyle = newStyle;
}

int32_t SurfaceElement::GetWaterHeight() const
{
    return WaterHeight * 16;
}

void SurfaceElement::SetWaterHeight(int32_t newWaterHeight)
{
    WaterHeight = newWaterHeight / 16;
}

bool SurfaceElement::CanGrassGrow() const
{
    auto surfaceStyle = GetSurfaceStyle();
    auto& objMgr = OpenRCT2::GetContext()->GetObjectManager();
    auto obj = objMgr.GetLoadedObject(ObjectType::TerrainSurface, surfaceStyle);
    if (obj != nullptr)
    {
        auto surfaceObject = static_cast<TerrainSurfaceObject*>(obj);
        if (surfaceObject->Flags & TERRAIN_SURFACE_FLAGS::CAN_GROW)
        {
            return true;
        }
    }
    return false;
}

uint8_t SurfaceElement::GetGrassLength() const
{
    return GrassLength;
}

void SurfaceElement::SetGrassLength(uint8_t newLength)
{
    GrassLength = newLength;
}

void SurfaceElement::SetGrassLengthAndInvalidate(uint8_t length, const CoordsXY& coords)
{
    uint8_t oldLength = GrassLength & 0x7;
    uint8_t newLength = length & 0x7;

    GrassLength = length;

    if (newLength == oldLength)
    {
        return;
    }

    // If the new grass length won't result in an actual visual change
    // then skip invalidating the tile, no point
    if (((oldLength > 0 && oldLength < 4) && (newLength > 0 && newLength < 4))
        || ((oldLength > 3 && oldLength < 7) && (newLength > 3 && newLength < 7)))
    {
        return;
    }

    int32_t z = GetBaseZ();
    map_invalidate_tile({ coords, z, z + 16 });
}

/**
 *
 *  rct2: 0x006647A1
 */
void SurfaceElement::UpdateGrassLength(const CoordsXY& coords)
{
    // Check if tile is grass
    if (!CanGrassGrow())
        return;

    uint8_t grassLengthTmp = GrassLength & 7;

    // Check if grass is underwater or outside park
    if (GetWaterHeight() > GetBaseZ() || !map_is_location_in_park(coords))
    {
        if (grassLengthTmp != GRASS_LENGTH_CLEAR_0)
            SetGrassLengthAndInvalidate(GRASS_LENGTH_CLEAR_0, coords);

        return;
    }

    // Grass can't grow any further than CLUMPS_2 but this code also cuts grass
    // if there is an object placed on top of it.

    int32_t baseZ = GetBaseZ();
    int32_t clearZ = GetBaseZ() + LAND_HEIGHT_STEP;
    if (Slope & TILE_ELEMENT_SLOPE_DOUBLE_HEIGHT)
        clearZ += LAND_HEIGHT_STEP;

    // Check objects above grass
    TileElement* tileElementAbove = reinterpret_cast<TileElement*>(this);
    for (;;)
    {
        if (tileElementAbove->IsLastForTile())
        {
            // Grow grass

            // Check interim grass lengths
            uint8_t lengthNibble = (GetGrassLength() & 0xF0) >> 4;
            if (lengthNibble < 0xF)
            {
                GrassLength += 0x10;
            }
            else
            {
                // Zeros the length nibble
                GrassLength += 0x10;
                GrassLength ^= 8;
                if (GrassLength & 8)
                {
                    // Random growth rate (length nibble)
                    GrassLength |= scenario_rand() & 0x70;
                }
                else
                {
                    // Increase length if not at max length
                    if (grassLengthTmp != GRASS_LENGTH_CLUMPS_2)
                        SetGrassLengthAndInvalidate(grassLengthTmp + 1, coords);
                }
            }
        }
        else
        {
            tileElementAbove++;
            if (tileElementAbove->GetType() == TileElementType::Wall)
                continue;
            // Grass should not be affected by ghost elements.
            if (tileElementAbove->IsGhost())
                continue;
            if (baseZ >= tileElementAbove->GetClearanceZ())
                continue;
            if (clearZ < tileElementAbove->GetBaseZ())
                continue;

            if (grassLengthTmp != GRASS_LENGTH_CLEAR_0)
                SetGrassLengthAndInvalidate(GRASS_LENGTH_CLEAR_0, coords);
        }
        break;
    }
}

uint8_t SurfaceElement::GetOwnership() const
{
    return (Ownership & TILE_ELEMENT_SURFACE_OWNERSHIP_MASK);
}

void SurfaceElement::SetOwnership(uint8_t newOwnership)
{
    Ownership &= ~TILE_ELEMENT_SURFACE_OWNERSHIP_MASK;
    Ownership |= (newOwnership & TILE_ELEMENT_SURFACE_OWNERSHIP_MASK);
}

uint8_t SurfaceElement::GetParkFences() const
{
    return (Ownership & TILE_ELEMENT_SURFACE_PARK_FENCE_MASK);
}

void SurfaceElement::SetParkFences(uint8_t newParkFences)
{
    Ownership &= ~TILE_ELEMENT_SURFACE_PARK_FENCE_MASK;
    Ownership |= (newParkFences & TILE_ELEMENT_SURFACE_PARK_FENCE_MASK);
}

uint8_t SurfaceElement::GetSlope() const
{
    return Slope;
}

void SurfaceElement::SetSlope(uint8_t newSlope)
{
    Slope = newSlope;
}

bool SurfaceElement::HasTrackThatNeedsWater() const
{
    return (type & SURFACE_ELEMENT_HAS_TRACK_THAT_NEEDS_WATER) != 0;
}

void SurfaceElement::SetHasTrackThatNeedsWater(bool on)
{
    type &= ~SURFACE_ELEMENT_HAS_TRACK_THAT_NEEDS_WATER;
    if (on)
        type |= SURFACE_ELEMENT_HAS_TRACK_THAT_NEEDS_WATER;
}
