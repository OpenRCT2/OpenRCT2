/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "Surface.h"

#include "../scenario/Scenario.h"
#include "Location.hpp"
#include "Map.h"

uint32_t SurfaceElement::GetSurfaceStyle() const
{
    uint32_t retVal = (terrain >> 5) & 7;
    if (type & 1)
        retVal |= (1 << 3);
    return retVal;
}

uint32_t SurfaceElement::GetEdgeStyle() const
{
    uint32_t terrain_edge = (slope >> 5) & 7;
    if (type & 128)
        terrain_edge |= (1 << 3);
    return terrain_edge;
}

void SurfaceElement::SetSurfaceStyle(uint32_t newStyle)
{
    // Bit 3 for terrain is stored in element.type bit 0
    if (newStyle & 8)
        type |= 1;
    else
        type &= ~1;

    // Bits 0, 1, 2 for terrain are stored in element.terrain bit 5, 6, 7
    terrain &= ~0xE0;
    terrain |= (newStyle & 7) << 5;
}

void SurfaceElement::SetEdgeStyle(uint32_t newStyle)
{
    // Bit 3 for terrain is stored in element.type bit 7
    if (newStyle & 8)
        type |= 128;
    else
        type &= ~128;

    // Bits 0, 1, 2 for terrain are stored in element.slope bit 5, 6, 7
    slope &= ~TILE_ELEMENT_SURFACE_EDGE_STYLE_MASK;
    slope |= (newStyle & 7) << 5;
}

uint32_t SurfaceElement::GetWaterHeight() const
{
    return terrain & TILE_ELEMENT_SURFACE_WATER_HEIGHT_MASK;
}

void SurfaceElement::SetWaterHeight(uint32_t newWaterHeight)
{
    newWaterHeight &= 0x1F;
    terrain &= ~TILE_ELEMENT_SURFACE_WATER_HEIGHT_MASK;
    terrain |= newWaterHeight;
}

uint8_t SurfaceElement::GetGrassLength() const
{
    return grass_length;
}

void SurfaceElement::SetGrassLength(uint8_t newLength)
{
    grass_length = newLength;
}

void SurfaceElement::SetGrassLengthAndInvalidate(uint8_t length, CoordsXY coords)
{
    uint8_t oldLength = grass_length & 0x7;
    uint8_t newLength = length & 0x7;

    grass_length = length;

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

    int32_t z = base_height * 8;
    map_invalidate_tile(coords.x, coords.y, z, z + 16);
}

/**
 *
 *  rct2: 0x006647A1
 */
void SurfaceElement::UpdateGrassLength(CoordsXY coords)
{
    // Check if tile is grass
    if (GetSurfaceStyle() != TERRAIN_GRASS)
        return;

    uint8_t grassLengthTmp = grass_length & 7;

    // Check if grass is underwater or outside park
    uint32_t waterHeight = GetWaterHeight() * 2;
    if (waterHeight > base_height || !map_is_location_in_park(coords))
    {
        if (grassLengthTmp != GRASS_LENGTH_CLEAR_0)
            SetGrassLengthAndInvalidate(GRASS_LENGTH_CLEAR_0, coords);

        return;
    }

    // Grass can't grow any further than CLUMPS_2 but this code also cuts grass
    // if there is an object placed on top of it.

    int32_t z0 = base_height;
    int32_t z1 = base_height + 2;
    if (slope & TILE_ELEMENT_SLOPE_DOUBLE_HEIGHT)
        z1 += 2;

    // Check objects above grass
    TileElement* tileElementAbove = (TileElement*)this;
    for (;;)
    {
        if (tileElementAbove->flags & TILE_ELEMENT_FLAG_LAST_TILE)
        {
            // Grow grass

            // Check interim grass lengths
            uint8_t lengthNibble = (GetGrassLength() & 0xF0) >> 4;
            if (lengthNibble < 0xF)
            {
                grass_length += 0x10;
            }
            else
            {
                // Zeros the length nibble
                grass_length += 0x10;
                grass_length ^= 8;
                if (grass_length & 8)
                {
                    // Random growth rate (length nibble)
                    grass_length |= scenario_rand() & 0x70;
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
            if (tileElementAbove->GetType() == TILE_ELEMENT_TYPE_WALL)
                continue;
            // Grass should not be affected by ghost elements.
            if (tileElementAbove->IsGhost())
                continue;
            if (z0 >= tileElementAbove->clearance_height)
                continue;
            if (z1 < tileElementAbove->base_height)
                continue;

            if (grassLengthTmp != GRASS_LENGTH_CLEAR_0)
                SetGrassLengthAndInvalidate(GRASS_LENGTH_CLEAR_0, coords);
        }
        break;
    }
}

uint8_t SurfaceElement::GetOwnership() const
{
    return (ownership & TILE_ELEMENT_SURFACE_OWNERSHIP_MASK);
}

void SurfaceElement::SetOwnership(uint8_t newOwnership)
{
    ownership &= ~TILE_ELEMENT_SURFACE_OWNERSHIP_MASK;
    ownership |= (newOwnership & TILE_ELEMENT_SURFACE_OWNERSHIP_MASK);
}

uint8_t SurfaceElement::GetParkFences() const
{
    return (ownership & TILE_ELEMENT_SURFACE_PARK_FENCE_MASK);
}

void SurfaceElement::SetParkFences(uint8_t newParkFences)
{
    ownership &= ~TILE_ELEMENT_SURFACE_PARK_FENCE_MASK;
    ownership |= (newParkFences & TILE_ELEMENT_SURFACE_PARK_FENCE_MASK);
}

uint8_t SurfaceElement::GetSlope() const
{
    return (slope & TILE_ELEMENT_SURFACE_SLOPE_MASK);
}

void SurfaceElement::SetSlope(uint8_t newSlope)
{
    slope &= ~TILE_ELEMENT_SURFACE_SLOPE_MASK;
    slope |= (newSlope & TILE_ELEMENT_SURFACE_SLOPE_MASK);
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