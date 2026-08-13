/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "SurfaceElement.h"

#include "../../Context.h"
#include "../../GameState.h"
#include "../../object/ObjectManager.h"
#include "../../object/TerrainEdgeObject.h"
#include "../../object/TerrainSurfaceObject.h"
#include "../../scenario/Scenario.h"
#include "../Map.h"
#include "Slope.h"
#include "TileElement.h"

namespace OpenRCT2
{
    ObjectEntryIndex SurfaceElement::getSurfaceObjectIndex() const
    {
        return surfaceStyle;
    }

    TerrainSurfaceObject* SurfaceElement::getSurfaceObject() const
    {
        auto& objManager = GetContext()->GetObjectManager();
        return objManager.GetLoadedObject<TerrainSurfaceObject>(getSurfaceObjectIndex());
    }

    ObjectEntryIndex SurfaceElement::getEdgeObjectIndex() const
    {
        return edgeObjectIndex;
    }

    TerrainEdgeObject* SurfaceElement::getEdgeObject() const
    {
        auto& objManager = GetContext()->GetObjectManager();
        return objManager.GetLoadedObject<TerrainEdgeObject>(getEdgeObjectIndex());
    }

    void SurfaceElement::setSurfaceObjectIndex(ObjectEntryIndex newStyle)
    {
        surfaceStyle = static_cast<ObjectEntryIndex>(newStyle);
    }

    void SurfaceElement::setEdgeObjectIndex(ObjectEntryIndex newIndex)
    {
        edgeObjectIndex = static_cast<ObjectEntryIndex>(newIndex);
    }

    int32_t SurfaceElement::getWaterHeight() const
    {
        return waterHeight * kWaterHeightStep;
    }

    void SurfaceElement::setWaterHeight(int32_t newWaterHeight)
    {
        waterHeight = newWaterHeight / kWaterHeightStep;
    }

    bool SurfaceElement::canGrassGrow() const
    {
        const auto* surfaceObject = getSurfaceObject();
        if (surfaceObject != nullptr)
        {
            if (surfaceObject->Flags.has(TerrainSurfaceFlag::canGrow))
            {
                return true;
            }
        }
        return false;
    }

    uint8_t SurfaceElement::getGrassLength() const
    {
        return grassLength;
    }

    void SurfaceElement::setGrassLength(uint8_t newLength)
    {
        grassLength = newLength;
    }

    void SurfaceElement::setGrassLengthAndInvalidate(uint8_t length, const CoordsXY& coords)
    {
        uint8_t oldLength = grassLength & 0x7;
        uint8_t newLength = length & 0x7;

        grassLength = length;

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

        int32_t z = getBaseZ();
        MapInvalidateTile({ coords, z, z + 16 });
    }

    /**
     *
     *  rct2: 0x006647A1
     */
    void SurfaceElement::updateGrassLength(const CoordsXY& coords)
    {
        // Check if tile is grass and if it's allowed to grow
        if (!canGrassGrow() || getGameState().cheats.disableGrassGrowing)
            return;

        uint8_t grassLengthTmp = grassLength & 7;

        // Check if grass is underwater or outside park
        if (getWaterHeight() > getBaseZ() || !MapIsLocationInPark(coords))
        {
            if (grassLengthTmp != GRASS_LENGTH_CLEAR_0)
                setGrassLengthAndInvalidate(GRASS_LENGTH_CLEAR_0, coords);

            return;
        }

        // Grass can't grow any further than CLUMPS_2 but this code also cuts grass
        // if there is an object placed on top of it.

        int32_t baseZ = getBaseZ();
        int32_t clearZ = getBaseZ() + kLandHeightStep;
        if (slope & kTileSlopeDiagonalFlag)
            clearZ += kLandHeightStep;

        // Check objects above grass
        TileElement* tileElementAbove = reinterpret_cast<TileElement*>(this);
        for (;;)
        {
            if (tileElementAbove->isLastForTile())
            {
                // Grow grass

                // Check interim grass lengths
                uint8_t lengthNibble = (getGrassLength() & 0xF0) >> 4;
                if (lengthNibble < 0xF)
                {
                    grassLength += 0x10;
                }
                else
                {
                    // Zeros the length nibble
                    grassLength += 0x10;
                    grassLength ^= 8;
                    if (grassLength & 8)
                    {
                        // Random growth rate (length nibble)
                        grassLength |= ScenarioRand() & 0x70;
                    }
                    else
                    {
                        // Increase length if not at max length
                        if (grassLengthTmp != GRASS_LENGTH_CLUMPS_2)
                            setGrassLengthAndInvalidate(grassLengthTmp + 1, coords);
                    }
                }
            }
            else
            {
                tileElementAbove++;
                if (tileElementAbove->getType() == TileElementType::wall)
                    continue;
                // Grass should not be affected by ghost elements.
                if (tileElementAbove->isGhost())
                    continue;
                if (baseZ >= tileElementAbove->getClearanceZ())
                    continue;
                if (clearZ < tileElementAbove->getBaseZ())
                    continue;

                if (grassLengthTmp != GRASS_LENGTH_CLEAR_0)
                    setGrassLengthAndInvalidate(GRASS_LENGTH_CLEAR_0, coords);
            }
            break;
        }
    }

    OwnershipFlags SurfaceElement::getOwnership() const
    {
        OwnershipFlags ret;
        ret.holder = (ownership & kTileElementSurfaceOwnershipMask) >> 4;
        return ret;
    }

    void SurfaceElement::setOwnership(OwnershipFlags newOwnership)
    {
        ownership &= ~kTileElementSurfaceOwnershipMask;
        ownership |= (((newOwnership.holder) << 4) & kTileElementSurfaceOwnershipMask);
    }

    uint8_t SurfaceElement::getParkFences() const
    {
        return (ownership & kTileElementSurfaceParkFenceMask);
    }

    void SurfaceElement::setParkFences(uint8_t newParkFences)
    {
        ownership &= ~kTileElementSurfaceParkFenceMask;
        ownership |= (newParkFences & kTileElementSurfaceParkFenceMask);
    }

    uint8_t SurfaceElement::getSlope() const
    {
        return slope;
    }

    void SurfaceElement::setSlope(uint8_t newSlope)
    {
        slope = newSlope;
    }

    bool SurfaceElement::hasTrackThatNeedsWater() const
    {
        return (type & SURFACE_ELEMENT_HAS_TRACK_THAT_NEEDS_WATER) != 0;
    }

    void SurfaceElement::setHasTrackThatNeedsWater(bool on)
    {
        type &= ~SURFACE_ELEMENT_HAS_TRACK_THAT_NEEDS_WATER;
        if (on)
            type |= SURFACE_ELEMENT_HAS_TRACK_THAT_NEEDS_WATER;
    }
} // namespace OpenRCT2
