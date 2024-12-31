/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "../Diagnostic.h"

#include <cassert>
#include <cctype>
#include <cwctype>
#include <iomanip>
#include <sstream>
#include <stdexcept>
#include <vector>

#ifndef _WIN32
    #if !defined(__FreeBSD__) && !defined(__NetBSD__) && !defined(__OpenBSD__)
        #include <alloca.h>
    #endif
    #include <unicode/ucnv.h>
    #include <unicode/unistr.h>
    #include <unicode/utypes.h>
#else
    #include <windows.h>
#endif

#include "Memory.hpp"
#include "String.hpp"
#include "StringBuilder.h"
#include "UTF8.h"

#if defined(__unix__) || (defined(__APPLE__) && defined(__MACH__))
    #include <strings.h>
    #define _stricmp(x, y) strcasecmp((x), (y))
#endif

namespace OpenRCT2::String
{
    std::string toStd(const utf8* str)
    {
        if (str == nullptr)
            return std::string();

        return std::string(str);
    }

    std::string toUtf8(std::wstring_view src)
    {
#ifdef _WIN32
        int srcLen = static_cast<int>(src.size());
        int sizeReq = WideCharToMultiByte(OpenRCT2::CodePage::UTF8, 0, src.data(), srcLen, nullptr, 0, nullptr, nullptr);
        auto result = std::string(sizeReq, 0);
        WideCharToMultiByte(OpenRCT2::CodePage::UTF8, 0, src.data(), srcLen, result.data(), sizeReq, nullptr, nullptr);
        return result;
#else
    // Which constructor to use depends on the size of wchar_t...
    // UTF-32 is the default on most POSIX systems; Windows uses UTF-16.
    // Unfortunately, we'll have to help the compiler here.
    #if U_SIZEOF_WCHAR_T == 4
        icu::UnicodeString str = icu::UnicodeString::fromUTF32(reinterpret_cast<const UChar32*>(src.data()), src.length());
    #elif U_SIZEOF_WCHAR_T == 2
        std::wstring wstr = std::wstring(src);
        icu::UnicodeString str = icu::UnicodeString(static_cast<const wchar_t*>(wstr.c_str()));
    #else
        #error Unsupported U_SIZEOF_WCHAR_T size
    #endif

        std::string result;
        str.toUTF8String(result);

        return result;
#endif
    }

    std::wstring toWideChar(std::string_view src)
    {
#ifdef _WIN32
        int srcLen = static_cast<int>(src.size());
        int sizeReq = MultiByteToWideChar(OpenRCT2::CodePage::UTF8, 0, src.data(), srcLen, nullptr, 0);
        auto result = std::wstring(sizeReq, 0);
        MultiByteToWideChar(OpenRCT2::CodePage::UTF8, 0, src.data(), srcLen, result.data(), sizeReq);
        return result;
#else
        icu::UnicodeString str = icu::UnicodeString::fromUTF8(std::string(src));

    // Which constructor to use depends on the size of wchar_t...
    // UTF-32 is the default on most POSIX systems; Windows uses UTF-16.
    // Unfortunately, we'll have to help the compiler here.
    #if U_SIZEOF_WCHAR_T == 4
        size_t length = static_cast<size_t>(str.length());
        std::wstring result(length, '\0');

        UErrorCode status = U_ZERO_ERROR;
        str.toUTF32(reinterpret_cast<UChar32*>(&result[0]), str.length(), status);

    #elif U_SIZEOF_WCHAR_T == 2
        const char16_t* buffer = str.getBuffer();
        std::wstring result = static_cast<wchar_t*>(buffer);

    #else
        #error Unsupported U_SIZEOF_WCHAR_T size
    #endif

        return result;
#endif
    }

    std::string_view toStringView(const char* ch, size_t maxLen)
    {
        size_t len{};
        for (size_t i = 0; i < maxLen; i++)
        {
            if (ch[i] == '\0')
            {
                break;
            }

            len++;
        }
        return std::string_view(ch, len);
    }

    bool isNullOrEmpty(const utf8* str)
    {
        return str == nullptr || str[0] == '\0';
    }

    int32_t compare(const std::string& a, const std::string& b, bool ignoreCase)
    {
        return compare(a.c_str(), b.c_str(), ignoreCase);
    }

    int32_t compare(const utf8* a, const utf8* b, bool ignoreCase)
    {
        if (a == b)
            return 0;
        if (a == nullptr)
            a = "";
        if (b == nullptr)
            b = "";
        if (ignoreCase)
        {
            return _stricmp(a, b);
        }

        return strcmp(a, b);
    }

    template<typename TString>
    static bool equalsImpl(TString&& lhs, TString&& rhs, bool ignoreCase)
    {
        return std::equal(lhs.begin(), lhs.end(), rhs.begin(), rhs.end(), [ignoreCase](auto a, auto b) {
            const auto first = static_cast<unsigned char>(a);
            const auto second = static_cast<unsigned char>(b);
            if (((first | second) & 0x80) != 0)
            {
                // Only do case insensitive comparison on ASCII characters
                return first == second;
            }
            return ignoreCase ? (tolower(first) == tolower(second)) : (first == second);
        });
    }

    bool equals(u8string_view a, u8string_view b)
    {
        return equalsImpl(a, b, false);
    }

    bool equals(const u8string& a, const u8string& b)
    {
        return equalsImpl(a, b, false);
    }

    bool equals(const utf8* a, const utf8* b, bool ignoreCase)
    {
        if (a == b)
            return true;
        if (a == nullptr || b == nullptr)
            return false;

        if (ignoreCase)
        {
            return _stricmp(a, b) == 0;
        }

        return strcmp(a, b) == 0;
    }

    bool iequals(u8string_view a, u8string_view b)
    {
        return equalsImpl(a, b, true);
    }

    bool iequals(const u8string& a, const u8string& b)
    {
        return equalsImpl(a, b, true);
    }

    bool iequals(const utf8* a, const utf8* b)
    {
        if (a == b)
            return true;
        if (a == nullptr || b == nullptr)
            return false;
        return _stricmp(a, b) == 0;
    }

    bool startsWith(std::string_view str, std::string_view match, bool ignoreCase)
    {
        if (str.size() >= match.size())
        {
            auto view = str.substr(0, match.size());
            return equalsImpl(view, match, ignoreCase);
        }
        return false;
    }

    bool endsWith(std::string_view str, std::string_view match, bool ignoreCase)
    {
        if (str.size() >= match.size())
        {
            auto view = str.substr(str.size() - match.size());
            return equalsImpl(view, match, ignoreCase);
        }
        return false;
    }

    bool contains(std::string_view haystack, std::string_view needle, bool ignoreCase)
    {
        if (needle.size() > haystack.size())
            return false;

        if (!ignoreCase)
            return haystack.find(needle) != std::string_view::npos;

        auto end = haystack.size() - needle.size();
        for (size_t start = 0; start <= end; start++)
        {
            auto sub = haystack.substr(start, needle.size());
            if (equalsImpl(sub, needle, ignoreCase))
            {
                return true;
            }
        }
        return false;
    }

    size_t indexOf(const utf8* str, utf8 match, size_t startIndex)
    {
        const utf8* ch = str + startIndex;
        for (; *ch != '\0'; ch++)
        {
            if (*ch == match)
            {
                return static_cast<size_t>(ch - str);
            }
        }
        return SIZE_MAX;
    }

    ptrdiff_t lastIndexOf(const utf8* str, utf8 match)
    {
        const utf8* lastOccurance = nullptr;
        const utf8* ch = str;
        for (; *ch != '\0'; ch++)
        {
            if (*ch == match)
            {
                lastOccurance = ch;
            }
        }

        if (lastOccurance == nullptr)
        {
            return -1;
        }

        return lastOccurance - str;
    }

    size_t lengthOf(const utf8* str)
    {
        return UTF8Length(str);
    }

    size_t sizeOf(const utf8* str)
    {
        return strlen(str);
    }

    utf8* set(utf8* buffer, size_t bufferSize, const utf8* src)
    {
        return safeUtf8Copy(buffer, src, bufferSize);
    }

    utf8* set(utf8* buffer, size_t bufferSize, const utf8* src, size_t srcSize)
    {
        utf8* dst = buffer;
        size_t minSize = std::min(bufferSize - 1, srcSize);
        for (size_t i = 0; i < minSize; i++)
        {
            *dst++ = *src;
            if (*src == '\0')
                break;
            src++;
        }
        *dst = '\0';
        return buffer;
    }

    utf8* append(utf8* buffer, size_t bufferSize, const utf8* src)
    {
        return safeConcat(buffer, src, bufferSize);
    }

    utf8* format(utf8* buffer, size_t bufferSize, const utf8* format, ...)
    {
        va_list args;

        va_start(args, format);
        vsnprintf(buffer, bufferSize, format, args);
        va_end(args);

        // Terminate buffer in case formatted string overflowed
        buffer[bufferSize - 1] = '\0';

        return buffer;
    }

    u8string stdFormat(const utf8* format, ...)
    {
        va_list args;
        va_start(args, format);
        auto result = formatVA(format, args);
        va_end(args);
        return result;
    }

    u8string formatVA(const utf8* format, va_list args)
    {
        // When passing no buffer and a size of 0, vsnprintf returns the numbers of chars it would have writte, excluding the
        // null terminator.
        va_list copy;
        va_copy(copy, args);
        auto len = vsnprintf(nullptr, 0, format, copy);
        va_end(copy);

        if (len >= 0)
        {
            // The length returned by vsnprintf does not include the null terminator, but this byte is accounted for when
            // writing to a buffer, so we need to allocate one additional byte to fit the entire string in.
            len++;
            auto buffer = static_cast<utf8*>(alloca(len));

            len = vsnprintf(buffer, len, format, args);

            return u8string(buffer, buffer + len);
        }

        LOG_WARNING("Encoding error occured");
        return u8string{};
    }

    utf8* appendFormat(utf8* buffer, size_t bufferSize, const utf8* format, ...)
    {
        utf8* dst = buffer;
        size_t i;
        for (i = 0; i < bufferSize; i++)
        {
            if (*dst == '\0')
                break;
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

    std::vector<std::string> split(std::string_view s, std::string_view delimiter)
    {
        if (delimiter.empty())
        {
            throw std::invalid_argument("delimiter can not be empty.");
        }

        std::vector<std::string> results;
        if (!s.empty())
        {
            size_t index = 0;
            size_t nextIndex;
            do
            {
                nextIndex = s.find(delimiter, index);
                if (nextIndex == std::string::npos)
                {
                    results.emplace_back(s.substr(index));
                }
                else
                {
                    results.emplace_back(s.substr(index, nextIndex - index));
                }
                index = nextIndex + delimiter.size();
            } while (nextIndex != SIZE_MAX);
        }
        return results;
    }

    utf8* skipBOM(utf8* buffer)
    {
        return const_cast<utf8*>(skipBOM(static_cast<const utf8*>(buffer)));
    }

    const utf8* skipBOM(const utf8* buffer)
    {
        if (static_cast<uint8_t>(buffer[0]) == 0xEF && static_cast<uint8_t>(buffer[1]) == 0xBB
            && static_cast<uint8_t>(buffer[2]) == 0xBF)
        {
            return buffer + 3;
        }
        return buffer;
    }

    size_t getCodepointLength(codepoint_t codepoint)
    {
        return UTF8GetCodepointLength(codepoint);
    }

    codepoint_t getNextCodepoint(utf8* ptr, utf8** nextPtr)
    {
        return getNextCodepoint(static_cast<const utf8*>(ptr), const_cast<const utf8**>(nextPtr));
    }

    codepoint_t getNextCodepoint(const utf8* ptr, const utf8** nextPtr)
    {
        return UTF8GetNext(ptr, nextPtr);
    }

    utf8* writeCodepoint(utf8* dst, codepoint_t codepoint)
    {
        return UTF8WriteCodepoint(dst, codepoint);
    }

    void appendCodepoint(std::string& str, codepoint_t codepoint)
    {
        char buffer[8]{};
        UTF8WriteCodepoint(buffer, codepoint);
        str.append(buffer);
    }

    bool isWhiteSpace(codepoint_t codepoint)
    {
        // 0x3000 is the 'ideographic space', a 'fullwidth' character used in CJK languages.
        return iswspace(static_cast<wchar_t>(codepoint)) || codepoint == 0x3000;
    }

    utf8* trim(utf8* str)
    {
        utf8* firstNonWhitespace = nullptr;

        codepoint_t codepoint;
        utf8* ch = str;
        utf8* nextCh;
        while ((codepoint = getNextCodepoint(ch, &nextCh)) != '\0')
        {
            if (codepoint <= WCHAR_MAX && !isWhiteSpace(codepoint))
            {
                if (firstNonWhitespace == nullptr)
                {
                    firstNonWhitespace = ch;
                }
            }
            ch = nextCh;
        }

        if (firstNonWhitespace != nullptr && firstNonWhitespace != str)
        {
            // Take multibyte characters into account: use the last byte of the
            // current character.
            size_t newStringSize = (nextCh - 1) - firstNonWhitespace;

#ifdef DEBUG
            size_t currentStringSize = sizeOf(str);
            Guard::Assert(newStringSize < currentStringSize, GUARD_LINE);
#endif

            std::memmove(str, firstNonWhitespace, newStringSize);
            str[newStringSize] = '\0';
        }
        else
        {
            *ch = '\0';
        }

        return str;
    }

    const utf8* trimStart(const utf8* str)
    {
        codepoint_t codepoint;
        const utf8* ch = str;
        const utf8* nextCh;
        while ((codepoint = getNextCodepoint(ch, &nextCh)) != '\0')
        {
            if (codepoint <= WCHAR_MAX && !isWhiteSpace(codepoint))
            {
                return ch;
            }
            ch = nextCh;
        }
        // String is all whitespace
        return ch;
    }

    std::string trimStart(const std::string& s)
    {
        const utf8* trimmed = trimStart(s.c_str());
        return std::string(trimmed);
    }

    std::string trim(const std::string& s)
    {
        codepoint_t codepoint;
        const utf8* ch = s.c_str();
        const utf8* nextCh;
        const utf8* startSubstr = nullptr;
        const utf8* endSubstr = nullptr;
        while ((codepoint = getNextCodepoint(ch, &nextCh)) != '\0')
        {
            bool whiteSpace = codepoint <= WCHAR_MAX && isWhiteSpace(codepoint);
            if (!whiteSpace)
            {
                if (startSubstr == nullptr)
                {
                    startSubstr = ch;
                }

                // Take multibyte characters into account: move pointer towards
                // the last byte of the current character.
                endSubstr = nextCh - 1;
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

#ifndef _WIN32
    static const char* getIcuCodePage(int32_t codePage)
    {
        switch (codePage)
        {
            case OpenRCT2::CodePage::CP_932:
                return "windows-932";

            case OpenRCT2::CodePage::CP_936:
                return "GB2312";

            case OpenRCT2::CodePage::CP_949:
                return "windows-949";

            case OpenRCT2::CodePage::CP_950:
                return "big5";

            case OpenRCT2::CodePage::CP_1252:
                return "windows-1252";

            case OpenRCT2::CodePage::UTF8:
                return "utf-8";

            default:
                throw std::runtime_error("Unsupported code page: " + std::to_string(codePage));
        }
    }
#endif

    std::string convertToUtf8(std::string_view src, int32_t srcCodePage)
    {
#ifdef _WIN32
        // Convert from source code page to UTF-16
        std::wstring u16;
        {
            int srcLen = static_cast<int>(src.size());
            int sizeReq = MultiByteToWideChar(srcCodePage, 0, src.data(), srcLen, nullptr, 0);
            u16 = std::wstring(sizeReq, 0);
            MultiByteToWideChar(srcCodePage, 0, src.data(), srcLen, u16.data(), sizeReq);
        }

        // Convert from UTF-16 to destination code page
        std::string dst;
        {
            int srcLen = static_cast<int>(u16.size());
            int sizeReq = WideCharToMultiByte(OpenRCT2::CodePage::UTF8, 0, u16.data(), srcLen, nullptr, 0, nullptr, nullptr);
            dst = std::string(sizeReq, 0);
            WideCharToMultiByte(OpenRCT2::CodePage::UTF8, 0, u16.data(), srcLen, dst.data(), sizeReq, nullptr, nullptr);
        }

        return dst;
#else
        const char* codepage = getIcuCodePage(srcCodePage);
        icu::UnicodeString convertString(src.data(), codepage);

        std::string result;
        convertString.toUTF8String(result);

        return result;
#endif
    }

    std::string toUpper(std::string_view src)
    {
#ifdef _WIN32
        auto srcW = toWideChar(src);

        // Measure how long the destination needs to be
        auto requiredSize = LCMapStringEx(
            LOCALE_NAME_USER_DEFAULT, LCMAP_UPPERCASE | LCMAP_LINGUISTIC_CASING, srcW.c_str(), static_cast<int>(srcW.length()),
            nullptr, 0, nullptr, nullptr, 0);

        auto dstW = std::wstring();
        dstW.resize(requiredSize);

        // Transform the string
        auto result = LCMapStringEx(
            LOCALE_NAME_USER_DEFAULT, LCMAP_UPPERCASE | LCMAP_LINGUISTIC_CASING, srcW.c_str(), static_cast<int>(srcW.length()),
            dstW.data(), static_cast<int>(dstW.length()), nullptr, nullptr, 0);
        if (result == 0)
        {
            // Check the error
            auto error = GetLastError();
            LOG_WARNING("LCMapStringEx failed with %d", error);
            return std::string(src);
        }

        return toUtf8(dstW);
#else
        icu::UnicodeString str = icu::UnicodeString::fromUTF8(std::string(src));
        str.toUpper();

        std::string res;
        str.toUTF8String(res);

        return res;
#endif
    }

    std::string_view utf8Truncate(std::string_view v, size_t size)
    {
        auto trunc = v.substr(0, size);
        for (size_t i = 0; i < trunc.size();)
        {
            auto length = utf8GetCodePointSize(trunc.substr(i, trunc.size()));
            if (!length.has_value())
            {
                return trunc.substr(0, i);
            }
            i += length.value();
        }

        return trunc;
    }

    std::string_view utf8TruncateCodePoints(std::string_view v, size_t size)
    {
        size_t i = 0;
        while (i < v.size() && size > 0)
        {
            auto length = utf8GetCodePointSize(v.substr(i, v.size()));
            if (!length.has_value())
            {
                return v.substr(0, i);
            }
            i += length.value();
            size--;
        }

        return v.substr(0, i);
    }

    std::string urlEncode(std::string_view value)
    {
        std::ostringstream escaped;
        escaped.fill('0');
        escaped << std::hex;

        for (auto c : value)
        {
            // Keep alphanumeric and other accepted characters intact
            if (std::isalnum(static_cast<unsigned char>(c)) || c == '-' || c == '_' || c == '.' || c == '~')
            {
                escaped << c;
            }
            else
            {
                // Any other characters are percent-escaped
                escaped << std::uppercase;
                escaped << '%' << std::setw(2) << int32_t(static_cast<unsigned char>(c));
                escaped << std::nouppercase;
            }
        }

        return escaped.str();
    }

    /* Case insensitive logical compare */
    // Example:
    // - Guest 10
    // - Guest 99
    // - Guest 100
    // - John v2.0
    // - John v2.1
    int32_t logicalCmp(const char* s1, const char* s2)
    {
        for (;;)
        {
            if (*s2 == '\0')
                return *s1 != '\0';
            if (*s1 == '\0')
                return -1;
            if (!(isdigit(static_cast<unsigned char>(*s1)) && isdigit(static_cast<unsigned char>(*s2))))
            {
                if (toupper(*s1) != toupper(*s2))
                    return toupper(*s1) - toupper(*s2);

                ++s1;
                ++s2;
            }
            else
            {
                char *lim1, *lim2;
                unsigned long n1 = strtoul(s1, &lim1, 10);
                unsigned long n2 = strtoul(s2, &lim2, 10);
                if (n1 > n2)
                    return 1;
                if (n1 < n2)
                    return -1;

                s1 = lim1;
                s2 = lim2;
            }
        }
    }

    char* safeUtf8Copy(char* destination, const char* source, size_t size)
    {
        assert(destination != nullptr);
        assert(source != nullptr);

        if (size == 0)
            return destination;

        char* result = destination;

        bool truncated = false;
        const char* sourceLimit = source + size - 1;
        const char* ch = source;
        uint32_t codepoint;
        while ((codepoint = UTF8GetNext(ch, &ch)) != 0)
        {
            if (ch <= sourceLimit)
            {
                destination = UTF8WriteCodepoint(destination, codepoint);
            }
            else
            {
                truncated = true;
            }
        }
        *destination = 0;

        if (truncated)
        {
            LOG_WARNING("Truncating string \"%s\" to %d bytes.", result, size);
        }
        return result;
    }

    char* safeConcat(char* destination, const char* source, size_t size)
    {
        assert(destination != nullptr);
        assert(source != nullptr);

        if (size == 0)
        {
            return destination;
        }

        char* result = destination;

        size_t i;
        for (i = 0; i < size; i++)
        {
            if (*destination == '\0')
            {
                break;
            }

            destination++;
        }

        bool terminated = false;
        for (; i < size; i++)
        {
            if (*source != '\0')
            {
                *destination++ = *source++;
            }
            else
            {
                *destination = *source;
                terminated = true;
                break;
            }
        }

        if (!terminated)
        {
            result[size - 1] = '\0';
            LOG_WARNING("Truncating string \"%s\" to %d bytes.", result, size);
        }

        return result;
    }
} // namespace OpenRCT2::String
