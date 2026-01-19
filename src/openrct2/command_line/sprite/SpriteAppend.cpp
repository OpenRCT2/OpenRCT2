/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "../../core/FileStream.h"
#include "../../drawing/Drawing.h"
#include "SpriteCommands.h"
#include "SpriteFile.h"

#include <cstdint>

using namespace OpenRCT2::Drawing;

namespace OpenRCT2::CommandLine::Sprite
{
    int32_t append(const char** argv, int32_t argc, ImportMode spriteMode)
    {
        if (argc != 3 && argc != 5)
        {
            fprintf(stderr, "usage: sprite append <spritefile> <input> [<x offset> <y offset>]\n");
            return -1;
        }

        const utf8* spriteFilePath = argv[1];
        const utf8* imagePath = argv[2];
        int16_t xOffset = 0;
        int16_t yOffset = 0;

        if (argc == 5)
        {
            char* endptr;

            xOffset = strtol(argv[3], &endptr, 0);
            if (*endptr != 0)
            {
                fprintf(stderr, "X offset must be an integer\n");
                return -1;
            }

            yOffset = strtol(argv[4], &endptr, 0);
            if (*endptr != 0)
            {
                fprintf(stderr, "Y offset must be an integer\n");
                return -1;
            }
        }

        constexpr uint8_t importFlags = EnumToFlag(ImportFlags::RLE);
        ImageImportMeta meta = { { xOffset, yOffset }, Palette::OpenRCT2, importFlags, spriteMode };
        const auto image = SpriteImageLoad(imagePath, meta);
        if (!image.has_value())
            return -1;
        ImageImporter importer;
        auto importResult = importer.Import(image.value(), meta);

        auto spriteFile = SpriteFile::Open(spriteFilePath);
        if (!spriteFile.has_value())
        {
            fprintf(stderr, "Unable to open input sprite file.\n");
            return -1;
        }

        spriteFile->AddImage(importResult);

        if (!spriteFile->Save(spriteFilePath))
            return -1;

        return 0;
    }
} // namespace OpenRCT2::CommandLine::Sprite
