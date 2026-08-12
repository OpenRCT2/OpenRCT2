/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../../object/SmallSceneryEntry.h"
#include "TileElementBase.h"

namespace OpenRCT2::Drawing
{
    enum class Colour : uint8_t;
}

namespace OpenRCT2
{
    enum
    {
        MAP_ELEM_SMALL_SCENERY_FLAGS2_NEEDS_SUPPORTS = (1 << 0),
    };

#pragma pack(push, 1)
    struct SmallSceneryElement : TileElementBase
    {
        static constexpr TileElementType kElementType = TileElementType::smallScenery;

    private:
        ObjectEntryIndex entryIndex; // 5
        uint8_t age;                 // 7
        Drawing::Colour colour[3];   // 8
        uint8_t flags2;              // B
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-private-field"
        uint8_t pad0B[4];
#pragma clang diagnostic pop

    public:
        ObjectEntryIndex getEntryIndex() const;
        void setEntryIndex(ObjectEntryIndex newIndex);
        const SmallSceneryEntry* getEntry() const;
        uint8_t getAge() const;
        void setAge(uint8_t newAge);
        void increaseAge(const CoordsXY& sceneryPos);
        uint8_t getSceneryQuadrant() const;
        void setSceneryQuadrant(uint8_t newQuadrant);
        Drawing::Colour getPrimaryColour() const;
        void setPrimaryColour(Drawing::Colour colour);
        Drawing::Colour getSecondaryColour() const;
        void setSecondaryColour(Drawing::Colour colour);
        Drawing::Colour getTertiaryColour() const;
        void setTertiaryColour(Drawing::Colour colour);
        bool needsSupports() const;
        void setNeedsSupports();
        void updateAge(const CoordsXY& sceneryPos);
    };
    static_assert(sizeof(SmallSceneryElement) == kTileElementSize);
#pragma pack(pop)
} // namespace OpenRCT2
