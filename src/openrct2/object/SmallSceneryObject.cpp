/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
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
#include "../interface/Cursors.h"
#include "../localisation/Language.h"
#include "../world/Scenery.h"
#include "../world/SmallScenery.h"

#include <algorithm>

void SmallSceneryObject::ReadLegacy(IReadObjectContext* context, OpenRCT2::IStream* stream)
{
    stream->Seek(6, OpenRCT2::STREAM_SEEK_CURRENT);
    _legacyType.small_scenery.flags = stream->ReadValue<uint32_t>();
    _legacyType.small_scenery.height = stream->ReadValue<uint8_t>();
    _legacyType.small_scenery.tool_id = static_cast<CursorID>(stream->ReadValue<uint8_t>());
    _legacyType.small_scenery.price = stream->ReadValue<int16_t>();
    _legacyType.small_scenery.removal_price = stream->ReadValue<int16_t>();
    stream->Seek(4, OpenRCT2::STREAM_SEEK_CURRENT);
    _legacyType.small_scenery.animation_delay = stream->ReadValue<uint16_t>();
    _legacyType.small_scenery.animation_mask = stream->ReadValue<uint16_t>();
    _legacyType.small_scenery.num_frames = stream->ReadValue<uint16_t>();
    _legacyType.small_scenery.scenery_tab_id = OBJECT_ENTRY_INDEX_NULL;

    GetStringTable().Read(context, stream, ObjectStringID::NAME);

    rct_object_entry sgEntry = stream->ReadValue<rct_object_entry>();
    SetPrimarySceneryGroup(ObjectEntryDescriptor(sgEntry));

    if (scenery_small_entry_has_flag(&_legacyType, SMALL_SCENERY_FLAG_HAS_FRAME_OFFSETS))
    {
        _frameOffsets = ReadFrameOffsets(stream);
    }
    // This crude method was used by RCT2. JSON objects have a flag for this property.
    if (_legacyType.small_scenery.height > 64)
    {
        _legacyType.small_scenery.flags |= SMALL_SCENERY_FLAG_IS_TREE;
    }

    GetImageTable().Read(context, stream);

    // Validate properties
    if (_legacyType.small_scenery.price <= 0)
    {
        context->LogError(ObjectError::InvalidProperty, "Price can not be free or negative.");
    }
    if (_legacyType.small_scenery.removal_price <= 0)
    {
        // Make sure you don't make a profit when placing then removing.
        money16 reimbursement = _legacyType.small_scenery.removal_price;
        if (reimbursement > _legacyType.small_scenery.price)
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

    _legacyType.small_scenery.scenery_tab_id = OBJECT_ENTRY_INDEX_NULL;

    if (scenery_small_entry_has_flag(&_legacyType, SMALL_SCENERY_FLAG_HAS_FRAME_OFFSETS))
    {
        _legacyType.small_scenery.frame_offsets = _frameOffsets.data();
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
    uint32_t imageId = _legacyType.image;
    if (scenery_small_entry_has_flag(&_legacyType, SMALL_SCENERY_FLAG_HAS_PRIMARY_COLOUR))
    {
        imageId |= 0x20D00000;
        if (scenery_small_entry_has_flag(&_legacyType, SMALL_SCENERY_FLAG_HAS_SECONDARY_COLOUR))
        {
            imageId |= 0x92000000;
        }
    }

    auto screenCoords = ScreenCoordsXY{ width / 2, (height / 2) + (_legacyType.small_scenery.height / 2) };
    screenCoords.y = std::min(screenCoords.y, height - 16);

    if ((scenery_small_entry_has_flag(&_legacyType, SMALL_SCENERY_FLAG_FULL_TILE))
        && (scenery_small_entry_has_flag(&_legacyType, SMALL_SCENERY_FLAG_VOFFSET_CENTRE)))
    {
        screenCoords.y -= 12;
    }

    gfx_draw_sprite(dpi, imageId, screenCoords, 0);

    if (scenery_small_entry_has_flag(&_legacyType, SMALL_SCENERY_FLAG_HAS_GLASS))
    {
        imageId = _legacyType.image + 0x44500004;
        if (scenery_small_entry_has_flag(&_legacyType, SMALL_SCENERY_FLAG_HAS_SECONDARY_COLOUR))
        {
            imageId |= 0x92000000;
        }
        gfx_draw_sprite(dpi, imageId, screenCoords, 0);
    }

    if (scenery_small_entry_has_flag(&_legacyType, SMALL_SCENERY_FLAG_ANIMATED_FG))
    {
        imageId = _legacyType.image + 4;
        if (scenery_small_entry_has_flag(&_legacyType, SMALL_SCENERY_FLAG_HAS_SECONDARY_COLOUR))
        {
            imageId |= 0x92000000;
        }
        gfx_draw_sprite(dpi, imageId, screenCoords, 0);
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

    // ToonTowner's base blocks. Make them allow supports on top and put them in the Walls and Roofs group.
    if (identifier == "XXBBCL01" ||
        identifier == "XXBBMD01" ||
        identifier == "ARBASE2 ")
    {
        SetPrimarySceneryGroup(scgWalls);

        _legacyType.small_scenery.flags |= SMALL_SCENERY_FLAG_BUILD_DIRECTLY_ONTOP;
    }

    // ToonTowner's Pirate roofs. Make them show up in the Pirate Theming.
    if (identifier == "TTPIRF02" ||
        identifier == "TTPIRF03" ||
        identifier == "TTPIRF04" ||
        identifier == "TTPIRF05" ||
        identifier == "TTPIRF07" ||
        identifier == "TTPIRF08" ||
        identifier == "TTPRF09 " ||
        identifier == "TTPRF10 " ||
        identifier == "TTPRF11 ")
    {
        static const auto& scgPirat = GetScgPiratHeader();
        SetPrimarySceneryGroup(scgPirat);
    }

    // ToonTowner's wooden roofs. Make them show up in the Mine Theming.
    if (identifier == "TTRFWD01" ||
        identifier == "TTRFWD02" ||
        identifier == "TTRFWD03" ||
        identifier == "TTRFWD04" ||
        identifier == "TTRFWD05" ||
        identifier == "TTRFWD06" ||
        identifier == "TTRFWD07" ||
        identifier == "TTRFWD08")
    {
        static const auto& scgMine = GetScgMineHeader();
        SetPrimarySceneryGroup(scgMine);
    }

    // ToonTowner's glass roofs. Make them show up in the Abstract Theming.
    if (identifier == "TTRFGL01" ||
        identifier == "TTRFGL02" ||
        identifier == "TTRFGL03")
    {
        static const auto& scgAbstr = GetScgAbstrHeader();
        SetPrimarySceneryGroup(scgAbstr);
    }
}
// clang-format on

ObjectEntryDescriptor SmallSceneryObject::GetScgPiratHeader() const
{
    return ObjectEntryDescriptor("rct2.scgpirat");
}

ObjectEntryDescriptor SmallSceneryObject::GetScgMineHeader() const
{
    return ObjectEntryDescriptor("rct2.scgpirat");
}

ObjectEntryDescriptor SmallSceneryObject::GetScgAbstrHeader() const
{
    return ObjectEntryDescriptor("rct2.scgabstr");
}

void SmallSceneryObject::ReadJson(IReadObjectContext* context, json_t& root)
{
    Guard::Assert(root.is_object(), "SmallSceneryObject::ReadJson expects parameter root to be object");

    auto properties = root["properties"];

    if (properties.is_object())
    {
        _legacyType.small_scenery.height = Json::GetNumber<uint8_t>(properties["height"]);
        _legacyType.small_scenery.tool_id = Cursor::FromString(Json::GetString(properties["cursor"]), CursorID::StatueDown);
        _legacyType.small_scenery.price = Json::GetNumber<uint16_t>(properties["price"]);
        _legacyType.small_scenery.removal_price = Json::GetNumber<uint16_t>(properties["removalPrice"]);
        _legacyType.small_scenery.animation_delay = Json::GetNumber<uint16_t>(properties["animationDelay"]);
        _legacyType.small_scenery.animation_mask = Json::GetNumber<uint16_t>(properties["animationMask"]);
        _legacyType.small_scenery.num_frames = Json::GetNumber<uint16_t>(properties["numFrames"]);

        _legacyType.small_scenery.flags = Json::GetFlags<uint32_t>(
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
            });

        // Determine shape flags from a shape string
        auto shape = Json::GetString(properties["shape"]);
        if (!shape.empty())
        {
            auto quarters = shape.substr(0, 3);
            if (quarters == "2/4")
            {
                _legacyType.small_scenery.flags |= SMALL_SCENERY_FLAG_FULL_TILE | SMALL_SCENERY_FLAG_HALF_SPACE;
            }
            else if (quarters == "3/4")
            {
                _legacyType.small_scenery.flags |= SMALL_SCENERY_FLAG_FULL_TILE | SMALL_SCENERY_FLAG_THREE_QUARTERS;
            }
            else if (quarters == "4/4")
            {
                _legacyType.small_scenery.flags |= SMALL_SCENERY_FLAG_FULL_TILE;
            }
            if (shape.size() >= 5)
            {
                if ((shape.substr(3) == "+D"))
                {
                    _legacyType.small_scenery.flags |= SMALL_SCENERY_FLAG_DIAGONAL;
                }
            }
        }

        auto jFrameOffsets = properties["frameOffsets"];
        if (jFrameOffsets.is_array())
        {
            _frameOffsets = ReadJsonFrameOffsets(jFrameOffsets);
            _legacyType.small_scenery.flags |= SMALL_SCENERY_FLAG_HAS_FRAME_OFFSETS;
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
