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
    Exitcode exportSingle(const char** argv, int32_t argc)
    {
        if (argc < 4)
        {
            fprintf(stdout, "usage: sprite export <spritefile> <idx> <output>\n");
            return Exitcode::fail;
        }

        const utf8* spriteFilePath = argv[1];
        int32_t spriteIndex = atoi(argv[2]);
        const utf8* outputPath = argv[3];
        auto spriteFile = SpriteFile::Open(spriteFilePath);
        if (!spriteFile.has_value())
        {
            fprintf(stderr, "Unable to open input sprite file.\n");
            return Exitcode::fail;
        }

        if (spriteIndex < 0 || spriteIndex >= static_cast<int32_t>(spriteFile->Header.numEntries))
        {
            fprintf(stderr, "Sprite #%d does not exist in sprite file.\n", spriteIndex);
            return Exitcode::fail;
        }

        const auto& spriteHeader = spriteFile->Entries[spriteIndex];
        if (!SpriteImageExport(spriteHeader, outputPath))
        {
            fprintf(stderr, "Could not export\n");
            return Exitcode::fail;
        }
        fprintf(stdout, "{ \"x\": %d, \"y\": %d }\n", spriteHeader.xOffset, spriteHeader.yOffset);
        return Exitcode::ok;
    }
} // namespace OpenRCT2::CommandLine::Sprite
