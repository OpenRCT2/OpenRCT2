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

#include <memory>
#include <string_view>
namespace OpenRCT2
{
    struct IObjectRepository;
    class Object;
    struct RCTObjectEntry;
    enum class ObjectType : uint8_t;

    namespace ObjectFactory
    {
        [[nodiscard]] std::unique_ptr<Object> CreateObjectFromLegacyFile(
            IObjectRepository& objectRepository, const utf8* path, bool loadImages);
        [[nodiscard]] std::unique_ptr<Object> CreateObjectFromLegacyData(
            IObjectRepository& objectRepository, const RCTObjectEntry* entry, const void* data, size_t dataSize);
        [[nodiscard]] std::unique_ptr<Object> CreateObjectFromZipFile(
            IObjectRepository& objectRepository, std::string_view path, bool loadImages);
        [[nodiscard]] std::unique_ptr<Object> CreateObject(ObjectType type);

        [[nodiscard]] std::unique_ptr<Object> CreateObjectFromJsonFile(
            IObjectRepository& objectRepository, const std::string& path, bool loadImages);
    } // namespace ObjectFactory
} // namespace OpenRCT2
