/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma warning(disable : 4706) // assignment within conditional expression

#include "CmdlineSprite.h"

#include "Context.h"
#include "OpenRCT2.h"
#include "core/Imaging.h"
#include "drawing/Drawing.h"
#include "drawing/ImageImporter.h"
#include "object/ObjectLimits.h"
#include "object/ObjectManager.h"
#include "object/ObjectRepository.h"
#include "platform/platform.h"
#include "util/Util.h"
#include "world/Entrance.h"
#include "world/Scenery.h"

#include <cmath>
#include <cstring>
#include <jansson.h>

#ifdef _WIN32
#    include "core/String.hpp"
#endif

using namespace OpenRCT2::Drawing;

#pragma pack(push, 1)

struct rct_sprite_file_header
{
    uint32_t num_entries;
    uint32_t total_size;
};

assert_struct_size(rct_sprite_file_header, 8);

#pragma pack(pop)

static rct_sprite_file_header spriteFileHeader;
static rct_g1_element* spriteFileEntries;
static uint8_t* spriteFileData;

#ifdef _WIN32

static FILE* fopen_utf8(const char* path, const char* mode)
{
    auto pathW = String::ToWideChar(path);
    auto modeW = String::ToWideChar(mode);
    auto file = _wfopen(pathW.c_str(), modeW.c_str());
    return file;
}

#    define fopen fopen_utf8

#endif

static void sprite_entries_make_absolute()
{
    for (uint32_t i = 0; i < spriteFileHeader.num_entries; i++)
        spriteFileEntries[i].offset += reinterpret_cast<uintptr_t>(spriteFileData);
}

static void sprite_entries_make_relative()
{
    for (uint32_t i = 0; i < spriteFileHeader.num_entries; i++)
        spriteFileEntries[i].offset -= reinterpret_cast<uintptr_t>(spriteFileData);
}

static bool sprite_file_open(const utf8* path)
{
    FILE* file = fopen(path, "rb");
    if (file == nullptr)
        return false;

    if (fread(&spriteFileHeader, sizeof(rct_sprite_file_header), 1, file) != 1)
    {
        fclose(file);
        return false;
    }

    if (spriteFileHeader.num_entries > 0)
    {
        std::unique_ptr<rct_g1_element_32bit[]> openElements = std::make_unique<rct_g1_element_32bit[]>(
            spriteFileHeader.num_entries);
        if (openElements == nullptr)
        {
            fclose(file);
            return false;
        }

        if (fread(openElements.get(), spriteFileHeader.num_entries * sizeof(rct_g1_element_32bit), 1, file) != 1)
        {
            fclose(file);
            return false;
        }

        spriteFileData = static_cast<uint8_t*>(malloc(spriteFileHeader.total_size));
        if (fread(spriteFileData, spriteFileHeader.total_size, 1, file) != 1)
        {
            free(spriteFileData);
            fclose(file);
            return false;
        }

        int32_t entryTableSize = spriteFileHeader.num_entries * sizeof(rct_g1_element);
        spriteFileEntries = static_cast<rct_g1_element*>(malloc(entryTableSize));
        for (uint32_t i = 0; i < spriteFileHeader.num_entries; i++)
        {
            rct_g1_element_32bit* inElement = &openElements[i];
            rct_g1_element* outElement = &spriteFileEntries[i];

            outElement->offset = reinterpret_cast<uint8_t*>(
                (static_cast<uintptr_t>(inElement->offset) + reinterpret_cast<uintptr_t>(spriteFileData)));
            outElement->width = inElement->width;
            outElement->height = inElement->height;
            outElement->x_offset = inElement->x_offset;
            outElement->y_offset = inElement->y_offset;
            outElement->flags = inElement->flags;
            outElement->zoomed_offset = inElement->zoomed_offset;
        }
    }

    fclose(file);
    return true;
}

static bool sprite_file_save(const char* path)
{
    FILE* file = fopen(path, "wb");
    if (file == nullptr)
        return false;

    if (fwrite(&spriteFileHeader, sizeof(rct_sprite_file_header), 1, file) != 1)
    {
        fclose(file);
        return false;
    }

    if (spriteFileHeader.num_entries > 0)
    {
        std::unique_ptr<rct_g1_element_32bit[]> saveElements = std::make_unique<rct_g1_element_32bit[]>(
            spriteFileHeader.num_entries);
        if (saveElements == nullptr)
        {
            fclose(file);
            return false;
        }

        for (uint32_t i = 0; i < spriteFileHeader.num_entries; i++)
        {
            rct_g1_element* inElement = &spriteFileEntries[i];
            rct_g1_element_32bit* outElement = &saveElements[i];

            outElement->offset = static_cast<uint32_t>(
                (reinterpret_cast<uintptr_t>(inElement->offset) - reinterpret_cast<uintptr_t>(spriteFileData)));
            outElement->width = inElement->width;
            outElement->height = inElement->height;
            outElement->x_offset = inElement->x_offset;
            outElement->y_offset = inElement->y_offset;
            outElement->flags = inElement->flags;
            outElement->zoomed_offset = inElement->zoomed_offset;
        }

        if (fwrite(saveElements.get(), spriteFileHeader.num_entries * sizeof(rct_g1_element_32bit), 1, file) != 1)
        {
            fclose(file);
            return false;
        }

        if (fwrite(spriteFileData, spriteFileHeader.total_size, 1, file) != 1)
        {
            fclose(file);
            return false;
        }
    }

    fclose(file);
    return true;
}

static void sprite_file_close()
{
    SafeFree(spriteFileEntries);
    SafeFree(spriteFileData);
}

static bool sprite_file_export(rct_g1_element* spriteHeader, const char* outPath)
{
    rct_drawpixelinfo dpi;
    uint8_t* pixels;
    int32_t pixelBufferSize;

    pixelBufferSize = spriteHeader->width * spriteHeader->height;
    std::unique_ptr<uint8_t[]> pixelBuffer(new uint8_t[pixelBufferSize]);
    pixels = pixelBuffer.get();
    std::fill_n(pixels, pixelBufferSize, 0x00);

    dpi.bits = pixels;
    dpi.x = 0;
    dpi.y = 0;
    dpi.width = spriteHeader->width;
    dpi.height = spriteHeader->height;
    dpi.pitch = 0;
    dpi.zoom_level = 0;

    DrawSpriteArgs args(
        &dpi, ImageId(), PaletteMap::GetDefault(), *spriteHeader, 0, 0, spriteHeader->width, spriteHeader->height, pixels);
    gfx_sprite_to_buffer(args);

    auto const pixels8 = dpi.bits;
    auto const pixelsLen = (dpi.width + dpi.pitch) * dpi.height;
    try
    {
        Image image;
        image.Width = dpi.width;
        image.Height = dpi.height;
        image.Depth = 8;
        image.Stride = dpi.width + dpi.pitch;
        image.Palette = std::make_unique<GamePalette>(StandardPalette);
        image.Pixels = std::vector<uint8_t>(pixels8, pixels8 + pixelsLen);
        Imaging::WriteToFile(outPath, image, IMAGE_FORMAT::PNG);
        return true;
    }
    catch (const std::exception& e)
    {
        fprintf(stderr, "Unable to write png: %s", e.what());
        return false;
    }
}

static std::optional<ImageImporter::ImportResult> sprite_file_import(
    const char* path, int16_t x_offset, int16_t y_offset, bool keep_palette, bool forceBmp, int32_t mode)
{
    try
    {
        auto format = IMAGE_FORMAT::PNG_32;
        auto flags = ImageImporter::IMPORT_FLAGS::NONE;

        if (!forceBmp)
        {
            flags = ImageImporter::IMPORT_FLAGS::RLE;
        }

        if (keep_palette)
        {
            format = IMAGE_FORMAT::PNG;
            flags = static_cast<ImageImporter::IMPORT_FLAGS>(flags | ImageImporter::IMPORT_FLAGS::KEEP_PALETTE);
        }

        ImageImporter importer;
        auto image = Imaging::ReadFromFile(path, format);

        return importer.Import(image, x_offset, y_offset, flags, static_cast<ImageImporter::IMPORT_MODE>(mode));
    }
    catch (const std::exception& e)
    {
        fprintf(stderr, "%s\n", e.what());
        return std::nullopt;
    }
}

int32_t cmdline_for_sprite(const char** argv, int32_t argc)
{
    gOpenRCT2Headless = true;
    if (argc == 0)
        return -1;

    if (_strcmpi(argv[0], "details") == 0)
    {
        if (argc < 2)
        {
            fprintf(stdout, "usage: sprite details <spritefile> [idx]\n");
            return -1;
        }
        else if (argc == 2)
        {
            const char* spriteFilePath = argv[1];

            if (!sprite_file_open(spriteFilePath))
            {
                fprintf(stderr, "Unable to open input sprite file.\n");
                return -1;
            }

            printf("sprites: %u\n", spriteFileHeader.num_entries);
            printf("data size: %u\n", spriteFileHeader.total_size);

            sprite_file_close();
            return 1;
        }
        else
        {
            const char* spriteFilePath = argv[1];
            int32_t spriteIndex = atoi(argv[2]);

            if (!sprite_file_open(spriteFilePath))
            {
                fprintf(stderr, "Unable to open input sprite file.\n");
                return -1;
            }

            if (spriteIndex < 0 || spriteIndex >= static_cast<int32_t>(spriteFileHeader.num_entries))
            {
                sprite_file_close();
                fprintf(stderr, "Sprite #%d does not exist in sprite file.\n", spriteIndex);
                return -1;
            }

            rct_g1_element* g1 = &spriteFileEntries[spriteIndex];
            printf("width: %d\n", g1->width);
            printf("height: %d\n", g1->height);
            printf("x offset: %d\n", g1->x_offset);
            printf("y offset: %d\n", g1->y_offset);
            printf("data offset: %p\n", g1->offset);

            sprite_file_close();
            return 1;
        }
    }
    else if (_strcmpi(argv[0], "export") == 0)
    {
        if (argc < 4)
        {
            fprintf(stdout, "usage: sprite export <spritefile> <idx> <output>\n");
            return -1;
        }

        const char* spriteFilePath = argv[1];
        int32_t spriteIndex = atoi(argv[2]);
        const char* outputPath = argv[3];

        if (!sprite_file_open(spriteFilePath))
        {
            fprintf(stderr, "Unable to open input sprite file.\n");
            return -1;
        }

        if (spriteIndex < 0 || spriteIndex >= static_cast<int32_t>(spriteFileHeader.num_entries))
        {
            fprintf(stderr, "Sprite #%d does not exist in sprite file.\n", spriteIndex);
            return -1;
        }

        rct_g1_element* spriteHeader = &spriteFileEntries[spriteIndex];
        if (!sprite_file_export(spriteHeader, outputPath))
        {
            fprintf(stderr, "Could not export\n");
            sprite_file_close();
            return -1;
        }

        sprite_file_close();
        return 1;
    }
    else if (_strcmpi(argv[0], "exportall") == 0)
    {
        if (argc < 3)
        {
            fprintf(stdout, "usage: sprite exportall <spritefile> <output directory>\n");
            return -1;
        }

        const char* spriteFilePath = argv[1];
        char outputPath[MAX_PATH];

        if (!sprite_file_open(spriteFilePath))
        {
            fprintf(stderr, "Unable to open input sprite file.\n");
            return -1;
        }

        safe_strcpy(outputPath, argv[2], MAX_PATH);
        path_end_with_separator(outputPath, MAX_PATH);

        if (!platform_ensure_directory_exists(outputPath))
        {
            fprintf(stderr, "Unable to create directory.\n");
            return -1;
        }

        int32_t maxIndex = static_cast<int32_t>(spriteFileHeader.num_entries);
        int32_t numbers = static_cast<int32_t>(std::floor(std::log(maxIndex)));
        size_t pathLen = strlen(outputPath);

        if (pathLen >= static_cast<size_t>(MAX_PATH - numbers - 5))
        {
            fprintf(stderr, "Path too long.\n");
            return -1;
        }

        for (int32_t x = 0; x < numbers; x++)
        {
            outputPath[pathLen + x] = '0';
        }
        safe_strcpy(outputPath + pathLen + numbers, ".png", MAX_PATH - pathLen - numbers);

        for (int32_t spriteIndex = 0; spriteIndex < maxIndex; spriteIndex++)
        {
            if (spriteIndex % 100 == 99)
            {
                // Status indicator
                printf("\r%d / %d, %d%%", spriteIndex, maxIndex, spriteIndex / maxIndex);
            }

            rct_g1_element* spriteHeader = &spriteFileEntries[spriteIndex];
            if (!sprite_file_export(spriteHeader, outputPath))
            {
                fprintf(stderr, "Could not export\n");
                sprite_file_close();
                return -1;
            }

            // Add to the index at the end of the file name
            char* counter = outputPath + pathLen + numbers - 1;
            (*counter)++;
            while (*counter > '9')
            {
                *counter = '0';
                counter--;
                (*counter)++;
            }
        }

        sprite_file_close();
        return 1;
    }
    else if (_strcmpi(argv[0], "exportalldat") == 0)
    {
        if (argc < 3)
        {
            fprintf(stdout, "usage: sprite exportalldat <DAT identifier> <output directory>\n");
            return -1;
        }

        char datName[DAT_NAME_LENGTH + 1] = { 0 };
        std::fill_n(datName, DAT_NAME_LENGTH, ' ');
        int32_t i = 0;
        for (const char* ch = argv[1]; *ch != '\0' && i < DAT_NAME_LENGTH; ch++)
        {
            datName[i++] = *ch;
        }

        auto context = OpenRCT2::CreateContext();
        context->Initialise();

        const ObjectRepositoryItem* ori = object_repository_find_object_by_name(datName);
        if (ori == nullptr)
        {
            fprintf(stderr, "Could not find the object.\n");
            return -1;
        }

        const rct_object_entry* entry = &ori->ObjectEntry;
        void* loadedObject = object_manager_load_object(entry);
        if (loadedObject == nullptr)
        {
            fprintf(stderr, "Unable to load object.\n");
            return -1;
        }
        auto entryIndex = object_manager_get_loaded_object_entry_index(loadedObject);
        uint8_t objectType = entry->GetType();

        auto& objManager = context->GetObjectManager();
        auto metaObject = objManager.GetLoadedObject(objectType, entryIndex);

        char outputPath[MAX_PATH];
        safe_strcpy(outputPath, argv[2], MAX_PATH);
        path_end_with_separator(outputPath, MAX_PATH);

        if (!platform_ensure_directory_exists(outputPath))
        {
            fprintf(stderr, "Unable to create directory.\n");
            return -1;
        }

        int32_t maxIndex = static_cast<int32_t>(metaObject->GetNumImages());
        int32_t imagesOffset = 0;
        switch (objectType)
        {
            case OBJECT_TYPE_RIDE:
            {
                auto rideEntry = get_ride_entry(entryIndex);
                imagesOffset = rideEntry->images_offset;
                break;
            }
            case OBJECT_TYPE_SMALL_SCENERY:
            case OBJECT_TYPE_LARGE_SCENERY:
            case OBJECT_TYPE_WALLS:
            case OBJECT_TYPE_BANNERS:
            case OBJECT_TYPE_PATH_BITS:
            {
                auto obj = objManager.GetLoadedObject(objectType, entryIndex);
                if (obj != nullptr)
                {
                    auto sceneryEntry = static_cast<rct_scenery_entry*>(obj->GetLegacyData());
                    imagesOffset = sceneryEntry->image;
                }
                break;
            }
            case OBJECT_TYPE_PATHS:
            {
                auto pathEntry = get_path_surface_entry(entryIndex);
                imagesOffset = pathEntry->image;
                break;
            }
            case OBJECT_TYPE_SCENERY_GROUP:
            {
                auto sceneryGroupEntry = get_scenery_group_entry(entryIndex);
                imagesOffset = sceneryGroupEntry->image;
                break;
            }
            case OBJECT_TYPE_PARK_ENTRANCE:
            {
                auto obj = objManager.GetLoadedObject(objectType, entryIndex);
                if (obj != nullptr)
                {
                    auto entranceEnty = static_cast<rct_entrance_type*>(obj->GetLegacyData());
                    imagesOffset = entranceEnty->image_id;
                }
                break;
            }
            default:
            {
                fprintf(stderr, "Cannot extract images from this type of object.\n");
                return -1;
            }
        }

        int32_t numDigits = std::max(1, static_cast<int32_t>(std::floor(std::log(maxIndex))));
        size_t pathLen = strlen(outputPath);

        if (pathLen >= static_cast<size_t>(MAX_PATH - numDigits - 5))
        {
            fprintf(stderr, "Path too long.\n");
            return -1;
        }

        for (int32_t x = 0; x < numDigits; x++)
        {
            outputPath[pathLen + x] = '0';
        }
        safe_strcpy(outputPath + pathLen + numDigits, ".png", MAX_PATH - pathLen - numDigits);

        for (int32_t spriteIndex = 0; spriteIndex < maxIndex; spriteIndex++)
        {
            const rct_g1_element* g1 = gfx_get_g1_element(spriteIndex + imagesOffset);
            if (g1 == nullptr)
            {
                fprintf(stderr, "Could not load image metadata\n");
                return -1;
            }

            if (!sprite_file_export(const_cast<rct_g1_element*>(g1), outputPath))
            {
                fprintf(stderr, "Could not export\n");
                sprite_file_close();
                return -1;
            }

            fprintf(stdout, "{ \"path\": \"%s\", \"x\": %d, \"y\": %d },\n", outputPath, g1->x_offset, g1->y_offset);

            // Add to the index at the end of the file name
            char* counter = outputPath + pathLen + numDigits - 1;
            (*counter)++;
            while (*counter > '9')
            {
                *counter = '0';
                counter--;
                (*counter)++;
            }
        }

        metaObject->Unload();
        return 1;
    }
    else if (_strcmpi(argv[0], "create") == 0)
    {
        if (argc < 2)
        {
            fprintf(stderr, "usage: sprite create <spritefile>\n");
            return -1;
        }

        const char* spriteFilePath = argv[1];

        spriteFileHeader.num_entries = 0;
        spriteFileHeader.total_size = 0;
        sprite_file_save(spriteFilePath);

        sprite_file_close();
        return 1;
    }
    else if (_strcmpi(argv[0], "append") == 0)
    {
        if (argc != 3 && argc != 5)
        {
            fprintf(stderr, "usage: sprite append <spritefile> <input> [<x offset> <y offset>]\n");
            return -1;
        }

        const char* spriteFilePath = argv[1];
        const char* imagePath = argv[2];
        int16_t x_offset = 0;
        int16_t y_offset = 0;

        if (argc == 5)
        {
            char* endptr;

            x_offset = strtol(argv[3], &endptr, 0);
            if (*endptr != 0)
            {
                fprintf(stderr, "X offset must be an integer\n");
                return -1;
            }

            y_offset = strtol(argv[4], &endptr, 0);
            if (*endptr != 0)
            {
                fprintf(stderr, "Y offset must be an integer\n");
                return -1;
            }
        }

        auto importResult = sprite_file_import(imagePath, x_offset, y_offset, false, false, gSpriteMode);
        if (importResult == std::nullopt)
            return -1;

        if (!sprite_file_open(spriteFilePath))
        {
            fprintf(stderr, "Unable to open input sprite file.\n");
            return -1;
        }

        spriteFileHeader.num_entries++;
        spriteFileHeader.total_size += static_cast<uint32_t>(importResult->Buffer.size());
        spriteFileEntries = static_cast<rct_g1_element*>(
            realloc(spriteFileEntries, spriteFileHeader.num_entries * sizeof(rct_g1_element)));

        sprite_entries_make_relative();
        spriteFileData = static_cast<uint8_t*>(realloc(spriteFileData, spriteFileHeader.total_size));
        sprite_entries_make_absolute();

        spriteFileEntries[spriteFileHeader.num_entries - 1] = importResult->Element;

        const auto& buffer = importResult->Buffer;
        std::memcpy(spriteFileData + (spriteFileHeader.total_size - buffer.size()), buffer.data(), buffer.size());
        spriteFileEntries[spriteFileHeader.num_entries - 1].offset = spriteFileData
            + (spriteFileHeader.total_size - buffer.size());

        if (!sprite_file_save(spriteFilePath))
            return -1;

        return 1;
    }
    else if (_strcmpi(argv[0], "build") == 0)
    {
        if (argc < 3)
        {
            fprintf(stdout, "usage: sprite build <spritefile> <sprite description file> [silent]\n");
            return -1;
        }

        const char* spriteFilePath = argv[1];
        const char* spriteDescriptionPath = argv[2];
        char* directoryPath = path_get_directory(spriteDescriptionPath);

        json_error_t error;
        auto fp = fopen(spriteDescriptionPath, "rb");
        if (fp == nullptr)
        {
            fprintf(stderr, "Unable to read sprite description file: %s\n", spriteDescriptionPath);
            return -1;
        }

        json_t* sprite_list = json_loadf(fp, JSON_REJECT_DUPLICATES, &error);
        fclose(fp);
        if (sprite_list == nullptr)
        {
            fprintf(
                stderr, "Error parsing sprite description file: %s at line %d column %d\n", error.text, error.line,
                error.column);
            return -1;
        }

        if (!json_is_array(sprite_list))
        {
            fprintf(stderr, "Error: expected array\n");
            json_decref(sprite_list);
            return -1;
        }

        bool silent = (argc >= 4 && strcmp(argv[3], "silent") == 0);

        spriteFileHeader.num_entries = 0;
        spriteFileHeader.total_size = 0;
        sprite_file_save(spriteFilePath);

        fprintf(stdout, "Building: %s\n", spriteFilePath);

        size_t i;
        json_t* sprite_description;

        json_array_foreach(sprite_list, i, sprite_description)
        {
            if (!json_is_object(sprite_description))
            {
                fprintf(stderr, "Error: expected object for sprite %lu\n", static_cast<unsigned long>(i));
                json_decref(sprite_list);
                return -1;
            }

            json_t* path = json_object_get(sprite_description, "path");
            if (!json_is_string(path))
            {
                fprintf(stderr, "Error: no path provided for sprite %lu\n", static_cast<unsigned long>(i));
                json_decref(sprite_list);
                return -1;
            }
            // Get x and y offsets, if present
            json_t* x_offset = json_object_get(sprite_description, "x_offset");
            json_t* y_offset = json_object_get(sprite_description, "y_offset");

            // Get palette option, if present
            bool keep_palette = false;
            json_t* palette = json_object_get(sprite_description, "palette");
            if (json_is_string(palette))
            {
                const char* option = json_string_value(palette);
                if (strncmp(option, "keep", 4) == 0)
                {
                    keep_palette = true;
                }
            }

            // Get forcebmp option, if present
            bool forceBmp = false;
            json_t* forceBmpObject = json_object_get(sprite_description, "forceBmp");
            if (palette && json_is_boolean(forceBmpObject))
            {
                forceBmp = json_boolean_value(forceBmpObject);
            }

            // Resolve absolute sprite path
            auto imagePath = platform_get_absolute_path(json_string_value(path), directoryPath);

            auto importResult = sprite_file_import(
                imagePath.c_str(), x_offset == nullptr ? 0 : json_integer_value(x_offset),
                y_offset == nullptr ? 0 : json_integer_value(y_offset), keep_palette, forceBmp, gSpriteMode);
            if (importResult == std::nullopt)
            {
                fprintf(stderr, "Could not import image file: %s\nCanceling\n", imagePath.c_str());
                json_decref(sprite_list);
                return -1;
            }

            if (!sprite_file_open(spriteFilePath))
            {
                fprintf(stderr, "Unable to open sprite file: %s\nCanceling\n", spriteFilePath);
                json_decref(sprite_list);
                return -1;
            }

            spriteFileHeader.num_entries++;
            spriteFileHeader.total_size += static_cast<uint32_t>(importResult->Buffer.size());
            spriteFileEntries = static_cast<rct_g1_element*>(
                realloc(spriteFileEntries, spriteFileHeader.num_entries * sizeof(rct_g1_element)));

            sprite_entries_make_relative();
            spriteFileData = static_cast<uint8_t*>(realloc(spriteFileData, spriteFileHeader.total_size));
            sprite_entries_make_absolute();

            spriteFileEntries[spriteFileHeader.num_entries - 1] = importResult->Element;

            const auto& buffer = importResult->Buffer;
            std::memcpy(spriteFileData + (spriteFileHeader.total_size - buffer.size()), buffer.data(), buffer.size());
            spriteFileEntries[spriteFileHeader.num_entries - 1].offset = spriteFileData
                + (spriteFileHeader.total_size - buffer.size());

            if (!sprite_file_save(spriteFilePath))
            {
                fprintf(stderr, "Could not save sprite file: %s\nCanceling\n", imagePath.c_str());
                json_decref(sprite_list);
                return -1;
            }

            if (!silent)
                fprintf(stdout, "Added: %s\n", imagePath.c_str());

            sprite_file_close();
        }

        json_decref(sprite_list);
        free(directoryPath);

        fprintf(stdout, "Finished\n");
        return 1;
    }
    else
    {
        fprintf(stderr, "Unknown sprite command.\n");
        return 1;
    }
}
