/*****************************************************************************
 * Copyright (c) 2014-2023 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "PathAdditionObject.h"

#include "../core/IStream.hpp"
#include "../core/Json.hpp"
#include "../drawing/Drawing.h"
#include "../interface/Cursors.h"
#include "../localisation/Localisation.h"
#include "../object/Object.h"
#include "../object/ObjectRepository.h"
#include "ObjectList.h"

#include <unordered_map>

void PathAdditionObject::ReadLegacy(IReadObjectContext* context, OpenRCT2::IStream* stream)
{
    stream->Seek(6, OpenRCT2::STREAM_SEEK_CURRENT);
    _legacyType.flags = stream->ReadValue<uint16_t>();
    _legacyType.draw_type = static_cast<PathAdditionDrawType>(stream->ReadValue<uint8_t>());
    _legacyType.tool_id = static_cast<CursorID>(stream->ReadValue<uint8_t>());
    _legacyType.price = stream->ReadValue<money16>();
    _legacyType.scenery_tab_id = OBJECT_ENTRY_INDEX_NULL;
    stream->Seek(2, OpenRCT2::STREAM_SEEK_CURRENT);

    GetStringTable().Read(context, stream, ObjectStringID::NAME);

    RCTObjectEntry sgEntry = stream->ReadValue<RCTObjectEntry>();
    SetPrimarySceneryGroup(ObjectEntryDescriptor(sgEntry));

    GetImageTable().Read(context, stream);

    // Validate properties
    if (_legacyType.price <= 0.00_GBP)
    {
        context->LogError(ObjectError::InvalidProperty, "Price can not be free or negative.");
    }

    // Add path additions to 'Signs and items for footpaths' group, rather than lumping them in the Miscellaneous tab.
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

void PathAdditionObject::Load()
{
    GetStringTable().Sort();
    _legacyType.name = LanguageAllocateObjectString(GetName());
    _legacyType.image = LoadImages();

    _legacyType.scenery_tab_id = OBJECT_ENTRY_INDEX_NULL;
}

void PathAdditionObject::Unload()
{
    LanguageFreeObjectString(_legacyType.name);
    UnloadImages();

    _legacyType.name = 0;
    _legacyType.image = 0;
}

void PathAdditionObject::DrawPreview(DrawPixelInfo& dpi, int32_t width, int32_t height) const
{
    auto screenCoords = ScreenCoordsXY{ width / 2, height / 2 };
    GfxDrawSprite(dpi, ImageId(_legacyType.image), screenCoords - ScreenCoordsXY{ 22, 24 });
}

static PathAdditionDrawType ParseDrawType(const std::string& s)
{
    if (s == "lamp")
        return PathAdditionDrawType::Light;
    if (s == "bin")
        return PathAdditionDrawType::Bin;
    if (s == "bench")
        return PathAdditionDrawType::Bench;
    if (s == "fountain")
        return PathAdditionDrawType::JumpingFountain;
    return PathAdditionDrawType::Light;
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
