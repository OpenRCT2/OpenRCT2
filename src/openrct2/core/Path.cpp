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
            else
            {
                return std::string(a) + std::string(b);
            }
        }
        else
        {
            if (Platform::IsPathSeparator(bBegin))
            {
                return std::string(a) + std::string(b);
            }
            else
            {
                return std::string(a) + PATH_SEPARATOR + std::string(b);
            }
        }
    }

    std::string GetDirectory(const std::string& path)
    {
        const utf8* directory = GetDirectory(path.c_str());
        std::string result(directory);
        Memory::Free(directory);
        return result;
    }

    utf8* GetDirectory(const utf8* path)
    {
        size_t maxSize = String::SizeOf(path) + 1;
        utf8* result = Memory::Allocate<utf8>(maxSize);
        GetDirectory(result, maxSize, path);
        size_t reducedSize = String::SizeOf(path) + 1;
        result = Memory::Reallocate(result, reducedSize);
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

    void CreateDirectory(const std::string& path)
    {
        platform_ensure_directory_exists(path.c_str());
    }

    bool DirectoryExists(const std::string& path)
    {
        return platform_directory_exists(path.c_str());
    }

    std::string GetFileName(const std::string& path)
    {
        return GetFileName(path.c_str());
    }

    const utf8* GetFileName(const utf8* path)
    {
        const utf8* lastPathSeparator = nullptr;
        for (const utf8* ch = path; *ch != '\0'; ch++)
        {
            if (*ch == *PATH_SEPARATOR || *ch == '/')
            {
                lastPathSeparator = ch;
            }
        }

        return lastPathSeparator == nullptr ? path : lastPathSeparator + 1;
    }

    std::string GetFileNameWithoutExtension(const std::string& path)
    {
        utf8* cstr = GetFileNameWithoutExtension(path.c_str());
        std::string result = String::ToStd(cstr);
        Memory::Free(cstr);
        return result;
    }

    utf8* GetFileNameWithoutExtension(const utf8* path)
    {
        size_t maxSize = String::SizeOf(path) + 1;
        utf8* result = Memory::Allocate<utf8>(maxSize);
        GetFileNameWithoutExtension(result, maxSize, path);
        size_t reducedSize = String::SizeOf(path) + 1;
        result = Memory::Reallocate(result, reducedSize);
        return result;
    }

    utf8* GetFileNameWithoutExtension(utf8* buffer, size_t bufferSize, const utf8* path)
    {
        path = GetFileName(path);

        const utf8* lastDot = nullptr;
        const utf8* ch = path;
        for (; *ch != '\0'; ch++)
        {
            if (*ch == '.')
            {
                lastDot = ch;
            }
        }

        if (lastDot == nullptr)
        {
            return String::Set(buffer, bufferSize, path);
        }

        size_t truncatedLength = std::min<size_t>(bufferSize - 1, lastDot - path);
        std::copy_n(path, truncatedLength, buffer);
        buffer[truncatedLength] = '\0';
        return buffer;
    }

    const std::string GetExtension(const std::string& path)
    {
        return fs::u8path(path).extension().string();
    }

    const utf8* GetExtension(const utf8* path)
    {
        const utf8* lastDot = nullptr;
        const utf8* ch = GetFileName(path);
        for (; *ch != '\0'; ch++)
        {
            if (*ch == '.')
            {
                lastDot = ch;
            }
        }

        if (lastDot == nullptr)
        {
            // Return the null terminator, i.e. a blank extension
            return ch;
        }

        // Return the extension including the dot
        return lastDot;
    }

    utf8* GetAbsolute(utf8* buffer, size_t bufferSize, const utf8* relativePath)
    {
        return Platform::GetAbsolutePath(buffer, bufferSize, relativePath);
    }

    std::string GetAbsolute(const std::string& relative)
    {
        utf8 absolute[MAX_PATH];
        return GetAbsolute(absolute, sizeof(absolute), relative.c_str());
    }

    bool Equals(const std::string& a, const std::string& b)
    {
        return String::Equals(a.c_str(), b.c_str());
    }

    bool Equals(const utf8* a, const utf8* b)
    {
        return String::Equals(a, b, Platform::ShouldIgnoreCase());
    }

    std::string ResolveCasing(const std::string& path)
    {
        return Platform::ResolveCasing(path, File::Exists(path));
    }
} // namespace Path
