/*****************************************************************************
 * Copyright (c) 2014-2021 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "Map.h"
#include "TileElement.h"

uint8_t TileElementBase::GetType() const
{
    return this->type & TILE_ELEMENT_TYPE_MASK;
}

void TileElementBase::SetType(uint8_t newType)
{
    this->type &= ~TILE_ELEMENT_TYPE_MASK;
    this->type |= (newType & TILE_ELEMENT_TYPE_MASK);
}

Direction TileElementBase::GetDirection() const
{
    return this->type & TILE_ELEMENT_DIRECTION_MASK;
}

void TileElementBase::SetDirection(Direction direction)
{
    this->type &= ~TILE_ELEMENT_DIRECTION_MASK;
    this->type |= (direction & TILE_ELEMENT_DIRECTION_MASK);
}

Direction TileElementBase::GetDirectionWithOffset(uint8_t offset) const
{
    return ((this->type & TILE_ELEMENT_DIRECTION_MASK) + offset) & TILE_ELEMENT_DIRECTION_MASK;
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
    tile_element_remove(static_cast<TileElement*>(this));
}

uint8_t TileElementBase::GetOccupiedQuadrants() const
{
    return Flags & TILE_ELEMENT_OCCUPIED_QUADRANTS_MASK;
}

void TileElementBase::SetOccupiedQuadrants(uint8_t quadrants)
{
    Flags &= ~TILE_ELEMENT_OCCUPIED_QUADRANTS_MASK;
    Flags |= (quadrants & TILE_ELEMENT_OCCUPIED_QUADRANTS_MASK);
}

int32_t TileElementBase::GetBaseZ() const
{
    return base_height * COORDS_Z_STEP;
}

void TileElementBase::SetBaseZ(int32_t newZ)
{
    base_height = (newZ / COORDS_Z_STEP);
}

int32_t TileElementBase::GetClearanceZ() const
{
    return clearance_height * COORDS_Z_STEP;
}

void TileElementBase::SetClearanceZ(int32_t newZ)
{
    clearance_height = (newZ / COORDS_Z_STEP);
}

uint8_t TileElementBase::GetOwner() const
{
    return owner & OWNER_MASK;
}

void TileElementBase::SetOwner(uint8_t newOwner)
{
    owner &= ~OWNER_MASK;
    owner |= (newOwner & OWNER_MASK);
}
