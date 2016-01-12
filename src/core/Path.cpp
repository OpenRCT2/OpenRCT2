extern "C"
{
    #include "../platform/platform.h"
    #include "../localisation/localisation.h"
    #include "../util/util.h"
}

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

    utf8 * GetAbsolute(utf8 *buffer, size_t bufferSize, const utf8 * relativePath)
    {
#if _WIN32
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
}
