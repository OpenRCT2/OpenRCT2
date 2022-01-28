/*****************************************************************************
 * Copyright (c) 2014-2021 OpenRCT2 developers
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
#include <string_view>
#include <vector>

namespace File
{
    bool Exists(u8string_view path);
    bool Copy(u8string_view srcPath, u8string_view dstPath, bool overwrite);
    bool Delete(u8string_view path);
    bool Move(u8string_view srcPath, u8string_view dstPath);
    std::vector<uint8_t> ReadAllBytes(u8string_view path);
    u8string ReadAllText(u8string_view path);
    std::vector<u8string> ReadAllLines(u8string_view path);
    void WriteAllBytes(u8string_view path, const void* buffer, size_t length);
    uint64_t GetLastModified(u8string_view path);
    uint64_t GetSize(u8string_view path);
} // namespace File
