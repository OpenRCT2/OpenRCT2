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
#include "../platform/Platform2.h"
#include "../platform/platform.h"
#include "../util/Util.h"
#include "File.h"
#include "FileSystem.hpp"
#include "Memory.hpp"
#include "String.hpp"

#include <algorithm>
#include <iterator>

namespace Path
{
    utf8* Append(utf8* buffer, size_t bufferSize, const utf8* src)
    {
        return safe_strcat_path(buffer, src, bufferSize);
    }

    std::string Combine(std::string_view a, std::string_view b)
    {
        if (a.empty())
            return std::string(b);
        if (b.empty())
            return std::string(a);
        auto aEnd = a.back();
        auto bBegin = b.front();
        if (Platform::IsPathSeparator(aEnd))
        {
            if (Platform::IsPathSeparator(bBegin))
            {
                return std::string(a) + std::string(b.substr(1));
            }

            return std::string(a) + std::string(b);
        }

        if (Platform::IsPathSeparator(bBegin))
        {
            return std::string(a) + std::string(b);
        }

        return std::string(a) + PATH_SEPARATOR + std::string(b);
    }

    std::string GetDirectory(std::string_view path)
    {
        return u8path(path).parent_path().u8string();
    }

    void CreateDirectory(std::string_view path)
    {
        platform_ensure_directory_exists(std::string(path).c_str());
    }

    bool DirectoryExists(std::string_view path)
    {
        return fs::is_directory(u8path(path));
    }

    std::string GetFileName(std::string_view path)
    {
        return u8path(path).filename().u8string();
    }

    std::string GetFileNameWithoutExtension(std::string_view path)
    {
        return u8path(path).stem().u8string();
    }

    std::string GetExtension(std::string_view path)
    {
        return u8path(path).extension().u8string();
    }

    utf8* GetAbsolute(utf8* buffer, size_t bufferSize, const utf8* relativePath)
    {
        return Platform::GetAbsolutePath(buffer, bufferSize, relativePath);
    }

    std::string GetAbsolute(std::string_view relative)
    {
        utf8 absolute[MAX_PATH];
        return GetAbsolute(absolute, sizeof(absolute), std::string(relative).c_str());
    }

    bool Equals(std::string_view a, std::string_view b)
    {
        return String::Equals(a, b, Platform::ShouldIgnoreCase());
    }

    std::string ResolveCasing(std::string_view path)
    {
        return Platform::ResolveCasing(path, File::Exists(path));
    }

    bool DeleteDirectory(std::string_view path)
    {
        return fs::remove_all(u8path(path)) > 0;
    }
} // namespace Path
