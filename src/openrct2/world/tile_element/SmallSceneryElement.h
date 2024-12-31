/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../../interface/Colour.h"
#include "../../object/ObjectTypes.h"
#include "../../object/SmallSceneryEntry.h"
#include "TileElementBase.h"

enum
{
    MAP_ELEM_SMALL_SCENERY_FLAGS2_NEEDS_SUPPORTS = (1 << 0),
};

#pragma pack(push, 1)
struct SmallSceneryElement : TileElementBase
{
    static constexpr TileElementType kElementType = TileElementType::SmallScenery;

private:
    ObjectEntryIndex entryIndex; // 5
    uint8_t age;                 // 7
    uint8_t Colour[3];           // 8
    uint8_t Flags2;              // B
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-private-field"
    uint8_t Pad0B[4];
#pragma clang diagnostic pop

public:
    ObjectEntryIndex GetEntryIndex() const;
    void SetEntryIndex(ObjectEntryIndex newIndex);
    const SmallSceneryEntry* GetEntry() const;
    uint8_t GetAge() const;
    void SetAge(uint8_t newAge);
    void IncreaseAge(const CoordsXY& sceneryPos);
    uint8_t GetSceneryQuadrant() const;
    void SetSceneryQuadrant(uint8_t newQuadrant);
    colour_t GetPrimaryColour() const;
    void SetPrimaryColour(colour_t colour);
    colour_t GetSecondaryColour() const;
    void SetSecondaryColour(colour_t colour);
    colour_t GetTertiaryColour() const;
    void SetTertiaryColour(colour_t colour);
    bool NeedsSupports() const;
    void SetNeedsSupports();
    void UpdateAge(const CoordsXY& sceneryPos);
};
static_assert(sizeof(SmallSceneryElement) == kTileElementSize);
#pragma pack(pop)
