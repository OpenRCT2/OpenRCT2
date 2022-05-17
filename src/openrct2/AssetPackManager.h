/*****************************************************************************
 * Copyright (c) 2014-2022 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "core/FileSystem.hpp"
#include "drawing/ImageId.hpp"

#include <memory>
#include <vector>

class AudioSampleTable;

namespace OpenRCT2
{
    class AssetPack;
    class AssetPackManager
    {
    private:
        std::vector<std::unique_ptr<AssetPack>> _assetPacks;

    public:
        AssetPackManager();
        ~AssetPackManager();

        void Scan();
        void Reload();

        ImageIndex AllocateImagesForObject(std::string_view id, uint32_t count);
        void LoadSamplesForObject(std::string_view id, AudioSampleTable& objectTable);

    private:
        void ClearAssetPacks();
        void AddAssetPack(const fs::path& path);
    };

} // namespace OpenRCT2
