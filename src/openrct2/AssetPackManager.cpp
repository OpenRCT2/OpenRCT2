/*****************************************************************************
 * Copyright (c) 2014-2023 OpenRCT2 developers
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
#include "config/Config.h"
#include "core/Console.hpp"
#include "core/FileSystem.hpp"
#include "core/String.hpp"
#include "object/AudioSampleTable.h"
#include "platform/Platform.h"

#include <cstdio>

namespace OpenRCT2
{

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

    AssetPack* AssetPackManager::GetAssetPack(std::string_view id)
    {
        auto index = GetAssetPackIndex(id);
        if (index != std::numeric_limits<size_t>::max())
            return _assetPacks[index].get();
        return nullptr;
    }

    size_t AssetPackManager::GetAssetPackIndex(std::string_view id)
    {
        auto it = std::find_if(_assetPacks.begin(), _assetPacks.end(), [&](const std::unique_ptr<AssetPack>& assetPack) {
            return assetPack != nullptr ? assetPack->Id == id : false;
        });
        if (it == _assetPacks.end())
            return std::numeric_limits<size_t>::max();
        return std::distance(_assetPacks.begin(), it);
    }

    void AssetPackManager::Scan()
    {
        ClearAssetPacks();

        auto context = GetContext();
        auto env = context->GetPlatformEnvironment();

        auto openrct2Dir = fs::u8path(env->GetDirectoryPath(DIRBASE::OPENRCT2, DIRID::ASSET_PACK));
        Scan(openrct2Dir);

        auto userDirectory = fs::u8path(env->GetDirectoryPath(DIRBASE::USER, DIRID::ASSET_PACK));
        Platform::EnsureDirectoryExists(userDirectory.u8string());
        Scan(userDirectory);
    }

    void AssetPackManager::Scan(const fs::path& directory)
    {
        // Recursively scan for .parkap files
        std::error_code ec;
        for (const fs::directory_entry& entry : fs::recursive_directory_iterator(directory, ec))
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
        std::for_each(_assetPacks.rbegin(), _assetPacks.rend(), [&](auto& assetPack) {
            if (assetPack->IsEnabled() && assetPack->ContainsObject(id))
            {
                assetPack->LoadSamplesForObject(id, objectTable);
            }
        });
    }

    void AssetPackManager::ClearAssetPacks()
    {
        _assetPacks.clear();
    }

    void AssetPackManager::AddAssetPack(const fs::path& path)
    {
        auto szPath = path.u8string();
        LOG_VERBOSE("Scanning asset pack: %s", szPath.c_str());
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

    template<typename TFunc> static void EnumerateCommaSeparatedList(std::string_view csl, TFunc func)
    {
        size_t elStart = 0;
        for (size_t i = 0; i <= csl.size(); i++)
        {
            if (i == csl.size() || csl[i] == ',')
            {
                auto el = csl.substr(elStart, i - elStart);
                if (el.size() != 0)
                    func(el);
                elStart = i + 1;
            }
        }
    }

    void AssetPackManager::LoadEnabledAssetPacks()
    {
        // Re-order asset packs
        std::vector<std::unique_ptr<AssetPack>> newAssetPacks;
        EnumerateCommaSeparatedList(gConfigGeneral.AssetPackOrder, [&](std::string_view id) {
            auto index = GetAssetPackIndex(id);
            if (index != std::numeric_limits<size_t>::max())
            {
                newAssetPacks.push_back(std::move(_assetPacks[index]));
            }
        });
        for (auto& assetPack : _assetPacks)
        {
            if (assetPack != nullptr)
            {
                newAssetPacks.push_back(std::move(assetPack));
            }
        }
        _assetPacks = std::move(newAssetPacks);

        // Set which asset packs are enabled
        EnumerateCommaSeparatedList(gConfigGeneral.EnabledAssetPacks, [&](std::string_view id) {
            auto assetPack = GetAssetPack(id);
            if (assetPack != nullptr)
            {
                assetPack->SetEnabled(true);
            }
        });
    }

    void AssetPackManager::SaveEnabledAssetPacks()
    {
        u8string orderList;
        u8string enabledList;
        for (const auto& assetPack : _assetPacks)
        {
            orderList.append(assetPack->Id);
            orderList.push_back(',');
            if (assetPack->IsEnabled())
            {
                enabledList.append(assetPack->Id);
                enabledList.push_back(',');
            }
        }
        if (orderList.size() > 0)
            orderList.pop_back();
        if (enabledList.size() > 0)
            enabledList.pop_back();
        gConfigGeneral.AssetPackOrder = orderList;
        gConfigGeneral.EnabledAssetPacks = enabledList;
        ConfigSaveDefault();
    }
} // namespace OpenRCT2
