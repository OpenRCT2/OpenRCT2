/*****************************************************************************
 * Copyright (c) 2014-2022 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "AssetPack.h"

#include "Context.h"
#include "core/Json.hpp"
#include "core/Zip.h"
#include "drawing/Image.h"
#include "localisation/LocalisationService.h"
#include "object/Object.h"

#include <algorithm>

using namespace OpenRCT2;

AssetPack::AssetPack(const fs::path& path)
    : Path(path)
{
}

AssetPack::~AssetPack()
{
}

void AssetPack::Fetch()
{
    auto archive = Zip::Open(Path.u8string(), ZIP_ACCESS::READ);
    if (!archive->Exists("object.json"))
    {
        throw std::runtime_error("Manifest does not exist.");
    }

    auto manifestJson = archive->GetFileData("object.json");
    auto jManifest = Json::FromVector(manifestJson);
    Id = jManifest["id"].get<std::string>();
    Version = jManifest["version"].get<std::string>();

    // TODO use a better string table class that can be used for objects, park files and asset packs
    auto& localisationService = GetContext()->GetLocalisationService();
    auto locale = std::string(localisationService.GetCurrentLanguageLocale());
    Name = GetString(jManifest, "name", locale);
    Description = GetString(jManifest, "description", locale);
}

std::string AssetPack::GetString(json_t& jManifest, const std::string& key, const std::string& locale)
{
    if (jManifest.contains("strings"))
    {
        auto& jStrings = jManifest["strings"];
        if (jStrings.contains(key))
        {
            auto& jKey = jStrings[key];
            if (jKey.contains(locale))
            {
                return jKey[locale].get<std::string>();
            }
            if (jKey.contains("en-GB"))
            {
                return jKey["en-GB"].get<std::string>();
            }
            if (jKey.contains("en-US"))
            {
                return jKey["en-US"].get<std::string>();
            }
        }
    }
    return {};
}

bool AssetPack::ContainsObject(std::string_view id) const
{
    auto it = std::find_if(_entries.begin(), _entries.end(), [id](const Entry& entry) { return entry.ObjectId == id; });
    return it != _entries.end();
}

ImageIndex AssetPack::AllocateImagesForObject(std::string_view id, uint32_t count)
{
    auto it = std::find_if(_entries.begin(), _entries.end(), [id](const Entry& entry) { return entry.ObjectId == id; });
    if (it != _entries.end())
    {
        auto images = _imageTable.GetImages() + it->ImageTableIndex;
        return gfx_object_allocate_images(images, static_cast<uint32_t>(it->ImageTableLength));
    }
    return ImageIndexUndefined;
}

class AssetPackLoadContext : public IReadObjectContext
{
private:
    IZipArchive* _zipArchive;

public:
    AssetPackLoadContext(IZipArchive* zipArchive)
        : _zipArchive(zipArchive)
    {
    }

    virtual ~AssetPackLoadContext() override
    {
    }

    std::string_view GetObjectIdentifier() override
    {
        throw std::runtime_error("Not implemented");
    }

    IObjectRepository& GetObjectRepository() override
    {
        throw std::runtime_error("Not implemented");
    }

    bool ShouldLoadImages() override
    {
        return true;
    }

    std::vector<uint8_t> GetData(std::string_view path) override
    {
        return _zipArchive->GetFileData(path);
    }

    ObjectAsset GetAsset(std::string_view path) override
    {
        throw std::runtime_error("Not implemented");
    }

    void LogVerbose(ObjectError code, const utf8* text) override
    {
    }

    void LogWarning(ObjectError code, const utf8* text) override
    {
    }

    void LogError(ObjectError code, const utf8* text) override
    {
    }
};

void AssetPack::Load()
{
    auto archive = Zip::Open(Path.u8string(), ZIP_ACCESS::READ);
    if (!archive->Exists("object.json"))
    {
        throw std::runtime_error("Manifest does not exist.");
    }

    AssetPackLoadContext loadContext(archive.get());

    auto manifestJson = archive->GetFileData("object.json");
    auto jManifest = Json::FromVector(manifestJson);
    auto& jObjects = jManifest["objects"];

    _entries.clear();
    for (auto& jObject : jObjects)
    {
        Entry entry;
        entry.ObjectId = jObject["id"].get<std::string>();
        entry.ImageTableIndex = _imageTable.GetCount();
        _imageTable.ReadJson(&loadContext, jObject);
        entry.ImageTableLength = _imageTable.GetCount() - entry.ImageTableIndex;
        _entries.push_back(entry);
    }
}
