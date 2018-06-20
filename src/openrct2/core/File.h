/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include <string>
#include <string_view>
#include <vector>
#include "../common.h"

namespace File
{
    bool Exists(const std::string &path);
    bool Copy(const std::string &srcPath, const std::string &dstPath, bool overwrite);
    bool Delete(const std::string &path);
    bool Move(const std::string &srcPath, const std::string &dstPath);
    std::vector<uint8_t> ReadAllBytes(const std::string_view& path);
    std::string ReadAllText(const std::string_view& path);
    void WriteAllBytes(const std::string &path, const void * buffer, size_t length);
    std::vector<std::string> ReadAllLines(const std::string &path);
    uint64_t GetLastModified(const std::string &path);
} // namespace File
