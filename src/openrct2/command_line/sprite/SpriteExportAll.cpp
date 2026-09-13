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
#include "../../core/Path.hpp"
#include "../../core/StringTypes.h"
#include "../../drawing/Drawing.h"
#include "../../object/Object.h"
#include "../../object/ObjectFactory.h"
#include "SpriteCommands.h"
#include "SpriteFile.h"

#include <cmath>
#include <cstdint>

namespace OpenRCT2::CommandLine::Sprite
{
    int32_t exportAll(const char** argv, int32_t argc)
    {
        if (argc < 3)
        {
            fprintf(stdout, "usage: sprite exportall <spritefile> <output directory>\n");
            return -1;
        }

        const utf8* spriteFilePath = argv[1];
        const utf8* outputPath = argv[2];

        auto spriteFile = SpriteFile::Open(spriteFilePath);
        if (!spriteFile.has_value())
        {
            fprintf(stderr, "Unable to open input sprite file.\n");
            return -1;
        }

        if (!Path::CreateDirectory(outputPath))
        {
            fprintf(stderr, "Unable to create directory.\n");
            return -1;
        }

        const uint32_t maxIndex = spriteFile->Header.numEntries;
        const int32_t numbers = static_cast<int32_t>(std::floor(std::log10(maxIndex) + 1));

        std::ostringstream oss; // TODO: Remove when C++20 is enabled and std::format can be used
        for (uint32_t spriteIndex = 0; spriteIndex < maxIndex; spriteIndex++)
        {
            // Status indicator
            printf("\r%u / %u, %u%%", spriteIndex + 1, maxIndex, ((spriteIndex + 1) * 100) / maxIndex);

            oss << std::setw(numbers) << std::setfill('0') << spriteIndex << ".png";

            const auto& spriteHeader = spriteFile->Entries[spriteIndex];
            if (!SpriteImageExport(spriteHeader, Path::Combine(outputPath, PopStr(oss))))
            {
                fprintf(stderr, "Could not export\n");
                return -1;
            }
        }
        return 0;
    }
} // namespace OpenRCT2::CommandLine::Sprite
