/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
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

TileElementType TileElementBase::GetType() const
{
    return static_cast<TileElementType>((this->Type & kTileElementTypeMask) >> 2);
}

void TileElementBase::SetType(TileElementType newType)
{
    this->Type &= ~kTileElementTypeMask;
    this->Type |= ((EnumValue(newType) << 2) & kTileElementTypeMask);
}

Direction TileElementBase::GetDirection() const
{
    return this->Type & kTileElementDirectionMask;
}

void TileElementBase::SetDirection(Direction direction)
{
    this->Type &= ~kTileElementDirectionMask;
    this->Type |= (direction & kTileElementDirectionMask);
}

Direction TileElementBase::GetDirectionWithOffset(uint8_t offset) const
{
    return ((this->Type & kTileElementDirectionMask) + offset) & kTileElementDirectionMask;
}

bool TileElementBase::IsLastForTile() const
{
    return (this->Flags & TILE_ELEMENT_FLAG_LAST_TILE) != 0;
}

void TileElementBase::SetLastForTile(bool on)
{
    if (on)
        Flags |= TILE_ELEMENT_FLAG_LAST_TILE;
    else
        Flags &= ~TILE_ELEMENT_FLAG_LAST_TILE;
}

bool TileElementBase::IsInvisible() const
{
    return (this->Flags & TILE_ELEMENT_FLAG_INVISIBLE) != 0;
}

void TileElementBase::SetInvisible(bool on)
{
    if (on)
        Flags |= TILE_ELEMENT_FLAG_INVISIBLE;
    else
        Flags &= ~TILE_ELEMENT_FLAG_INVISIBLE;
}

bool TileElementBase::IsGhost() const
{
    return (this->Flags & TILE_ELEMENT_FLAG_GHOST) != 0;
}

void TileElementBase::SetGhost(bool isGhost)
{
    if (isGhost)
    {
        this->Flags |= TILE_ELEMENT_FLAG_GHOST;
    }
    else
    {
        this->Flags &= ~TILE_ELEMENT_FLAG_GHOST;
    }
}

void TileElementBase::Remove()
{
    TileElementRemove(static_cast<TileElement*>(this));
}

uint8_t TileElementBase::GetOccupiedQuadrants() const
{
    return Flags & kTileElementOccupiedQuadrantsMask;
}

void TileElementBase::SetOccupiedQuadrants(uint8_t quadrants)
{
    Flags &= ~kTileElementOccupiedQuadrantsMask;
    Flags |= (quadrants & kTileElementOccupiedQuadrantsMask);
}

int32_t TileElementBase::GetBaseZ() const
{
    return BaseHeight * kCoordsZStep;
}

void TileElementBase::SetBaseZ(int32_t newZ)
{
    BaseHeight = (newZ / kCoordsZStep);
}

int32_t TileElementBase::GetClearanceZ() const
{
    return ClearanceHeight * kCoordsZStep;
}

void TileElementBase::SetClearanceZ(int32_t newZ)
{
    ClearanceHeight = (newZ / kCoordsZStep);
}

uint8_t TileElementBase::GetOwner() const
{
    return Owner & kTileElementOwnerMask;
}

void TileElementBase::SetOwner(uint8_t newOwner)
{
    Owner &= ~kTileElementOwnerMask;
    Owner |= (newOwner & kTileElementOwnerMask);
}

const SurfaceElement* TileElementBase::AsSurface() const
{
    return as<SurfaceElement>();
}
SurfaceElement* TileElementBase::AsSurface()
{
    return as<SurfaceElement>();
}
const PathElement* TileElementBase::AsPath() const
{
    return as<PathElement>();
}
PathElement* TileElementBase::AsPath()
{
    return as<PathElement>();
}
const TrackElement* TileElementBase::AsTrack() const
{
    return as<TrackElement>();
}
TrackElement* TileElementBase::AsTrack()
{
    return as<TrackElement>();
}
const SmallSceneryElement* TileElementBase::AsSmallScenery() const
{
    return as<SmallSceneryElement>();
}
SmallSceneryElement* TileElementBase::AsSmallScenery()
{
    return as<SmallSceneryElement>();
}
const LargeSceneryElement* TileElementBase::AsLargeScenery() const
{
    return as<LargeSceneryElement>();
}
LargeSceneryElement* TileElementBase::AsLargeScenery()
{
    return as<LargeSceneryElement>();
}
const WallElement* TileElementBase::AsWall() const
{
    return as<WallElement>();
}
WallElement* TileElementBase::AsWall()
{
    return as<WallElement>();
}

const EntranceElement* TileElementBase::AsEntrance() const
{
    return as<EntranceElement>();
}

EntranceElement* TileElementBase::AsEntrance()
{
    return as<EntranceElement>();
}

const BannerElement* TileElementBase::AsBanner() const
{
    return as<BannerElement>();
}
BannerElement* TileElementBase::AsBanner()
{
    return as<BannerElement>();
}
