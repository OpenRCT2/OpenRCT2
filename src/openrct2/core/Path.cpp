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
        size_t maxSize = String::SizeOf(std::string(path).c_str()) + 1;
        utf8* buffer = Memory::Allocate<utf8>(maxSize);
        GetDirectory(buffer, maxSize, std::string(path).c_str());
        std::string result(buffer);
        Memory::Free(buffer);
        return result;
    }

    utf8* GetDirectory(utf8* buffer, size_t bufferSize, const utf8* path)
    {
        auto lastPathSepIndex = std::max(String::LastIndexOf(path, *PATH_SEPARATOR), String::LastIndexOf(path, '/'));
        if (lastPathSepIndex < 0)
        {
            return String::Set(buffer, bufferSize, String::Empty);
        }

        size_t copyLength = std::min(lastPathSepIndex, static_cast<ptrdiff_t>(bufferSize - 1));
        std::copy_n(path, copyLength, buffer);
        buffer[copyLength] = '\0';
        return buffer;
    }

    void CreateDirectory(std::string_view path)
    {
        platform_ensure_directory_exists(std::string(path).c_str());
    }

    bool DirectoryExists(std::string_view path)
    {
        return platform_directory_exists(std::string(path).c_str());
    }

    std::string GetFileName(std::string_view path)
    {
        return u8path(path).filename().string();
    }

    std::string GetFileNameWithoutExtension(std::string_view path)
    {
        return u8path(path).stem().string();
    }

    std::string GetExtension(std::string_view path)
    {
        return u8path(path).extension().string();
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
} // namespace Path
