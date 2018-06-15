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
#include "../common.h"

namespace Path
{
    utf8 * Append(utf8 * buffer, size_t bufferSize, const utf8 * src);
    std::string Combine(const std::string &a, const std::string &b);

    template<typename... Args>
    static std::string Combine(const std::string &a, const std::string &b, Args... args)
    {
        return Combine(a, Combine(b, args...));
    }

    std::string GetDirectory(const std::string &path);
    utf8 * GetDirectory(const utf8 * path);
    utf8 * GetDirectory(utf8 * buffer, size_t bufferSize, const utf8 * path);
    void CreateDirectory(const std::string &path);
    bool DirectoryExists(const std::string &path);
    std::string GetFileName(const std::string &path);
    const utf8 * GetFileName(const utf8 * path);
    std::string GetFileNameWithoutExtension(const std::string &path);
    utf8 * GetFileNameWithoutExtension(const utf8 * path);
    utf8 * GetFileNameWithoutExtension(utf8 * buffer, size_t bufferSize, const utf8 * path);
    const std::string GetExtension(const std::string &path);
    const utf8 * GetExtension(const utf8 * path);
    utf8 * GetAbsolute(utf8 * buffer, size_t bufferSize, const utf8 * relativePath);
    std::string GetAbsolute(const std::string &relative);
    bool Equals(const std::string &a, const std::string &b);
    bool Equals(const utf8 * a, const utf8 * b);

    /**
     * Checks if the given path is a file. If not, checks to see if
     * there are any files with different casing and selects the first
     * one found based on a straight forward character sort.
     * Note: This will not resolve the case for Windows.
     */
    std::string ResolveCasing(const std::string &path);
} // namespace Path
