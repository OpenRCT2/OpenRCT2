/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "../../core/FileStream.h"
#include "../../drawing/Drawing.h"
#include "SpriteCommands.h"

#include <cstdint>

namespace OpenRCT2::CommandLine::Sprite
{
    int32_t combine(const char** argv, int32_t argc)
    {
        if (argc < 4)
        {
            fprintf(stdout, "usage: sprite combine <index file> <image file> <output>\n");
            return -1;
        }

        const utf8* indexFile = argv[1];
        const utf8* dataFile = argv[2];
        const utf8* outputPath = argv[3];

        auto fileHeader = FileStream(indexFile, FileMode::open);
        auto fileData = FileStream(dataFile, FileMode::open);
        auto fileHeaderSize = fileHeader.GetLength();
        auto fileDataSize = fileData.GetLength();

        uint32_t numEntries = fileHeaderSize / sizeof(RCTG1Element);

        RCTG1Header header = {};
        header.num_entries = numEntries;
        header.total_size = fileDataSize;
        FileStream outputStream(outputPath, FileMode::write);

        outputStream.Write(&header, sizeof(RCTG1Header));
        auto g1Elements32 = std::make_unique<RCTG1Element[]>(numEntries);
        fileHeader.Read(g1Elements32.get(), numEntries * sizeof(RCTG1Element));
        for (uint32_t i = 0; i < numEntries; i++)
        {
            // RCT1 used zoomed offsets that counted from the beginning of the file, rather than from the current sprite.
            if (g1Elements32[i].flags.has(G1Flag::hasZoomSprite))
            {
                g1Elements32[i].zoomed_offset = i - g1Elements32[i].zoomed_offset;
            }

            outputStream.Write(&g1Elements32[i], sizeof(RCTG1Element));
        }

        std::vector<uint8_t> data;
        data.resize(fileDataSize);
        fileData.Read(data.data(), fileDataSize);
        outputStream.Write(data.data(), fileDataSize);

        return 0;
    }
} // namespace OpenRCT2::CommandLine::Sprite
