/*****************************************************************************
 * Copyright (c) 2014-2022 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "AssetPackManager.h"

#include "AssetPack.h"
#include "Context.h"
#include "PlatformEnvironment.h"
#include "core/Console.hpp"
#include "core/FileSystem.hpp"
#include "core/String.hpp"
#include "object/AudioSampleTable.h"

#include <cstdio>

using namespace OpenRCT2;

AssetPackManager::AssetPackManager()
{
}

AssetPackManager::~AssetPackManager()
{
}

size_t AssetPackManager::GetCount() const
{
    return _assetPacks.size();
}

AssetPack* AssetPackManager::GetAssetPack(size_t index)
{
    if (index >= _assetPacks.size())
        return nullptr;
    return _assetPacks[index].get();
}

void AssetPackManager::Scan()
{
    ClearAssetPacks();

    auto context = GetContext();
    auto env = context->GetPlatformEnvironment();
    auto assetPackDirectory = fs::u8path(env->GetDirectoryPath(DIRBASE::USER, DIRID::ASSET_PACK));
    for (const fs::directory_entry& entry : fs::recursive_directory_iterator(assetPackDirectory))
    {
        if (!entry.is_directory())
        {
            auto path = entry.path().u8string();
            if (String::EndsWith(path, ".parkap", true))
            {
                AddAssetPack(path);
            }
        }
    }
}

void AssetPackManager::Reload()
{
    for (auto& assetPack : _assetPacks)
    {
        assetPack->Load();
    }
}

void AssetPackManager::Swap(size_t index, size_t otherIndex)
{
    if (index < _assetPacks.size() && otherIndex < _assetPacks.size() && index != otherIndex)
    {
        std::swap(_assetPacks[index], _assetPacks[otherIndex]);
    }
}

void AssetPackManager::LoadSamplesForObject(std::string_view id, AudioSampleTable& objectTable)
{
    for (auto& assetPack : _assetPacks)
    {
        if (assetPack->IsEnabled() && assetPack->ContainsObject(id))
        {
            return assetPack->LoadSamplesForObject(id, objectTable);
        }
    }
}

void AssetPackManager::ClearAssetPacks()
{
    _assetPacks.clear();
}

void AssetPackManager::AddAssetPack(const fs::path& path)
{
    auto szPath = path.u8string();
    log_verbose("Scanning asset pack: %s", szPath.c_str());
    try
    {
        auto ap = std::make_unique<AssetPack>(path);
        ap->Fetch();
        _assetPacks.push_back(std::move(ap));
    }
    catch (const std::exception& e)
    {
        auto fileName = path.filename().u8string();
        Console::Error::WriteFormat("Unable to load asset pack: %s (%s)", fileName.c_str(), e.what());
    }
}
