/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
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
    std::string toStd(const utf8* str);
    std::string toUtf8(std::wstring_view src);
    std::wstring toWideChar(std::string_view src);

    /**
     * Creates a string_view from a char pointer with a length up to either the
     * first null terminator or a given maximum length, whatever is smallest.
     */
    std::string_view toStringView(const char* ch, size_t maxLen);

    bool isNullOrEmpty(const utf8* str);
    int32_t compare(const std::string& a, const std::string& b, bool ignoreCase = false);
    int32_t compare(const utf8* a, const utf8* b, bool ignoreCase = false);

    bool equals(u8string_view a, u8string_view b);
    bool equals(const u8string& a, const u8string& b);
    bool equals(const utf8* a, const utf8* b, bool ignoreCase = false);
    bool iequals(u8string_view a, u8string_view b);
    bool iequals(const u8string& a, const u8string& b);
    bool iequals(const utf8* a, const utf8* b);

    bool startsWith(std::string_view str, std::string_view match, bool ignoreCase = false);
    bool endsWith(std::string_view str, std::string_view match, bool ignoreCase = false);
    bool contains(std::string_view haystack, std::string_view needle, bool ignoreCase = false);
    size_t indexOf(const utf8* str, utf8 match, size_t startIndex = 0);
    ptrdiff_t lastIndexOf(const utf8* str, utf8 match);

    /**
     * Gets the length of the given string in codepoints.
     */
    size_t lengthOf(const utf8* str);

    /**
     * Gets the size of the given string in bytes excluding the null terminator.
     */
    size_t sizeOf(const utf8* str);

    utf8* set(utf8* buffer, size_t bufferSize, const utf8* src);
    utf8* set(utf8* buffer, size_t bufferSize, const utf8* src, size_t srcSize);
    utf8* append(utf8* buffer, size_t bufferSize, const utf8* src);
    utf8* format(utf8* buffer, size_t bufferSize, const utf8* format, ...);
    u8string stdFormat(const utf8* format, ...);
    u8string formatVA(const utf8* format, va_list args);
    utf8* appendFormat(utf8* buffer, size_t bufferSize, const utf8* format, ...);

    /**
     * Splits the given string by a delimiter and returns the values as a new string array.
     * @returns the number of values.
     */
    std::vector<std::string> split(std::string_view s, std::string_view delimiter);

    utf8* skipBOM(utf8* buffer);
    const utf8* skipBOM(const utf8* buffer);

    size_t getCodepointLength(codepoint_t codepoint);
    codepoint_t getNextCodepoint(utf8* ptr, utf8** nextPtr = nullptr);
    codepoint_t getNextCodepoint(const utf8* ptr, const utf8** nextPtr = nullptr);
    utf8* writeCodepoint(utf8* dst, codepoint_t codepoint);
    void appendCodepoint(std::string& str, codepoint_t codepoint);

    bool isWhiteSpace(codepoint_t codepoint);
    utf8* trim(utf8* str);
    const utf8* trimStart(const utf8* str);
    [[nodiscard]] std::string trimStart(const std::string& s);
    [[nodiscard]] std::string trim(const std::string& s);

    /**
     * Converts a multi-byte string from one code page to UTF-8.
     */
    std::string convertToUtf8(std::string_view src, int32_t srcCodePage);

    /**
     * Returns an uppercased version of a UTF-8 string.
     */
    std::string toUpper(std::string_view src);

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
    constexpr std::optional<int> utf8GetCodePointSize(std::string_view v)
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
    std::string_view utf8Truncate(std::string_view v, size_t size);

    /**
     * Truncates a string to at most `size` codepoints,
     * making sure not to cut in the middle of a sequence.
     */
    std::string_view utf8TruncateCodePoints(std::string_view v, size_t size);

    // Escapes special characters in a string to the percentage equivalent that can be used in URLs.
    std::string urlEncode(std::string_view value);

    int32_t logicalCmp(char const* a, char const* b);
    char* safeUtf8Copy(char* destination, const char* source, size_t num);
    char* safeConcat(char* destination, const char* source, size_t size);
} // namespace OpenRCT2::String
