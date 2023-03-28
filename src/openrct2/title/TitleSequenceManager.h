/*****************************************************************************
 * Copyright (c) 2014-2023 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once
#include "../common.h"
#include "../core/String.hpp"

#include <limits>
#include <string>

struct TitleSequenceManagerItem
{
    std::string Name;
    std::string Path;
    size_t PredefinedIndex;
    bool IsZip;
};

namespace TitleSequenceManager
{
    size_t GetCount();
    const TitleSequenceManagerItem* GetItem(size_t i);
    void DeleteItem(size_t i);
    size_t RenameItem(size_t i, const utf8* name);
    size_t DuplicateItem(size_t i, const utf8* name);
    size_t CreateItem(const utf8* name);
    void Scan();
} // namespace TitleSequenceManager

constexpr const size_t PREDEFINED_INDEX_CUSTOM = std::numeric_limits<size_t>::max();

size_t TitleSequenceManagerGetCount();
const utf8* TitleSequenceManagerGetName(size_t index);
const utf8* TitleSequenceManagerGetPath(size_t index);
const utf8* TitleSequenceManagerGetConfigID(size_t index);
size_t TitleSequenceManagerGetPredefinedIndex(size_t index);
size_t TitleSequenceManagerGetIndexForConfigID(const utf8* configId);
size_t TitleSequenceManagerGetIndexForName(const utf8* name);
void TitleSequenceManagerScan();
void TitleSequenceManagerDelete(size_t i);
size_t TitleSequenceManagerRename(size_t i, const utf8* name);
size_t TitleSequenceManagerDuplicate(size_t i, const utf8* name);
size_t TitleSequenceManagerCreate(const utf8* name);
