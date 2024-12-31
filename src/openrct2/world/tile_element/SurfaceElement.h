/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../../object/TerrainEdgeObject.h"
#include "../../object/TerrainSurfaceObject.h"
#include "TileElementBase.h"

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

#pragma pack(push, 1)
struct SurfaceElement : TileElementBase
{
    static constexpr TileElementType kElementType = TileElementType::Surface;

private:
    uint8_t Slope;
    uint8_t WaterHeight;
    uint8_t GrassLength;
    uint8_t Ownership;
    uint8_t SurfaceStyle;
    uint8_t EdgeObjectIndex;
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-private-field"
    uint8_t Pad0B[5];
#pragma clang diagnostic pop

public:
    uint8_t GetSlope() const;
    void SetSlope(uint8_t newSlope);

    ObjectEntryIndex GetSurfaceObjectIndex() const;
    TerrainSurfaceObject* GetSurfaceObject() const;
    void SetSurfaceObjectIndex(ObjectEntryIndex newStyle);

    ObjectEntryIndex GetEdgeObjectIndex() const;
    TerrainEdgeObject* GetEdgeObject() const;
    void SetEdgeObjectIndex(ObjectEntryIndex newStyle);

    bool CanGrassGrow() const;
    uint8_t GetGrassLength() const;
    void SetGrassLength(uint8_t newLength);
    void SetGrassLengthAndInvalidate(uint8_t newLength, const CoordsXY& coords);
    void UpdateGrassLength(const CoordsXY& coords);

    uint8_t GetOwnership() const;
    void SetOwnership(uint8_t newOwnership);

    int32_t GetWaterHeight() const;
    void SetWaterHeight(int32_t newWaterHeight);

    uint8_t GetParkFences() const;
    void SetParkFences(uint8_t newParkFences);

    bool HasTrackThatNeedsWater() const;
    void SetHasTrackThatNeedsWater(bool on);
};
static_assert(sizeof(SurfaceElement) == kTileElementSize);
#pragma pack(pop)
