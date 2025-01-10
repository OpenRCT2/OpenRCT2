/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "TitleSequenceManager.h"

#include "../../Context.h"
#include "../../OpenRCT2.h"
#include "../../PlatformEnvironment.h"
#include "../../core/Collections.hpp"
#include "../../core/File.h"
#include "../../core/FileScanner.h"
#include "../../core/Memory.hpp"
#include "../../core/Path.hpp"
#include "../../core/String.hpp"
#include "../../localisation/StringIds.h"
#include "../../platform/Platform.h"
#include "TitleSequence.h"

#include <algorithm>
#include <iterator>
#include <vector>

namespace OpenRCT2::TitleSequenceManager
{
    struct PredefinedSequence
    {
        const utf8* ConfigId;
        const utf8* Filename;
        ::StringId StringId;
    };

    static constexpr PredefinedSequence PredefinedSequences[] = {
        { "*RCT1", "rct1.parkseq", STR_TITLE_SEQUENCE_RCT1 },
        { "*RCT1AA", "rct1aa.parkseq", STR_TITLE_SEQUENCE_RCT1_AA },
        { "*RCT1AALL", "rct1aall.parkseq", STR_TITLE_SEQUENCE_RCT1_AA_LL },
        { "*RCT2", "rct2.parkseq", STR_TITLE_SEQUENCE_RCT2 },
        { "*OPENRCT2", "openrct2.parkseq", STR_TITLE_SEQUENCE_OPENRCT2 },
    };

    static std::vector<Item> _items;

    static std::string GetNewTitleSequencePath(const std::string& name, bool isZip);
    static size_t FindItemIndexByPath(const std::string& path);
    static void Scan(const std::string& directory);
    static void AddSequence(const std::string& scanPath);
    static void SortSequences();
    static std::string GetNameFromSequencePath(const std::string& path);
    static std::string GetDataSequencesPath();
    static std::string GetUserSequencesPath();
    static bool IsNameReserved(const std::string& name);

    size_t GetCount()
    {
        return _items.size();
    }

    const Item* GetItem(size_t i)
    {
        if (i >= _items.size())
        {
            return nullptr;
        }
        return &_items[i];
    }

    static size_t FindItemIndexByPath(const std::string& path)
    {
        size_t index = Collections::IndexOf(_items, [path](const Item& item) -> bool { return path == item.Path; });
        return index;
    }

    void DeleteItem(size_t i)
    {
        auto item = GetItem(i);
        if (item == nullptr)
        {
            return;
        }
        const utf8* path = item->Path.c_str();
        if (item->IsZip)
        {
            File::Delete(path);
        }
        else
        {
            Path::DeleteDirectory(path);
        }
        _items.erase(_items.begin() + i);
    }

    size_t RenameItem(size_t i, const utf8* newName)
    {
        auto item = &_items[i];
        const auto& oldPath = item->Path;

        auto newPath = Path::Combine(Path::GetDirectory(oldPath), newName);
        if (item->IsZip)
        {
            newPath += Title::TITLE_SEQUENCE_EXTENSION;
            File::Move(oldPath, newPath);
        }
        else
        {
            File::Move(oldPath, newPath);
        }

        item->Name = newName;
        item->Path = newPath;

        SortSequences();
        size_t index = FindItemIndexByPath(newPath);
        return index;
    }

    size_t DuplicateItem(size_t i, const utf8* name)
    {
        auto item = &_items[i];
        const auto& srcPath = item->Path;

        std::string dstPath = GetNewTitleSequencePath(std::string(name), item->IsZip);
        if (!File::Copy(srcPath, dstPath, true))
        {
            return SIZE_MAX;
        }

        AddSequence(dstPath);
        SortSequences();
        size_t index = FindItemIndexByPath(dstPath);
        return index;
    }

    size_t CreateItem(const utf8* name)
    {
        auto seq = Title::CreateTitleSequence();
        seq->Name = name;
        seq->Path = GetNewTitleSequencePath(seq->Name, true);
        seq->IsZip = true;

        size_t index = SIZE_MAX;
        if (TitleSequenceSave(*seq))
        {
            AddSequence(seq->Path);
            SortSequences();
            index = FindItemIndexByPath(seq->Path);
        }

        return index;
    }

    static std::string GetNewTitleSequencePath(const std::string& name, bool isZip)
    {
        auto path = Path::Combine(GetUserSequencesPath(), name);
        if (isZip)
        {
            path += Title::TITLE_SEQUENCE_EXTENSION;
        }
        return path;
    }

    static size_t GetPredefinedIndex(const std::string& path)
    {
        auto filename = Path::GetFileName(path);
        for (size_t i = 0; i < std::size(PredefinedSequences); i++)
        {
            if (String::iequals(filename, PredefinedSequences[i].Filename))
            {
                return i;
            }
        }
        return kPredefinedIndexCustom;
    }

    static void SortSequences()
    {
        // Sort sequences by predefined index and then name
        std::sort(_items.begin(), _items.end(), [](const Item& a, const Item& b) -> bool {
            if (a.PredefinedIndex != b.PredefinedIndex)
            {
                return a.PredefinedIndex < b.PredefinedIndex;
            }
            return String::compare(a.Name, b.Name, true) < 0;
        });
    }

    void Scan()
    {
        _items.clear();

        // Scan data path
        Scan(GetDataSequencesPath());

        // Scan user path
        Scan(GetUserSequencesPath());

        SortSequences();
    }

    static void Scan(const std::string& directory)
    {
        auto pattern = Path::Combine(directory, u8"script.txt;*.parkseq");
        auto fileScanner = Path::ScanDirectory(pattern, true);
        while (fileScanner->Next())
        {
            AddSequence(fileScanner->GetPath());
        }
    }

    static void AddSequence(const std::string& scanPath)
    {
        Item item{};

        if (String::iequals(Path::GetExtension(scanPath), u8".txt"))
        {
            // If we are given a .txt file, set the path to the containing directory
            item.Path = Path::GetDirectory(scanPath);
            item.Name = Path::GetFileName(item.Path);
            item.IsZip = false;
        }
        else
        {
            item.Path = scanPath;
            item.Name = GetNameFromSequencePath(item.Path);
            item.IsZip = true;
        }

        item.PredefinedIndex = GetPredefinedIndex(item.Path);

        if (item.PredefinedIndex != kPredefinedIndexCustom)
        {
            StringId stringId = PredefinedSequences[item.PredefinedIndex].StringId;
            item.Name = LanguageGetString(stringId);
        }
        else if (IsNameReserved(item.Name))
        {
            // Reserved names are not allowed because they map to the
            // actual predefined names and also prevent editing
            return;
        }

        _items.push_back(std::move(item));
    }

    static std::string GetNameFromSequencePath(const std::string& path)
    {
        auto name = Path::GetFileNameWithoutExtension(path);
        return name;
    }

    static std::string GetDataSequencesPath()
    {
        auto env = GetContext()->GetPlatformEnvironment();
        return env->GetDirectoryPath(DIRBASE::OPENRCT2, DIRID::SEQUENCE);
    }

    static std::string GetUserSequencesPath()
    {
        auto env = GetContext()->GetPlatformEnvironment();
        return env->GetDirectoryPath(DIRBASE::USER, DIRID::SEQUENCE);
    }

    static bool IsNameReserved(const std::string& name)
    {
        for (const auto& pseq : TitleSequenceManager::PredefinedSequences)
        {
            if (String::iequals(name, pseq.ConfigId))
            {
                return true;
            }
        }
        return false;
    }

    const utf8* GetName(size_t index)
    {
        auto item = GetItem(index);
        if (item == nullptr)
        {
            return nullptr;
        }
        return item->Name.c_str();
    }

    const utf8* GetPath(size_t index)
    {
        auto item = GetItem(index);
        if (item == nullptr)
        {
            return nullptr;
        }
        return item->Path.c_str();
    }

    const utf8* GetConfigID(size_t index)
    {
        auto item = GetItem(index);
        if (item == nullptr)
        {
            return nullptr;
        }
        const auto& name = item->Name;
        const auto filename = Path::GetFileName(item->Path);
        for (const auto& pseq : PredefinedSequences)
        {
            if (String::iequals(filename, pseq.Filename))
            {
                return pseq.ConfigId;
            }
        }
        return name.c_str();
    }

    size_t GetPredefinedIndex(size_t index)
    {
        auto item = GetItem(index);
        if (item == nullptr)
        {
            return 0;
        }
        size_t predefinedIndex = item->PredefinedIndex;
        return predefinedIndex;
    }

    size_t GetIndexForConfigID(const utf8* configId)
    {
        size_t count = GetCount();
        for (size_t i = 0; i < count; i++)
        {
            const utf8* cid = GetConfigID(i);
            if (String::equals(cid, configId))
            {
                return i;
            }
        }
        return SIZE_MAX;
    }

    size_t GetIndexForName(const utf8* name)
    {
        size_t count = GetCount();
        for (size_t i = 0; i < count; i++)
        {
            const utf8* tn = GetName(i);
            if (String::equals(tn, name))
            {
                return i;
            }
        }
        return SIZE_MAX;
    }
} // namespace OpenRCT2::TitleSequenceManager
