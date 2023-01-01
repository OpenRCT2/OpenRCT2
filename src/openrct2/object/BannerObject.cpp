/*****************************************************************************
 * Copyright (c) 2014-2023 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "BannerObject.h"

#include "../core/IStream.hpp"
#include "../core/Json.hpp"
#include "../drawing/Drawing.h"
#include "../drawing/Image.h"
#include "../localisation/Language.h"
#include "../object/Object.h"
#include "../object/ObjectRepository.h"
#include "ObjectList.h"

void BannerObject::ReadLegacy(IReadObjectContext* context, OpenRCT2::IStream* stream)
{
    stream->Seek(6, OpenRCT2::STREAM_SEEK_CURRENT);
    _legacyType.scrolling_mode = stream->ReadValue<uint8_t>();
    _legacyType.flags = stream->ReadValue<uint8_t>();
    _legacyType.price = stream->ReadValue<int16_t>();
    _legacyType.scenery_tab_id = OBJECT_ENTRY_INDEX_NULL;
    stream->Seek(2, OpenRCT2::STREAM_SEEK_CURRENT);

    GetStringTable().Read(context, stream, ObjectStringID::NAME);

    rct_object_entry sgEntry = stream->ReadValue<rct_object_entry>();
    SetPrimarySceneryGroup(ObjectEntryDescriptor(sgEntry));

    GetImageTable().Read(context, stream);

    // Validate properties
    if (_legacyType.price <= 0)
    {
        context->LogError(ObjectError::InvalidProperty, "Price can not be free or negative.");
    }

    // Add banners to 'Signs and items for footpaths' group, rather than lumping them in the Miscellaneous tab.
    // Since this is already done the other way round for original items, avoid adding those to prevent duplicates.
    auto identifier = GetLegacyIdentifier();

    auto& objectRepository = context->GetObjectRepository();
    auto item = objectRepository.FindObjectLegacy(identifier);
    if (item != nullptr)
    {
        auto sourceGame = item->GetFirstSourceGame();
        if (sourceGame == ObjectSourceGame::WackyWorlds || sourceGame == ObjectSourceGame::TimeTwister
            || sourceGame == ObjectSourceGame::Custom)
        {
            auto scgPathX = Object::GetScgPathXHeader();
            SetPrimarySceneryGroup(scgPathX);
        }
    }
}

void BannerObject::Load()
{
    GetStringTable().Sort();
    _legacyType.name = language_allocate_object_string(GetName());
    _legacyType.image = gfx_object_allocate_images(GetImageTable().GetImages(), GetImageTable().GetCount());
}

void BannerObject::Unload()
{
    language_free_object_string(_legacyType.name);
    gfx_object_free_images(_legacyType.image, GetImageTable().GetCount());

    _legacyType.name = 0;
    _legacyType.image = 0;
}

void BannerObject::DrawPreview(rct_drawpixelinfo* dpi, int32_t width, int32_t height) const
{
    auto screenCoords = ScreenCoordsXY{ width / 2, height / 2 };

    auto image0 = ImageId(_legacyType.image, COLOUR_BORDEAUX_RED);
    auto image1 = ImageId(_legacyType.image + 1, COLOUR_BORDEAUX_RED);

    gfx_draw_sprite(dpi, image0, screenCoords + ScreenCoordsXY{ -12, 8 });
    gfx_draw_sprite(dpi, image1, screenCoords + ScreenCoordsXY{ -12, 8 });
}

void BannerObject::ReadJson(IReadObjectContext* context, json_t& root)
{
    Guard::Assert(root.is_object(), "BannerObject::ReadJson expects parameter root to be object");
    json_t properties = root["properties"];

    if (properties.is_object())
    {
        _legacyType.scrolling_mode = Json::GetNumber<uint8_t>(properties["scrollingMode"]);
        _legacyType.price = Json::GetNumber<int16_t>(properties["price"]);
        _legacyType.flags = Json::GetFlags<uint8_t>(
            properties,
            {
                { "hasPrimaryColour", BANNER_ENTRY_FLAG_HAS_PRIMARY_COLOUR },
            });

        SetPrimarySceneryGroup(ObjectEntryDescriptor(Json::GetString(properties["sceneryGroup"])));
    }

    PopulateTablesFromJson(context, root);
}
