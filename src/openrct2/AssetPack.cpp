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
#include "localisation/LocalisationService.h"

using namespace OpenRCT2;

AssetPack::AssetPack(const fs::path& path)
    : Path(path)
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
