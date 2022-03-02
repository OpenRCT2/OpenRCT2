/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "CmdlineSprite.h"

#include "Context.h"
#include "OpenRCT2.h"
#include "core/FileStream.h"
#include "core/Imaging.h"
#include "core/Json.hpp"
#include "core/Path.hpp"
#include "drawing/Drawing.h"
#include "drawing/ImageImporter.h"
#include "object/ObjectLimits.h"
#include "object/ObjectManager.h"
#include "object/ObjectRepository.h"
#include "platform/Platform.h"
#include "util/Util.h"

#include <cmath>
#include <cstring>
#include <optional>

#ifdef _WIN32
#    include "core/String.hpp"
#endif

using namespace OpenRCT2::Drawing;

class SpriteFile
{
public:
    rct_g1_header Header{};
    std::vector<rct_g1_element> Entries;
    std::vector<uint8_t> Data;
    void AddImage(ImageImporter::ImportResult& image);
    bool Save(const utf8* path);
    static std::optional<SpriteFile> Open(const utf8* path);

private:
    class ScopedRelativeSpriteFile
    {
    private:
        SpriteFile& _SpriteFile;
        bool _WasAbsolute;

    public:
        ScopedRelativeSpriteFile(SpriteFile& sFile)
            : _SpriteFile(sFile)
            , _WasAbsolute(sFile.isAbsolute)
        {
            if (_WasAbsolute)
                _SpriteFile.MakeEntriesRelative();
        }

        ~ScopedRelativeSpriteFile()
        {
            if (_WasAbsolute)
                _SpriteFile.MakeEntriesAbsolute();
        }
    };
    bool isAbsolute = false;
    void MakeEntriesAbsolute();
    void MakeEntriesRelative();
};

void SpriteFile::MakeEntriesAbsolute()
{
    if (!isAbsolute)
    {
        for (auto& entry : Entries)
            entry.offset += reinterpret_cast<uintptr_t>(Data.data());
    }
    isAbsolute = true;
}

void SpriteFile::MakeEntriesRelative()
{
    if (isAbsolute)
    {
        for (auto& entry : Entries)
            entry.offset -= reinterpret_cast<uintptr_t>(Data.data());
    }
    isAbsolute = false;
}

void SpriteFile::AddImage(ImageImporter::ImportResult& image)
{
    Header.num_entries++;
    // New image will have its data inserted after previous image
    uint8_t* newElementOffset = reinterpret_cast<uint8_t*>(static_cast<uintptr_t>(Header.total_size));
    Header.total_size += static_cast<uint32_t>(image.Buffer.size());
    Entries.reserve(Header.num_entries);

    {
        ScopedRelativeSpriteFile scopedRelative(*this);
        Data.reserve(Header.total_size);
        Entries.push_back(image.Element);
        Entries.back().offset = newElementOffset;
        const auto& buffer = image.Buffer;
        std::copy(buffer.begin(), buffer.end(), std::back_inserter(Data));
    }
}

std::optional<SpriteFile> SpriteFile::Open(const utf8* path)
{
    try
    {
        OpenRCT2::FileStream stream(path, OpenRCT2::FILE_MODE_OPEN);

        SpriteFile spriteFile;
        stream.Read(&spriteFile.Header, sizeof(rct_g1_header));

        if (spriteFile.Header.num_entries > 0)
        {
            spriteFile.Entries.reserve(spriteFile.Header.num_entries);

            for (uint32_t i = 0; i < spriteFile.Header.num_entries; ++i)
            {
                rct_g1_element_32bit entry32bit{};
                stream.Read(&entry32bit, sizeof(entry32bit));
                rct_g1_element entry{};

                entry.offset = reinterpret_cast<uint8_t*>(static_cast<uintptr_t>(entry32bit.offset));
                entry.width = entry32bit.width;
                entry.height = entry32bit.height;
                entry.x_offset = entry32bit.x_offset;
                entry.y_offset = entry32bit.y_offset;
                entry.flags = entry32bit.flags;
                entry.zoomed_offset = entry32bit.zoomed_offset;
                spriteFile.Entries.push_back(std::move(entry));
            }
            spriteFile.Data.resize(spriteFile.Header.total_size);
            stream.Read(spriteFile.Data.data(), spriteFile.Header.total_size);
        }
        spriteFile.MakeEntriesAbsolute();
        return spriteFile;
    }
    catch (IOException&)
    {
        return std::nullopt;
    }
}

bool SpriteFile::Save(const utf8* path)
{
    try
    {
        OpenRCT2::FileStream stream(path, OpenRCT2::FILE_MODE_WRITE);
        stream.Write(&Header, sizeof(rct_g1_header));

        if (Header.num_entries > 0)
        {
            ScopedRelativeSpriteFile scopedRelative(*this);

            for (const auto& entry : Entries)
            {
                rct_g1_element_32bit entry32bit{};

                entry32bit.offset = static_cast<uint32_t>(reinterpret_cast<uintptr_t>(const_cast<uint8_t*>(entry.offset)));
                entry32bit.width = entry.width;
                entry32bit.height = entry.height;
                entry32bit.x_offset = entry.x_offset;
                entry32bit.y_offset = entry.y_offset;
                entry32bit.flags = entry.flags;
                entry32bit.zoomed_offset = entry.zoomed_offset;

                stream.Write(&entry32bit, sizeof(entry32bit));
            }
            stream.Write(Data.data(), Header.total_size);
        }
        return true;
    }
    catch (IOException&)
    {
        return false;
    }
}

static bool SpriteImageExport(const rct_g1_element& spriteElement, const char* outPath)
{
    const auto pixelBufferSize = spriteElement.width * spriteElement.height;
    auto pixelBuffer = std::make_unique<uint8_t[]>(pixelBufferSize);
    auto pixels = pixelBuffer.get();
    std::fill_n(pixels, pixelBufferSize, 0x00);

    rct_drawpixelinfo dpi;
    dpi.bits = pixels;
    dpi.x = 0;
    dpi.y = 0;
    dpi.width = spriteElement.width;
    dpi.height = spriteElement.height;
    dpi.pitch = 0;
    dpi.zoom_level = ZoomLevel{ 0 };

    DrawSpriteArgs args(
        ImageId(), PaletteMap::GetDefault(), spriteElement, 0, 0, spriteElement.width, spriteElement.height, pixels);
    gfx_sprite_to_buffer(dpi, args);

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

static std::optional<ImageImporter::ImportResult> SpriteImageImport(
    const char* path, int16_t x_offset, int16_t y_offset, ImageImporter::Palette palette, bool forceBmp,
    ImageImporter::ImportMode mode)
{
    try
    {
        auto format = IMAGE_FORMAT::PNG_32;
        auto flags = ImageImporter::ImportFlags::None;

        if (!forceBmp)
        {
            flags = ImageImporter::ImportFlags::RLE;
        }

        if (palette == ImageImporter::Palette::KeepIndices)
        {
            format = IMAGE_FORMAT::PNG;
        }

        ImageImporter importer;
        auto image = Imaging::ReadFromFile(path, format);

        return importer.Import(image, x_offset, y_offset, palette, flags, mode);
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

        if (argc == 2)
        {
            const char* spriteFilePath = argv[1];
            auto spriteFile = SpriteFile::Open(spriteFilePath);
            if (!spriteFile.has_value())
            {
                fprintf(stderr, "Unable to open input sprite file.\n");
                return -1;
            }

            printf("sprites: %u\n", spriteFile->Header.num_entries);
            printf("data size: %u\n", spriteFile->Header.total_size);
            return 1;
        }

        const char* spriteFilePath = argv[1];
        int32_t spriteIndex = atoi(argv[2]);
        auto spriteFile = SpriteFile::Open(spriteFilePath);
        if (!spriteFile.has_value())
        {
            fprintf(stderr, "Unable to open input sprite file.\n");
            return -1;
        }

        if (spriteIndex < 0 || spriteIndex >= static_cast<int32_t>(spriteFile->Header.num_entries))
        {
            fprintf(stderr, "Sprite #%d does not exist in sprite file.\n", spriteIndex);
            return -1;
        }

        rct_g1_element* g1 = &spriteFile->Entries[spriteIndex];
        printf("width: %d\n", g1->width);
        printf("height: %d\n", g1->height);
        printf("x offset: %d\n", g1->x_offset);
        printf("y offset: %d\n", g1->y_offset);
        printf("data offset: %p\n", g1->offset);
        return 1;
    }

    if (_strcmpi(argv[0], "export") == 0)
    {
        if (argc < 4)
        {
            fprintf(stdout, "usage: sprite export <spritefile> <idx> <output>\n");
            return -1;
        }

        const char* spriteFilePath = argv[1];
        int32_t spriteIndex = atoi(argv[2]);
        const char* outputPath = argv[3];
        auto spriteFile = SpriteFile::Open(spriteFilePath);
        if (!spriteFile.has_value())
        {
            fprintf(stderr, "Unable to open input sprite file.\n");
            return -1;
        }

        if (spriteIndex < 0 || spriteIndex >= static_cast<int32_t>(spriteFile->Header.num_entries))
        {
            fprintf(stderr, "Sprite #%d does not exist in sprite file.\n", spriteIndex);
            return -1;
        }

        const auto& spriteHeader = spriteFile->Entries[spriteIndex];
        if (!SpriteImageExport(spriteHeader, outputPath))
        {
            fprintf(stderr, "Could not export\n");
            return -1;
        }
        return 1;
    }

    if (_strcmpi(argv[0], "exportall") == 0)
    {
        if (argc < 3)
        {
            fprintf(stdout, "usage: sprite exportall <spritefile> <output directory>\n");
            return -1;
        }

        const char* spriteFilePath = argv[1];
        char outputPath[MAX_PATH];

        auto spriteFile = SpriteFile::Open(spriteFilePath);
        if (!spriteFile.has_value())
        {
            fprintf(stderr, "Unable to open input sprite file.\n");
            return -1;
        }

        safe_strcpy(outputPath, argv[2], MAX_PATH);
        path_end_with_separator(outputPath, MAX_PATH);

        if (!Platform::EnsureDirectoryExists(outputPath))
        {
            fprintf(stderr, "Unable to create directory.\n");
            return -1;
        }

        int32_t maxIndex = static_cast<int32_t>(spriteFile->Header.num_entries);
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

            const auto& spriteHeader = spriteFile->Entries[spriteIndex];
            if (!SpriteImageExport(spriteHeader, outputPath))
            {
                fprintf(stderr, "Could not export\n");
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
        return 1;
    }

    if (_strcmpi(argv[0], "exportalldat") == 0)
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
        const auto* loadedObject = object_manager_load_object(entry);
        if (loadedObject == nullptr)
        {
            fprintf(stderr, "Unable to load object.\n");
            return -1;
        }
        auto entryIndex = object_manager_get_loaded_object_entry_index(loadedObject);
        ObjectType objectType = entry->GetType();

        auto& objManager = context->GetObjectManager();
        const auto* const metaObject = objManager.GetLoadedObject(objectType, entryIndex);

        char outputPath[MAX_PATH];
        safe_strcpy(outputPath, argv[2], MAX_PATH);
        path_end_with_separator(outputPath, MAX_PATH);

        if (!Platform::EnsureDirectoryExists(outputPath))
        {
            fprintf(stderr, "Unable to create directory.\n");
            return -1;
        }

        auto maxIndex = metaObject->GetNumImages();

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

        for (uint32_t spriteIndex = 0; spriteIndex < maxIndex; spriteIndex++)
        {
            const auto& g1 = metaObject->GetImageTable().GetImages()[spriteIndex];
            if (!SpriteImageExport(g1, outputPath))
            {
                fprintf(stderr, "Could not export\n");
                return -1;
            }

            fprintf(stdout, "{ \"path\": \"%s\", \"x\": %d, \"y\": %d },\n", outputPath, g1.x_offset, g1.y_offset);

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
        return 1;
    }

    if (_strcmpi(argv[0], "create") == 0)
    {
        if (argc < 2)
        {
            fprintf(stderr, "usage: sprite create <spritefile>\n");
            return -1;
        }

        const char* spriteFilePath = argv[1];

        SpriteFile spriteFile;
        spriteFile.Save(spriteFilePath);
        return 1;
    }

    if (_strcmpi(argv[0], "append") == 0)
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

        auto importResult = SpriteImageImport(
            imagePath, x_offset, y_offset, ImageImporter::Palette::OpenRCT2, false, gSpriteMode);
        if (!importResult.has_value())
            return -1;

        auto spriteFile = SpriteFile::Open(spriteFilePath);
        if (!spriteFile.has_value())
        {
            fprintf(stderr, "Unable to open input sprite file.\n");
            return -1;
        }

        spriteFile->AddImage(importResult.value());

        if (!spriteFile->Save(spriteFilePath))
            return -1;

        return 1;
    }

    if (_strcmpi(argv[0], "build") == 0)
    {
        if (argc < 3)
        {
            fprintf(stdout, "usage: sprite build <spritefile> <sprite description file> [silent]\n");
            return -1;
        }

        const char* spriteFilePath = argv[1];
        const char* spriteDescriptionPath = argv[2];
        const auto directoryPath = Path::GetDirectory(spriteDescriptionPath);

        json_t jsonSprites = Json::ReadFromFile(spriteDescriptionPath);
        if (jsonSprites.is_null())
        {
            fprintf(stderr, "Unable to read sprite description file: %s\n", spriteDescriptionPath);
            return -1;
        }

        if (!jsonSprites.is_array())
        {
            fprintf(stderr, "Error: expected array\n");
            return -1;
        }

        bool silent = (argc >= 4 && strcmp(argv[3], "silent") == 0);

        // keep sprite file entirely in memory until ready to write out a complete,
        // correct file
        SpriteFile spriteFile;
        spriteFile.Header.num_entries = 0;
        spriteFile.Header.total_size = 0;

        fprintf(stdout, "Building: %s\n", spriteFilePath);

        json_t sprite_description;

        // Note: jsonSprite is deliberately left non-const: json_t behaviour changes when const
        for (auto& [jsonKey, jsonSprite] : jsonSprites.items())
        {
            if (!jsonSprite.is_object())
            {
                fprintf(stderr, "Error: expected object for sprite %s\n", jsonKey.c_str());
                return -1;
            }

            json_t path = jsonSprite["path"];
            if (!path.is_string())
            {
                fprintf(stderr, "Error: no path provided for sprite %s\n", jsonKey.c_str());
                return -1;
            }
            std::string strPath = Json::GetString(path);

            json_t x_offset = jsonSprite["x_offset"];
            json_t y_offset = jsonSprite["y_offset"];

            auto palette = (Json::GetString(jsonSprite["palette"]) == "keep") ? ImageImporter::Palette::KeepIndices
                                                                              : ImageImporter::Palette::OpenRCT2;
            bool forceBmp = !jsonSprite["palette"].is_null() && Json::GetBoolean(jsonSprite["forceBmp"]);

            auto imagePath = Path::GetAbsolute(Path::Combine(directoryPath, strPath));

            auto importResult = SpriteImageImport(
                imagePath.c_str(), Json::GetNumber<int16_t>(x_offset), Json::GetNumber<int16_t>(y_offset), palette, forceBmp,
                gSpriteMode);
            if (importResult == std::nullopt)
            {
                fprintf(stderr, "Could not import image file: %s\nCanceling\n", imagePath.c_str());
                return -1;
            }

            spriteFile.AddImage(importResult.value());

            if (!silent)
                fprintf(stdout, "Added: %s\n", imagePath.c_str());
        }

        if (!spriteFile.Save(spriteFilePath))
        {
            log_error("Could not save sprite file, cancelling.");
            return -1;
        }

        fprintf(stdout, "Finished\n");
        return 1;
    }

    fprintf(stderr, "Unknown sprite command.\n");
    return 1;
}
