/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../Location.hpp"
#include "TileElementType.h"

#include <cstdint>

constexpr uint8_t kTileElementSize = 16;
constexpr uint8_t kMaxTileElementHeight = 255;
constexpr uint8_t kTileElementOwnerMask = 0b00001111;
constexpr uint8_t kTileElementQuadrantMask = 0b11000000;
constexpr uint8_t kTileElementTypeMask = 0b00111100;
constexpr uint8_t kTileElementDirectionMask = 0b00000011;
constexpr uint8_t kTileElementOccupiedQuadrantsMask = 0b00001111;

enum
{
    TILE_ELEMENT_DIRECTION_WEST,
    TILE_ELEMENT_DIRECTION_NORTH,
    TILE_ELEMENT_DIRECTION_EAST,
    TILE_ELEMENT_DIRECTION_SOUTH
};

enum
{
    TILE_ELEMENT_FLAG_GHOST = (1 << 4),
    TILE_ELEMENT_FLAG_INVISIBLE = (1 << 5),
    TILE_ELEMENT_FLAG_LAST_TILE = (1 << 7)
};

struct TileElement;
struct SurfaceElement;
struct PathElement;
struct TrackElement;
struct SmallSceneryElement;
struct LargeSceneryElement;
struct WallElement;
struct EntranceElement;
struct BannerElement;

#pragma pack(push, 1)
struct TileElementBase
{
    uint8_t Type;            // 0
    uint8_t Flags;           // 1. Upper nibble: flags. Lower nibble: occupied quadrants (one bit per quadrant).
    uint8_t BaseHeight;      // 2
    uint8_t ClearanceHeight; // 3
    uint8_t Owner;           // 4

    void Remove();

    TileElementType GetType() const;
    void SetType(TileElementType newType);

    Direction GetDirection() const;
    void SetDirection(Direction direction);
    Direction GetDirectionWithOffset(uint8_t offset) const;

    bool IsLastForTile() const;
    void SetLastForTile(bool on);
    bool IsGhost() const;
    void SetGhost(bool isGhost);
    bool IsInvisible() const;
    void SetInvisible(bool on);

    uint8_t GetOccupiedQuadrants() const;
    void SetOccupiedQuadrants(uint8_t quadrants);

    int32_t GetBaseZ() const;
    void SetBaseZ(int32_t newZ);

    int32_t GetClearanceZ() const;
    void SetClearanceZ(int32_t newZ);

    uint8_t GetOwner() const;
    void SetOwner(uint8_t newOwner);

    template<typename TType>
    const TType* as() const
    {
        if constexpr (std::is_same_v<TType, TileElement>)
            return reinterpret_cast<const TileElement*>(this);
        else
            return GetType() == TType::kElementType ? reinterpret_cast<const TType*>(this) : nullptr;
    }

    template<typename TType>
    TType* as()
    {
        if constexpr (std::is_same_v<TType, TileElement>)
            return reinterpret_cast<TileElement*>(this);
        else
            return GetType() == TType::kElementType ? reinterpret_cast<TType*>(this) : nullptr;
    }

    const SurfaceElement* AsSurface() const;
    SurfaceElement* AsSurface();
    const PathElement* AsPath() const;
    PathElement* AsPath();
    const TrackElement* AsTrack() const;
    TrackElement* AsTrack();
    const SmallSceneryElement* AsSmallScenery() const;
    SmallSceneryElement* AsSmallScenery();
    const LargeSceneryElement* AsLargeScenery() const;
    LargeSceneryElement* AsLargeScenery();
    const WallElement* AsWall() const;
    WallElement* AsWall();
    const EntranceElement* AsEntrance() const;
    EntranceElement* AsEntrance();
    const BannerElement* AsBanner() const;
    BannerElement* AsBanner();
};
#pragma pack(pop)
