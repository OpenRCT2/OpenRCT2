/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "../../Context.h"
#include "../../core/FileStream.h"
#include "../../core/Json.hpp"
#include "../../core/Path.hpp"
#include "../../core/StringTypes.h"
#include "../../drawing/Drawing.h"
#include "../../object/Object.h"
#include "../../object/ObjectFactory.h"
#include "SpriteCommands.h"
#include "SpriteFile.h"

#include <cmath>
#include <cstdint>
#include <cstring>
#include <sstream>

namespace OpenRCT2::CommandLine::Sprite
{
    ExitCode exportAll(const char** argv, int32_t argc)
    {
        if (argc < 3)
        {
            fprintf(stdout, "usage: sprite exportall <spritefile> <output directory>\n");
            return ExitCode::fail;
        }

        const utf8* spriteFilePath = argv[1];
        const utf8* outputPath = argv[2];

        auto spriteFile = SpriteFile::Open(spriteFilePath);
        if (!spriteFile.has_value())
        {
            fprintf(stderr, "Unable to open input sprite file.\n");
            return ExitCode::fail;
        }

        if (!Path::CreateDirectory(outputPath))
        {
            fprintf(stderr, "Unable to create directory.\n");
            return ExitCode::fail;
        }

        const uint32_t numEntries = spriteFile->Header.numEntries;
        const int32_t numbers = static_cast<int32_t>(std::floor(std::log10(numEntries) + 1));

        auto jsonEntries = nlohmann::json::array();

        std::ostringstream oss; // TODO: Remove when C++20 is enabled and std::format can be used
        for (uint32_t spriteIndex = 0; spriteIndex < numEntries; spriteIndex++)
        {
            // Status indicator
            printf("\r%u / %u, %u%%", spriteIndex + 1, numEntries, ((spriteIndex + 1) * 100) / numEntries);

            oss << std::setw(numbers) << std::setfill('0') << spriteIndex << ".png";
            auto localFilename = PopStr(oss);

            const auto& spriteHeader = spriteFile->Entries[spriteIndex];

            if (spriteHeader.flags.has(G1Flag::isPalette))
            {
                auto palette = spriteHeader.asPalette();
                auto jsonColours = nlohmann::json::array();

                for (auto colourIndex = 0; colourIndex < palette->numColours; colourIndex++)
                {
                    auto& colour = palette->palette[colourIndex];
                    utf8 colourString[8];
                    snprintf(colourString, sizeof(colourString), "#%02X%02X%02X", colour.red, colour.green, colour.blue);
                    jsonColours.push_back(colourString);
                }

                jsonEntries.push_back(
                    {
                        { "index", palette->startIndex },
                        { "colours", jsonColours },
                    });
            }
            else
            {
                if (!SpriteImageExport(spriteHeader, Path::Combine(outputPath, localFilename)))
                {
                    fprintf(stderr, "Could not export\n");
                    return ExitCode::fail;
                }

                u8string format = spriteHeader.flags.has(G1Flag::hasRLECompression) ? "rle" : "raw";
                json_t entry = {
                    { "path", localFilename },
                    { "format", format },
                };
                if (spriteHeader.xOffset != 0)
                    entry["x"] = spriteHeader.xOffset;
                if (spriteHeader.yOffset != 0)
                    entry["y"] = spriteHeader.yOffset;
                if (spriteHeader.flags.has(G1Flag::hasZoomSprite) && spriteHeader.zoomedOffset != 0)
                    entry["zoom"] = spriteHeader.zoomedOffset;
                if (spriteHeader.flags.has(G1Flag::noZoomDraw))
                    entry["noDrawOnZoom"] = true;

                jsonEntries.push_back(entry);
            }
        }

        auto outputResourcesPath = Path::Combine(outputPath, "sprites.json");
        Json::WriteToFile(outputResourcesPath, jsonEntries);

        return ExitCode::ok;
    }
} // namespace OpenRCT2::CommandLine::Sprite
