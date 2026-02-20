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
#include "../../object/LargeSceneryEntry.h"
#include "../../object/LargeSceneryObject.h"
#include "TileElementBase.h"

struct Banner;

namespace OpenRCT2
{
    enum
    {
        LARGE_SCENERY_ELEMENT_FLAGS2_ACCOUNTED = 1 << 0,
    };

#pragma pack(push, 1)
    struct LargeSceneryElement : TileElementBase
    {
        static constexpr TileElementType kElementType = TileElementType::LargeScenery;

    private:
        ObjectEntryIndex EntryIndex;
        ::BannerIndex BannerIndex;
        uint8_t SequenceIndex;
        Drawing::Colour Colour[3];
        uint8_t Flags2;
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-private-field"
        uint8_t pad[2];
#pragma clang diagnostic pop

    public:
        ObjectEntryIndex GetEntryIndex() const;
        void SetEntryIndex(ObjectEntryIndex newIndex);
        const LargeSceneryEntry* GetEntry() const;
        const LargeSceneryObject* GetObject() const;

        uint8_t GetSequenceIndex() const;
        void SetSequenceIndex(uint8_t newIndex);

        Drawing::Colour GetPrimaryColour() const;
        void SetPrimaryColour(Drawing::Colour colour);
        Drawing::Colour GetSecondaryColour() const;
        void SetSecondaryColour(Drawing::Colour colour);
        Drawing::Colour GetTertiaryColour() const;
        void SetTertiaryColour(Drawing::Colour colour);

        Banner* GetBanner() const;
        ::BannerIndex GetBannerIndex() const;
        void SetBannerIndex(::BannerIndex newIndex);

        bool IsAccounted() const;
        void SetIsAccounted(bool isAccounted);
    };
    static_assert(sizeof(LargeSceneryElement) == kTileElementSize);
#pragma pack(pop)

} // namespace OpenRCT2
