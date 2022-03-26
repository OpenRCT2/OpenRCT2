/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../common.h"

#include <cstdarg>
#include <cstddef>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

using utf8 = char;
using utf8string = utf8*;
using const_utf8string = const utf8*;
using u8string = std::basic_string<utf8>;
using u8string_view = std::basic_string_view<utf8>;

using codepoint_t = uint32_t;

namespace CODE_PAGE
{
    constexpr int32_t OPENRCT2_CP_932 = 932;   // ANSI/OEM Japanese; Japanese (Shift-JIS)
    constexpr int32_t OPENRCT2_CP_936 = 936;   // ANSI/OEM Simplified Chinese (PRC, Singapore); Chinese Simplified (GB2312)
    constexpr int32_t OPENRCT2_CP_949 = 949;   // ANSI/OEM Korean (Unified Hangul Code)
    constexpr int32_t OPENRCT2_CP_950 = 950;   // ANSI/OEM Traditional Chinese (Taiwan; Hong Kong SAR, PRC); Chinese Traditional
                                               // (Big5)
    constexpr int32_t OPENRCT2_CP_1252 = 1252; // ANSI Latin 1; Western European (Windows)
    constexpr int32_t OPENRCT2_CP_UTF8 = 65001; // Unicode (UTF-8)
} // namespace CODE_PAGE

namespace String
{
    constexpr const utf8* Empty = "";

    std::string ToStd(const utf8* str);
    std::string StdFormat_VA(const utf8* format, va_list args);
    std::string StdFormat(const utf8* format, ...);
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
    bool Equals(std::string_view a, std::string_view b, bool ignoreCase = false);
    bool Equals(const std::string& a, const std::string& b, bool ignoreCase = false);
    bool Equals(const utf8* a, const utf8* b, bool ignoreCase = false);
    bool StartsWith(std::string_view str, std::string_view match, bool ignoreCase = false);
    bool EndsWith(std::string_view str, std::string_view match, bool ignoreCase = false);
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
    utf8* Format(const utf8* format, ...);
    utf8* Format_VA(const utf8* format, va_list args);
    utf8* AppendFormat(utf8* buffer, size_t bufferSize, const utf8* format, ...);
    utf8* Duplicate(const std::string& src);
    utf8* Duplicate(const utf8* src);

    /**
     * Helper method to free the string a string pointer points to and set it to a replacement string.
     */
    utf8* DiscardUse(utf8** ptr, utf8* replacement);

    /**
     * Helper method to free the string a string pointer points to and set it to a copy of a replacement string.
     */
    utf8* DiscardDuplicate(utf8** ptr, const utf8* replacement);

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
     * Converts a multi-byte string from one code page to another.
     */
    std::string Convert(std::string_view src, int32_t srcCodePage, int32_t dstCodePage);

    /**
     * Returns an uppercased version of a UTF-8 string.
     */
    std::string ToUpper(std::string_view src);

    template<typename T> std::optional<T> Parse(std::string_view input)
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

    // Escapes special characters in a string to the percentage equivalent that can be used in URLs.
    std::string URLEncode(std::string_view value);
} // namespace String

class CodepointView
{
private:
    std::string_view _str;

public:
    class iterator
    {
    private:
        std::string_view _str;
        size_t _index;

    public:
        iterator(std::string_view str, size_t index)
            : _str(str)
            , _index(index)
        {
        }

        bool operator==(const iterator& rhs) const
        {
            return _index == rhs._index;
        }
        bool operator!=(const iterator& rhs) const
        {
            return _index != rhs._index;
        }
        char32_t operator*() const
        {
            return GetNextCodepoint(&_str[_index], nullptr);
        }
        iterator& operator++()
        {
            if (_index < _str.size())
            {
                const utf8* nextch;
                GetNextCodepoint(&_str[_index], &nextch);
                _index = nextch - _str.data();
            }
            return *this;
        }
        iterator operator++(int)
        {
            auto result = *this;
            if (_index < _str.size())
            {
                const utf8* nextch;
                GetNextCodepoint(&_str[_index], &nextch);
                _index = nextch - _str.data();
            }
            return result;
        }

        size_t GetIndex() const
        {
            return _index;
        }

        static char32_t GetNextCodepoint(const char* ch, const char** next);
    };

    CodepointView(std::string_view str)
        : _str(str)
    {
    }

    iterator begin() const
    {
        return iterator(_str, 0);
    }

    iterator end() const
    {
        return iterator(_str, _str.size());
    }
};
