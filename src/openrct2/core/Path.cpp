#pragma region Copyright (c) 2014-2017 OpenRCT2 Developers
/*****************************************************************************
 * OpenRCT2, an open source clone of Roller Coaster Tycoon 2.
 *
 * OpenRCT2 is the work of many authors, a full list can be found in contributors.md
 * For more information, visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * A full copy of the GNU General Public License can be found in licence.txt
 *****************************************************************************/
#pragma endregion

extern "C"
{
    #include "../platform/platform.h"
    #include "../localisation/localisation.h"
    #include "../util/util.h"
}

#include "Math.hpp"
#include "Memory.hpp"
#include "Path.hpp"
#include "String.hpp"
#include "Util.hpp"

namespace Path
{
    utf8 * Append(utf8 * buffer, size_t bufferSize, const utf8 * src)
    {
        return safe_strcat_path(buffer, src, bufferSize);
    }

    std::string Combine(const std::string &a, const std::string &b)
    {
        utf8 buffer[MAX_PATH];
        String::Set(buffer, sizeof(buffer), a.c_str());
        Path::Append(buffer, sizeof(buffer), b.c_str());
        return std::string(buffer);
    }

    std::string GetDirectory(const std::string &path)
    {
        const utf8* directory = GetDirectory(path.c_str());
        std::string result(directory);
        Memory::Free(directory);
        return result;
    }

    utf8 * GetDirectory(const utf8 * path)
    {
        size_t maxSize = String::SizeOf(path) + 1;
        utf8 * result = Memory::Allocate<utf8>(maxSize);
        GetDirectory(result, maxSize, path);
        size_t reducedSize = String::SizeOf(path) + 1;
        result = Memory::Reallocate(result, reducedSize);
        return result;
    }

    utf8 * GetDirectory(utf8 * buffer, size_t bufferSize, const utf8 * path)
    {
        size_t lastPathSepIndex = String::LastIndexOf(path, *PATH_SEPARATOR);
        if (lastPathSepIndex == SIZE_MAX)
        {
            return String::Set(buffer, bufferSize, String::Empty);
        }

        size_t copyLength = Math::Min(lastPathSepIndex, bufferSize - 1);
        Memory::Copy(buffer, path, copyLength);
        buffer[copyLength] = '\0';
        return buffer;
    }

    std::string GetFileName(const std::string &path)
    {
        return GetFileName(path.c_str());
    }

    const utf8 * GetFileName(const utf8 * path)
    {
        const utf8 * lastPathSeperator = nullptr;
        for (const utf8 * ch = path; *ch != '\0'; ch++)
        {
            if (*ch == *PATH_SEPARATOR)
            {
                lastPathSeperator = ch;
            }
#ifdef __WINDOWS__
            // Windows also allows forward slashes in paths
            else if (*ch == '/')
            {
                lastPathSeperator = ch;
            }
#endif
        }

        return lastPathSeperator == nullptr ?
            path :
            lastPathSeperator + 1;
    }

    std::string GetFileNameWithoutExtension(const std::string &path)
    {
        utf8 * cstr = GetFileNameWithoutExtension(path.c_str());
        std::string result = String::ToStd(cstr);
        Memory::Free(cstr);
        return result;
    }

    utf8 * GetFileNameWithoutExtension(const utf8 * path)
    {
        size_t maxSize = String::SizeOf(path) + 1;
        utf8 * result = Memory::Allocate<utf8>(maxSize);
        GetFileNameWithoutExtension(result, maxSize, path);
        size_t reducedSize = String::SizeOf(path) + 1;
        result = Memory::Reallocate(result, reducedSize);
        return result;
    }

    utf8 * GetFileNameWithoutExtension(utf8 * buffer, size_t bufferSize, const utf8 * path)
    {
        path = GetFileName(path);

        const utf8 * lastDot = nullptr;
        const utf8 * ch = path;
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

        size_t truncatedLength = Math::Min<size_t>(bufferSize - 1, lastDot - path);
        Memory::Copy(buffer, path, truncatedLength);
        buffer[truncatedLength] = '\0';
        return buffer;
    }

    const std::string GetExtension(const std::string &path)
    {
        return GetExtension(path.c_str());
    }

    const utf8 * GetExtension(const utf8 * path)
    {
        const utf8 * lastDot = nullptr;
        const utf8 * ch = path;
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

    utf8 * GetAbsolute(utf8 *buffer, size_t bufferSize, const utf8 * relativePath)
    {
#ifdef __WINDOWS__
        wchar_t * relativePathW = utf8_to_widechar(relativePath);
        wchar_t   absolutePathW[MAX_PATH];
        DWORD length = GetFullPathNameW(relativePathW, (DWORD)Util::CountOf(absolutePathW), absolutePathW, NULL);
        Memory::Free(relativePathW);
        if (length == 0)
        {
            return String::Set(buffer, bufferSize, relativePath);
        }
        else
        {
            utf8 * absolutePath = widechar_to_utf8(absolutePathW);
            String::Set(buffer, bufferSize, absolutePath);
            Memory::Free(absolutePath);
            return buffer;
        }
#else
        utf8 * absolutePath = realpath(relativePath, NULL);
        if (absolutePath == nullptr)
        {
            return String::Set(buffer, bufferSize, relativePath);
        }
        else
        {
            String::Set(buffer, bufferSize, absolutePath);
            Memory::Free(absolutePath);
            return buffer;
        }
#endif
    }

    bool Equals(const std::string &a, const std::string &b)
    {
        return String::Equals(a.c_str(), b.c_str());
    }

    bool Equals(const utf8 * a, const utf8 * b)
    {
        bool ignoreCase = false;
#ifdef __WINDOWS__
        ignoreCase = true;
#endif
        return String::Equals(a, b, ignoreCase);
    }
}
