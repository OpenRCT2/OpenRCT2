/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
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

namespace OpenRCT2
{
    void SmallSceneryObject::ReadLegacy(IReadObjectContext* context, IStream* stream)
    {
        stream->Seek(6, STREAM_SEEK_CURRENT);
        _legacyType.flags = stream->ReadValue<SmallSceneryFlags>();
        _legacyType.height = stream->ReadValue<uint8_t>();
        _legacyType.tool_id = static_cast<CursorID>(stream->ReadValue<uint8_t>());
        _legacyType.price = stream->ReadValue<int16_t>() * 10;
        _legacyType.removal_price = stream->ReadValue<int16_t>() * 10;
        stream->Seek(4, STREAM_SEEK_CURRENT);
        _legacyType.animation_delay = stream->ReadValue<uint16_t>();
        _legacyType.animation_mask = stream->ReadValue<uint16_t>();
        _legacyType.num_frames = stream->ReadValue<uint16_t>();
        _legacyType.scenery_tab_id = kObjectEntryIndexNull;
        // Skip past the reserved space for the scenery_tab_id and an unused byte - the string table starts at offset 0x1C.
        stream->Seek(2, STREAM_SEEK_CURRENT);

        GetStringTable().Read(context, stream, ObjectStringID::name);

        RCTObjectEntry sgEntry = stream->ReadValue<RCTObjectEntry>();
        SetPrimarySceneryGroup(ObjectEntryDescriptor(sgEntry));

        if (_legacyType.flags.has(SmallSceneryFlag::hasFrameOffsets))
        {
            _frameOffsets = ReadFrameOffsets(stream);
        }
        // This crude method was used by RCT2. JSON objects have a flag for this property.
        if (_legacyType.height > 64)
        {
            _legacyType.flags.set(SmallSceneryFlag::isTree);
        }

        GetImageTable().Read(context, stream);

        // Validate properties
        if (_legacyType.price <= 0.00_GBP)
        {
            context->LogError(ObjectError::invalidProperty, "Price can not be free or negative.");
        }
        if (_legacyType.removal_price <= 0.00_GBP)
        {
            // Make sure you don't make a profit when placing then removing.
            const auto reimbursement = _legacyType.removal_price;
            if (reimbursement > _legacyType.price)
            {
                context->LogError(ObjectError::invalidProperty, "Sell price can not be more than buy price.");
            }
        }
    }

    void SmallSceneryObject::Load()
    {
        GetStringTable().Sort();
        _legacyType.name = LanguageAllocateObjectString(GetName());
        _legacyType.image = LoadImages();

        _legacyType.scenery_tab_id = kObjectEntryIndexNull;
        _legacyType.FrameOffsetCount = 0;

        if (_legacyType.flags.has(SmallSceneryFlag::hasFrameOffsets))
        {
            _legacyType.frame_offsets = _frameOffsets.data();
            _legacyType.FrameOffsetCount = static_cast<uint16_t>(_frameOffsets.size());
        }

        PerformFixes();
    }

    void SmallSceneryObject::Unload()
    {
        LanguageFreeObjectString(_legacyType.name);
        UnloadImages();

        _legacyType.name = 0;
        _legacyType.image = 0;
    }

    void SmallSceneryObject::DrawPreview(Drawing::RenderTarget& rt, int32_t width, int32_t height) const
    {
        auto imageId = ImageId(_legacyType.image);
        if (_legacyType.flags.has(SmallSceneryFlag::hasPrimaryColour))
        {
            imageId = imageId.WithPrimary(Drawing::Colour::bordeauxRed);
            if (_legacyType.flags.has(SmallSceneryFlag::hasSecondaryColour))
            {
                imageId = imageId.WithSecondary(Drawing::Colour::yellow);
            }
        }
        if (_legacyType.flags.has(SmallSceneryFlag::hasTertiaryColour))
        {
            imageId = imageId.WithSecondary(Drawing::Colour::darkBrown);
        }

        auto screenCoords = ScreenCoordsXY{ width / 2, (height / 2) + (_legacyType.height / 2) };
        screenCoords.y = std::min(screenCoords.y, height - 16);

        if (_legacyType.flags.hasAll(SmallSceneryFlag::occupiesFullTile, SmallSceneryFlag::vOffsetCentre))
        {
            screenCoords.y -= 12;
        }

        GfxDrawSprite(rt, imageId, screenCoords);

        if (_legacyType.flags.has(SmallSceneryFlag::hasGlass))
        {
            imageId = ImageId(_legacyType.image + 4).WithTransparency(Drawing::Colour::bordeauxRed);
            GfxDrawSprite(rt, imageId, screenCoords);
        }

        if (_legacyType.flags.has(SmallSceneryFlag::hasOverlayImage))
        {
            imageId = ImageId(_legacyType.image + 4);
            if (_legacyType.flags.has(SmallSceneryFlag::hasSecondaryColour))
            {
                imageId = imageId.WithSecondary(Drawing::Colour::yellow);
            }
            GfxDrawSprite(rt, imageId, screenCoords);
        }
    }

    std::vector<uint8_t> SmallSceneryObject::ReadFrameOffsets(IStream* stream)
    {
        uint8_t frameOffset;
        auto data = std::vector<uint8_t>();
        data.push_back(stream->ReadValue<uint8_t>());
        while ((frameOffset = stream->ReadValue<uint8_t>()) != 0xFF)
        {
            data.push_back(frameOffset);
        }
        return data;
    }

    // clang-format off
void SmallSceneryObject::PerformFixes()
{
    auto identifier = GetLegacyIdentifier();

    // ToonTowner's base blocks. Put them in the Walls and Roofs group.
    if (identifier == "XXBBCL01" ||
        identifier == "XXBBMD01" ||
        identifier == "ARBASE2 ")
    {
        SetPrimarySceneryGroup(ObjectEntryDescriptor("rct2.scenery_group.scgwalls"));
    }

    // ToonTowner's Pirate roofs. Make them show up in the Pirate Theming.
    if (identifier == "TTPIRF05" ||
        identifier == "TTPRF09 " ||
        identifier == "TTPRF10 " ||
        identifier == "TTPRF11 ")
    {
        SetPrimarySceneryGroup(ObjectEntryDescriptor("rct2.scenery_group.scgpirat"));
    }
}
    // clang-format on

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

            _legacyType.flags = Json::GetFlagHolder<SmallSceneryFlags, SmallSceneryFlag>(
                properties,
                {
                    { "SMALL_SCENERY_FLAG_VOFFSET_CENTRE", SmallSceneryFlag::vOffsetCentre },
                    { "requiresFlatSurface", SmallSceneryFlag::requiresFlatSurface },
                    { "isRotatable", SmallSceneryFlag::isRotatable },
                    { "isAnimated", SmallSceneryFlag::isAnimated },
                    { "canWither", SmallSceneryFlag::canWither },
                    { "canBeWatered", SmallSceneryFlag::canBeWatered },
                    { "hasOverlayImage", SmallSceneryFlag::hasOverlayImage },
                    { "hasGlass", SmallSceneryFlag::hasGlass },
                    { "hasPrimaryColour", SmallSceneryFlag::hasPrimaryColour },
                    { "SMALL_SCENERY_FLAG_FOUNTAIN_SPRAY_1", SmallSceneryFlag::isFountain },
                    { "SMALL_SCENERY_FLAG_FOUNTAIN_SPRAY_4", SmallSceneryFlag::isCupidFountain },
                    { "isClock", SmallSceneryFlag::isClock },
                    { "SMALL_SCENERY_FLAG_SWAMP_GOO", SmallSceneryFlag::isSwampGoo },
                    { "SMALL_SCENERY_FLAG17", SmallSceneryFlag::flag17 },
                    { "isStackable", SmallSceneryFlag::isStackable },
                    { "prohibitWalls", SmallSceneryFlag::prohibitWalls },
                    { "hasSecondaryColour", SmallSceneryFlag::hasSecondaryColour },
                    { "hasNoSupports", SmallSceneryFlag::hasNoSupports },
                    { "SMALL_SCENERY_FLAG_VISIBLE_WHEN_ZOOMED", SmallSceneryFlag::isVisibleWhenZoomed },
                    { "SMALL_SCENERY_FLAG_COG", SmallSceneryFlag::isCogwheel },
                    { "allowSupportsAbove", SmallSceneryFlag::allowSupportsAbove },
                    { "supportsHavePrimaryColour", SmallSceneryFlag::supportsHavePrimaryColour },
                    { "SMALL_SCENERY_FLAG27", SmallSceneryFlag::flag27 },
                    { "isTree", SmallSceneryFlag::isTree },
                    { "hasTertiaryColour", SmallSceneryFlag::hasTertiaryColour },
                });

            // Determine shape flags from a shape string
            auto shape = Json::GetString(properties["shape"]);
            if (!shape.empty())
            {
                auto quarters = shape.substr(0, 3);
                if (quarters == "2/4")
                {
                    _legacyType.flags.set(SmallSceneryFlag::occupiesFullTile, SmallSceneryFlag::occupiesHalfTile);
                }
                else if (quarters == "3/4")
                {
                    _legacyType.flags.set(SmallSceneryFlag::occupiesFullTile, SmallSceneryFlag::occupiesThreeQuarters);
                }
                else if (quarters == "4/4")
                {
                    _legacyType.flags.set(SmallSceneryFlag::occupiesFullTile);
                }
                if (shape.size() >= 5)
                {
                    if (shape.substr(3) == "+D")
                    {
                        _legacyType.flags.set(SmallSceneryFlag::isDiagonal);
                    }
                }
            }

            auto jFrameOffsets = properties["frameOffsets"];
            if (jFrameOffsets.is_array())
            {
                _frameOffsets = ReadJsonFrameOffsets(jFrameOffsets);
                _legacyType.flags.set(SmallSceneryFlag::hasFrameOffsets);
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
} // namespace OpenRCT2
