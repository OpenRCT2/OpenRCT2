/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "../../core/FileStream.h"
#include "../../core/Json.hpp"
#include "../../core/Path.hpp"
#include "../../drawing/Drawing.h"
#include "SpriteCommands.h"
#include "SpriteFile.h"

#include <cstdint>

using namespace OpenRCT2::Drawing;

namespace OpenRCT2::CommandLine::Sprite
{
    int32_t build(const char** argv, int32_t argc, ImportMode spriteMode)
    {
        if (argc < 3)
        {
            fprintf(stdout, "usage: sprite build <spritefile> <sprite description file> [silent]\n");
            return -1;
        }

        const utf8* spriteFilePath = argv[1];
        const utf8* spriteDescriptionPath = argv[2];
        const auto directoryPath = Path::GetDirectory(spriteDescriptionPath);

        json_t jsonSprites = Json::ReadFromFile(spriteDescriptionPath);
        if (jsonSprites.is_null())
        {
            fprintf(stderr, "Unable to read sprite description file: %s\n", spriteDescriptionPath);
            return -1;
        }

        if (jsonSprites.is_object() && !jsonSprites["images"].is_null())
            jsonSprites = jsonSprites["images"];

        if (!jsonSprites.is_array())
        {
            fprintf(stderr, "Error: expected array\n");
            return -1;
        }

        bool silent = (argc >= 4 && strcmp(argv[3], "silent") == 0);

        // keep sprite file entirely in memory until ready to write out a complete,
        // correct file
        SpriteFile spriteFile;
        spriteFile.Header.numEntries = 0;
        spriteFile.Header.totalSize = 0;

        fprintf(stdout, "Building: %s\n", spriteFilePath);

        json_t sprite_description;

        uint32_t numSuccessful = 0;

        std::unordered_map<u8string, Image> images{};

        ImageImporter importer;

        // Note: jsonSprite is deliberately left non-const: json_t behaviour changes when const
        for (auto& [jsonKey, jsonSprite] : jsonSprites.items())
        {
            if (!jsonSprite.is_object())
            {
                fprintf(stderr, "Error: expected object for sprite %s\n", jsonKey.c_str());
                return -1;
            }

            json_t colours = jsonSprite["colours"];
            if (colours.is_array())
            {
                auto importResult = importer.importJSONPalette(jsonSprite);
                spriteFile.addPalette(importResult);

                if (!silent)
                    fprintf(stdout, "Added palette\n");
            }
            else
            {
                json_t path = jsonSprite["path"];
                if (!path.is_string())
                {
                    fprintf(stderr, "Error: no path provided for sprite %s\n", jsonKey.c_str());
                    return -1;
                }
                std::string strPath = Json::GetString(path);

                auto meta = createImageImportMetaFromJson(jsonSprite);
                meta.importMode = spriteMode;

                auto imagePath = Path::GetAbsolute(Path::Combine(directoryPath, strPath));

                const auto image_iter = images.find(imagePath);
                if (image_iter != images.end())
                {
                    auto importResult = importer.Import(image_iter->second, meta);
                    spriteFile.AddImage(importResult);
                }
                else
                {
                    const auto image = SpriteImageLoad(imagePath, meta);
                    if (image == std::nullopt)
                    {
                        fprintf(stderr, "Could not read image file: %s\nCancelling\n", imagePath.c_str());
                        return -1;
                    }
                    images[imagePath] = image.value();

                    auto importResult = importer.Import(image.value(), meta);

                    spriteFile.AddImage(importResult);
                }

                if (!silent)
                    fprintf(stdout, "Added: %s\n", imagePath.c_str());
            }

            numSuccessful++;
        }

        if (!spriteFile.Save(spriteFilePath))
        {
            fprintf(stderr, "Could not save sprite file, cancelling.");
            return -1;
        }

        fprintf(stdout, "Finished building graphics repository with %u images\n", numSuccessful);

        if (numSuccessful > 0)
        {
            printf("Replace the object's images table entries with this:\n");
            const auto spriteFileName = Path::GetFileName(spriteFilePath);
            fprintf(stdout, "\"$LGX:%s[0..%u]\"\n", spriteFileName.c_str(), numSuccessful - 1);
        }

        return 0;
    }
} // namespace OpenRCT2::CommandLine::Sprite
