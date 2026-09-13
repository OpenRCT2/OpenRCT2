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
    int32_t create(const char** argv, int32_t argc)
    {
        if (argc < 2)
        {
            fprintf(stderr, "usage: sprite create <spritefile>\n");
            return -1;
        }

        const utf8* spriteFilePath = argv[1];

        SpriteFile spriteFile;
        spriteFile.Save(spriteFilePath);
        return 0;
    }
} // namespace OpenRCT2::CommandLine::Sprite
