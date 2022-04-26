/*****************************************************************************
 * Copyright (c) 2014-2022 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "AssetPack.h"

#include <vector>

namespace OpenRCT2
{
    class AssetPackManager
    {
    private:
        std::vector<AssetPack> _assetPacks;

    public:
        void Scan();

    private:
        void ClearAssetPacks();
        void AddAssetPack(const fs::path& path);
    };

} // namespace OpenRCT2
