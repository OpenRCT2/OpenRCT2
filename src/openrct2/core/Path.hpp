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

namespace Path
{
    utf8* Append(utf8* buffer, size_t bufferSize, const utf8* src);
    std::string Combine(std::string_view a, std::string_view b);

    template<typename... Args> static std::string Combine(std::string_view a, std::string_view b, Args... args)
    {
        return Combine(a, Combine(b, args...));
    }

    std::string GetDirectory(std::string_view path);
    void CreateDirectory(std::string_view path);
    bool DirectoryExists(std::string_view path);
    bool DeleteDirectory(std::string_view path);
    std::string GetFileName(std::string_view origPath);
    std::string GetFileNameWithoutExtension(std::string_view path);
    std::string GetExtension(std::string_view path);
    utf8* GetAbsolute(utf8* buffer, size_t bufferSize, const utf8* relativePath);
    std::string GetAbsolute(std::string_view relative);
    bool Equals(std::string_view a, std::string_view b);

    /**
     * Checks if the given path is a file. If not, checks to see if
     * there are any files with different casing and selects the first
     * one found based on a straight forward character sort.
     * Note: This will not resolve the case for Windows.
     */
    std::string ResolveCasing(std::string_view path);
} // namespace Path
