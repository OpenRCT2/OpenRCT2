/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "SurfaceElement.h"

#include "../../Context.h"
#include "../../object/ObjectManager.h"
#include "../../object/TerrainEdgeObject.h"
#include "../../object/TerrainSurfaceObject.h"
#include "../../scenario/Scenario.h"
#include "../Map.h"
#include "Slope.h"
#include "TileElement.h"

ObjectEntryIndex SurfaceElement::GetSurfaceObjectIndex() const
{
    return SurfaceStyle;
}

TerrainSurfaceObject* SurfaceElement::GetSurfaceObject() const
{
    auto& objManager = OpenRCT2::GetContext()->GetObjectManager();
    return objManager.GetLoadedObject<TerrainSurfaceObject>(GetSurfaceObjectIndex());
}

ObjectEntryIndex SurfaceElement::GetEdgeObjectIndex() const
{
    return EdgeObjectIndex;
}

TerrainEdgeObject* SurfaceElement::GetEdgeObject() const
{
    auto& objManager = OpenRCT2::GetContext()->GetObjectManager();
    return objManager.GetLoadedObject<TerrainEdgeObject>(GetEdgeObjectIndex());
}

void SurfaceElement::SetSurfaceObjectIndex(ObjectEntryIndex newStyle)
{
    SurfaceStyle = static_cast<ObjectEntryIndex>(newStyle);
}

void SurfaceElement::SetEdgeObjectIndex(ObjectEntryIndex newIndex)
{
    EdgeObjectIndex = static_cast<ObjectEntryIndex>(newIndex);
}

int32_t SurfaceElement::GetWaterHeight() const
{
    return WaterHeight * kWaterHeightStep;
}

void SurfaceElement::SetWaterHeight(int32_t newWaterHeight)
{
    WaterHeight = newWaterHeight / kWaterHeightStep;
}

bool SurfaceElement::CanGrassGrow() const
{
    auto surfaceStyle = GetSurfaceObjectIndex();
    auto& objMgr = OpenRCT2::GetContext()->GetObjectManager();
    auto obj = objMgr.GetLoadedObject(ObjectType::TerrainSurface, surfaceStyle);
    if (obj != nullptr)
    {
        const auto* surfaceObject = static_cast<TerrainSurfaceObject*>(obj);
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
    MapInvalidateTile({ coords, z, z + 16 });
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
    if (GetWaterHeight() > GetBaseZ() || !MapIsLocationInPark(coords))
    {
        if (grassLengthTmp != GRASS_LENGTH_CLEAR_0)
            SetGrassLengthAndInvalidate(GRASS_LENGTH_CLEAR_0, coords);

        return;
    }

    // Grass can't grow any further than CLUMPS_2 but this code also cuts grass
    // if there is an object placed on top of it.

    int32_t baseZ = GetBaseZ();
    int32_t clearZ = GetBaseZ() + kLandHeightStep;
    if (Slope & kTileSlopeDiagonalFlag)
        clearZ += kLandHeightStep;

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
                    GrassLength |= ScenarioRand() & 0x70;
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
    return (Ownership & kTileElementSurfaceOwnershipMask);
}

void SurfaceElement::SetOwnership(uint8_t newOwnership)
{
    Ownership &= ~kTileElementSurfaceOwnershipMask;
    Ownership |= (newOwnership & kTileElementSurfaceOwnershipMask);
}

uint8_t SurfaceElement::GetParkFences() const
{
    return (Ownership & kTileElementSurfaceParkFenceMask);
}

void SurfaceElement::SetParkFences(uint8_t newParkFences)
{
    Ownership &= ~kTileElementSurfaceParkFenceMask;
    Ownership |= (newParkFences & kTileElementSurfaceParkFenceMask);
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
    return (Type & SURFACE_ELEMENT_HAS_TRACK_THAT_NEEDS_WATER) != 0;
}

void SurfaceElement::SetHasTrackThatNeedsWater(bool on)
{
    Type &= ~SURFACE_ELEMENT_HAS_TRACK_THAT_NEEDS_WATER;
    if (on)
        Type |= SURFACE_ELEMENT_HAS_TRACK_THAT_NEEDS_WATER;
}
