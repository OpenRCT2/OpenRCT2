#pragma region Copyright (c) 2014-2017 OpenRCT2 Developers
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

#pragma once

#include "../common.h"

#ifdef __cplusplus

#include <string>

struct TitleSequenceManagerItem
{
    std::string Name;
    std::string Path;
    uint16      PredefinedIndex;
    bool        IsZip;
};

namespace TitleSequenceManager
{
    size_t GetCount();
    const TitleSequenceManagerItem * GetItem(size_t i);
    void DeleteItem(size_t i);
    size_t RenameItem(size_t i, const utf8 * name);
    size_t DuplicateItem(size_t i, const utf8 * name);
    size_t CreateItem(const utf8 * name);
    void Scan();
}

constexpr uint16 PREDEFINED_INDEX_CUSTOM = UINT16_MAX;

extern "C" {
#endif

    size_t title_sequence_manager_get_count();
    const utf8 * title_sequence_manager_get_name(size_t index);
    const utf8 * title_sequence_manager_get_path(size_t index);
    const utf8 * title_sequence_manager_get_config_id(size_t index);
    uint16 title_sequence_manager_get_predefined_index(size_t index);
    size_t title_sequence_manager_get_index_for_config_id(const utf8 * configId);
    size_t title_sequence_manager_get_index_for_name(const utf8 * name);
    void title_sequence_manager_scan();
    void title_sequence_manager_delete(size_t i);
    size_t title_sequence_manager_rename(size_t i, const utf8 * name);
    size_t title_sequence_manager_duplicate(size_t i, const utf8 * name);
    size_t title_sequence_manager_create(const utf8 * name);

#ifdef __cplusplus
}
#endif
