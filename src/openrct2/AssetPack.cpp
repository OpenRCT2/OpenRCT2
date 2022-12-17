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
#include "core/Path.hpp"
#include "core/Zip.h"
#include "drawing/Image.h"
#include "localisation/LocalisationService.h"
#include "object/Object.h"

#include <algorithm>

using namespace OpenRCT2;

constexpr std::string_view ManifestFileName = "manifest.json";

AssetPack::AssetPack(const fs::path& path)
    : Path(path)
{
}

AssetPack::~AssetPack()
{
}

bool AssetPack::IsEnabled() const
{
    return _enabled;
}

void AssetPack::SetEnabled(bool value)
{
    _enabled = value;
}

void AssetPack::Fetch()
{
    auto archive = Zip::Open(Path.u8string(), ZIP_ACCESS::READ);
    if (!archive->Exists(ManifestFileName))
    {
        throw std::runtime_error("Manifest does not exist.");
    }

    auto manifestJson = archive->GetFileData(ManifestFileName);
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

void AssetPack::LoadSamplesForObject(std::string_view id, AudioSampleTable& objectTable)
{
    auto it = std::find_if(_entries.begin(), _entries.end(), [id](const Entry& entry) { return entry.ObjectId == id; });
    if (it != _entries.end())
    {
        objectTable.LoadFrom(_sampleTable, it->TableIndex, it->TableLength);
    }
}

class AssetPackLoadContext : public IReadObjectContext
{
private:
    std::string _zipPath;
    IZipArchive* _zipArchive;

public:
    AssetPackLoadContext(std::string_view zipPath, IZipArchive* zipArchive)
        : _zipPath(zipPath)
        , _zipArchive(zipArchive)
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

    u8string ReadAllText(std::string_view path) override
    {
        return "";
    }

    ObjectAsset GetAsset(std::string_view path) override
    {
        return ObjectAsset(_zipPath, path);
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
    auto path = Path.u8string();
    auto archive = Zip::Open(path, ZIP_ACCESS::READ);
    if (!archive->Exists(ManifestFileName))
    {
        throw std::runtime_error("Manifest does not exist.");
    }

    AssetPackLoadContext loadContext(path, archive.get());

    auto manifestJson = archive->GetFileData(ManifestFileName);
    auto jManifest = Json::FromVector(manifestJson);
    auto& jObjects = jManifest["objects"];

    _entries.clear();
    for (auto& jObject : jObjects)
    {
        Entry entry;
        entry.ObjectId = jObject["id"].get<std::string>();

        if (jObject.contains("samples"))
        {
            entry.TableIndex = _sampleTable.GetCount();
            _sampleTable.ReadFromJson(&loadContext, jObject);
            entry.TableLength = _sampleTable.GetCount() - entry.TableIndex;
        }
        _entries.push_back(entry);
    }
}
