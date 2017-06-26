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

#pragma once

#include <cstdarg>
#include <string>
#include <vector>
#include "../common.h"

namespace String
{
    constexpr const utf8 * Empty = "";

    std::string     ToStd(const utf8 * str);
    std::string     StdFormat(const utf8 * format, ...);
    std::string     ToUtf8(const std::wstring &s);
    std::wstring    ToUtf16(const std::string &s);

    bool   IsNullOrEmpty(const utf8 * str);
    sint32 Compare(const std::string &a, const std::string &b, bool ignoreCase = false);
    sint32 Compare(const utf8 * a, const utf8 * b, bool ignoreCase = false);
    bool   Equals(const std::string &a, const std::string &b, bool ignoreCase = false);
    bool   Equals(const utf8 * a, const utf8 * b, bool ignoreCase = false);
    bool   StartsWith(const utf8 * str, const utf8 * match, bool ignoreCase = false);
    size_t IndexOf(const utf8 * str, utf8 match, size_t startIndex = 0);
    size_t LastIndexOf(const utf8 * str, utf8 match);

    /**
     * Gets the length of the given string in codepoints.
     */
    size_t LengthOf(const utf8 * str);

    /**
     * Gets the size of the given string in bytes excluding the null terminator.
     */
    size_t SizeOf(const utf8 * str);

    utf8 * Set(utf8 * buffer, size_t bufferSize, const utf8 * src);
    utf8 * Set(utf8 * buffer, size_t bufferSize, const utf8 * src, size_t srcSize);
    utf8 * Append(utf8 * buffer, size_t bufferSize, const utf8 * src);
    utf8 * Format(utf8 * buffer, size_t bufferSize, const utf8 * format, ...);
    utf8 * Format(const utf8 * format, ...);
    utf8 * Format_VA(const utf8 * format, va_list args);
    utf8 * AppendFormat(utf8 * buffer, size_t bufferSize, const utf8 * format, ...);
    utf8 * Duplicate(const std::string &src);
    utf8 * Duplicate(const utf8 * src);

    /**
     * Helper method to free the string a string pointer points to and set it to a replacement string.
     */
    utf8 * DiscardUse(utf8 * * ptr, utf8 * replacement);

    /**
     * Helper method to free the string a string pointer points to and set it to a copy of a replacement string.
     */
    utf8 * DiscardDuplicate(utf8 * * ptr, const utf8 * replacement);

    /**
     * Creates a new string containing the characters between index and the end of the input string.
     */
    utf8 * Substring(const utf8 * buffer, size_t index);

    /**
     * Creates a new string containing the characters between index and index + size of the input string.
     */
    utf8 * Substring(const utf8 * buffer, size_t index, size_t size);

    /**
     * Splits the given string by a delimiter and returns the values as a new string array.
     * @returns the number of values.
     */
    std::vector<std::string> Split(const std::string &s, const std::string &delimiter);

    utf8 *       SkipBOM(utf8 * buffer);
    const utf8 * SkipBOM(const utf8 * buffer);

    size_t      GetCodepointLength(codepoint_t codepoint);
    codepoint_t GetNextCodepoint(utf8 * ptr, utf8 * * nextPtr = nullptr);
    codepoint_t GetNextCodepoint(const utf8 * ptr, const utf8 * * nextPtr = nullptr);
    utf8 *      WriteCodepoint(utf8 * dst, codepoint_t codepoint);

    utf8 *          Trim(utf8 * str);
    const utf8 *    TrimStart(const utf8 * str);
    utf8 *          TrimStart(utf8 * buffer, size_t bufferSize, const utf8 * src);
    std::string     Trim(const std::string &s);
}
