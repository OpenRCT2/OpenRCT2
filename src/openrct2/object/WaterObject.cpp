/*****************************************************************************
 * Copyright (c) 2014-2023 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma warning(disable : 4706) // assignment within conditional expression

#include "WaterObject.h"

#include "../OpenRCT2.h"
#include "../common.h"
#include "../core/IStream.hpp"
#include "../core/Json.hpp"
#include "../localisation/Formatter.h"
#include "../localisation/Language.h"
#include "../localisation/StringIds.h"
#include "../world/Location.hpp"

#include <array>
#include <cstring>
#include <memory>

void WaterObject::ReadLegacy(IReadObjectContext* context, OpenRCT2::IStream* stream)
{
    stream->Seek(14, OpenRCT2::STREAM_SEEK_CURRENT);
    _legacyType.flags = stream->ReadValue<uint16_t>();

    GetStringTable().Read(context, stream, ObjectStringID::NAME);
    GetImageTable().Read(context, stream);
}

void WaterObject::Load()
{
    GetStringTable().Sort();
    _legacyType.string_idx = LanguageAllocateObjectString(GetName());
    _legacyType.image_id = LoadImages();
    _legacyType.palette_index_1 = _legacyType.image_id + 1;
    _legacyType.palette_index_2 = _legacyType.image_id + 4;

    LoadPalette();
}

void WaterObject::Unload()
{
    UnloadImages();
    LanguageFreeObjectString(_legacyType.string_idx);

    _legacyType.string_idx = 0;
    _legacyType.image_id = 0;
}

void WaterObject::DrawPreview(DrawPixelInfo& dpi, int32_t width, int32_t height) const
{
    // Write (no image)
    auto screenCoords = ScreenCoordsXY{ width / 2, height / 2 };
    DrawTextBasic(dpi, screenCoords, STR_WINDOW_NO_IMAGE, {}, { TextAlignment::CENTRE });
}

void WaterObject::ReadJson([[maybe_unused]] IReadObjectContext* context, json_t& root)
{
    Guard::Assert(root.is_object(), "WaterObject::ReadJson expects parameter root to be object");

    auto properties = root["properties"];

    PopulateTablesFromJson(context, root);

    if (properties.is_object())
    {
        _legacyType.flags = Json::GetFlags<uint16_t>(
            properties,
            {
                { "allowDucks", WATER_FLAGS_ALLOW_DUCKS },
            });

        auto jPalettes = properties["palettes"];
        if (jPalettes.is_object())
        {
            // Images which are actually palette data
            static const char* paletteNames[] = {
                "general", "waves-0", "waves-1", "waves-2", "sparkles-0", "sparkles-1", "sparkles-2",
            };
            for (auto paletteName : paletteNames)
            {
                auto jPalette = jPalettes[paletteName];
                if (jPalette.is_object())
                {
                    ReadJsonPalette(jPalette);
                }
            }
        }
    }
}

void WaterObject::ReadJsonPalette(json_t& jPalette)
{
    Guard::Assert(jPalette.is_object(), "WaterObject::ReadJsonPalette expects parameter jPalette to be object");

    auto jColours = jPalette["colours"];
    auto numColours = jColours.size();

    // This pointer gets memcopied in ImageTable::AddImage so it's fine for the unique_ptr to go out of scope
    auto data = std::make_unique<uint8_t[]>(numColours * 3);
    size_t dataIndex = 0;

    for (auto& jColour : jColours)
    {
        if (jColour.is_string())
        {
            auto colour = ParseColour(Json::GetString(jColour));
            data[dataIndex + 0] = (colour >> 16) & 0xFF;
            data[dataIndex + 1] = (colour >> 8) & 0xFF;
            data[dataIndex + 2] = colour & 0xFF;
        }
        dataIndex += 3;
    }

    G1Element g1 = {};
    g1.offset = data.get();
    g1.width = static_cast<int16_t>(numColours);
    g1.x_offset = Json::GetNumber<int16_t>(jPalette["index"]);
    g1.flags = G1_FLAG_PALETTE;

    auto& imageTable = GetImageTable();
    imageTable.AddImage(&g1);
}

uint32_t WaterObject::ParseColour(const std::string& s) const
{
    uint8_t r = 0;
    uint8_t g = 0;
    uint8_t b = 0;
    if (s[0] == '#' && s.size() == 7)
    {
        // Expect #RRGGBB
        r = std::stoul(s.substr(1, 2), nullptr, 16) & 0xFF;
        g = std::stoul(s.substr(3, 2), nullptr, 16) & 0xFF;
        b = std::stoul(s.substr(5, 2), nullptr, 16) & 0xFF;
    }
    return (b << 16) | (g << 8) | r;
}
