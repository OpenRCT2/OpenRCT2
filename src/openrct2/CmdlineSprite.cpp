/*****************************************************************************
 * Copyright (c) 2014-2022 OpenRCT2 developers
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
#include "drawing/DefaultPalettes.h"
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

// TODO: Remove when C++20 is enabled and std::format can be used
#include <iomanip>
#include <sstream>

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

static std::optional<GamePalette> PaletteImageImport(const utf8* path)
{
    try
    {
        auto format = IMAGE_FORMAT::PNG_32;
        auto image = Imaging::ReadFromFile(path, format);

        auto width = image.Width;
        auto height = image.Height;
        if (width * height != 256)
        {
            throw "Custom palette image is not 256 pixels";
        }
        GamePalette newPalette;
        auto pixels = reinterpret_cast<uint32_t*>(image.Pixels.data());
        for (uint16_t palIndex = 0; palIndex < 256; palIndex++)
        {
            newPalette.SetPaletteColour(palIndex, reinterpret_cast<uint8_t*>(pixels++));
        }
        return newPalette;
    }
    catch (const std::exception& e)
    {
        fprintf(stderr, "%s\n", e.what());
        return std::nullopt;
    }
}

static GamePalette GetPaletteFromString(std::string_view paletteOption, std::string_view palettePath = "")
{
    if (paletteOption == "green")
    {
        return PrimaryRemapGreenPalette;
    }
    else if (paletteOption != "openrct2")
    {
        std::string finalPath{ palettePath };
        finalPath.append(paletteOption);
        auto tempPalette = PaletteImageImport(finalPath.c_str());
        if (!tempPalette.has_value())
        {
            fprintf(stderr, "Unable to open custom palette image.\n");
            return DefaultPalette;
        }
        return tempPalette.value();
    }
    return DefaultPalette;
}

static bool SpriteImageExport(const rct_g1_element& spriteElement, u8string_view outPath, GamePalette spritePalette)
{
    const size_t pixelBufferSize = static_cast<size_t>(spriteElement.width) * spriteElement.height;
    auto pixelBuffer = std::make_unique<uint8_t[]>(pixelBufferSize);
    auto pixels = pixelBuffer.get();

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
        image.Palette = std::make_unique<GamePalette>(spritePalette);
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
    const char* path, const GamePalette& spritePalette, int16_t x_offset, int16_t y_offset, ImageImporter::ImportFlags flags,
    ImageImporter::ImportMode mode)
{
    try
    {
        auto format = IMAGE_FORMAT::PNG_32;
        if (flags & ImageImporter::ImportFlags::KeepIndices)
        {
            format = IMAGE_FORMAT::PNG;
        }
        ImageImporter importer;
        auto image = Imaging::ReadFromFile(path, format);

        return importer.Import(image, spritePalette, x_offset, y_offset, flags, mode);
    }
    catch (const std::exception& e)
    {
        fprintf(stderr, "%s\n", e.what());
        return std::nullopt;
    }
}

// TODO: Remove when C++20 is enabled and std::format can be used
static std::string PopStr(std::ostringstream& oss)
{
    auto str = oss.str();
    oss.str("");
    oss.clear();
    return str;
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
            const utf8* spriteFilePath = argv[1];
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

        const utf8* spriteFilePath = argv[1];
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
            fprintf(stdout, "usage: sprite export <spritefile> <idx> <output> [{openrct2 | green | <palette image>}]\n");
            return -1;
        }

        const utf8* spriteFilePath = argv[1];
        int32_t spriteIndex = atoi(argv[2]);
        const utf8* outputPath = argv[3];
        GamePalette spritePalette = GetPaletteFromString(argc == 5 ? argv[4] : "openrct2");
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
        if (!SpriteImageExport(spriteHeader, outputPath, spritePalette))
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
            fprintf(stdout, "usage: sprite exportall <spritefile> <output directory> [{openrct2 | green | <palette image>}]\n");
            return -1;
        }

        const utf8* spriteFilePath = argv[1];
        const utf8* outputPath = argv[2];
        GamePalette spritePalette = GetPaletteFromString(argc == 4 ? argv[3] : "openrct2");

        auto spriteFile = SpriteFile::Open(spriteFilePath);
        if (!spriteFile.has_value())
        {
            fprintf(stderr, "Unable to open input sprite file.\n");
            return -1;
        }

        if (!Platform::EnsureDirectoryExists(outputPath))
        {
            fprintf(stderr, "Unable to create directory.\n");
            return -1;
        }

        const uint32_t maxIndex = spriteFile->Header.num_entries;
        const int32_t numbers = static_cast<int32_t>(std::floor(std::log10(maxIndex) + 1));

        std::ostringstream oss; // TODO: Remove when C++20 is enabled and std::format can be used
        for (uint32_t spriteIndex = 0; spriteIndex < maxIndex; spriteIndex++)
        {
            // Status indicator
            printf("\r%u / %u, %u%%", spriteIndex + 1, maxIndex, ((spriteIndex + 1) * 100) / maxIndex);

            oss << std::setw(numbers) << std::setfill('0') << spriteIndex << ".png";

            const auto& spriteHeader = spriteFile->Entries[spriteIndex];
            if (!SpriteImageExport(spriteHeader, Path::Combine(outputPath, PopStr(oss)), spritePalette))
            {
                fprintf(stderr, "Could not export\n");
                return -1;
            }
        }
        return 1;
    }

    if (_strcmpi(argv[0], "exportalldat") == 0)
    {
        if (argc < 3)
        {
            fprintf(
                stdout,
                "usage: sprite exportalldat <DAT identifier> <output directory> [{openrct2 | green | <palette image>}]\n");
            return -1;
        }

        const char* datName = argv[1];
        const utf8* outputPath = argv[2];
        GamePalette spritePalette = GetPaletteFromString(argc == 4 ? argv[3] : "openrct2");

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

        if (!Platform::EnsureDirectoryExists(outputPath))
        {
            fprintf(stderr, "Unable to create directory.\n");
            return -1;
        }

        const uint32_t maxIndex = metaObject->GetNumImages();
        const int32_t numbers = static_cast<int32_t>(std::floor(std::log10(maxIndex) + 1));

        std::ostringstream oss; // TODO: Remove when C++20 is enabled and std::format can be used
        for (uint32_t spriteIndex = 0; spriteIndex < maxIndex; spriteIndex++)
        {
            oss << std::setw(numbers) << std::setfill('0') << spriteIndex << ".png";
            auto path = Path::Combine(outputPath, PopStr(oss));

            const auto& g1 = metaObject->GetImageTable().GetImages()[spriteIndex];
            if (!SpriteImageExport(g1, path, spritePalette))
            {
                fprintf(stderr, "Could not export\n");
                return -1;
            }

            path = fs::u8path(path).generic_u8string();
            fprintf(stdout, "{ \"path\": \"%s\", \"x\": %d, \"y\": %d },\n", path.c_str(), g1.x_offset, g1.y_offset);
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

        const utf8* spriteFilePath = argv[1];

        SpriteFile spriteFile;
        spriteFile.Save(spriteFilePath);
        return 1;
    }

    if (_strcmpi(argv[0], "append") == 0)
    {
        if (argc != 3 && argc != 5 && argc != 6)
        {
            fprintf(
                stderr,
                "usage: sprite append <spritefile> <input> [<x offset> <y offset> [{openrct2 | green | <palette image>}]]\n");
            return -1;
        }

        const utf8* spriteFilePath = argv[1];
        const utf8* imagePath = argv[2];
        int16_t x_offset = 0;
        int16_t y_offset = 0;
        GamePalette spritePalette = DefaultPalette;
        ImageImporter::ImportFlags flags = ImageImporter::ImportFlags::RLE;

        if (argc >= 5)
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

        if (argc == 6)
        {
            spritePalette = GetPaletteFromString(argv[5]);
        }

        auto importResult = SpriteImageImport(imagePath, spritePalette, x_offset, y_offset, flags, gSpriteMode);
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

        const utf8* spriteFilePath = argv[1];
        const utf8* spriteDescriptionPath = argv[2];
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
            GamePalette spritePalette = DefaultPalette;
            ImageImporter::ImportFlags flags = ImageImporter::ImportFlags::RLE;

            // {"x": n, "y", n} in parkobj
            json_t x_offset = jsonSprite["x_offset"];
            json_t y_offset = jsonSprite["y_offset"];

            // {"format": "raw"} in parkobj
            if (Json::GetBoolean(jsonSprite["forceBmp"]))
            {
                flags = ImageImporter::ImportFlags::BMP;
            }

            if (jsonSprite["palette"].is_string())
            {
                std::string paletteString = Json::GetString(jsonSprite["palette"]);
                if (paletteString == "keep")
                {
                    flags = static_cast<ImageImporter::ImportFlags>(flags | ImageImporter::ImportFlags::KeepIndices);
                }
                else if (paletteString.length() > 0)
                {
                    spritePalette = GetPaletteFromString(paletteString);
                }
            }

            auto imagePath = Path::GetAbsolute(Path::Combine(directoryPath, strPath));

            auto importResult = SpriteImageImport(
                imagePath.c_str(), spritePalette, Json::GetNumber<int16_t>(x_offset), Json::GetNumber<int16_t>(y_offset), flags,
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
