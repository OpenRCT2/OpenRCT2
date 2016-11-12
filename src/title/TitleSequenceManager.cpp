#pragma region Copyright (c) 2014-2016 OpenRCT2 Developers
/*****************************************************************************
 * OpenRCT2, an open source clone of Roller Coaster Tycoon 2.
 *
 * OpenRCT2 is the work of many authors, a full list can be found in contributors.md
 * For more information, visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * A full copy of the GNU General Public License can be found in licence.txt
 *****************************************************************************/
#pragma endregion

#include <algorithm>
#include <vector>
#include "../core/FileScanner.h"
#include "../core/Memory.hpp"
#include "../core/Path.hpp"
#include "../core/String.hpp"
#include "../core/Util.hpp"
#include "TitleSequenceManager.h"

extern "C"
{
    #include "../localisation/localisation.h"
    #include "../openrct2.h"
}

namespace TitleSequenceManager
{
    struct PredefinedSequence
    {
        const utf8 * ConfigId;
        const utf8 * Filename;
        rct_string_id StringId;
    };

    const PredefinedSequence PredefinedSequences[] =
    {
        { "*RCT1",      "rct1.parkseq",         STR_TITLE_SEQUENCE_RCT1 },
        { "*RCT1AA",    "rct1aa.parkseq",       STR_TITLE_SEQUENCE_RCT1_AA },
        { "*RCT1AALL",  "rct1aall.parkseq",     STR_TITLE_SEQUENCE_RCT1_AA_LL },
        { "*RCT2",      "rct2.parkseq",         STR_TITLE_SEQUENCE_RCT2 },
        { "*OPENRCT2",  "openrct2.parkseq",     STR_TITLE_SEQUENCE_OPENRCT2 },
    };

    std::vector<TitleSequenceManagerItem> _items;

    static void Scan(const utf8 * directory);
    static std::string GetNameFromSequencePath(const utf8 * path);
    static void GetDataSequencesPath(utf8 * buffer, size_t bufferSize);
    static void GetUserSequencesPath(utf8 * buffer, size_t bufferSize);

    size_t GetCount()
    {
        return _items.size();
    }

    const TitleSequenceManagerItem * GetItem(size_t i)
    {
        return &_items[i];
    }

    static const uint16 GetPredefinedIndex(const utf8 * path)
    {
        const utf8 * filename = Path::GetFileName(path);
        for (uint16 i = 0; i < Util::CountOf(PredefinedSequences); i++)
        {
            if (String::Equals(filename, PredefinedSequences[i].Filename, true))
            {
                return i;
            }
        }
        return PREDEFINED_INDEX_CUSTOM;
    }

    static void Scan()
    {
        utf8 path[MAX_PATH];

        _items.clear();

        // Scan data path
        GetDataSequencesPath(path, sizeof(path));
        Scan(path);

        // Scan user path
        GetUserSequencesPath(path, sizeof(path));
        Scan(path);

        // Sort sequences by predefined index and then name
        std::sort(_items.begin(), _items.end(), [](const TitleSequenceManagerItem &a,
                                                   const TitleSequenceManagerItem &b) -> bool
        {
            if (a.PredefinedIndex < b.PredefinedIndex)
            {
                return true;
            }
            else if (a.PredefinedIndex > b.PredefinedIndex)
            {
                return false;
            }
            return _strcmpi(a.Name.c_str(), b.Name.c_str()) < 0;
        });
    }

    static void Scan(const utf8 * directory)
    {
        utf8 pattern[MAX_PATH];
        String::Set(pattern, sizeof(pattern), directory);
        Path::Append(pattern, sizeof(pattern), "*.parkseq");

        IFileScanner * fileScanner = Path::ScanDirectory(pattern, true);
        while (fileScanner->Next())
        {
            const utf8 * path = fileScanner->GetPath();

            TitleSequenceManagerItem item;
            item.PredefinedIndex = GetPredefinedIndex(path);
            item.Path = std::string(path);
            if (item.PredefinedIndex != PREDEFINED_INDEX_CUSTOM)
            {
                rct_string_id stringId = PredefinedSequences[item.PredefinedIndex].StringId;
                item.Name = String::Duplicate(language_get_string(stringId));
            }
            else
            {
                item.Name = GetNameFromSequencePath(path);
            }
            _items.push_back(item);
        }
        delete fileScanner;
    }

    static std::string GetNameFromSequencePath(const utf8 * path)
    {
        utf8 * name = Path::GetFileNameWithoutExtension(path);
        std::string result = std::string(name);
        Memory::Free(name);
        return result;
    }

    static void GetDataSequencesPath(utf8 * buffer, size_t bufferSize)
    {
        platform_get_openrct_data_path(buffer, bufferSize);
        Path::Append(buffer, bufferSize, "title");
    }

    static void GetUserSequencesPath(utf8 * buffer, size_t bufferSize)
    {
        platform_get_user_directory(buffer, "title sequences", bufferSize);
    }
}

extern "C"
{
    size_t title_sequence_manager_get_count()
    {
        return TitleSequenceManager::GetCount();
    }

    const utf8 * title_sequence_manager_get_name(size_t index)
    {
        auto item = TitleSequenceManager::GetItem(index);
        const utf8 * name = item->Name.c_str();
        return name;
    }

    const utf8 * title_sequence_manager_get_path(size_t index)
    {
        auto item = TitleSequenceManager::GetItem(index);
        const utf8 * name = item->Path.c_str();
        return name;
    }

    const utf8 * title_sequence_manager_get_config_id(size_t index)
    {
        auto item = TitleSequenceManager::GetItem(index);
        const utf8 * name = item->Name.c_str();
        const utf8 * filename = Path::GetFileName(item->Path.c_str());
        for (const auto &pseq : TitleSequenceManager::PredefinedSequences)
        {
            if (String::Equals(filename, pseq.Filename, true))
            {
                return pseq.ConfigId;
            }
        }
        return name;
    }

    size_t title_sequence_manager_get_index_for_config_id(const utf8 * configId)
    {
        size_t count = TitleSequenceManager::GetCount();
        for (size_t i = 0; i < count; i++)
        {
            const utf8 * cid = title_sequence_manager_get_config_id(i);
            if (String::Equals(cid, configId))
            {
                return i;
            }
        }
        return SIZE_MAX;
    }

    void title_sequence_manager_scan()
    {
        TitleSequenceManager::Scan();
    }
}
