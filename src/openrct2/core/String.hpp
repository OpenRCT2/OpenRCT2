/*****************************************************************************
 * Copyright (c) 2014-2024 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "StringTypes.h"

#include <cstdarg>
#include <cstddef>
#include <optional>
#include <string_view>
#include <vector>

namespace OpenRCT2::String
{
    constexpr const utf8* Empty = "";

    std::string ToStd(const utf8* str);
    std::string ToUtf8(std::wstring_view src);
    std::wstring ToWideChar(std::string_view src);

    /**
     * Creates a string_view from a char pointer with a length up to either the
     * first null terminator or a given maximum length, whatever is smallest.
     */
    std::string_view ToStringView(const char* ch, size_t maxLen);

    bool IsNullOrEmpty(const utf8* str);
    int32_t Compare(const std::string& a, const std::string& b, bool ignoreCase = false);
    int32_t Compare(const utf8* a, const utf8* b, bool ignoreCase = false);

    bool Equals(u8string_view a, u8string_view b);
    bool Equals(const u8string& a, const u8string& b);
    bool Equals(const utf8* a, const utf8* b, bool ignoreCase = false);
    bool IEquals(u8string_view a, u8string_view b);
    bool IEquals(const u8string& a, const u8string& b);
    bool IEquals(const utf8* a, const utf8* b);

    bool StartsWith(std::string_view str, std::string_view match, bool ignoreCase = false);
    bool EndsWith(std::string_view str, std::string_view match, bool ignoreCase = false);
    bool Contains(std::string_view haystack, std::string_view needle, bool ignoreCase = false);
    size_t IndexOf(const utf8* str, utf8 match, size_t startIndex = 0);
    ptrdiff_t LastIndexOf(const utf8* str, utf8 match);

    /**
     * Gets the length of the given string in codepoints.
     */
    size_t LengthOf(const utf8* str);

    /**
     * Gets the size of the given string in bytes excluding the null terminator.
     */
    size_t SizeOf(const utf8* str);

    utf8* Set(utf8* buffer, size_t bufferSize, const utf8* src);
    utf8* Set(utf8* buffer, size_t bufferSize, const utf8* src, size_t srcSize);
    utf8* Append(utf8* buffer, size_t bufferSize, const utf8* src);
    utf8* Format(utf8* buffer, size_t bufferSize, const utf8* format, ...);
    u8string StdFormat(const utf8* format, ...);
    u8string Format_VA(const utf8* format, va_list args);
    utf8* AppendFormat(utf8* buffer, size_t bufferSize, const utf8* format, ...);

    /**
     * Splits the given string by a delimiter and returns the values as a new string array.
     * @returns the number of values.
     */
    std::vector<std::string> Split(std::string_view s, std::string_view delimiter);

    utf8* SkipBOM(utf8* buffer);
    const utf8* SkipBOM(const utf8* buffer);

    size_t GetCodepointLength(codepoint_t codepoint);
    codepoint_t GetNextCodepoint(utf8* ptr, utf8** nextPtr = nullptr);
    codepoint_t GetNextCodepoint(const utf8* ptr, const utf8** nextPtr = nullptr);
    utf8* WriteCodepoint(utf8* dst, codepoint_t codepoint);
    void AppendCodepoint(std::string& str, codepoint_t codepoint);

    bool IsWhiteSpace(codepoint_t codepoint);
    utf8* Trim(utf8* str);
    const utf8* TrimStart(const utf8* str);
    utf8* TrimStart(utf8* buffer, size_t bufferSize, const utf8* src);
    std::string TrimStart(const std::string& s);
    std::string Trim(const std::string& s);

    /**
     * Converts a multi-byte string from one code page to UTF-8.
     */
    std::string ConvertToUtf8(std::string_view src, int32_t srcCodePage);

    /**
     * Returns an uppercased version of a UTF-8 string.
     */
    std::string ToUpper(std::string_view src);

    template<typename T>
    std::optional<T> Parse(std::string_view input)
    {
        if (input.size() == 0)
            return std::nullopt;

        T result = 0;
        for (size_t i = 0; i < input.size(); i++)
        {
            auto chr = input[i];
            if (chr >= '0' && chr <= '9')
            {
                auto digit = chr - '0';
                auto last = result;
                result = static_cast<T>((result * 10) + digit);
                if (result <= last)
                {
                    // Overflow, number too large for type
                    return std::nullopt;
                }
            }
            else
            {
                // Bad character
                return std::nullopt;
            }
        }
        return result;
    }

    /**
     * Returns string representation of a hexadecimal input, such as SHA256 hash
     */
    template<typename T>
    std::string StringFromHex(T input)
    {
        std::string result;
        result.reserve(input.size() * 2);
        for (auto b : input)
        {
            static_assert(sizeof(b) == 1);

            char buf[3];
            snprintf(buf, 3, "%02x", static_cast<int32_t>(b));
            result.append(buf);
        }
        return result;
    }

    /**
     * Returns codepoint size or no value if not valid
     */
    constexpr std::optional<int> UTF8GetCodePointSize(std::string_view v)
    {
        if (v.size() >= 1 && !(v[0] & 0x80))
        {
            return { 1 };
        }
        if (v.size() >= 2 && ((v[0] & 0xE0) == 0xC0))
        {
            return { 2 };
        }
        if (v.size() >= 3 && ((v[0] & 0xF0) == 0xE0))
        {
            return { 3 };
        }
        if (v.size() >= 4 && ((v[0] & 0xF8) == 0xF0))
        {
            return { 4 };
        }
        return std::nullopt;
    }

    /**
     * Truncates a string to at most `size` bytes,
     * making sure not to cut in the middle of a sequence.
     */
    std::string_view UTF8Truncate(std::string_view v, size_t size);

    /**
     * Truncates a string to at most `size` codepoints,
     * making sure not to cut in the middle of a sequence.
     */
    std::string_view UTF8TruncateCodePoints(std::string_view v, size_t size);

    // Escapes special characters in a string to the percentage equivalent that can be used in URLs.
    std::string URLEncode(std::string_view value);
} // namespace OpenRCT2::String
