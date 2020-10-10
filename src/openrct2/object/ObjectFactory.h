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

#include <memory>
#include <string_view>

struct IObjectRepository;
class Object;
struct rct_object_entry;

namespace ObjectFactory
{
    std::unique_ptr<Object> CreateObjectFromLegacyFile(IObjectRepository& objectRepository, const utf8* path);
    std::unique_ptr<Object> CreateObjectFromLegacyData(
        IObjectRepository& objectRepository, const rct_object_entry* entry, const void* data, size_t dataSize);
    std::unique_ptr<Object> CreateObjectFromZipFile(IObjectRepository& objectRepository, const std::string_view& path);
    std::unique_ptr<Object> CreateObject(const rct_object_entry& entry);

    std::unique_ptr<Object> CreateObjectFromJsonFile(IObjectRepository& objectRepository, const std::string& path);
} // namespace ObjectFactory
