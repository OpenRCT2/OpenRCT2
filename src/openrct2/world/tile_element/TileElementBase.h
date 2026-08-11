/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
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

namespace OpenRCT2
{
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
        uint8_t type;            // 0
        uint8_t flags;           // 1. Upper nibble: flags. Lower nibble: occupied quadrants (one bit per quadrant).
        uint8_t baseHeight;      // 2
        uint8_t clearanceHeight; // 3
        uint8_t owner;           // 4

        void remove();

        TileElementType getType() const;
        void setType(TileElementType newType);

        Direction getDirection() const;
        void setDirection(Direction direction);
        Direction getDirectionWithOffset(uint8_t offset) const;

        bool isLastForTile() const;
        void setLastForTile(bool on);
        bool isGhost() const;
        void setGhost(bool isGhost);
        bool isInvisible() const;
        void setInvisible(bool on);

        uint8_t getOccupiedQuadrants() const;
        void setOccupiedQuadrants(uint8_t quadrants);

        int32_t getBaseZ() const;
        void setBaseZ(int32_t newZ);

        int32_t getClearanceZ() const;
        void setClearanceZ(int32_t newZ);

        uint8_t getOwner() const;
        void setOwner(uint8_t newOwner);

        template<typename TType>
        const TType* as() const
        {
            if constexpr (std::is_same_v<TType, TileElement>)
                return reinterpret_cast<const TileElement*>(this);
            else
                return getType() == TType::kElementType ? reinterpret_cast<const TType*>(this) : nullptr;
        }

        template<typename TType>
        TType* as()
        {
            if constexpr (std::is_same_v<TType, TileElement>)
                return reinterpret_cast<TileElement*>(this);
            else
                return getType() == TType::kElementType ? reinterpret_cast<TType*>(this) : nullptr;
        }

        const SurfaceElement* asSurface() const;
        SurfaceElement* asSurface();
        const PathElement* asPath() const;
        PathElement* asPath();
        const TrackElement* asTrack() const;
        TrackElement* asTrack();
        const SmallSceneryElement* asSmallScenery() const;
        SmallSceneryElement* asSmallScenery();
        const LargeSceneryElement* asLargeScenery() const;
        LargeSceneryElement* asLargeScenery();
        const WallElement* asWall() const;
        WallElement* asWall();
        const EntranceElement* asEntrance() const;
        EntranceElement* asEntrance();
        const BannerElement* asBanner() const;
        BannerElement* asBanner();
    };
#pragma pack(pop)
} // namespace OpenRCT2
