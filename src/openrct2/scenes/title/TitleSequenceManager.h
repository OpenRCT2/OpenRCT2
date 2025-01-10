/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../../core/StringTypes.h"

#include <limits>

namespace OpenRCT2::TitleSequenceManager
{
    struct Item
    {
        std::string Name;
        std::string Path;
        size_t PredefinedIndex;
        bool IsZip;
    };

    constexpr size_t kPredefinedIndexCustom = std::numeric_limits<size_t>::max();

    size_t GetCount();
    const Item* GetItem(size_t i);
    void DeleteItem(size_t i);
    size_t RenameItem(size_t i, const utf8* name);
    size_t DuplicateItem(size_t i, const utf8* name);
    size_t CreateItem(const utf8* name);
    void Scan();

    const utf8* GetName(size_t index);
    const utf8* GetPath(size_t index);
    const utf8* GetConfigID(size_t index);
    size_t GetPredefinedIndex(size_t index);
    size_t GetIndexForConfigID(const utf8* configId);
    size_t GetIndexForName(const utf8* name);
} // namespace OpenRCT2::TitleSequenceManager
