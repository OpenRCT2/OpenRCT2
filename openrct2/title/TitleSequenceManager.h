/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once
#include "../common.h"
#include "../core/String.hpp"

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

constexpr const size_t PREDEFINED_INDEX_CUSTOM = SIZE_MAX;

size_t title_sequence_manager_get_count();
const utf8* title_sequence_manager_get_name(size_t index);
const utf8* title_sequence_manager_get_path(size_t index);
const utf8* title_sequence_manager_get_config_id(size_t index);
size_t title_sequence_manager_get_predefined_index(size_t index);
size_t title_sequence_manager_get_index_for_config_id(const utf8* configId);
size_t title_sequence_manager_get_index_for_name(const utf8* name);
bool title_sequence_manager_is_name_reserved(const utf8* name);
void title_sequence_manager_scan();
void title_sequence_manager_delete(size_t i);
size_t title_sequence_manager_rename(size_t i, const utf8* name);
size_t title_sequence_manager_duplicate(size_t i, const utf8* name);
size_t title_sequence_manager_create(const utf8* name);
