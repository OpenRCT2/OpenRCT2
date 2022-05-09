/*****************************************************************************
 * Copyright (c) 2014-2022 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../audio/AudioSource.h"
#include "../core/JsonFwd.hpp"

struct IReadObjectContext;

class AudioSampleTable
{
private:
    struct Entry
    {
        OpenRCT2::Audio::IAudioSource* Source{};
        std::optional<ObjectAsset> Asset;
        std::optional<uint32_t> PathIndex;
        int32_t Modifier{};
    };

    std::vector<Entry> _entries;

public:
    /**
     * Read the entries from the given JSON into the table, but do not load anything.
     */
    void ReadFromJson(IReadObjectContext* context, const json_t& root);

    /**
     * Load all available entries from the given table.
     */
    void LoadFrom(const AudioSampleTable& table, size_t index, size_t length);

    /**
     * Load all available entries.
     */
    void Load();

    /**
     * Unloads all entries that are currently loaded.
     */
    void Unload();

    size_t GetCount() const;
    OpenRCT2::Audio::IAudioSource* GetSample(uint32_t index) const;
    int32_t GetSampleModifier(uint32_t index) const;
};
