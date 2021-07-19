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
    bool Exists(const std::string& path);
    bool Copy(const std::string& srcPath, const std::string& dstPath, bool overwrite);
    bool Delete(const std::string& path);
    bool Move(const std::string& srcPath, const std::string& dstPath);
    std::vector<uint8_t> ReadAllBytes(std::string_view path);
    std::string ReadAllText(std::string_view path);
    std::vector<std::string> ReadAllLines(std::string_view path);
    void WriteAllBytes(const std::string& path, const void* buffer, size_t length);
    uint64_t GetLastModified(const std::string& path);
    uint64_t GetSize(std::string_view path);
} // namespace File
