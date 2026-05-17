/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "TileElementBase.h"

#include "../../core/EnumUtils.hpp"
#include "../Map.h"
#include "BannerElement.h"
#include "EntranceElement.h"
#include "LargeSceneryElement.h"
#include "PathElement.h"
#include "SmallSceneryElement.h"
#include "SurfaceElement.h"
#include "TileElement.h"
#include "TrackElement.h"
#include "WallElement.h"

namespace OpenRCT2
{
    TileElementType TileElementBase::getType() const
    {
        return static_cast<TileElementType>((this->type & kTileElementTypeMask) >> 2);
    }

    void TileElementBase::setType(TileElementType newType)
    {
        this->type &= ~kTileElementTypeMask;
        this->type |= ((EnumValue(newType) << 2) & kTileElementTypeMask);
    }

    Direction TileElementBase::getDirection() const
    {
        return this->type & kTileElementDirectionMask;
    }

    void TileElementBase::setDirection(Direction direction)
    {
        this->type &= ~kTileElementDirectionMask;
        this->type |= (direction & kTileElementDirectionMask);
    }

    Direction TileElementBase::getDirectionWithOffset(uint8_t offset) const
    {
        return ((this->type & kTileElementDirectionMask) + offset) & kTileElementDirectionMask;
    }

    bool TileElementBase::isLastForTile() const
    {
        return (this->flags & TILE_ELEMENT_FLAG_LAST_TILE) != 0;
    }

    void TileElementBase::setLastForTile(bool on)
    {
        if (on)
            flags |= TILE_ELEMENT_FLAG_LAST_TILE;
        else
            flags &= ~TILE_ELEMENT_FLAG_LAST_TILE;
    }

    bool TileElementBase::isInvisible() const
    {
        return (this->flags & TILE_ELEMENT_FLAG_INVISIBLE) != 0;
    }

    void TileElementBase::setInvisible(bool on)
    {
        if (on)
            flags |= TILE_ELEMENT_FLAG_INVISIBLE;
        else
            flags &= ~TILE_ELEMENT_FLAG_INVISIBLE;
    }

    bool TileElementBase::isGhost() const
    {
        return (this->flags & TILE_ELEMENT_FLAG_GHOST) != 0;
    }

    void TileElementBase::setGhost(bool isGhost)
    {
        if (isGhost)
        {
            this->flags |= TILE_ELEMENT_FLAG_GHOST;
        }
        else
        {
            this->flags &= ~TILE_ELEMENT_FLAG_GHOST;
        }
    }

    void TileElementBase::remove()
    {
        TileElementRemove(static_cast<TileElement*>(this));
    }

    uint8_t TileElementBase::getOccupiedQuadrants() const
    {
        return flags & kTileElementOccupiedQuadrantsMask;
    }

    void TileElementBase::setOccupiedQuadrants(uint8_t quadrants)
    {
        flags &= ~kTileElementOccupiedQuadrantsMask;
        flags |= (quadrants & kTileElementOccupiedQuadrantsMask);
    }

    int32_t TileElementBase::getBaseZ() const
    {
        return baseHeight * kCoordsZStep;
    }

    void TileElementBase::setBaseZ(int32_t newZ)
    {
        baseHeight = (newZ / kCoordsZStep);
    }

    int32_t TileElementBase::getClearanceZ() const
    {
        return clearanceHeight * kCoordsZStep;
    }

    void TileElementBase::setClearanceZ(int32_t newZ)
    {
        clearanceHeight = (newZ / kCoordsZStep);
    }

    uint8_t TileElementBase::getOwner() const
    {
        return owner & kTileElementOwnerMask;
    }

    void TileElementBase::setOwner(uint8_t newOwner)
    {
        owner &= ~kTileElementOwnerMask;
        owner |= (newOwner & kTileElementOwnerMask);
    }

    const SurfaceElement* TileElementBase::asSurface() const
    {
        return as<SurfaceElement>();
    }
    SurfaceElement* TileElementBase::asSurface()
    {
        return as<SurfaceElement>();
    }
    const PathElement* TileElementBase::asPath() const
    {
        return as<PathElement>();
    }
    PathElement* TileElementBase::asPath()
    {
        return as<PathElement>();
    }
    const TrackElement* TileElementBase::asTrack() const
    {
        return as<TrackElement>();
    }
    TrackElement* TileElementBase::asTrack()
    {
        return as<TrackElement>();
    }
    const SmallSceneryElement* TileElementBase::asSmallScenery() const
    {
        return as<SmallSceneryElement>();
    }
    SmallSceneryElement* TileElementBase::asSmallScenery()
    {
        return as<SmallSceneryElement>();
    }
    const LargeSceneryElement* TileElementBase::asLargeScenery() const
    {
        return as<LargeSceneryElement>();
    }
    LargeSceneryElement* TileElementBase::asLargeScenery()
    {
        return as<LargeSceneryElement>();
    }
    const WallElement* TileElementBase::asWall() const
    {
        return as<WallElement>();
    }
    WallElement* TileElementBase::asWall()
    {
        return as<WallElement>();
    }

    const EntranceElement* TileElementBase::asEntrance() const
    {
        return as<EntranceElement>();
    }

    EntranceElement* TileElementBase::asEntrance()
    {
        return as<EntranceElement>();
    }

    const BannerElement* TileElementBase::asBanner() const
    {
        return as<BannerElement>();
    }
    BannerElement* TileElementBase::asBanner()
    {
        return as<BannerElement>();
    }
} // namespace OpenRCT2
