/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "../../core/FileStream.h"
#include "../../core/StringTypes.h"
#include "../../drawing/Drawing.h"
#include "SpriteCommands.h"
#include "SpriteFile.h"

#include <cstdint>

namespace OpenRCT2::CommandLine::Sprite
{
    ExitCode details(const char** argv, int32_t argc)
    {
        if (argc < 2)
        {
            fprintf(stdout, "usage: sprite details <spritefile> [idx]\n");
            return ExitCode::fail;
        }

        if (argc == 2)
        {
            const utf8* spriteFilePath = argv[1];
            auto spriteFile = SpriteFile::Open(spriteFilePath);
            if (!spriteFile.has_value())
            {
                fprintf(stderr, "Unable to open input sprite file.\n");
                return ExitCode::fail;
            }

            printf("sprites: %u\n", spriteFile->Header.numEntries);
            printf("data size: %u\n", spriteFile->Header.totalSize);
            return ExitCode::ok;
        }

        const utf8* spriteFilePath = argv[1];
        int32_t spriteIndex = atoi(argv[2]);
        auto spriteFile = SpriteFile::Open(spriteFilePath);
        if (!spriteFile.has_value())
        {
            fprintf(stderr, "Unable to open input sprite file.\n");
            return ExitCode::fail;
        }

        if (spriteIndex < 0 || spriteIndex >= static_cast<int32_t>(spriteFile->Header.numEntries))
        {
            fprintf(stderr, "Sprite #%d does not exist in sprite file.\n", spriteIndex);
            return ExitCode::fail;
        }

        G1Element* g1 = &spriteFile->Entries[spriteIndex];
        printf("width: %d\n", g1->width);
        printf("height: %d\n", g1->height);
        printf("x offset: %d\n", g1->xOffset);
        printf("y offset: %d\n", g1->yOffset);
        printf("data offset: %p\n", g1->offset);
        return ExitCode::ok;
    }
} // namespace OpenRCT2::CommandLine::Sprite
