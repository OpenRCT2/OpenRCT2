/*****************************************************************************
 * Copyright (c) 2014-2023 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "Map.h"
#include "TileElement.h"
namespace OpenRCT2
{
    TileElementType TileElementBase::GetType() const
    {
        return static_cast<TileElementType>((this->Type & TILE_ELEMENT_TYPE_MASK) >> 2);
    }

    void TileElementBase::SetType(TileElementType newType)
    {
        this->Type &= ~TILE_ELEMENT_TYPE_MASK;
        this->Type |= ((EnumValue(newType) << 2) & TILE_ELEMENT_TYPE_MASK);
    }

    Direction TileElementBase::GetDirection() const
    {
        return this->Type & TILE_ELEMENT_DIRECTION_MASK;
    }

    void TileElementBase::SetDirection(Direction direction)
    {
        this->Type &= ~TILE_ELEMENT_DIRECTION_MASK;
        this->Type |= (direction & TILE_ELEMENT_DIRECTION_MASK);
    }

    Direction TileElementBase::GetDirectionWithOffset(uint8_t offset) const
    {
        return ((this->Type & TILE_ELEMENT_DIRECTION_MASK) + offset) & TILE_ELEMENT_DIRECTION_MASK;
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
        return Flags & TILE_ELEMENT_OCCUPIED_QUADRANTS_MASK;
    }

    void TileElementBase::SetOccupiedQuadrants(uint8_t quadrants)
    {
        Flags &= ~TILE_ELEMENT_OCCUPIED_QUADRANTS_MASK;
        Flags |= (quadrants & TILE_ELEMENT_OCCUPIED_QUADRANTS_MASK);
    }

    int32_t TileElementBase::GetBaseZ() const
    {
        return BaseHeight * COORDS_Z_STEP;
    }

    void TileElementBase::SetBaseZ(int32_t newZ)
    {
        BaseHeight = (newZ / COORDS_Z_STEP);
    }

    int32_t TileElementBase::GetClearanceZ() const
    {
        return ClearanceHeight * COORDS_Z_STEP;
    }

    void TileElementBase::SetClearanceZ(int32_t newZ)
    {
        ClearanceHeight = (newZ / COORDS_Z_STEP);
    }

    uint8_t TileElementBase::GetOwner() const
    {
        return Owner & OWNER_MASK;
    }

    void TileElementBase::SetOwner(uint8_t newOwner)
    {
        Owner &= ~OWNER_MASK;
        Owner |= (newOwner & OWNER_MASK);
    }
} // namespace OpenRCT2
