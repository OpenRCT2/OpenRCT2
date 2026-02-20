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

#include <cmath>
#include <cstdint>

namespace OpenRCT2::CommandLine::Sprite
{
    int32_t exportObject(const char** argv, int32_t argc)
    {
        if (argc < 3)
        {
            fprintf(stdout, "usage: sprite exportobject <path to object> <output directory>\n");
            return -1;
        }

        const char* objectPath = argv[1];
        const utf8* outputPath = argv[2];
        auto context = CreateContext();
        context->Initialise();

        std::unique_ptr<Object> metaObject = ObjectFactory::CreateObjectFromFile(objectPath, true);
        if (metaObject == nullptr)
        {
            fprintf(stderr, "Could not load the object.\n");
            return -1;
        }

        if (!Path::CreateDirectory(outputPath))
        {
            fprintf(stderr, "Unable to create output directory.\n");
            return -1;
        }

        const auto* imageTableStart = metaObject->GetImageTable().GetImages();
        const uint32_t maxIndex = metaObject->GetNumImages();
        const int32_t numbers = static_cast<int32_t>(std::floor(std::log10(maxIndex) + 1));

        std::ostringstream oss; // TODO: Remove when C++20 is enabled and std::format can be used
        for (uint32_t spriteIndex = 0; spriteIndex < maxIndex; spriteIndex++)
        {
            oss << std::setw(numbers) << std::setfill('0') << spriteIndex << ".png";
            auto path = Path::Combine(outputPath, PopStr(oss));

            const auto& g1 = imageTableStart[spriteIndex];
            if (g1.width == 0 || g1.height == 0)
            {
                fprintf(stdout, "\"\"");
            }
            else
            {
                if (!SpriteImageExport(g1, path))
                {
                    fprintf(stderr, "Could not export\n");
                    return -1;
                }

                path = fs::u8path(path).generic_u8string();
                fprintf(stdout, "{ \"path\": \"%s\", \"x\": %d, \"y\": %d }", path.c_str(), g1.xOffset, g1.yOffset);
            }

            fprintf(stdout, (spriteIndex + 1 != maxIndex) ? ",\n" : "\n");
        }
        return 0;
    }

} // namespace OpenRCT2::CommandLine::Sprite
