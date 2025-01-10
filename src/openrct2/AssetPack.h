/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "core/FileSystem.hpp"
#include "core/JsonFwd.hpp"
#include "object/AudioSampleTable.h"

#include <string>
#include <string_view>
#include <vector>

namespace OpenRCT2
{
    class AssetPack
    {
    private:
        struct Entry
        {
            std::string ObjectId;
            size_t TableIndex{};
            size_t TableLength{};
        };

        AudioSampleTable _sampleTable;
        std::vector<Entry> _entries;
        bool _enabled{};

    public:
        fs::path Path;
        std::string Id;
        std::string Version;
        std::string Name;
        std::string Description;

        AssetPack(const fs::path& path);
        AssetPack(const AssetPack&) = delete;
        ~AssetPack();

        bool IsEnabled() const;
        void SetEnabled(bool value);

        void Fetch();
        void Load();
        bool ContainsObject(std::string_view id) const;
        void LoadSamplesForObject(std::string_view id, AudioSampleTable& objectTable);

    private:
        static std::string GetString(json_t& jManifest, const std::string& key, const std::string& locale);
    };
} // namespace OpenRCT2
