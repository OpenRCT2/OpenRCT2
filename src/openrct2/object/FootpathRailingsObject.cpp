/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "FootpathRailingsObject.h"

#include "../core/Guard.hpp"
#include "../core/IStream.hpp"
#include "../core/Json.hpp"
#include "../drawing/Drawing.h"

namespace OpenRCT2
{
    void FootpathRailingsObject::Load()
    {
        GetStringTable().Sort();
        NameStringId = LanguageAllocateObjectString(GetName());

        auto numImages = GetImageTable().GetCount();
        if (numImages != 0)
        {
            PreviewImageId = LoadImages();
            BridgeImageId = PreviewImageId + 37;
            RailingsImageId = PreviewImageId + 1;
        }

        _descriptor.name = NameStringId;
        _descriptor.bridgeImage = BridgeImageId;
        _descriptor.previewImage = PreviewImageId;
        _descriptor.flags = Flags;
        _descriptor.scrollingMode = ScrollingMode;
        _descriptor.supportType = SupportType;
        _descriptor.supportColour = Colour;
        _descriptor.railingsImage = RailingsImageId;
    }

    void FootpathRailingsObject::Unload()
    {
        LanguageFreeObjectString(NameStringId);
        UnloadImages();

        NameStringId = 0;
        PreviewImageId = 0;
        BridgeImageId = 0;
        RailingsImageId = 0;
    }

    void FootpathRailingsObject::DrawPreview(Drawing::RenderTarget& rt, int32_t width, int32_t height) const
    {
        auto x = width / 2;
        auto y = height / 2;
        auto helper = ImageId(kImageIndexUndefined);
        if (Colour != Drawing::kColourNull)
            helper = helper.WithPrimary(Colour);

        if (SupportType == RailingEntrySupportType::pole)
        {
            auto img = helper.WithIndex(BridgeImageId + 20 + 15);
            for (int i = 0; i < 2; i++)
            {
                auto h = i * 16;
                GfxDrawSprite(rt, img, { x - 8, y + 8 + h });
                GfxDrawSprite(rt, img, { x + 8, y + 16 + h });
            }

            GfxDrawSprite(rt, helper.WithIndex(BridgeImageId + 5), { x, y - 17 });
            GfxDrawSprite(rt, ImageId(RailingsImageId + 1), { x + 4, y - 14 });
            GfxDrawSprite(rt, ImageId(RailingsImageId + 1), { x + 27, y - 2 });
        }
        else
        {
            GfxDrawSprite(rt, helper.WithIndex(BridgeImageId + 22), { x + 0, y + 16 });
            GfxDrawSprite(rt, helper.WithIndex(BridgeImageId + 49), { x, y - 17 });
            GfxDrawSprite(rt, ImageId(RailingsImageId + 1), { x + 4, y - 14 });
            GfxDrawSprite(rt, ImageId(RailingsImageId + 1), { x + 27, y - 3 });
        }
    }

    void FootpathRailingsObject::ReadJson(IReadObjectContext* context, json_t& root)
    {
        Guard::Assert(root.is_object(), "FootpathObject::ReadJson expects parameter root to be object");

        auto properties = root["properties"];
        if (properties.is_object())
        {
            SupportType = ParseSupportType(Json::GetString(properties["supportType"]));
            ScrollingMode = Json::GetNumber<uint8_t>(properties["scrollingMode"]);
            Colour = Drawing::colourFromString(Json::GetString(properties["colour"]), Drawing::kColourNull);
            Flags = Json::GetFlags<uint8_t>(
                properties,
                {
                    { "hasSupportImages", RAILING_ENTRY_FLAG_HAS_SUPPORT_BASE_SPRITE },
                    { "hasElevatedPathImages", RAILING_ENTRY_FLAG_DRAW_PATH_OVER_SUPPORTS },
                    { "noQueueBanner", RAILING_ENTRY_FLAG_NO_QUEUE_BANNER },
                });
        }

        PopulateTablesFromJson(context, root);
    }

    RailingEntrySupportType FootpathRailingsObject::ParseSupportType(std::string_view s)
    {
        if (s == "pole")
            return RailingEntrySupportType::pole;
        else /* if (s == "box") */
            return RailingEntrySupportType::box;
    }
} // namespace OpenRCT2
