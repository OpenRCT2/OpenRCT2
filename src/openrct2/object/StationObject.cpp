/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "StationObject.h"

#include "../core/Guard.hpp"
#include "../core/IStream.hpp"
#include "../core/Json.hpp"
#include "../core/String.hpp"
#include "../drawing/Drawing.h"
#include "../drawing/ScrollingText.h"

namespace OpenRCT2
{
    constexpr std::array kAllowedStationIDs = std::to_array<u8string_view>({
        "rct2.station.abstract",
        "rct2.station.canvas_tent",
        "rct2.station.castle_brown",
        "rct2.station.castle_grey",
        "rct2.station.classical",
        "rct2.station.jungle",
        "rct2.station.log",
        "rct2.station.pagoda",
        "rct2.station.plain",
        "rct2.station.snow",
        "rct2.station.space",
        "rct2.station.wooden",
        "openrct2.station.noentrance",
        "openrct2.station.noplatformnoentrance",
        "tygrysek90.station.abstract_open",
        "tygrysek90.station.canvas_tent_open",
        "tygrysek90.station.castle_brown_open",
        "tygrysek90.station.castle_grey",
        "tygrysek90.station.castle_grey_open",
        "tygrysek90.station.classical_open",
        "tygrysek90.station.jungle_open",
        "tygrysek90.station.log_open",
        "tygrysek90.station.pagoda_open",
        "tygrysek90.station.snow_open",
        "tygrysek90.station.space_open",
        "tygrysek90.station.wooden_open",
    });

    void StationObject::Load()
    {
        GetStringTable().Sort();
        NameStringId = LanguageAllocateObjectString(GetName());

        auto numImages = GetImageTable().GetCount();
        if (numImages == 0)
            return;

        baseImageIndex = LoadImages();

        // An odd number of images suggests there is an icon present
        auto iconOffset = numImages & 1;
        if (iconOffset == 1)
        {
            iconIndex = baseImageIndex;
            if (numImages == 1)
                return;
        }

        entranceBackIndex = baseImageIndex + iconOffset;
        entranceFrontIndex = entranceBackIndex + 4;

        exitBackIndex = entranceBackIndex + 8;
        exitFrontIndex = entranceBackIndex + 12;

        if (!(Flags & StationObjectFlags::isTransparent))
        {
            if (numImages > iconOffset + 16)
                shelterIndex = entranceBackIndex + 16;

            return;
        }

        entranceBackGlassIndex = entranceBackIndex + 16;
        entranceFrontGlassIndex = entranceBackIndex + 20;

        exitBackGlassIndex = entranceBackIndex + 24;
        exitFrontGlassIndex = entranceBackIndex + 28;

        if (numImages > iconOffset + 32)
            shelterIndex = entranceBackIndex + 32;

        if (numImages > iconOffset + 44)
            shelterGlassIndex = entranceBackIndex + 44;
    }

    void StationObject::Unload()
    {
        LanguageFreeObjectString(NameStringId);
        UnloadImages();

        NameStringId = 0;
        baseImageIndex = kImageIndexUndefined;

        entranceBackIndex = kImageIndexUndefined;
        entranceFrontIndex = kImageIndexUndefined;
        exitBackIndex = kImageIndexUndefined;
        exitFrontIndex = kImageIndexUndefined;

        entranceBackGlassIndex = kImageIndexUndefined;
        entranceFrontGlassIndex = kImageIndexUndefined;
        exitBackGlassIndex = kImageIndexUndefined;
        exitFrontGlassIndex = kImageIndexUndefined;

        shelterIndex = kImageIndexUndefined;
        shelterGlassIndex = kImageIndexUndefined;
    }

    void StationObject::DrawPreview(Drawing::RenderTarget& rt, int32_t width, int32_t height) const
    {
        auto screenCoords = ScreenCoordsXY{ width / 2, (height / 2) + 16 };

        auto colour0 = Drawing::Colour::lightBlue;
        auto colour1 = Drawing::Colour::bordeauxRed;
        auto tcolour0 = colour0;

        // Draw back sprite
        auto backImageId = ImageId(entranceBackIndex, colour0, colour1);
        GfxDrawSprite(rt, backImageId, screenCoords);
        if (Flags & StationObjectFlags::isTransparent)
        {
            auto tImageId = ImageId(entranceBackGlassIndex).WithTransparency(tcolour0);
            GfxDrawSprite(rt, tImageId, screenCoords);
        }

        // Draw front sprite
        auto frontImageId = ImageId(entranceFrontIndex, colour0, colour1);
        GfxDrawSprite(rt, frontImageId, screenCoords);
        if (Flags & StationObjectFlags::isTransparent)
        {
            auto frontGlassImageId = ImageId(entranceFrontGlassIndex).WithTransparency(tcolour0);
            GfxDrawSprite(rt, frontGlassImageId, screenCoords);
        }
    }

    static bool isKnownStationStyle(u8string_view identifier)
    {
        for (const auto& entry : kAllowedStationIDs)
        {
            if (entry == identifier)
                return true;
        }

        return false;
    }

    void StationObject::ReadJson(IReadObjectContext* context, json_t& root)
    {
        Guard::Assert(root.is_object(), "StationObject::ReadJson expects parameter root to be object");
        if (!isKnownStationStyle(GetIdentifier()))
        {
            throw std::runtime_error("Unknown station style");
        }

        auto properties = root["properties"];

        if (properties.is_object())
        {
            Height = Json::GetNumber<int32_t>(properties["height"]);
            ScrollingMode = Json::GetNumber<uint8_t>(properties["scrollingMode"], kScrollingModeNone);
            Flags = Json::GetFlags<uint32_t>(
                properties,
                {
                    { "hasPrimaryColour", StationObjectFlags::hasPrimaryColour },
                    { "hasSecondaryColour", StationObjectFlags::hasSecondaryColour },
                    { "isTransparent", StationObjectFlags::isTransparent },
                    { "noPlatforms", StationObjectFlags::noPlatforms },
                    { "hasShelter", StationObjectFlags::hasShelter },
                });
        }

        PopulateTablesFromJson(context, root);
    }
} // namespace OpenRCT2
