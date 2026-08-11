/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../../object/ObjectTypes.h"
#include "TileElementBase.h"

namespace OpenRCT2
{
    enum
    {
        SURFACE_ELEMENT_HAS_TRACK_THAT_NEEDS_WATER = (1 << 6),
    };

    enum GrassLength : uint8_t
    {
        GRASS_LENGTH_MOWED,
        GRASS_LENGTH_CLEAR_0,
        GRASS_LENGTH_CLEAR_1,
        GRASS_LENGTH_CLEAR_2,
        GRASS_LENGTH_CLUMPS_0,
        GRASS_LENGTH_CLUMPS_1,
        GRASS_LENGTH_CLUMPS_2,
    };

    enum
    {
        OWNERSHIP_UNOWNED = 0,
        OWNERSHIP_CONSTRUCTION_RIGHTS_OWNED = (1 << 4),
        OWNERSHIP_OWNED = (1 << 5),
        OWNERSHIP_CONSTRUCTION_RIGHTS_AVAILABLE = (1 << 6),
        OWNERSHIP_AVAILABLE = (1 << 7)
    };

    constexpr uint8_t kTileElementSurfaceOwnershipMask = 0xF0;
    constexpr uint8_t kTileElementSurfaceParkFenceMask = 0x0F;

    class TerrainEdgeObject;
    class TerrainSurfaceObject;

#pragma pack(push, 1)
    struct SurfaceElement : TileElementBase
    {
        static constexpr TileElementType kElementType = TileElementType::surface;

    private:
        uint8_t slope;
        uint8_t waterHeight;
        uint8_t grassLength;
        uint8_t ownership;
        uint8_t surfaceStyle;
        uint8_t edgeObjectIndex;
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-private-field"
        uint8_t pad0B[5];
#pragma clang diagnostic pop

    public:
        uint8_t getSlope() const;
        void setSlope(uint8_t newSlope);

        ObjectEntryIndex getSurfaceObjectIndex() const;
        TerrainSurfaceObject* getSurfaceObject() const;
        void setSurfaceObjectIndex(ObjectEntryIndex newStyle);

        ObjectEntryIndex getEdgeObjectIndex() const;
        TerrainEdgeObject* getEdgeObject() const;
        void setEdgeObjectIndex(ObjectEntryIndex newIndex);

        bool canGrassGrow() const;
        uint8_t getGrassLength() const;
        void setGrassLength(uint8_t newLength);
        void setGrassLengthAndInvalidate(uint8_t newLength, const CoordsXY& coords);
        void updateGrassLength(const CoordsXY& coords);

        uint8_t getOwnership() const;
        void setOwnership(uint8_t newOwnership);

        int32_t getWaterHeight() const;
        void setWaterHeight(int32_t newWaterHeight);

        uint8_t getParkFences() const;
        void setParkFences(uint8_t newParkFences);

        bool hasTrackThatNeedsWater() const;
        void setHasTrackThatNeedsWater(bool on);
    };
    static_assert(sizeof(SurfaceElement) == kTileElementSize);
#pragma pack(pop)
} // namespace OpenRCT2
