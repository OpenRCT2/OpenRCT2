/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "PathAdditionObject.h"

#include "../core/Guard.hpp"
#include "../core/IStream.hpp"
#include "../core/Json.hpp"
#include "../drawing/Drawing.h"
#include "../interface/Cursors.h"
#include "ObjectList.h"
#include "ObjectRepository.h"

#include <unordered_map>

namespace OpenRCT2
{
    void PathAdditionObject::ReadLegacy(IReadObjectContext* context, IStream* stream)
    {
        stream->Seek(6, STREAM_SEEK_CURRENT);
        _legacyType.flags = stream->ReadValue<uint16_t>();
        _legacyType.draw_type = static_cast<PathAdditionDrawType>(stream->ReadValue<uint8_t>());
        _legacyType.tool_id = static_cast<CursorID>(stream->ReadValue<uint8_t>());
        _legacyType.price = stream->ReadValue<money16>();
        _legacyType.scenery_tab_id = kObjectEntryIndexNull;
        stream->Seek(2, STREAM_SEEK_CURRENT);

        GetStringTable().Read(context, stream, ObjectStringID::name);

        RCTObjectEntry sgEntry = stream->ReadValue<RCTObjectEntry>();
        SetPrimarySceneryGroup(ObjectEntryDescriptor(sgEntry));

        GetImageTable().Read(context, stream);

        // Validate properties
        if (_legacyType.price <= 0.00_GBP)
        {
            context->LogError(ObjectError::invalidProperty, "Price can not be free or negative.");
        }

        // Add path additions to 'Signs and items for footpaths' group, rather than lumping them in the Miscellaneous tab.
        // Since this is already done the other way round for original items, avoid adding those to prevent duplicates.
        auto firstSourceGame = GetFirstSourceGame();
        if (firstSourceGame == ObjectSourceGame::custom)
        {
            auto scgPathX = Object::GetScgPathXHeader();
            SetPrimarySceneryGroup(scgPathX);
        }
    }

    void PathAdditionObject::Load()
    {
        GetStringTable().Sort();
        _legacyType.name = LanguageAllocateObjectString(GetName());
        _legacyType.image = LoadImages();

        _legacyType.scenery_tab_id = kObjectEntryIndexNull;
    }

    void PathAdditionObject::Unload()
    {
        LanguageFreeObjectString(_legacyType.name);
        UnloadImages();

        _legacyType.name = 0;
        _legacyType.image = 0;
    }

    void PathAdditionObject::DrawPreview(Drawing::RenderTarget& rt, int32_t width, int32_t height) const
    {
        auto screenCoords = ScreenCoordsXY{ width / 2, height / 2 };
        GfxDrawSprite(rt, ImageId(_legacyType.image), screenCoords - ScreenCoordsXY{ 22, 24 });
    }

    static PathAdditionDrawType ParseDrawType(const std::string& s)
    {
        if (s == "lamp")
            return PathAdditionDrawType::light;
        if (s == "bin")
            return PathAdditionDrawType::bin;
        if (s == "bench")
            return PathAdditionDrawType::bench;
        if (s == "fountain")
            return PathAdditionDrawType::jumpingFountain;
        return PathAdditionDrawType::light;
    }

    void PathAdditionObject::ReadJson(IReadObjectContext* context, json_t& root)
    {
        Guard::Assert(root.is_object(), "PathAdditionObject::ReadJson expects parameter root to be object");

        json_t properties = root["properties"];

        if (properties.is_object())
        {
            _legacyType.draw_type = ParseDrawType(Json::GetString(properties["renderAs"]));
            _legacyType.tool_id = Cursor::FromString(Json::GetString(properties["cursor"]), CursorID::LamppostDown);
            _legacyType.price = Json::GetNumber<money64>(properties["price"]);

            SetPrimarySceneryGroup(ObjectEntryDescriptor(Json::GetString(properties["sceneryGroup"])));

            // clang-format off
        _legacyType.flags = Json::GetFlags<uint16_t>(
            properties,
            {
                { "isBin",                  PATH_ADDITION_FLAG_IS_BIN,                   Json::FlagType::Normal },
                { "isBench",                PATH_ADDITION_FLAG_IS_BENCH,                 Json::FlagType::Normal },
                { "isBreakable",            PATH_ADDITION_FLAG_BREAKABLE,                Json::FlagType::Normal },
                { "isLamp",                 PATH_ADDITION_FLAG_LAMP,                     Json::FlagType::Normal },
                { "isJumpingFountainWater", PATH_ADDITION_FLAG_JUMPING_FOUNTAIN_WATER,   Json::FlagType::Normal },
                { "isJumpingFountainSnow",  PATH_ADDITION_FLAG_JUMPING_FOUNTAIN_SNOW,    Json::FlagType::Normal },
                { "isAllowedOnQueue",       PATH_ADDITION_FLAG_DONT_ALLOW_ON_QUEUE,      Json::FlagType::Inverted },
                { "isAllowedOnSlope",       PATH_ADDITION_FLAG_DONT_ALLOW_ON_SLOPE,      Json::FlagType::Inverted },
                { "isTelevision",           PATH_ADDITION_FLAG_IS_QUEUE_SCREEN,          Json::FlagType::Normal },
            });
            // clang-format on
        }

        PopulateTablesFromJson(context, root);
    }
} // namespace OpenRCT2
