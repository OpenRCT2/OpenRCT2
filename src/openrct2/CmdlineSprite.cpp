/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma warning(disable : 4706) // assignment within conditional expression

#include <cmath>
#include <cstring>
#include <jansson.h>
#include "CmdlineSprite.h"
#include "core/Imaging.h"
#include "drawing/Drawing.h"
#include "drawing/ImageImporter.h"
#include "localisation/Language.h"
#include "OpenRCT2.h"
#include "platform/platform.h"
#include "util/Util.h"

using namespace OpenRCT2::Drawing;

#pragma pack(push, 1)

struct rct_sprite_file_header {
    uint32_t num_entries;
    uint32_t total_size;
};

assert_struct_size(rct_sprite_file_header, 8);

struct rct_sprite_file_palette_entry {
    uint8_t b, g, r, a;
};

struct rle_code {
    uint8_t num_pixels;
    uint8_t offset_x;
};

assert_struct_size(rle_code, 2);

#pragma pack(pop)

assert_struct_size(rct_sprite_file_palette_entry, 4);

class CmdlineSprite
{
public:
    static const rct_sprite_file_palette_entry _standardPalette[256];
};

static rct_sprite_file_palette_entry spriteFilePalette[256];

static rct_sprite_file_header spriteFileHeader;
static rct_g1_element *spriteFileEntries;
static uint8_t *spriteFileData;

#ifdef _WIN32

static FILE * fopen_utf8(const char * path, const char * mode)
{
    wchar_t * pathW = utf8_to_widechar(path);
    wchar_t * modeW = utf8_to_widechar(mode);
    FILE * file = _wfopen(pathW, modeW);
    free(pathW);
    free(modeW);
    return file;
}

#define fopen fopen_utf8

#endif

static void sprite_entries_make_absolute()
{
    for (uint32_t i = 0; i < spriteFileHeader.num_entries; i++)
        spriteFileEntries[i].offset += (uintptr_t)spriteFileData;
}

static void sprite_entries_make_relative()
{
    for (uint32_t i = 0; i < spriteFileHeader.num_entries; i++)
        spriteFileEntries[i].offset -= (uintptr_t)spriteFileData;
}

static bool sprite_file_open(const utf8 *path)
{
    FILE * file = fopen(path, "rb");
    if (file == nullptr)
        return false;

    if (fread(&spriteFileHeader, sizeof(rct_sprite_file_header), 1, file) != 1) {
        fclose(file);
        return false;
    }

    if (spriteFileHeader.num_entries > 0) {
        int32_t openEntryTableSize = spriteFileHeader.num_entries * sizeof(rct_g1_element_32bit);
        rct_g1_element_32bit * openElements = (rct_g1_element_32bit *)malloc(openEntryTableSize);
        if (openElements == nullptr) {
            fclose(file);
            return false;
        }

        if (fread(openElements, openEntryTableSize, 1, file) != 1) {
            free(openElements);
            fclose(file);
            return false;
        }

        spriteFileData = (uint8_t *)malloc(spriteFileHeader.total_size);
        if (fread(spriteFileData, spriteFileHeader.total_size, 1, file) != 1) {
            free(spriteFileData);
            free(openElements);
            fclose(file);
            return false;
        }

        int32_t entryTableSize = spriteFileHeader.num_entries * sizeof(rct_g1_element);
        spriteFileEntries = (rct_g1_element *)malloc(entryTableSize);
        for (uint32_t i = 0; i < spriteFileHeader.num_entries; i++) {
            rct_g1_element_32bit * inElement = &openElements[i];
            rct_g1_element * outElement = &spriteFileEntries[i];

            outElement->offset = (uint8_t*)((uintptr_t)inElement->offset + (uintptr_t)spriteFileData);
            outElement->width = inElement->width;
            outElement->height = inElement->height;
            outElement->x_offset = inElement->x_offset;
            outElement->y_offset = inElement->y_offset;
            outElement->flags = inElement->flags;
            outElement->zoomed_offset = inElement->zoomed_offset;
        }

        free(openElements);
    }

    fclose(file);
    return true;
}

static bool sprite_file_save(const char *path)
{
    FILE * file = fopen(path, "wb");
    if (file == nullptr)
        return false;

    if (fwrite(&spriteFileHeader, sizeof(rct_sprite_file_header), 1, file) != 1) {
        fclose(file);
        return false;
    }

    if (spriteFileHeader.num_entries > 0) {
        int32_t saveEntryTableSize = spriteFileHeader.num_entries * sizeof(rct_g1_element_32bit);
        rct_g1_element_32bit * saveElements = (rct_g1_element_32bit *)malloc(saveEntryTableSize);
        if (saveElements == nullptr) {
            fclose(file);
            return false;
        }

        for (uint32_t i = 0; i < spriteFileHeader.num_entries; i++) {
            rct_g1_element * inElement = &spriteFileEntries[i];
            rct_g1_element_32bit * outElement = &saveElements[i];

            outElement->offset = (uint32_t)((uintptr_t)inElement->offset - (uintptr_t)spriteFileData);
            outElement->width = inElement->width;
            outElement->height = inElement->height;
            outElement->x_offset = inElement->x_offset;
            outElement->y_offset = inElement->y_offset;
            outElement->flags = inElement->flags;
            outElement->zoomed_offset = inElement->zoomed_offset;
        }

        if (fwrite(saveElements, saveEntryTableSize, 1, file) != 1) {
            free(saveElements);
            fclose(file);
            return false;
        }
        free(saveElements);

        if (fwrite(spriteFileData, spriteFileHeader.total_size, 1, file) != 1) {
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

static bool sprite_file_export(int32_t spriteIndex, const char *outPath)
{
    rct_g1_element *spriteHeader;
    rct_drawpixelinfo dpi;
    uint8_t *pixels;
    int32_t pixelBufferSize;

    spriteHeader = &spriteFileEntries[spriteIndex];
    pixelBufferSize = spriteHeader->width * spriteHeader->height;
    pixels = (uint8_t *)malloc(pixelBufferSize);
    memset(pixels, 0, pixelBufferSize);

    dpi.bits = pixels;
    dpi.x = 0;
    dpi.y = 0;
    dpi.width = spriteHeader->width;
    dpi.height = spriteHeader->height;
    dpi.pitch = 0;
    dpi.zoom_level = 0;

    memcpy(spriteFilePalette, CmdlineSprite::_standardPalette, 256 * 4);

    if (spriteHeader->flags & G1_FLAG_RLE_COMPRESSION) {
        gfx_rle_sprite_to_buffer(spriteHeader->offset, pixels, (uint8_t*)spriteFilePalette, &dpi, IMAGE_TYPE_DEFAULT, 0, spriteHeader->height, 0, spriteHeader->width);
    } else {
        gfx_bmp_sprite_to_buffer((uint8_t*)spriteFilePalette, spriteHeader->offset, pixels, spriteHeader, &dpi, spriteHeader->height, spriteHeader->width, IMAGE_TYPE_DEFAULT);
    }

    auto const pixels8 = dpi.bits;
    auto const pixelsLen = (dpi.width + dpi.pitch) * dpi.height;
    try
    {
        Image image;
        image.Width = dpi.width;
        image.Height = dpi.height;
        image.Depth = 8;
        image.Stride = dpi.width + dpi.pitch;
        image.Palette = std::make_unique<rct_palette>(*((rct_palette *)&spriteFilePalette));
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

static bool sprite_file_import(const char *path, int16_t x_offset, int16_t y_offset, bool keep_palette, rct_g1_element *outElement, uint8_t **outBuffer, int *outBufferLength, int32_t mode)
{
    try
    {
        auto format = IMAGE_FORMAT::PNG_32;
        auto flags = ImageImporter::IMPORT_FLAGS::RLE;
        if (keep_palette)
        {
            format = IMAGE_FORMAT::PNG;
            flags = (ImageImporter::IMPORT_FLAGS)(flags | ImageImporter::IMPORT_FLAGS::KEEP_PALETTE);
        }

        ImageImporter importer;
        auto image = Imaging::ReadFromFile(path, format);
        auto result = importer.Import(image, x_offset, y_offset, flags, (ImageImporter::IMPORT_MODE)mode);

        *outElement = result.Element;
        *outBuffer = (uint8_t *)result.Buffer;
        *outBufferLength = (int)result.BufferLength;
        return true;
    }
    catch (const std::exception& e)
    {
        fprintf(stderr, "%s\n", e.what());
        return false;
    }
}

int32_t cmdline_for_sprite(const char **argv, int32_t argc)
{
    gOpenRCT2Headless = true;
    if (argc == 0)
        return -1;

    if (_strcmpi(argv[0], "details") == 0) {
        if (argc < 2) {
            fprintf(stdout, "usage: sprite details <spritefile> [idx]\n");
            return -1;
        } else if (argc == 2) {
            const char *spriteFilePath = argv[1];

            if (!sprite_file_open(spriteFilePath)) {
                fprintf(stderr, "Unable to open input sprite file.\n");
                return -1;
            }

            printf("sprites: %u\n", spriteFileHeader.num_entries);
            printf("data size: %u\n", spriteFileHeader.total_size);

            sprite_file_close();
            return 1;
        } else {
            const char *spriteFilePath = argv[1];
            int32_t spriteIndex = atoi(argv[2]);

            if (!sprite_file_open(spriteFilePath)) {
                fprintf(stderr, "Unable to open input sprite file.\n");
                return -1;
            }

            if (spriteIndex < 0 || spriteIndex >= (int32_t)spriteFileHeader.num_entries) {
                sprite_file_close();
                fprintf(stderr, "Sprite #%d does not exist in sprite file.\n", spriteIndex);
                return -1;
            }

            rct_g1_element *g1 = &spriteFileEntries[spriteIndex];
            printf("width: %d\n", g1->width);
            printf("height: %d\n", g1->height);
            printf("x offset: %d\n", g1->x_offset);
            printf("y offset: %d\n", g1->y_offset);
            printf("data offset: %p\n", g1->offset);

            sprite_file_close();
            return 1;
        }
    } else if (_strcmpi(argv[0], "export") == 0) {
        if (argc < 4) {
            fprintf(stdout, "usage: sprite export <spritefile> <idx> <output>\n");
            return -1;
        }

        const char *spriteFilePath = argv[1];
        int32_t spriteIndex = atoi(argv[2]);
        const char *outputPath = argv[3];

        if (!sprite_file_open(spriteFilePath)) {
            fprintf(stderr, "Unable to open input sprite file.\n");
            return -1;
        }

        if (spriteIndex < 0 || spriteIndex >= (int32_t)spriteFileHeader.num_entries) {
            fprintf(stderr, "Sprite #%d does not exist in sprite file.\n", spriteIndex);
            return -1;
        }

        if (!sprite_file_export(spriteIndex, outputPath)) {
            fprintf(stderr, "Could not export\n");
            sprite_file_close();
            return -1;
        }

        sprite_file_close();
        return 1;
    } else if (_strcmpi(argv[0], "exportall") == 0) {
        if (argc < 3) {
            fprintf(stdout, "usage: sprite exportall <spritefile> <output directory>\n");
            return -1;
        }

        const char *spriteFilePath = argv[1];
        char outputPath[MAX_PATH];

        if (!sprite_file_open(spriteFilePath)) {
            fprintf(stderr, "Unable to open input sprite file.\n");
            return -1;
        }

        safe_strcpy(outputPath, argv[2], MAX_PATH);
        path_end_with_separator(outputPath, MAX_PATH);

        if (!platform_ensure_directory_exists(outputPath)){
            fprintf(stderr, "Unable to create directory.\n");
            return -1;
        }

        int32_t maxIndex = (int32_t)spriteFileHeader.num_entries;
        int32_t numbers = (int32_t)std::floor(std::log(maxIndex));
        size_t pathLen = strlen(outputPath);

        if (pathLen >= (size_t)(MAX_PATH - numbers - 5)) {
            fprintf(stderr, "Path too long.\n");
            return -1;
        }

        for (int32_t x = 0; x < numbers; x++){
            outputPath[pathLen + x] = '0';
        }
        safe_strcpy(outputPath + pathLen + numbers, ".png", MAX_PATH - pathLen - numbers);

        for (int32_t spriteIndex = 0; spriteIndex < maxIndex; spriteIndex++){

            if (spriteIndex % 100 == 99)
            {
                // Status indicator
                printf("\r%d / %d, %d%%", spriteIndex, maxIndex, spriteIndex / maxIndex);
            }

            if (!sprite_file_export(spriteIndex, outputPath))
            {
                fprintf(stderr, "Could not export\n");
                sprite_file_close();
                return -1;
            }

            // Add to the index at the end of the file name
            char *counter = outputPath + pathLen + numbers - 1;
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

    } else if (_strcmpi(argv[0], "create") == 0) {
        if (argc < 2) {
            fprintf(stderr, "usage: sprite create <spritefile>\n");
            return -1;
        }

        const char *spriteFilePath = argv[1];

        spriteFileHeader.num_entries = 0;
        spriteFileHeader.total_size = 0;
        sprite_file_save(spriteFilePath);

        sprite_file_close();
        return 1;
    } else if (_strcmpi(argv[0], "append") == 0) {
        if (argc != 3 && argc != 5) {
            fprintf(stderr, "usage: sprite append <spritefile> <input> [<x offset> <y offset>]\n");
            return -1;
        }


        const char *spriteFilePath = argv[1];
        const char *imagePath = argv[2];
        int16_t x_offset = 0;
        int16_t y_offset = 0;

        if (argc == 5)
        {
            char *endptr;

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

        rct_g1_element spriteElement;
        uint8_t *buffer;

        int32_t bufferLength;
        if (!sprite_file_import(imagePath, x_offset, y_offset, false, &spriteElement, &buffer, &bufferLength, gSpriteMode))
            return -1;

        if (!sprite_file_open(spriteFilePath)) {
            fprintf(stderr, "Unable to open input sprite file.\n");
            return -1;
        }

        spriteFileHeader.num_entries++;
        spriteFileHeader.total_size += bufferLength;
        spriteFileEntries = (rct_g1_element *)realloc(spriteFileEntries, spriteFileHeader.num_entries * sizeof(rct_g1_element));

        sprite_entries_make_relative();
        spriteFileData = (uint8_t *)realloc(spriteFileData, spriteFileHeader.total_size);
        sprite_entries_make_absolute();

        spriteFileEntries[spriteFileHeader.num_entries - 1] = spriteElement;
        memcpy(spriteFileData + (spriteFileHeader.total_size - bufferLength), buffer, bufferLength);
        spriteFileEntries[spriteFileHeader.num_entries - 1].offset = spriteFileData + (spriteFileHeader.total_size - bufferLength);

        free(buffer);
        if (!sprite_file_save(spriteFilePath))
            return -1;

        return 1;
    } else if (_strcmpi(argv[0], "build") == 0) {
        if (argc < 3) {
            fprintf(stdout, "usage: sprite build <spritefile> <sprite description file> [silent]\n");
            return -1;
        }

        const char *spriteFilePath = argv[1];
        const char *spriteDescriptionPath = argv[2];
        char* directoryPath = path_get_directory(spriteDescriptionPath);

        json_error_t error;
        json_t* sprite_list=json_load_file(spriteDescriptionPath, JSON_REJECT_DUPLICATES, &error);

        if (sprite_list == nullptr)
        {
            fprintf(stderr, "Error parsing sprite description file: %s at line %d column %d\n", error.text, error.line, error.column);
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
            if(!json_is_object(sprite_description))
            {
                fprintf(stderr, "Error: expected object for sprite %lu\n", (unsigned long)i);
                json_decref(sprite_list);
                return -1;
            }

            json_t* path = json_object_get(sprite_description,"path");
            if(!path || !json_is_string(path))
            {
                fprintf(stderr, "Error: no path provided for sprite %lu\n", (unsigned long)i);
                json_decref(sprite_list);
                return -1;
            }
            // Get x and y offsets, if present
            json_t* x_offset = json_object_get(sprite_description, "x_offset");
            json_t* y_offset = json_object_get(sprite_description, "y_offset");

            // Get palette option, if present
            bool keep_palette = false;
            json_t* palette = json_object_get(sprite_description, "palette");
            if (palette && json_is_string(palette))
            {
                const char *option = json_string_value(palette);
                if (strncmp(option, "keep", 4) == 0)
                {
                    keep_palette = true;
                }
            }

            // Resolve absolute sprite path
            char *imagePath = platform_get_absolute_path(json_string_value(path), directoryPath);

            rct_g1_element spriteElement;
            uint8_t *buffer;
            int bufferLength;

            if (!sprite_file_import(imagePath,
                                    x_offset == nullptr ? 0 : json_integer_value(x_offset),
                                    y_offset == nullptr ? 0 : json_integer_value(y_offset),
                                    keep_palette, &spriteElement, &buffer, &bufferLength, gSpriteMode))
            {
                fprintf(stderr, "Could not import image file: %s\nCanceling\n", imagePath);
                json_decref(sprite_list);
                free(imagePath);
                return -1;
            }

            if (!sprite_file_open(spriteFilePath))
            {
                fprintf(stderr, "Unable to open sprite file: %s\nCanceling\n", spriteFilePath);
                json_decref(sprite_list);
                free(imagePath);
                return -1;
            }

            spriteFileHeader.num_entries++;
            spriteFileHeader.total_size += bufferLength;
            spriteFileEntries = (rct_g1_element *)realloc(spriteFileEntries, spriteFileHeader.num_entries * sizeof(rct_g1_element));

            sprite_entries_make_relative();
            spriteFileData = (uint8_t *)realloc(spriteFileData, spriteFileHeader.total_size);
            sprite_entries_make_absolute();

            spriteFileEntries[spriteFileHeader.num_entries - 1] = spriteElement;
            memcpy(spriteFileData + (spriteFileHeader.total_size - bufferLength), buffer, bufferLength);
            spriteFileEntries[spriteFileHeader.num_entries - 1].offset = spriteFileData + (spriteFileHeader.total_size - bufferLength);

            free(buffer);

            if (!sprite_file_save(spriteFilePath))
            {
                fprintf(stderr, "Could not save sprite file: %s\nCanceling\n", imagePath);
                json_decref(sprite_list);
                free(imagePath);
                return -1;
            }

            if (!silent)
                fprintf(stdout, "Added: %s\n", imagePath);

            free(imagePath);
            sprite_file_close();

        }

        json_decref(sprite_list);
        free(directoryPath);

        fprintf(stdout, "Finished\n");
        return 1;
    } else {
        fprintf(stderr, "Unknown sprite command.\n");
        return 1;
    }
}

const rct_sprite_file_palette_entry CmdlineSprite::_standardPalette[256] = {
    // 0 (unused)
    { 0, 0, 0, 255 },

    // 1 - 9 (misc. e.g. font and water)
    { 0, 0, 0, 255 },
    { 0, 0, 0, 255 },
    { 0, 0, 0, 255 },
    { 0, 0, 0, 255 },
    { 0, 0, 0, 255 },
    { 0, 0, 0, 255 },
    { 0, 0, 0, 255 },
    { 0, 0, 0, 255 },
    { 0, 0, 0, 255 },

    //
    { 35, 35, 23, 255 },
    { 51, 51, 35, 255 },
    { 67, 67, 47, 255 },
    { 83, 83, 63, 255 },
    { 99, 99, 75, 255 },
    { 115, 115, 91, 255 },
    { 131, 131, 111, 255 },
    { 151, 151, 131, 255 },
    { 175, 175, 159, 255 },
    { 195, 195, 183, 255 },
    { 219, 219, 211, 255 },
    { 243, 243, 239, 255 },
    { 0, 47, 51, 255 },
    { 0, 59, 63, 255 },
    { 11, 75, 79, 255 },
    { 19, 91, 91, 255 },
    { 31, 107, 107, 255 },
    { 47, 123, 119, 255 },
    { 59, 139, 135, 255 },
    { 79, 155, 151, 255 },
    { 95, 175, 167, 255 },
    { 115, 191, 187, 255 },
    { 139, 207, 203, 255 },
    { 163, 227, 223, 255 },
    { 7, 43, 67, 255 },
    { 11, 59, 87, 255 },
    { 23, 75, 111, 255 },
    { 31, 87, 127, 255 },
    { 39, 99, 143, 255 },
    { 51, 115, 159, 255 },
    { 67, 131, 179, 255 },
    { 87, 151, 191, 255 },
    { 111, 175, 203, 255 },
    { 135, 199, 219, 255 },
    { 163, 219, 231, 255 },
    { 195, 239, 247, 255 },
    { 0, 27, 71, 255 },
    { 0, 43, 95, 255 },
    { 0, 63, 119, 255 },
    { 7, 83, 143, 255 },
    { 7, 111, 167, 255 },
    { 15, 139, 191, 255 },
    { 19, 167, 215, 255 },
    { 27, 203, 243, 255 },
    { 47, 231, 255, 255 },
    { 95, 243, 255, 255 },
    { 143, 251, 255, 255 },
    { 195, 255, 255, 255 },
    { 0, 0, 35, 255 },
    { 0, 0, 79, 255 },
    { 7, 7, 95, 255 },
    { 15, 15, 111, 255 },
    { 27, 27, 127, 255 },
    { 39, 39, 143, 255 },
    { 59, 59, 163, 255 },
    { 79, 79, 179, 255 },
    { 103, 103, 199, 255 },
    { 127, 127, 215, 255 },
    { 159, 159, 235, 255 },
    { 191, 191, 255, 255 },
    { 19, 51, 27, 255 },
    { 23, 63, 35, 255 },
    { 31, 79, 47, 255 },
    { 39, 95, 59, 255 },
    { 43, 111, 71, 255 },
    { 51, 127, 87, 255 },
    { 59, 143, 99, 255 },
    { 67, 155, 115, 255 },
    { 75, 171, 131, 255 },
    { 83, 187, 147, 255 },
    { 95, 203, 163, 255 },
    { 103, 219, 183, 255 },
    { 27, 55, 31, 255 },
    { 35, 71, 47, 255 },
    { 43, 83, 59, 255 },
    { 55, 99, 75, 255 },
    { 67, 111, 91, 255 },
    { 79, 135, 111, 255 },
    { 95, 159, 135, 255 },
    { 111, 183, 159, 255 },
    { 127, 207, 183, 255 },
    { 147, 219, 195, 255 },
    { 167, 231, 207, 255 },
    { 191, 247, 223, 255 },
    { 0, 63, 15, 255 },
    { 0, 83, 19, 255 },
    { 0, 103, 23, 255 },
    { 0, 123, 31, 255 },
    { 7, 143, 39, 255 },
    { 23, 159, 55, 255 },
    { 39, 175, 71, 255 },
    { 63, 191, 91, 255 },
    { 87, 207, 111, 255 },
    { 115, 223, 139, 255 },
    { 143, 239, 163, 255 },
    { 179, 255, 195, 255 },
    { 19, 43, 79, 255 },
    { 27, 55, 99, 255 },
    { 43, 71, 119, 255 },
    { 59, 87, 139, 255 },
    { 67, 99, 167, 255 },
    { 83, 115, 187, 255 },
    { 99, 131, 207, 255 },
    { 115, 151, 215, 255 },
    { 131, 171, 227, 255 },
    { 151, 191, 239, 255 },
    { 171, 207, 247, 255 },
    { 195, 227, 255, 255 },
    { 55, 19, 15, 255 },
    { 87, 43, 39, 255 },
    { 103, 55, 51, 255 },
    { 119, 67, 63, 255 },
    { 139, 83, 83, 255 },
    { 155, 99, 99, 255 },
    { 175, 119, 119, 255 },
    { 191, 139, 139, 255 },
    { 207, 159, 159, 255 },
    { 223, 183, 183, 255 },
    { 239, 211, 211, 255 },
    { 255, 239, 239, 255 },
    { 111, 27, 0, 255 },
    { 151, 39, 0, 255 },
    { 167, 51, 7, 255 },
    { 187, 67, 15, 255 },
    { 203, 83, 27, 255 },
    { 223, 103, 43, 255 },
    { 227, 135, 67, 255 },
    { 231, 163, 91, 255 },
    { 239, 187, 119, 255 },
    { 243, 211, 143, 255 },
    { 251, 231, 175, 255 },
    { 255, 247, 215, 255 },
    { 15, 43, 11, 255 },
    { 23, 55, 15, 255 },
    { 31, 71, 23, 255 },
    { 43, 83, 35, 255 },
    { 59, 99, 47, 255 },
    { 75, 115, 59, 255 },
    { 95, 135, 79, 255 },
    { 119, 155, 99, 255 },
    { 139, 175, 123, 255 },
    { 167, 199, 147, 255 },
    { 195, 219, 175, 255 },
    { 223, 243, 207, 255 },
    { 95, 0, 63, 255 },
    { 115, 7, 75, 255 },
    { 127, 15, 83, 255 },
    { 143, 31, 95, 255 },
    { 155, 43, 107, 255 },
    { 171, 63, 123, 255 },
    { 187, 83, 135, 255 },
    { 199, 103, 155, 255 },
    { 215, 127, 171, 255 },
    { 231, 155, 191, 255 },
    { 243, 195, 215, 255 },
    { 255, 235, 243, 255 },
    { 0, 0, 63, 255 },
    { 0, 0, 87, 255 },
    { 0, 0, 115, 255 },
    { 0, 0, 143, 255 },
    { 0, 0, 171, 255 },
    { 0, 0, 199, 255 },
    { 0, 7, 227, 255 },
    { 0, 7, 255, 255 },
    { 67, 79, 255, 255 },
    { 115, 123, 255, 255 },
    { 163, 171, 255, 255 },
    { 215, 219, 255, 255 },
    { 0, 39, 79, 255 },
    { 0, 51, 111, 255 },
    { 0, 63, 147, 255 },
    { 0, 71, 183, 255 },
    { 0, 79, 219, 255 },
    { 0, 83, 255, 255 },
    { 23, 111, 255, 255 },
    { 51, 139, 255, 255 },
    { 79, 163, 255, 255 },
    { 107, 183, 255, 255 },
    { 135, 203, 255, 255 },
    { 163, 219, 255, 255 },
    { 47, 51, 0, 255 },
    { 55, 63, 0, 255 },
    { 67, 75, 0, 255 },
    { 79, 87, 0, 255 },
    { 99, 107, 7, 255 },
    { 119, 127, 23, 255 },
    { 143, 147, 43, 255 },
    { 163, 167, 71, 255 },
    { 187, 187, 99, 255 },
    { 207, 207, 131, 255 },
    { 231, 231, 171, 255 },
    { 255, 255, 207, 255 },

    // 203 - 214 (Secondary remap)
    { 27, 0, 63, 255 },
    { 51, 0, 103, 255 },
    { 63, 11, 123, 255 },
    { 79, 23, 143, 255 },
    { 95, 31, 163, 255 },
    { 111, 39, 183, 255 },
    { 143, 59, 219, 255 },
    { 171, 91, 239, 255 },
    { 187, 119, 243, 255 },
    { 203, 151, 247, 255 },
    { 223, 183, 251, 255 },
    { 239, 215, 255, 255 },

    // 214 - 225 (Brown)
    { 0, 19, 39, 255 },
    { 7, 31, 55, 255 },
    { 15, 47, 71, 255 },
    { 31, 63, 91, 255 },
    { 51, 83, 107, 255 },
    { 75, 103, 123, 255 },
    { 107, 127, 143, 255 },
    { 127, 147, 163, 255 },
    { 147, 171, 187, 255 },
    { 171, 195, 207, 255 },
    { 195, 219, 231, 255 },
    { 223, 243, 255, 255 },

    // 226 (unknown)
    { 75, 75, 55, 255 },

    // 227 - 229 (tertiary remap)
    { 0, 183, 255, 255 },
    { 0, 219, 255, 255 },
    { 0, 255, 255, 255 },

    // 230 - 239 (water)
    { 99, 107, 7, 255 },
    { 99, 107, 7, 255 },
    { 135, 143, 39, 255 },
    { 123, 131, 27, 255 },
    { 99, 107, 7, 255 },
    { 151, 155, 55, 255 },
    { 151, 155, 55, 255 },
    { 227, 227, 155, 255 },
    { 203, 203, 115, 255 },
    { 151, 155, 55, 255 },

    // 240 - 242 (chain lift)
    { 91, 91, 67, 255 },
    { 107, 107, 83, 255 },
    { 123, 123, 99, 255 },

    // Old 243 - 245, changed to nice shade remap below
    // { 47, 47, 47, 255 },
    // { 47, 47, 47, 255 },
    // { 47, 71, 87, 255 },

    // 243 to 254 (primary remap)
    { 47, 51, 111, 255 },
    { 47, 55, 131, 255 },
    { 51, 63, 151, 255 },
    { 51, 67, 171, 255 },
    { 47, 75, 191, 255 },
    { 43, 79, 211, 255 },
    { 35, 87, 231, 255 },
    { 31, 95, 255, 255 },
    { 39, 127, 255, 255 },
    { 51, 155, 255, 255 },
    { 63, 183, 255, 255 },
    { 75, 207, 255, 255 },

    // 255 (unused?)
    { 0, 0, 0, 255 }
};
