/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../../Identifiers.h"
#include "../../drawing/Colour.h"
#include "../../object/LargeSceneryEntry.h"
#include "TileElementBase.h"

struct Banner;

namespace OpenRCT2
{
    class LargeSceneryObject;

    enum
    {
        LARGE_SCENERY_ELEMENT_FLAGS2_ACCOUNTED = 1 << 0,
    };

#pragma pack(push, 1)
    struct LargeSceneryElement : TileElementBase
    {
        static constexpr TileElementType kElementType = TileElementType::largeScenery;

    private:
        ObjectEntryIndex entryIndex;
        ::BannerIndex bannerIndex;
        uint8_t sequenceIndex;
        Drawing::Colour colour[3];
        uint8_t flags2;
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-private-field"
        uint8_t pad[2];
#pragma clang diagnostic pop

    public:
        ObjectEntryIndex getEntryIndex() const;
        void setEntryIndex(ObjectEntryIndex newIndex);
        const LargeSceneryEntry* getEntry() const;

        uint8_t getSequenceIndex() const;
        void setSequenceIndex(uint8_t newIndex);

        Drawing::Colour getPrimaryColour() const;
        void setPrimaryColour(Drawing::Colour colour);
        Drawing::Colour getSecondaryColour() const;
        void setSecondaryColour(Drawing::Colour colour);
        Drawing::Colour getTertiaryColour() const;
        void setTertiaryColour(Drawing::Colour colour);

        Banner* getBanner() const;
        ::BannerIndex getBannerIndex() const;
        void setBannerIndex(::BannerIndex newIndex);

        bool isAccounted() const;
        void setIsAccounted(bool isAccounted);
    };
    static_assert(sizeof(LargeSceneryElement) == kTileElementSize);
#pragma pack(pop)

} // namespace OpenRCT2
