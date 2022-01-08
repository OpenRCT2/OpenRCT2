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

#include <string>
#include <string_view>
#include <vector>

namespace File
{
    bool Exists(std::string_view path);
    bool Copy(std::string_view srcPath, std::string_view dstPath, bool overwrite);
    bool Delete(std::string_view path);
    bool Move(std::string_view srcPath, std::string_view dstPath);
    std::vector<uint8_t> ReadAllBytes(std::string_view path);
    std::string ReadAllText(std::string_view path);
    std::vector<std::string> ReadAllLines(std::string_view path);
    void WriteAllBytes(std::string_view path, const void* buffer, size_t length);
    uint64_t GetLastModified(std::string_view path);
    uint64_t GetSize(std::string_view path);
} // namespace File
