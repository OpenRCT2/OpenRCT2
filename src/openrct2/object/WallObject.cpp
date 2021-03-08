/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "WallObject.h"

#include "../core/IStream.hpp"
#include "../core/Json.hpp"
#include "../core/String.hpp"
#include "../drawing/Drawing.h"
#include "../interface/Cursors.h"
#include "../localisation/Language.h"
#include "../world/Banner.h"

void WallObject::ReadLegacy(IReadObjectContext* context, OpenRCT2::IStream* stream)
{
    stream->Seek(6, OpenRCT2::STREAM_SEEK_CURRENT);
    _legacyType.wall.tool_id = static_cast<CursorID>(stream->ReadValue<uint8_t>());
    _legacyType.wall.flags = stream->ReadValue<uint8_t>();
    _legacyType.wall.height = stream->ReadValue<uint8_t>();
    _legacyType.wall.flags2 = stream->ReadValue<uint8_t>();
    _legacyType.wall.price = stream->ReadValue<uint16_t>();
    _legacyType.wall.scenery_tab_id = OBJECT_ENTRY_INDEX_NULL;
    stream->Seek(1, OpenRCT2::STREAM_SEEK_CURRENT);
    _legacyType.wall.scrolling_mode = stream->ReadValue<uint8_t>();

    GetStringTable().Read(context, stream, ObjectStringID::NAME);

    rct_object_entry sgEntry = stream->ReadValue<rct_object_entry>();
    SetPrimarySceneryGroup(ObjectEntryDescriptor(sgEntry));

    GetImageTable().Read(context, stream);

    // Validate properties
    if (_legacyType.wall.price <= 0)
    {
        context->LogError(ObjectError::InvalidProperty, "Price can not be free or negative.");
    }

    // Autofix this object (will be turned into an official object later).
    auto identifier = GetLegacyIdentifier();
    if (identifier == "XXWLBR03")
    {
        _legacyType.wall.flags2 &= ~WALL_SCENERY_2_DOOR_SOUND_MASK;
        _legacyType.wall.flags2 |= (1u << WALL_SCENERY_2_DOOR_SOUND_SHIFT) & WALL_SCENERY_2_DOOR_SOUND_MASK;
    }
}

void WallObject::Load()
{
    GetStringTable().Sort();
    _legacyType.name = language_allocate_object_string(GetName());
    _legacyType.image = gfx_object_allocate_images(GetImageTable().GetImages(), GetImageTable().GetCount());
}

void WallObject::Unload()
{
    language_free_object_string(_legacyType.name);
    gfx_object_free_images(_legacyType.image, GetImageTable().GetCount());

    _legacyType.name = 0;
    _legacyType.image = 0;
}

void WallObject::DrawPreview(rct_drawpixelinfo* dpi, int32_t width, int32_t height) const
{
    auto screenCoords = ScreenCoordsXY{ width / 2, height / 2 };

    screenCoords.x += 14;
    screenCoords.y += (_legacyType.wall.height * 2) + 16;

    uint32_t imageId = 0x20D00000 | _legacyType.image;
    if (_legacyType.wall.flags & WALL_SCENERY_HAS_SECONDARY_COLOUR)
    {
        imageId |= 0x92000000;
    }

    gfx_draw_sprite(dpi, imageId, screenCoords, 0);

    if (_legacyType.wall.flags & WALL_SCENERY_HAS_GLASS)
    {
        imageId = _legacyType.image + 0x44500006;
        gfx_draw_sprite(dpi, imageId, screenCoords, 0);
    }
    else if (_legacyType.wall.flags & WALL_SCENERY_IS_DOOR)
    {
        imageId++;
        gfx_draw_sprite(dpi, imageId, screenCoords, 0);
    }
}

void WallObject::ReadJson(IReadObjectContext* context, json_t& root)
{
    Guard::Assert(root.is_object(), "WallObject::ReadJson expects parameter root to be object");

    auto properties = root["properties"];

    if (properties.is_object())
    {
        _legacyType.wall.tool_id = Cursor::FromString(Json::GetString(properties["cursor"]), CursorID::FenceDown);
        _legacyType.wall.height = Json::GetNumber<uint8_t>(properties["height"]);
        _legacyType.wall.price = Json::GetNumber<int16_t>(properties["price"]);

        _legacyType.wall.scrolling_mode = Json::GetNumber<uint8_t>(properties["scrollingMode"], SCROLLING_MODE_NONE);

        SetPrimarySceneryGroup(ObjectEntryDescriptor(Json::GetString(properties["sceneryGroup"])));

        // clang-format off
        _legacyType.wall.flags = Json::GetFlags<uint8_t>(
            properties,
            {
                { "hasPrimaryColour",       WALL_SCENERY_HAS_PRIMARY_COLOUR,    Json::FlagType::Normal },
                { "isAllowedOnSlope",       WALL_SCENERY_CANT_BUILD_ON_SLOPE,   Json::FlagType::Inverted },
                { "hasSecondaryColour",     WALL_SCENERY_HAS_SECONDARY_COLOUR,  Json::FlagType::Normal },
                { "hasTernaryColour",       WALL_SCENERY_HAS_TERNARY_COLOUR,    Json::FlagType::Normal },
                { "hasGlass",               WALL_SCENERY_HAS_GLASS,             Json::FlagType::Normal },
                { "isBanner",               WALL_SCENERY_IS_BANNER,             Json::FlagType::Normal },
                { "isDoor",                 WALL_SCENERY_IS_DOOR,               Json::FlagType::Normal },
                { "isLongDoorAnimation",    WALL_SCENERY_LONG_DOOR_ANIMATION,   Json::FlagType::Normal },
            });
        // clang-format on

        _legacyType.wall.flags2 = Json::GetFlags<uint8_t>(
            properties,
            {
                { "isOpaque", WALL_SCENERY_2_IS_OPAQUE },
                { "isAnimated", WALL_SCENERY_2_ANIMATED },
            });

        // HACK WALL_SCENERY_HAS_PRIMARY_COLOUR actually means, has any colour but we simplify the
        //      JSON and handle this on load. We should change code base in future to reflect the JSON.
        if (!(_legacyType.wall.flags & WALL_SCENERY_HAS_PRIMARY_COLOUR))
        {
            if (_legacyType.wall.flags & (WALL_SCENERY_HAS_SECONDARY_COLOUR | WALL_SCENERY_HAS_TERNARY_COLOUR))
            {
                _legacyType.wall.flags |= WALL_SCENERY_HAS_PRIMARY_COLOUR;
                _legacyType.wall.flags2 |= WALL_SCENERY_2_NO_SELECT_PRIMARY_COLOUR;
            }
        }

        // Door sound
        auto jDoorSound = properties["doorSound"];
        if (jDoorSound.is_number())
        {
            auto doorSound = Json::GetNumber<uint8_t>(jDoorSound);
            _legacyType.wall.flags2 |= (doorSound << WALL_SCENERY_2_DOOR_SOUND_SHIFT) & WALL_SCENERY_2_DOOR_SOUND_MASK;
        }
    }

    PopulateTablesFromJson(context, root);
}
