#pragma region Copyright (c) 2014-2016 OpenRCT2 Developers
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

#include <cwctype>

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
    std::string ToStd(const utf8 * str)
    {
        if (str == nullptr) return std::string();
        else return std::string(str);
    }

    bool IsNullOrEmpty(const utf8 * str)
    {
        return str == nullptr || str[0] == '\0';
    }

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

    size_t LastIndexOf(const utf8 * str, utf8 match)
    {
        const utf8 * lastOccurance = nullptr;
        const utf8 * ch = str;
        for (; *ch != '\0'; ch++)
        {
            if (*ch == match)
            {
                lastOccurance = ch;
            }
        }

        if (lastOccurance == nullptr)
        {
            return SIZE_MAX;
        }
        else
        {
            return (size_t)(lastOccurance - str);
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
        utf8 * result = nullptr;
        if (src != nullptr)
        {
            size_t srcSize = SizeOf(src);
            result = Memory::DuplicateArray(src, srcSize + 1);
        }
        return result;
    }

    utf8 * DiscardUse(utf8 * * ptr, utf8 * replacement)
    {
        Memory::Free(*ptr);
        *ptr = replacement;
        return replacement;
    }

    utf8 * DiscardDuplicate(utf8 * * ptr, const utf8 * replacement)
    {
        return DiscardUse(ptr, String::Duplicate(replacement));
    }

    utf8 * SkipBOM(utf8 * buffer)
    {
        return (utf8*)SkipBOM((const utf8 *)buffer);
    }

    const utf8 * SkipBOM(const utf8 * buffer)
    {
        if ((unsigned char)buffer[0] == 0xEF && (unsigned char)buffer[1] == 0xBB && (unsigned char)buffer[2] == 0xBF)
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

    utf8 * Trim(utf8 * str)
    {
        utf8 * firstNonWhitespace = nullptr;

        codepoint_t codepoint;
        utf8 * ch = str;
        utf8 * nextCh;
        while ((codepoint = GetNextCodepoint(ch, &nextCh)) != '\0')
        {
            if (codepoint <= WCHAR_MAX && !iswspace((wchar_t)codepoint))
            {
                if (firstNonWhitespace == nullptr)
                {
                    firstNonWhitespace = ch;
                }
            }
            ch = nextCh;
        }

        if (firstNonWhitespace != nullptr &&
            firstNonWhitespace != str)
        {
            size_t newStringSize = ch - firstNonWhitespace;
#if DEBUG
            size_t currentStringSize = String::SizeOf(str);
            Guard::Assert(newStringSize < currentStringSize, GUARD_LINE);
#endif

            Memory::Move(str, firstNonWhitespace, newStringSize);
            str[newStringSize] = '\0';
        }
        else
        {
            *ch = '\0';
        }

        return str;
    }

    const utf8 * TrimStart(const utf8 * str)
    {
        codepoint_t codepoint;
        const utf8 * ch = str;
        const utf8 * nextCh;
        while ((codepoint = GetNextCodepoint(ch, &nextCh)) != '\0')
        {
            if (codepoint <= WCHAR_MAX && !iswspace((wchar_t)codepoint))
            {
                return ch;
            }
            ch = nextCh;
        }
        return str;
    }

    utf8 * TrimStart(utf8 * buffer, size_t bufferSize, const utf8 * src)
    {
        return String::Set(buffer, bufferSize, TrimStart(src));
    }
}
