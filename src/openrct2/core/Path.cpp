/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "Path.hpp"

#include "../localisation/Language.h"
#include "../platform/Platform.h"
#include "../util/Util.h"
#include "File.h"
#include "FileSystem.hpp"
#include "Memory.hpp"
#include "String.hpp"

#include <algorithm>
#include <iterator>

namespace Path
{
    u8string Combine(u8string_view a, u8string_view b)
    {
        if (a.empty())
            return u8string(b);
        if (b.empty())
            return u8string(a);
        auto aEnd = a.back();
        auto bBegin = b.front();
        if (Platform::IsPathSeparator(aEnd))
        {
            if (Platform::IsPathSeparator(bBegin))
            {
                return u8string(a) + u8string(b.substr(1));
            }

            return u8string(a) + u8string(b);
        }

        if (Platform::IsPathSeparator(bBegin))
        {
            return u8string(a) + u8string(b);
        }

        return u8string(a) + PATH_SEPARATOR + u8string(b);
    }

    u8string GetDirectory(u8string_view path)
    {
        return u8path(path).parent_path().u8string();
    }

    void CreateDirectory(u8string_view path)
    {
        Platform::EnsureDirectoryExists(u8string(path).c_str());
    }

    bool DirectoryExists(u8string_view path)
    {
        std::error_code ec;
        const auto result = fs::is_directory(u8path(path), ec);
        return result && ec.value() == 0;
    }

    u8string GetFileName(u8string_view path)
    {
        return u8path(path).filename().u8string();
    }

    u8string GetFileNameWithoutExtension(u8string_view path)
    {
        return u8path(path).stem().u8string();
    }

    u8string GetExtension(u8string_view path)
    {
        return u8path(path).extension().u8string();
    }

    u8string GetAbsolute(u8string_view relative)
    {
        std::error_code ec;
        return fs::absolute(u8path(relative), ec).u8string();
    }

    bool Equals(u8string_view a, u8string_view b)
    {
        return String::Equals(a, b, Platform::ShouldIgnoreCase());
    }

    u8string ResolveCasing(u8string_view path)
    {
        return Platform::ResolveCasing(path, File::Exists(path));
    }

    bool DeleteDirectory(u8string_view path)
    {
        std::error_code ec;
        const auto result = fs::remove_all(u8path(path), ec);
        return (result > 0) && ec.value() == 0;
    }
} // namespace Path
