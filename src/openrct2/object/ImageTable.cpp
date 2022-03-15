/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "ImageTable.h"

#include "../Context.h"
#include "../OpenRCT2.h"
#include "../PlatformEnvironment.h"
#include "../core/File.h"
#include "../core/FileScanner.h"
#include "../core/IStream.hpp"
#include "../core/Json.hpp"
#include "../core/Path.hpp"
#include "../core/String.hpp"
#include "../drawing/ImageImporter.h"
#include "../sprites.h"
#include "Object.h"
#include "ObjectFactory.h"

#include <algorithm>
#include <memory>
#include <stdexcept>

using namespace OpenRCT2;
using namespace OpenRCT2::Drawing;

struct ImageTable::SubImageTable
{
    std::vector<uint8_t> Data;
    std::vector<rct_g1_element> Entries;

    // For entries where the data is owned by this table
    void Add(const rct_g1_element& srcEntry, const std::vector<uint8_t>& srcData)
    {
        rct_g1_element newEntry = srcEntry;
        // Move the image data pointer from old to a relative position
        newEntry.offset = reinterpret_cast<uint8_t*>(newEntry.offset - srcData.data());
        Entries.push_back(newEntry);
    }
    // For entries where data is owned elsewhere (g1, g2, csg)
    void Add(const rct_g1_element& srcEntry)
    {
        Entries.push_back(srcEntry);
    }
    void AddEmpty()
    {
        Entries.push_back({});
    }
};

ImageTable::SubImageTable ImageTable::ParseImages(IReadObjectContext* context, std::string s)
{
    ImageTable::SubImageTable result;
    if (s.empty())
    {
        return result;
    }
    else if (String::StartsWith(s, "$CSG"))
    {
        auto range = ParseRange(s.substr(4));
        if (!range.empty())
        {
            if (is_csg_loaded())
            {
                for (auto i : range)
                {
                    result.Add(*gfx_get_g1_element(SPR_CSG_BEGIN + i));
                }
            }
            else
            {
                std::string id(context->GetObjectIdentifier());
                log_warning("CSG not loaded inserting placeholder images for %s", id.c_str());
                result.Entries.resize(range.size());
            }
        }
    }
    else if (String::StartsWith(s, "$G1"))
    {
        auto range = ParseRange(s.substr(3));
        if (!range.empty())
        {
            for (auto i : range)
            {
                result.Add(*gfx_get_g1_element(i));
            }
        }
    }
    else if (String::StartsWith(s, "$RCT2:OBJDATA/"))
    {
        auto name = s.substr(14);
        auto rangeStart = name.find('[');
        if (rangeStart != std::string::npos)
        {
            auto rangeString = name.substr(rangeStart);
            auto range = ParseRange(name.substr(rangeStart));
            name = name.substr(0, rangeStart);
            result = LoadObjectImages(context, name, range);
        }
    }
    else
    {
        try
        {
            auto imageData = context->GetData(s);
            auto image = Imaging::ReadFromBuffer(imageData);

            ImageImporter importer;
            auto importResult = importer.Import(image, 0, 0, ImageImporter::Palette::OpenRCT2, ImageImporter::ImportFlags::RLE);
            result.Data = importResult.Buffer;
            result.Add(importResult.Element, importResult.Buffer);
        }
        catch (const std::exception& e)
        {
            auto msg = String::StdFormat("Unable to load image '%s': %s", s.c_str(), e.what());
            context->LogWarning(ObjectError::BadImageTable, msg.c_str());
            result.AddEmpty();
        }
    }
    return result;
}

ImageTable::SubImageTable ImageTable::ParseImages(
    IReadObjectContext* context, std::vector<std::pair<std::string, Image>>& imageSources, json_t& el)
{
    Guard::Assert(el.is_object(), "ImageTable::ParseImages expects parameter el to be object");

    auto path = Json::GetString(el["path"]);
    auto x = Json::GetNumber<int16_t>(el["x"]);
    auto y = Json::GetNumber<int16_t>(el["y"]);
    auto srcX = Json::GetNumber<int16_t>(el["srcX"]);
    auto srcY = Json::GetNumber<int16_t>(el["srcY"]);
    auto srcWidth = Json::GetNumber<int16_t>(el["srcWidth"]);
    auto srcHeight = Json::GetNumber<int16_t>(el["srcHeight"]);
    auto raw = Json::GetString(el["format"]) == "raw";
    auto keepPalette = Json::GetString(el["palette"]) == "keep";
    auto zoomOffset = Json::GetNumber<int32_t>(el["zoom"]);

    SubImageTable result;
    try
    {
        auto flags = ImageImporter::ImportFlags::None;
        auto palette = ImageImporter::Palette::OpenRCT2;
        if (!raw)
        {
            flags = static_cast<ImageImporter::ImportFlags>(flags | ImageImporter::ImportFlags::RLE);
        }
        if (keepPalette)
        {
            palette = ImageImporter::Palette::KeepIndices;
        }

        auto itSource = std::find_if(
            imageSources.begin(), imageSources.end(),
            [&path](const std::pair<std::string, Image>& item) { return item.first == path; });
        if (itSource == imageSources.end())
        {
            throw std::runtime_error("Unable to find image in image source list.");
        }
        auto& image = itSource->second;

        if (srcWidth == 0)
            srcWidth = image.Width;

        if (srcHeight == 0)
            srcHeight = image.Height;

        ImageImporter importer;
        auto importResult = importer.Import(image, srcX, srcY, srcWidth, srcHeight, x, y, palette, flags);
        auto g1element = importResult.Element;
        g1element.zoomed_offset = zoomOffset;
        result.Data = importResult.Buffer;
        result.Add(g1element, importResult.Buffer);
    }
    catch (const std::exception& e)
    {
        auto msg = String::StdFormat("Unable to load image '%s': %s", path.c_str(), e.what());
        context->LogWarning(ObjectError::BadImageTable, msg.c_str());
        result.AddEmpty();
    }
    return result;
}

ImageTable::SubImageTable ImageTable::LoadObjectImages(
    IReadObjectContext* context, const std::string& name, const std::vector<int32_t>& range)
{
    SubImageTable result;
    auto objectPath = FindLegacyObject(name);
    auto obj = ObjectFactory::CreateObjectFromLegacyFile(
        context->GetObjectRepository(), objectPath.c_str(), !gOpenRCT2NoGraphics);
    if (obj != nullptr)
    {
        auto& imgTable = static_cast<const Object*>(obj.get())->GetImageTable();
        auto numImages = static_cast<int32_t>(imgTable.GetCount());
        auto& images = imgTable.GetEntries();

        // Copy all the image data
        result.Data = imgTable.GetImageData();

        size_t placeHoldersAdded = 0;
        for (auto i : range)
        {
            if (i >= 0 && i < numImages)
            {
                result.Add(images[i], imgTable.GetImageData());
            }
            else
            {
                result.AddEmpty();
                placeHoldersAdded++;
            }
        }

        // Log place holder information
        if (placeHoldersAdded > 0)
        {
            std::string msg = "Adding " + std::to_string(placeHoldersAdded) + " placeholders";
            context->LogWarning(ObjectError::InvalidProperty, msg.c_str());
        }
    }
    else
    {
        std::string msg = "Unable to open '" + objectPath + "'";
        context->LogWarning(ObjectError::InvalidProperty, msg.c_str());
        result.Entries.resize(range.size());
    }
    return result;
}

std::vector<int32_t> ImageTable::ParseRange(std::string s)
{
    // Currently only supports [###] or [###..###]
    std::vector<int32_t> result = {};
    if (s.length() >= 3 && s[0] == '[' && s[s.length() - 1] == ']')
    {
        s = s.substr(1, s.length() - 2);
        auto parts = String::Split(s, "..");
        if (parts.size() == 1)
        {
            result.push_back(std::stoi(parts[0]));
        }
        else
        {
            auto left = std::stoi(parts[0]);
            auto right = std::stoi(parts[1]);
            if (left <= right)
            {
                for (auto i = left; i <= right; i++)
                {
                    result.push_back(i);
                }
            }
            else
            {
                for (auto i = right; i >= left; i--)
                {
                    result.push_back(i);
                }
            }
        }
    }
    return result;
}

std::string ImageTable::FindLegacyObject(const std::string& name)
{
    const auto env = GetContext()->GetPlatformEnvironment();
    auto objectsPath = env->GetDirectoryPath(DIRBASE::RCT2, DIRID::OBJECT);
    auto objectPath = Path::Combine(objectsPath, name);
    if (File::Exists(objectPath))
    {
        return objectPath;
    }

    std::string altName = name;
    auto rangeStart = name.find(".DAT");
    if (rangeStart != std::string::npos)
    {
        altName.replace(rangeStart, 4, ".POB");
    }
    objectPath = Path::Combine(objectsPath, altName);
    if (File::Exists(objectPath))
    {
        return objectPath;
    }

    if (!File::Exists(objectPath))
    {
        // Search recursively for any file with the target name (case insensitive)
        auto filter = Path::Combine(objectsPath, u8"*.dat;*.pob");
        auto scanner = Path::ScanDirectory(filter, true);
        while (scanner->Next())
        {
            auto currentName = Path::GetFileName(scanner->GetPathRelative());
            if (String::Equals(currentName, name, true) || String::Equals(currentName, altName, true))
            {
                objectPath = scanner->GetPath();
                break;
            }
        }
    }
    return objectPath;
}

void ImageTable::Read(IReadObjectContext* context, OpenRCT2::IStream* stream)
{
    if (gOpenRCT2NoGraphics)
    {
        return;
    }

    try
    {
        uint32_t numImages = stream->ReadValue<uint32_t>();
        uint32_t imageDataSize = stream->ReadValue<uint32_t>();

        uint64_t headerTableSize = numImages * 16;
        uint64_t remainingBytes = stream->GetLength() - stream->GetPosition() - headerTableSize;
        if (remainingBytes > imageDataSize)
        {
            context->LogVerbose(ObjectError::BadImageTable, "Image table size longer than expected.");
            imageDataSize = static_cast<uint32_t>(remainingBytes);
        }

        auto dataSize = static_cast<size_t>(imageDataSize);
        _data.resize(dataSize);
        if (_data.empty())
        {
            context->LogError(ObjectError::BadImageTable, "Image table too large.");
            throw std::runtime_error("Image table too large.");
        }

        // Read g1 element headers
        uintptr_t imageDataBase = reinterpret_cast<uintptr_t>(_data.data());
        _entries.resize(numImages);
        for (auto& g1Element : _entries)
        {
            uintptr_t imageDataOffset = static_cast<uintptr_t>(stream->ReadValue<uint32_t>());
            g1Element.offset = reinterpret_cast<uint8_t*>(imageDataBase + imageDataOffset);

            g1Element.width = stream->ReadValue<int16_t>();
            g1Element.height = stream->ReadValue<int16_t>();
            g1Element.x_offset = stream->ReadValue<int16_t>();
            g1Element.y_offset = stream->ReadValue<int16_t>();
            g1Element.flags = stream->ReadValue<uint16_t>();
            g1Element.zoomed_offset = stream->ReadValue<uint16_t>();
        }

        // Read g1 element data
        size_t readBytes = static_cast<size_t>(stream->TryRead(_data.data(), dataSize));

        // If data is shorter than expected (some custom objects are unfortunately like that)
        size_t unreadBytes = dataSize - readBytes;
        if (unreadBytes > 0)
        {
            std::fill_n(_data.data() + readBytes, unreadBytes, 0);
            context->LogWarning(ObjectError::BadImageTable, "Image table size shorter than expected.");
        }
    }
    catch (const std::exception&)
    {
        context->LogError(ObjectError::BadImageTable, "Bad image table.");
        throw;
    }
}

std::vector<std::pair<std::string, Image>> ImageTable::GetImageSources(IReadObjectContext* context, json_t& jsonImages)
{
    std::vector<std::pair<std::string, Image>> result;
    for (auto& jsonImage : jsonImages)
    {
        if (jsonImage.is_object())
        {
            auto path = Json::GetString(jsonImage["path"]);
            auto keepPalette = Json::GetString(jsonImage["palette"]) == "keep";
            auto itSource = std::find_if(result.begin(), result.end(), [&path](const std::pair<std::string, Image>& item) {
                return item.first == path;
            });
            if (itSource == result.end())
            {
                auto imageData = context->GetData(path);
                auto imageFormat = keepPalette ? IMAGE_FORMAT::PNG : IMAGE_FORMAT::PNG_32;
                auto image = Imaging::ReadFromBuffer(imageData, imageFormat);
                auto pair = std::make_pair<std::string, Image>(std::move(path), std::move(image));
                result.push_back(std::move(pair));
            }
        }
    }
    return result;
}

bool ImageTable::ReadJson(IReadObjectContext* context, json_t& root)
{
    Guard::Assert(root.is_object(), "ImageTable::ReadJson expects parameter root to be object");

    bool usesFallbackSprites = false;

    if (context->ShouldLoadImages())
    {
        // First gather all the required images from inspecting the JSON and convert to 32bpp bmp's
        std::vector<SubImageTable> allImages;
        auto jsonImages = root["images"];
        if (!is_csg_loaded() && root.contains("noCsgImages"))
        {
            jsonImages = root["noCsgImages"];
            usesFallbackSprites = true;
        }

        auto imageSources = GetImageSources(context, jsonImages);

        for (auto& jsonImage : jsonImages)
        {
            if (jsonImage.is_string())
            {
                auto strImage = jsonImage.get<std::string>();
                allImages.emplace_back(ParseImages(context, strImage));
            }
            else if (jsonImage.is_object())
            {
                allImages.emplace_back(ParseImages(context, imageSources, jsonImage));
            }
        }

        // For most tables there is only one "image" so just move the result
        if (allImages.size() == 1)
        {
            SetImage(std::move(allImages[0].Entries), std::move(allImages[0].Data));
            return usesFallbackSprites;
        }

        // Otherwise
        std::vector<size_t> dataOffsets;
        for (const auto& img : allImages)
        {
            dataOffsets.push_back(MergeImageData(img.Data));
        }
        for (size_t i = 0; i < allImages.size(); ++i)
        {
            MergeEntries(allImages[i].Entries, dataOffsets[i]);
        }
    }

    return usesFallbackSprites;
}

void ImageTable::SetImage(std::vector<rct_g1_element>&& entries, std::vector<uint8_t>&& imageData)
{
    _entries = std::move(entries);
    _data = std::move(imageData);
    AddDataOffsetToEntries(0, _entries.size(), _data.data());
}

size_t ImageTable::MergeImageData(const std::vector<uint8_t>& newData)
{
    // Must not be performed with empty entries
    Guard::Assert(_entries.empty());
    const auto originalSize = _data.size();
    _data.resize(_data.size() + newData.size());
    std::copy(std::begin(newData), std::end(newData), std::begin(_data) + originalSize);
    // Returns offset to start of merged data (required for creating merged entries)
    return originalSize;
}

void ImageTable::MergeEntries(const std::vector<rct_g1_element>& newEntries, size_t newEntriesImageDataOffset)
{
    const auto originalSize = _entries.size();
    _entries.resize(_entries.size() + newEntries.size());
    std::copy(std::begin(newEntries), std::end(newEntries), std::begin(_entries) + originalSize);
    AddDataOffsetToEntries(originalSize, newEntries.size(), _data.data() + newEntriesImageDataOffset);
}

void ImageTable::AddDataOffsetToEntries(size_t begin, size_t count, uint8_t* dataOffset)
{
    for (size_t i = begin; i < begin + count; ++i)
    {
        _entries[i].offset += reinterpret_cast<uintptr_t>(dataOffset);
    }
}
