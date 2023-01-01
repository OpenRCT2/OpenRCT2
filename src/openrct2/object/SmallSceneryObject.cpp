/*****************************************************************************
 * Copyright (c) 2014-2023 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma warning(disable : 4706) // assignment within conditional expression

#include "SmallSceneryObject.h"

#include "../core/IStream.hpp"
#include "../core/Json.hpp"
#include "../core/Memory.hpp"
#include "../core/String.hpp"
#include "../drawing/Drawing.h"
#include "../drawing/Image.h"
#include "../interface/Cursors.h"
#include "../localisation/Language.h"
#include "../world/Scenery.h"
#include "../world/SmallScenery.h"

#include <algorithm>

void SmallSceneryObject::ReadLegacy(IReadObjectContext* context, OpenRCT2::IStream* stream)
{
    stream->Seek(6, OpenRCT2::STREAM_SEEK_CURRENT);
    _legacyType.flags = stream->ReadValue<uint32_t>();
    _legacyType.height = stream->ReadValue<uint8_t>();
    _legacyType.tool_id = static_cast<CursorID>(stream->ReadValue<uint8_t>());
    _legacyType.price = stream->ReadValue<int16_t>() * 10;
    _legacyType.removal_price = stream->ReadValue<int16_t>() * 10;
    stream->Seek(4, OpenRCT2::STREAM_SEEK_CURRENT);
    _legacyType.animation_delay = stream->ReadValue<uint16_t>();
    _legacyType.animation_mask = stream->ReadValue<uint16_t>();
    _legacyType.num_frames = stream->ReadValue<uint16_t>();
    _legacyType.scenery_tab_id = OBJECT_ENTRY_INDEX_NULL;

    GetStringTable().Read(context, stream, ObjectStringID::NAME);

    rct_object_entry sgEntry = stream->ReadValue<rct_object_entry>();
    SetPrimarySceneryGroup(ObjectEntryDescriptor(sgEntry));

    if (_legacyType.HasFlag(SMALL_SCENERY_FLAG_HAS_FRAME_OFFSETS))
    {
        _frameOffsets = ReadFrameOffsets(stream);
    }
    // This crude method was used by RCT2. JSON objects have a flag for this property.
    if (_legacyType.height > 64)
    {
        _legacyType.flags |= SMALL_SCENERY_FLAG_IS_TREE;
    }

    GetImageTable().Read(context, stream);

    // Validate properties
    if (_legacyType.price <= 0)
    {
        context->LogError(ObjectError::InvalidProperty, "Price can not be free or negative.");
    }
    if (_legacyType.removal_price <= 0)
    {
        // Make sure you don't make a profit when placing then removing.
        const auto reimbursement = _legacyType.removal_price;
        if (reimbursement > _legacyType.price)
        {
            context->LogError(ObjectError::InvalidProperty, "Sell price can not be more than buy price.");
        }
    }
}

void SmallSceneryObject::Load()
{
    GetStringTable().Sort();
    _legacyType.name = language_allocate_object_string(GetName());
    _legacyType.image = gfx_object_allocate_images(GetImageTable().GetImages(), GetImageTable().GetCount());

    _legacyType.scenery_tab_id = OBJECT_ENTRY_INDEX_NULL;

    if (_legacyType.HasFlag(SMALL_SCENERY_FLAG_HAS_FRAME_OFFSETS))
    {
        _legacyType.frame_offsets = _frameOffsets.data();
    }

    PerformFixes();
}

void SmallSceneryObject::Unload()
{
    language_free_object_string(_legacyType.name);
    gfx_object_free_images(_legacyType.image, GetImageTable().GetCount());

    _legacyType.name = 0;
    _legacyType.image = 0;
}

void SmallSceneryObject::DrawPreview(rct_drawpixelinfo* dpi, int32_t width, int32_t height) const
{
    auto imageId = ImageId(_legacyType.image);
    if (_legacyType.HasFlag(SMALL_SCENERY_FLAG_HAS_PRIMARY_COLOUR))
    {
        imageId = imageId.WithPrimary(COLOUR_BORDEAUX_RED);
        if (_legacyType.HasFlag(SMALL_SCENERY_FLAG_HAS_SECONDARY_COLOUR))
        {
            imageId = imageId.WithSecondary(COLOUR_YELLOW);
        }
    }
    if (_legacyType.HasFlag(SMALL_SCENERY_FLAG_HAS_TERTIARY_COLOUR))
    {
        imageId = imageId.WithSecondary(COLOUR_DARK_BROWN);
    }

    auto screenCoords = ScreenCoordsXY{ width / 2, (height / 2) + (_legacyType.height / 2) };
    screenCoords.y = std::min(screenCoords.y, height - 16);

    if ((_legacyType.HasFlag(SMALL_SCENERY_FLAG_FULL_TILE)) && (_legacyType.HasFlag(SMALL_SCENERY_FLAG_VOFFSET_CENTRE)))
    {
        screenCoords.y -= 12;
    }

    gfx_draw_sprite(dpi, imageId, screenCoords);

    if (_legacyType.HasFlag(SMALL_SCENERY_FLAG_HAS_GLASS))
    {
        imageId = ImageId(_legacyType.image + 4).WithTransparency(COLOUR_BORDEAUX_RED);
        gfx_draw_sprite(dpi, imageId, screenCoords);
    }

    if (_legacyType.HasFlag(SMALL_SCENERY_FLAG_ANIMATED_FG))
    {
        imageId = ImageId(_legacyType.image + 4);
        if (_legacyType.HasFlag(SMALL_SCENERY_FLAG_HAS_SECONDARY_COLOUR))
        {
            imageId = imageId.WithSecondary(COLOUR_YELLOW);
        }
        gfx_draw_sprite(dpi, imageId, screenCoords);
    }
}

std::vector<uint8_t> SmallSceneryObject::ReadFrameOffsets(OpenRCT2::IStream* stream)
{
    uint8_t frameOffset;
    auto data = std::vector<uint8_t>();
    data.push_back(stream->ReadValue<uint8_t>());
    while ((frameOffset = stream->ReadValue<uint8_t>()) != 0xFF)
    {
        data.push_back(frameOffset);
    }
    data.push_back(frameOffset);
    return data;
}

// clang-format off
void SmallSceneryObject::PerformFixes()
{
    auto identifier = GetLegacyIdentifier();
    static const auto& scgWalls = Object::GetScgWallsHeader();

    // ToonTowner's base blocks. Put them in the Walls and Roofs group.
    if (identifier == "XXBBCL01" ||
        identifier == "XXBBMD01" ||
        identifier == "ARBASE2 ")
    {
        SetPrimarySceneryGroup(scgWalls);
    }

    // ToonTowner's Pirate roofs. Make them show up in the Pirate Theming.
    if (identifier == "TTPIRF05" ||
        identifier == "TTPRF09 " ||
        identifier == "TTPRF10 " ||
        identifier == "TTPRF11 ")
    {
        static const auto& scgPirat = GetScgPiratHeader();
        SetPrimarySceneryGroup(scgPirat);
    }
}
// clang-format on

ObjectEntryDescriptor SmallSceneryObject::GetScgPiratHeader() const
{
    return ObjectEntryDescriptor("rct2.scenery_group.scgpirat");
}

void SmallSceneryObject::ReadJson(IReadObjectContext* context, json_t& root)
{
    Guard::Assert(root.is_object(), "SmallSceneryObject::ReadJson expects parameter root to be object");

    auto properties = root["properties"];

    if (properties.is_object())
    {
        _legacyType.height = Json::GetNumber<uint8_t>(properties["height"]);
        _legacyType.tool_id = Cursor::FromString(Json::GetString(properties["cursor"]), CursorID::StatueDown);
        _legacyType.price = Json::GetNumber<int16_t>(properties["price"]) * 10;
        _legacyType.removal_price = Json::GetNumber<int16_t>(properties["removalPrice"]) * 10;
        _legacyType.animation_delay = Json::GetNumber<uint16_t>(properties["animationDelay"]);
        _legacyType.animation_mask = Json::GetNumber<uint16_t>(properties["animationMask"]);
        _legacyType.num_frames = Json::GetNumber<uint16_t>(properties["numFrames"]);

        _legacyType.flags = Json::GetFlags<uint32_t>(
            properties,
            {
                { "SMALL_SCENERY_FLAG_VOFFSET_CENTRE", SMALL_SCENERY_FLAG_VOFFSET_CENTRE },
                { "requiresFlatSurface", SMALL_SCENERY_FLAG_REQUIRE_FLAT_SURFACE },
                { "isRotatable", SMALL_SCENERY_FLAG_ROTATABLE },
                { "isAnimated", SMALL_SCENERY_FLAG_ANIMATED },
                { "canWither", SMALL_SCENERY_FLAG_CAN_WITHER },
                { "canBeWatered", SMALL_SCENERY_FLAG_CAN_BE_WATERED },
                { "hasOverlayImage", SMALL_SCENERY_FLAG_ANIMATED_FG },
                { "hasGlass", SMALL_SCENERY_FLAG_HAS_GLASS },
                { "hasPrimaryColour", SMALL_SCENERY_FLAG_HAS_PRIMARY_COLOUR },
                { "SMALL_SCENERY_FLAG_FOUNTAIN_SPRAY_1", SMALL_SCENERY_FLAG_FOUNTAIN_SPRAY_1 },
                { "SMALL_SCENERY_FLAG_FOUNTAIN_SPRAY_4", SMALL_SCENERY_FLAG_FOUNTAIN_SPRAY_4 },
                { "isClock", SMALL_SCENERY_FLAG_IS_CLOCK },
                { "SMALL_SCENERY_FLAG_SWAMP_GOO", SMALL_SCENERY_FLAG_SWAMP_GOO },
                { "SMALL_SCENERY_FLAG17", SMALL_SCENERY_FLAG17 },
                { "isStackable", SMALL_SCENERY_FLAG_STACKABLE },
                { "prohibitWalls", SMALL_SCENERY_FLAG_NO_WALLS },
                { "hasSecondaryColour", SMALL_SCENERY_FLAG_HAS_SECONDARY_COLOUR },
                { "hasNoSupports", SMALL_SCENERY_FLAG_NO_SUPPORTS },
                { "SMALL_SCENERY_FLAG_VISIBLE_WHEN_ZOOMED", SMALL_SCENERY_FLAG_VISIBLE_WHEN_ZOOMED },
                { "SMALL_SCENERY_FLAG_COG", SMALL_SCENERY_FLAG_COG },
                { "allowSupportsAbove", SMALL_SCENERY_FLAG_BUILD_DIRECTLY_ONTOP },
                { "supportsHavePrimaryColour", SMALL_SCENERY_FLAG_PAINT_SUPPORTS },
                { "SMALL_SCENERY_FLAG27", SMALL_SCENERY_FLAG27 },
                { "isTree", SMALL_SCENERY_FLAG_IS_TREE },
                { "hasTertiaryColour", SMALL_SCENERY_FLAG_HAS_TERTIARY_COLOUR },
            });

        // Determine shape flags from a shape string
        auto shape = Json::GetString(properties["shape"]);
        if (!shape.empty())
        {
            auto quarters = shape.substr(0, 3);
            if (quarters == "2/4")
            {
                _legacyType.flags |= SMALL_SCENERY_FLAG_FULL_TILE | SMALL_SCENERY_FLAG_HALF_SPACE;
            }
            else if (quarters == "3/4")
            {
                _legacyType.flags |= SMALL_SCENERY_FLAG_FULL_TILE | SMALL_SCENERY_FLAG_THREE_QUARTERS;
            }
            else if (quarters == "4/4")
            {
                _legacyType.flags |= SMALL_SCENERY_FLAG_FULL_TILE;
            }
            if (shape.size() >= 5)
            {
                if ((shape.substr(3) == "+D"))
                {
                    _legacyType.flags |= SMALL_SCENERY_FLAG_DIAGONAL;
                }
            }
        }

        auto jFrameOffsets = properties["frameOffsets"];
        if (jFrameOffsets.is_array())
        {
            _frameOffsets = ReadJsonFrameOffsets(jFrameOffsets);
            _legacyType.flags |= SMALL_SCENERY_FLAG_HAS_FRAME_OFFSETS;
        }

        SetPrimarySceneryGroup(ObjectEntryDescriptor(Json::GetString(properties["sceneryGroup"])));
    }

    PopulateTablesFromJson(context, root);
}

std::vector<uint8_t> SmallSceneryObject::ReadJsonFrameOffsets(json_t& jFrameOffsets)
{
    std::vector<uint8_t> offsets;

    for (const auto& jOffset : jFrameOffsets)
    {
        offsets.push_back(Json::GetNumber<uint8_t>(jOffset));
    }
    return offsets;
}
