/*****************************************************************************
 * Copyright (c) 2014-2023 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "CommandLineSprite.h"

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
#include "util/Util.h"

#include <cmath>
#include <cstring>
#include <optional>

// TODO: Remove when C++20 is enabled and std::format can be used
#include <iomanip>
#include <sstream>

using namespace OpenRCT2::Drawing;

static int32_t CommandLineForSpriteCombine(const char** argv, int32_t argc);

class SpriteFile
{
public:
    RCTG1Header Header{};
    std::vector<G1Element> Entries;
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
        stream.Read(&spriteFile.Header, sizeof(RCTG1Header));

        if (spriteFile.Header.num_entries > 0)
        {
            spriteFile.Entries.reserve(spriteFile.Header.num_entries);

            for (uint32_t i = 0; i < spriteFile.Header.num_entries; ++i)
            {
                RCTG1Element entry32bit{};
                stream.Read(&entry32bit, sizeof(entry32bit));
                G1Element entry{};

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
        stream.Write(&Header, sizeof(RCTG1Header));

        if (Header.num_entries > 0)
        {
            ScopedRelativeSpriteFile scopedRelative(*this);

            for (const auto& entry : Entries)
            {
                RCTG1Element entry32bit{};

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

static bool SpriteImageExport(const G1Element& spriteElement, u8string_view outPath)
{
    const size_t pixelBufferSize = static_cast<size_t>(spriteElement.width) * spriteElement.height;
    auto pixelBuffer = std::make_unique<uint8_t[]>(pixelBufferSize);
    auto pixels = pixelBuffer.get();

    DrawPixelInfo dpi;
    dpi.bits = pixels;
    dpi.x = 0;
    dpi.y = 0;
    dpi.width = spriteElement.width;
    dpi.height = spriteElement.height;
    dpi.pitch = 0;
    dpi.zoom_level = ZoomLevel{ 0 };

    DrawSpriteArgs args(
        ImageId(), PaletteMap::GetDefault(), spriteElement, 0, 0, spriteElement.width, spriteElement.height, pixels);
    GfxSpriteToBuffer(dpi, args);

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

// TODO: Remove when C++20 is enabled and std::format can be used
static std::string PopStr(std::ostringstream& oss)
{
    auto str = oss.str();
    oss.str("");
    oss.clear();
    return str;
}

int32_t CommandLineForSprite(const char** argv, int32_t argc)
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

        G1Element* g1 = &spriteFile->Entries[spriteIndex];
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

        const utf8* spriteFilePath = argv[1];
        int32_t spriteIndex = atoi(argv[2]);
        const utf8* outputPath = argv[3];
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
        fprintf(stdout, "{ \"x\": %d, \"y\": %d }\n", spriteHeader.x_offset, spriteHeader.y_offset);
        return 1;
    }

    if (_strcmpi(argv[0], "exportall") == 0)
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

        const uint32_t maxIndex = spriteFile->Header.num_entries;
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
        return 1;
    }

    if (_strcmpi(argv[0], "exportalldat") == 0)
    {
        if (argc < 3)
        {
            fprintf(stdout, "usage: sprite exportalldat <DAT identifier> <output directory>\n");
            return -1;
        }

        const char* datName = argv[1];
        const utf8* outputPath = argv[2];
        auto context = OpenRCT2::CreateContext();
        context->Initialise();

        const ObjectRepositoryItem* ori = ObjectRepositoryFindObjectByName(datName);
        if (ori == nullptr)
        {
            fprintf(stderr, "Could not find the object.\n");
            return -1;
        }

        const RCTObjectEntry* entry = &ori->ObjectEntry;
        const auto* loadedObject = ObjectManagerLoadObject(entry);
        if (loadedObject == nullptr)
        {
            fprintf(stderr, "Unable to load object.\n");
            return -1;
        }
        auto entryIndex = ObjectManagerGetLoadedObjectEntryIndex(loadedObject);
        ObjectType objectType = entry->GetType();

        auto& objManager = context->GetObjectManager();
        const auto* const metaObject = objManager.GetLoadedObject(objectType, entryIndex);

        if (!Path::CreateDirectory(outputPath))
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
            if (!SpriteImageExport(g1, path))
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
        if (argc != 3 && argc != 5)
        {
            fprintf(stderr, "usage: sprite append <spritefile> <input> [<x offset> <y offset>]\n");
            return -1;
        }

        const utf8* spriteFilePath = argv[1];
        const utf8* imagePath = argv[2];
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
            LOG_ERROR("Could not save sprite file, cancelling.");
            return -1;
        }

        fprintf(stdout, "Finished\n");
        return 1;
    }

    if (_strcmpi(argv[0], "combine") == 0)
    {
        return CommandLineForSpriteCombine(argv, argc);
    }

    fprintf(stderr, "Unknown sprite command.\n");
    return 1;
}

static int32_t CommandLineForSpriteCombine(const char** argv, int32_t argc)
{
    if (argc < 4)
    {
        fprintf(stdout, "usage: sprite combine <index file> <image file> <output>\n");
        return -1;
    }

    const utf8* indexFile = argv[1];
    const utf8* dataFile = argv[2];
    const utf8* outputPath = argv[3];

    auto fileHeader = OpenRCT2::FileStream(indexFile, OpenRCT2::FILE_MODE_OPEN);
    auto fileData = OpenRCT2::FileStream(dataFile, OpenRCT2::FILE_MODE_OPEN);
    auto fileHeaderSize = fileHeader.GetLength();
    auto fileDataSize = fileData.GetLength();

    uint32_t numEntries = fileHeaderSize / sizeof(RCTG1Element);

    RCTG1Header header = {};
    header.num_entries = numEntries;
    header.total_size = fileDataSize;
    OpenRCT2::FileStream outputStream(outputPath, OpenRCT2::FILE_MODE_WRITE);

    outputStream.Write(&header, sizeof(RCTG1Header));
    auto g1Elements32 = std::make_unique<RCTG1Element[]>(numEntries);
    fileHeader.Read(g1Elements32.get(), numEntries * sizeof(RCTG1Element));
    for (uint32_t i = 0; i < numEntries; i++)
    {
        // RCT1 used zoomed offsets that counted from the beginning of the file, rather than from the current sprite.
        if (g1Elements32[i].flags & G1_FLAG_HAS_ZOOM_SPRITE)
        {
            g1Elements32[i].zoomed_offset = i - g1Elements32[i].zoomed_offset;
        }

        outputStream.Write(&g1Elements32[i], sizeof(RCTG1Element));
    }

    std::vector<uint8_t> data;
    data.resize(fileDataSize);
    fileData.Read(data.data(), fileDataSize);
    outputStream.Write(data.data(), fileDataSize);

    return 1;
}
