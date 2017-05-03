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
#include <stdexcept>
#include <vector>

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

    std::string StdFormat(const utf8 * format, ...)
    {
        va_list args;
        va_start(args, format);
        const utf8 * buffer = Format_VA(format, args);
        va_end(args);
        std::string returnValue = ToStd(buffer);
        Memory::Free(buffer);
        return returnValue;
    }

    bool IsNullOrEmpty(const utf8 * str)
    {
        return str == nullptr || str[0] == '\0';
    }

    sint32 Compare(const std::string &a, const std::string &b, bool ignoreCase)
    {
        return Compare(a.c_str(), b.c_str(), ignoreCase);
    }

    sint32 Compare(const utf8 * a, const utf8 * b, bool ignoreCase)
    {
        if (a == b) return true;
        if (a == nullptr || b == nullptr) return false;

        if (ignoreCase)
        {
            return _stricmp(a, b);
        }
        else
        {
            return strcmp(a, b);
        }
    }

    bool Equals(const std::string &a, const std::string &b, bool ignoreCase)
    {
        return Equals(a.c_str(), b.c_str(), ignoreCase);
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

    size_t IndexOf(const utf8 * str, utf8 match, size_t startIndex)
    {
        const utf8 * ch = str + startIndex;
        for (; *ch != '\0'; ch++)
        {
            if (*ch == match)
            {
                return (size_t)(ch - str);
            }
        }
        return SIZE_MAX;
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

    utf8 * Format(const utf8 * format, ...)
    {
        va_list args;
        va_start(args, format);
        utf8 * result = Format_VA(format, args);
        va_end(args);
        return result;
    }

    utf8 * Format_VA(const utf8 * format, va_list args)
    {
        va_list args1, args2;
        va_copy(args1, args);
        va_copy(args2, args);

        // Try to format to a initial buffer, enlarge if not big enough
        size_t bufferSize = 4096;
        utf8 * buffer = Memory::Allocate<utf8>(bufferSize);

        // Start with initial buffer
        sint32 len = vsnprintf(buffer, bufferSize, format, args);
        if (len < 0)
        {
            Memory::Free(buffer);
            va_end(args1);
            va_end(args2);

            // An error occurred...
            return nullptr;
        }

        size_t requiredSize = (size_t)len + 1;
        if (requiredSize > bufferSize)
        {
            // Try again with bigger buffer
            buffer = Memory::Reallocate<utf8>(buffer, bufferSize);
            len = vsnprintf(buffer, bufferSize, format, args);
            if (len < 0)
            {
                Memory::Free(buffer);
                va_end(args1);
                va_end(args2);

                // An error occurred...
                return nullptr;
            }
        }
        else
        {
            // Reduce buffer size to only what was required
            bufferSize = requiredSize;
            buffer = Memory::Reallocate<utf8>(buffer, bufferSize);
        }

        // Ensure buffer is terminated
        buffer[bufferSize - 1] = '\0';

        va_end(args1);
        va_end(args2);
        return buffer;
    }

    utf8 * AppendFormat(utf8 * buffer, size_t bufferSize, const utf8 * format, ...)
    {
        utf8 * dst = buffer;
        size_t i;
        for (i = 0; i < bufferSize; i++)
        {
            if (*dst == '\0') break;
            dst++;
        }

        if (i < bufferSize - 1)
        {
            va_list args;
            va_start(args, format);
            vsnprintf(dst, bufferSize - i - 1, format, args);
            va_end(args);

            // Terminate buffer in case formatted string overflowed
            buffer[bufferSize - 1] = '\0';
        }

        return buffer;
    }

    utf8 * Duplicate(const std::string &src)
    {
        return String::Duplicate(src.c_str());
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

    utf8 * Substring(const utf8 * buffer, size_t index)
    {
        size_t bufferSize = String::SizeOf(buffer);
        bool goodSubstring = index <= bufferSize;
        Guard::Assert(goodSubstring, "Substring past end of input string.");

        // If assertion continues, return empty string to avoid crash
        if (!goodSubstring)
        {
            return String::Duplicate("");
        }

        return String::Duplicate(buffer + index);
    }

    utf8 * Substring(const utf8 * buffer, size_t index, size_t size)
    {
        size_t bufferSize = String::SizeOf(buffer);
        bool goodSubstring = index + size <= bufferSize;
        Guard::Assert(goodSubstring, "Substring past end of input string.");

        // If assertion continues, cap the substring to avoid crash
        if (!goodSubstring)
        {
            if (index >= bufferSize)
            {
                size = 0;
            }
            else
            {
                size = bufferSize - index;
            }
        }

        utf8 * result = Memory::Allocate<utf8>(size + 1);
        Memory::Copy(result, buffer + index, size);
        result[size] = '\0';
        return result;
    }

    std::vector<std::string> Split(const std::string &s, const std::string &delimiter)
    {
        if (delimiter.empty())
        {
            throw std::invalid_argument(nameof(delimiter) " can not be empty.");
        }

        std::vector<std::string> results;
        if (!s.empty())
        {
            size_t index = 0;
            size_t nextIndex;
            do
            {
                nextIndex = s.find(delimiter, index);
                std::string value;
                if (nextIndex == std::string::npos)
                {
                    value = s.substr(index);
                }
                else
                {
                    value = s.substr(index, nextIndex - index);
                }
                results.push_back(value);
                index = nextIndex + delimiter.size();
            }
            while (nextIndex != SIZE_MAX);
        }
        return results;
    }

    utf8 * SkipBOM(utf8 * buffer)
    {
        return (utf8*)SkipBOM((const utf8 *)buffer);
    }

    const utf8 * SkipBOM(const utf8 * buffer)
    {
        if ((uint8)buffer[0] == 0xEF && (uint8)buffer[1] == 0xBB && (uint8)buffer[2] == 0xBF)
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
#ifdef DEBUG
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

    std::string Trim(const std::string &s)
    {
        codepoint_t codepoint;
        const utf8 * ch = s.c_str();
        const utf8 * nextCh;
        const utf8 * startSubstr = nullptr;
        const utf8 * endSubstr = nullptr;
        while ((codepoint = GetNextCodepoint(ch, &nextCh)) != '\0')
        {
            bool isWhiteSpace = codepoint <= WCHAR_MAX && iswspace((wchar_t)codepoint);
            if (!isWhiteSpace)
            {
                if (startSubstr == nullptr)
                {
                    startSubstr = ch;
                }
                endSubstr = ch;
            }
            ch = nextCh;
        }

        if (startSubstr == nullptr)
        {
            // String is all whitespace
            return std::string();
        }

        size_t stringLength = endSubstr - startSubstr + 1;
        return std::string(startSubstr, stringLength);
    }
}
