/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../core/StringTypes.h"

namespace OpenRCT2::Path
{
    [[nodiscard]] u8string Combine(u8string_view a, u8string_view b);

    template<typename... Args>
    static u8string Combine(u8string_view a, u8string_view b, Args... args)
    {
        return Combine(a, Combine(b, args...));
    }

    u8string GetDirectory(u8string_view path);
    bool CreateDirectory(u8string_view path);
    bool DirectoryExists(u8string_view path);
    bool DeleteDirectory(u8string_view path);
    u8string GetFileName(u8string_view origPath);
    u8string GetFileNameWithoutExtension(u8string_view path);
    u8string GetExtension(u8string_view path);
    u8string WithExtension(u8string_view path, u8string_view newExtension);
    bool IsAbsolute(u8string_view path);
    u8string GetAbsolute(u8string_view relative);
    u8string GetRelative(u8string_view path, u8string_view base);
    bool Equals(u8string_view a, u8string_view b);

    /**
     * Checks if the given path is a file. If not, checks to see if
     * there are any files with different casing and selects the first
     * one found based on a straight forward character sort.
     * Note: This will not resolve the case for Windows.
     */
    u8string ResolveCasing(u8string_view path);
} // namespace OpenRCT2::Path
