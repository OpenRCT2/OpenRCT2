/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../core/StringTypes.h"

#include <memory>
#include <string_view>

namespace OpenRCT2
{
    struct IObjectRepository;
    class Object;
    struct RCTObjectEntry;
    enum class ObjectType : uint8_t;
} // namespace OpenRCT2

namespace OpenRCT2::ObjectFactory
{
    [[nodiscard]] std::unique_ptr<Object> CreateObjectFromFile(u8string_view path, bool loadImages);
    [[nodiscard]] std::unique_ptr<Object> CreateObjectFromLegacyFile(const utf8* path, bool loadImages);
    [[nodiscard]] std::unique_ptr<Object> CreateObjectFromLegacyData(
        const RCTObjectEntry* entry, const void* data, size_t dataSize);
    [[nodiscard]] std::unique_ptr<Object> CreateObjectFromZipFile(std::string_view path, bool loadImages);
    [[nodiscard]] std::unique_ptr<Object> CreateObject(ObjectType type);

    [[nodiscard]] std::unique_ptr<Object> CreateObjectFromJsonFile(const std::string& path, bool loadImages);
} // namespace OpenRCT2::ObjectFactory
