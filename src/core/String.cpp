extern "C"
{
    #include "../localisation/localisation.h"
    #include "../util/util.h"
}

#include "Math.hpp"
#include "Memory.hpp"
#include "String.hpp"

namespace String
{
    bool Equals(const utf8 * a, const utf8 * b, bool ignoreCase)
    {
        if (a == b) return true;
        if (a == nullptr || b == nullptr) return false;

        if (ignoreCase)
        {
            return _stricmp(a, b) == 0;
        }
        else
        {
            return strcmp(a, b) == 0;
        }
    }

    bool StartsWith(const utf8 * str, const utf8 * match, bool ignoreCase)
    {
        if (ignoreCase)
        {
            while (*str != '\0' && *match != '\0')
            {
                if (tolower(*str++) != tolower(*match++))
                {
                    return false;
                }
            }
            return true;
        }
        else
        {
            while (*str != '\0' && *match != '\0')
            {
                if (*str++ != *match++)
                {
                    return false;
                }
            }
            return true;
        }
    }

    size_t LengthOf(const utf8 * str)
    {
        return utf8_length(str);
    }

    size_t SizeOf(const utf8 * str)
    {
        return strlen(str);
    }

    utf8 * Set(utf8 * buffer, size_t bufferSize, const utf8 * src)
    {
        return safe_strcpy(buffer, src, bufferSize);
    }

    utf8 * Set(utf8 * buffer, size_t bufferSize, const utf8 * src, size_t srcSize)
    {
        utf8 * dst = buffer;
        size_t minSize = Math::Min(bufferSize - 1, srcSize);
        for (size_t i = 0; i < minSize; i++)
        {
            *dst++ = *src;
            if (*src == '\0') break;
            src++;
        }
        *dst = '\0';
        return buffer;
    }

    utf8 * Append(utf8 * buffer, size_t bufferSize, const utf8 * src)
    {
        return safe_strcat(buffer, src, bufferSize);
    }

    utf8 * Format(utf8 * buffer, size_t bufferSize, const utf8 * format, ...)
    {
        va_list args;

        va_start(args, format);
        vsnprintf(buffer, bufferSize, format, args);
        va_end(args);

        // Terminate buffer in case formatted string overflowed
        buffer[bufferSize - 1] = '\0';

        return buffer;
    }

    utf8 * AppendFormat(utf8 * buffer, size_t bufferSize, const utf8 * format, ...)
    {
        va_list args;

        utf8 * dst = buffer;
        size_t i;
        for (i = 0; i < bufferSize; i++)
        {
            if (*dst == '\0') break;
            dst++;
        }

        if (i < bufferSize - 1)
        {
            va_start(args, format);
            vsnprintf(buffer, bufferSize - i - 1, format, args);
            va_end(args);

            // Terminate buffer in case formatted string overflowed
            buffer[bufferSize - 1] = '\0';
        }

        return buffer;
    }

    utf8 * Duplicate(const utf8 * src)
    {
        size_t srcSize = SizeOf(src);
        return Memory::DuplicateArray(src, srcSize + 1);
    }

    utf8 * DiscardUse(utf8 * * ptr, utf8 * replacement)
    {
        Memory::Free(*ptr);
        *ptr = replacement;
        return replacement;
    }

    utf8 * DiscardDuplicate(utf8 * * ptr, utf8 * replacement)
    {
        return DiscardUse(ptr, String::Duplicate(replacement));
    }

    utf8 * SkipBOM(utf8 * buffer)
    {
        return (utf8*)SkipBOM(buffer);
    }

    const utf8 * SkipBOM(const utf8 * buffer)
    {
        if (buffer[0] == 0xEF && buffer[1] == 0xBB && buffer[2] == 0xBF)
        {
            return buffer + 3;
        }
        return buffer;
    }

    size_t GetCodepointLength(codepoint_t codepoint)
    {
        return utf8_get_codepoint_length(codepoint);
    }

    codepoint_t GetNextCodepoint(utf8 * ptr, utf8 * * nextPtr)
    {
        return GetNextCodepoint((const utf8 *)ptr, (const utf8 * *)nextPtr);
    }

    codepoint_t GetNextCodepoint(const utf8 * ptr, const utf8 * * nextPtr)
    {
        return utf8_get_next(ptr, nextPtr);
    }

    utf8 * WriteCodepoint(utf8 * dst, codepoint_t codepoint)
    {
        return utf8_write_codepoint(dst, codepoint);
    }
}
