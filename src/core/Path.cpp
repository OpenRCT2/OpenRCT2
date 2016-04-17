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

    utf8 * GetDirectory(utf8 * buffer, size_t bufferSize, const utf8 * path)
    {
        const char pathSeperator = platform_get_path_separator();
        size_t lastPathSepIndex = String::LastIndexOf(path, pathSeperator);
        if (lastPathSepIndex == SIZE_MAX)
        {
            return String::Set(buffer, bufferSize, String::Empty);
        }

        size_t copyLength = Math::Min(lastPathSepIndex, bufferSize - 1);
        Memory::Copy(buffer, path, copyLength);
        buffer[copyLength] = '\0';
        return buffer;
    }

    const utf8 * GetFileName(const utf8 * path)
    {
        const utf8 * lastPathSeperator = nullptr;
        for (const utf8 * ch = path; *ch != '\0'; ch++)
        {
            if (*ch == platform_get_path_separator())
            {
                lastPathSeperator = ch;
            }
#ifdef _WINDOWS_
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

    utf8 * GetFileNameWithoutExtension(utf8 * buffer, size_t bufferSize, const utf8 * path)
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
            return String::Set(buffer, bufferSize, path);
        }

        size_t truncatedLength = Math::Min<size_t>(bufferSize - 1, lastDot - path);
        Memory::Copy(buffer, path, truncatedLength);
        buffer[truncatedLength] = '\0';
        return buffer;
    }

    utf8 * GetAbsolute(utf8 *buffer, size_t bufferSize, const utf8 * relativePath)
    {
#if __WINDOWS__
        wchar_t * relativePathW = utf8_to_widechar(relativePath);
        wchar_t   absolutePathW[MAX_PATH];
        DWORD length = GetFullPathNameW(relativePathW, Util::CountOf(absolutePathW), absolutePathW, NULL);
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

    bool Equals(const utf8 * a, const utf8 * b)
    {
        bool ignoreCase = false;
#if __WINDOWS__
        ignoreCase = true;
#endif
        return String::Equals(a, b, ignoreCase);
    }
}
