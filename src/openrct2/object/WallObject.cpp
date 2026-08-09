/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "WallObject.h"

#include "../core/Guard.hpp"
#include "../core/IStream.hpp"
#include "../core/Json.hpp"
#include "../drawing/Drawing.h"
#include "../drawing/ScrollingText.h"
#include "../interface/Cursors.h"
#include "../localisation/Language.h"
#include "../world/Location.hpp"

namespace OpenRCT2
{
    static constexpr uint8_t kFlags2DoorSoundMask = 0b0110;
    static constexpr uint8_t kFlags2DoorSoundShift = 1;

    void WallObject::ReadLegacy(IReadObjectContext* context, IStream* stream)
    {
        stream->Seek(6, STREAM_SEEK_CURRENT);
        _legacyType.tool_id = static_cast<CursorID>(stream->ReadValue<uint8_t>());
        _legacyType.flags = static_cast<WallSceneryFlags>(stream->ReadValue<uint8_t>());
        _legacyType.height = stream->ReadValue<uint8_t>();
        auto combinedFlagsAndDoorSound = stream->ReadValue<uint8_t>();
        auto doorSound = (combinedFlagsAndDoorSound & kFlags2DoorSoundMask) >> kFlags2DoorSoundShift;
        auto flags2 = combinedFlagsAndDoorSound &= ~kFlags2DoorSoundMask;
        _legacyType.flags2.holder = flags2;
        _legacyType.doorSound = static_cast<Audio::DoorSoundType>(doorSound);
        _legacyType.price = stream->ReadValue<money16>();
        _legacyType.scenery_tab_id = kObjectEntryIndexNull;
        stream->Seek(1, STREAM_SEEK_CURRENT);
        _legacyType.scrolling_mode = stream->ReadValue<uint8_t>();

        GetStringTable().Read(context, stream, ObjectStringID::name);

        RCTObjectEntry sgEntry = stream->ReadValue<RCTObjectEntry>();
        SetPrimarySceneryGroup(ObjectEntryDescriptor(sgEntry));

        GetImageTable().Read(context, stream);

        // Validate properties
        if (_legacyType.price <= 0.00_GBP)
        {
            context->LogError(ObjectError::invalidProperty, "Price can not be free or negative.");
        }

        // Autofix this object (will be turned into an official object later).
        auto identifier = GetLegacyIdentifier();
        if (identifier == "XXWLBR03")
        {
            _legacyType.doorSound = Audio::DoorSoundType::door;
        }
    }

    void WallObject::Load()
    {
        GetStringTable().Sort();
        _legacyType.name = LanguageAllocateObjectString(GetName());
        _legacyType.image = LoadImages();
    }

    void WallObject::Unload()
    {
        LanguageFreeObjectString(_legacyType.name);
        UnloadImages();

        _legacyType.name = 0;
        _legacyType.image = 0;
    }

    void WallObject::DrawPreview(Drawing::RenderTarget& rt, int32_t width, int32_t height) const
    {
        auto screenCoords = ScreenCoordsXY{ width / 2, height / 2 };

        screenCoords.x += 14;
        screenCoords.y += (_legacyType.height * 2) + 16;

        auto imageId = ImageId(_legacyType.image, Drawing::Colour::bordeauxRed);
        if (_legacyType.flags.has(WallSceneryFlag::hasSecondaryColour))
        {
            imageId = imageId.WithSecondary(Drawing::Colour::yellow);
        }

        GfxDrawSprite(rt, imageId, screenCoords);

        if (_legacyType.flags.has(WallSceneryFlag::hasGlass))
        {
            auto glassImageId = imageId.WithTransparency(Drawing::Colour::bordeauxRed).WithIndexOffset(6);
            GfxDrawSprite(rt, glassImageId, screenCoords);
        }
        else if (_legacyType.flags.has(WallSceneryFlag::isDoor))
        {
            GfxDrawSprite(rt, imageId.WithIndexOffset(1), screenCoords);
        }
    }

    void WallObject::ReadJson(IReadObjectContext* context, json_t& root)
    {
        Guard::Assert(root.is_object(), "WallObject::ReadJson expects parameter root to be object");

        auto properties = root["properties"];

        if (properties.is_object())
        {
            _legacyType.tool_id = Cursor::FromString(Json::GetString(properties["cursor"]), CursorID::fenceDown);
            _legacyType.height = Json::GetNumber<uint8_t>(properties["height"]);
            _legacyType.price = Json::GetNumber<money64>(properties["price"]);

            _legacyType.scrolling_mode = Json::GetNumber<uint8_t>(properties["scrollingMode"], kScrollingModeNone);

            SetPrimarySceneryGroup(ObjectEntryDescriptor(Json::GetString(properties["sceneryGroup"])));

            // clang-format off
        _legacyType.flags = Json::GetFlagHolder<WallSceneryFlags, WallSceneryFlag>(
            properties,
            {
                { "hasPrimaryColour",       WallSceneryFlag::hasPrimaryColour,     Json::FlagType::normal },
                { "isAllowedOnSlope",       WallSceneryFlag::cannotBuildOnSlope,   Json::FlagType::inverted },
                { "hasSecondaryColour",     WallSceneryFlag::hasSecondaryColour,   Json::FlagType::normal },
                { "hasTertiaryColour",      WallSceneryFlag::hasTertiaryColour,    Json::FlagType::normal },
                { "hasTernaryColour",       WallSceneryFlag::hasTertiaryColour,    Json::FlagType::normal },
                { "hasGlass",               WallSceneryFlag::hasGlass,             Json::FlagType::normal },
                { "isBanner",               WallSceneryFlag::isDoubleSided,        Json::FlagType::normal },
                { "isDoubleSided",          WallSceneryFlag::isDoubleSided,        Json::FlagType::normal },
                { "isDoor",                 WallSceneryFlag::isDoor,               Json::FlagType::normal },
                { "isLongDoorAnimation",    WallSceneryFlag::hasLongDoorAnimation, Json::FlagType::normal },
            });
            // clang-format on

            _legacyType.flags2 = Json::GetFlagHolder<WallSceneryFlags2, WallSceneryFlag2>(
                properties,
                {
                    { "isTransparent", WallSceneryFlag2::isTransparent },
                    // Deprecated because it did the opposite of what the name implied.
                    { "isOpaque", WallSceneryFlag2::isTransparent },
                    { "isAnimated", WallSceneryFlag2::isAnimated },
                });

            // HACK WallSceneryFlag::hasPrimaryColour actually means, has any colour but we simplify the
            //      JSON and handle this on load. We should change code base in future to reflect the JSON.
            if (!_legacyType.flags.has(WallSceneryFlag::hasPrimaryColour))
            {
                if (_legacyType.flags.hasAny(WallSceneryFlag::hasSecondaryColour, WallSceneryFlag::hasTertiaryColour))
                {
                    _legacyType.flags.set(WallSceneryFlag::hasPrimaryColour);
                    _legacyType.flags2.set(WallSceneryFlag2::disablePrimaryColour);
                }
            }

            // Door sound
            auto jDoorSound = properties["doorSound"];
            if (jDoorSound.is_number())
            {
                auto doorSound = Json::GetNumber<uint8_t>(jDoorSound);
                _legacyType.doorSound = static_cast<Audio::DoorSoundType>(doorSound);
            }
        }

        PopulateTablesFromJson(context, root);
    }
} // namespace OpenRCT2
