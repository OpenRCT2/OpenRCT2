/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma warning(disable : 4706) // assignment within conditional expression

#include "WaterObject.h"

#include "../OpenRCT2.h"
#include "../core/IStream.hpp"
#include "../core/Json.hpp"
#include "../drawing/Drawing.h"
#include "../drawing/ImageImporter.h"
#include "../localisation/Formatter.h"
#include "../localisation/Language.h"
#include "../localisation/StringIds.h"
#include "../world/Location.hpp"

#include <array>
#include <cstring>
#include <memory>

namespace OpenRCT2
{
    void WaterObject::ReadLegacy(IReadObjectContext* context, IStream* stream)
    {
        stream->Seek(14, STREAM_SEEK_CURRENT);
        _legacyType.flags = stream->ReadValue<uint16_t>();

        GetStringTable().Read(context, stream, ObjectStringID::name);
        GetImageTable().Read(context, stream);
    }

    void WaterObject::Load()
    {
        GetStringTable().Sort();
        _legacyType.stringId = LanguageAllocateObjectString(GetName());
        _legacyType.mainPalette = LoadImages();
        _legacyType.waterWavesPalette = _legacyType.mainPalette + 1;
        _legacyType.waterSparklesPalette = _legacyType.mainPalette + 4;

        LoadPalette();
    }

    void WaterObject::Unload()
    {
        UnloadImages();
        LanguageFreeObjectString(_legacyType.stringId);

        _legacyType.stringId = 0;
        _legacyType.mainPalette = 0;
        _legacyType.waterWavesPalette = 0;
        _legacyType.waterSparklesPalette = 0;
    }

    void WaterObject::DrawPreview(Drawing::RenderTarget& rt, int32_t width, int32_t height) const
    {
        // Write (no image)
        auto screenCoords = ScreenCoordsXY{ width / 2, height / 2 };
        DrawTextBasic(rt, screenCoords, STR_WINDOW_NO_IMAGE, {}, { TextAlignment::centre });
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
        auto importer = Drawing::ImageImporter();
        const auto importResult = importer.importJSONPalette(jPalette);

        auto& imageTable = GetImageTable();
        imageTable.addPalette(importResult.element);
    }
} // namespace OpenRCT2
