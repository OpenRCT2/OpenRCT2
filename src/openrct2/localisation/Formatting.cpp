/*****************************************************************************
 * Copyright (c) 2014-2023 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "Formatting.h"

#include "../config/Config.h"
#include "../util/Util.h"
#include "Formatter.h"
#include "Localisation.h"
#include "StringIds.h"

#include <cmath>
#include <cstdint>

namespace OpenRCT2
{
    static void FormatMonthYear(FormatBuffer& ss, int32_t month, int32_t year);

    static std::optional<int32_t> ParseNumericToken(std::string_view s)
    {
        if (s.size() >= 3 && s.size() <= 5 && s[0] == '{' && s[s.size() - 1] == '}')
        {
            char buffer[8]{};
            std::memcpy(buffer, s.data() + 1, s.size() - 2);
            return std::atoi(buffer);
        }
        return std::nullopt;
    }

    static std::optional<int32_t> ParseNumericToken(std::string_view str, size_t& i)
    {
        if (i < str.size() && str[i] == '{')
        {
            auto parameterStart = i;
            do
            {
                i++;
            } while (i < str.size() && str[i] != '}');
            if (i < str.size() && str[i] == '}')
            {
                i++;
            }

            auto paramter = str.substr(parameterStart, i - parameterStart);
            return ParseNumericToken(paramter);
        }
        return std::nullopt;
    }

    FmtString::token::token(FormatToken k, std::string_view s, uint32_t p)
        : kind(k)
        , text(s)
        , parameter(p)
    {
    }

    bool FmtString::token::IsLiteral() const
    {
        return kind == FormatToken::Literal;
    }

    bool FmtString::token::IsCodepoint() const
    {
        return kind == FormatToken::Escaped;
    }

    codepoint_t FmtString::token::GetCodepoint() const
    {
        if (kind == FormatToken::Escaped)
        {
            // Assume text is only "{{" or "}}" for now
            return text[0];
        }
        return 0;
    }

    FmtString::iterator::iterator(std::string_view s, size_t i)
        : str(s)
        , index(i)
    {
        update();
    }

    void FmtString::iterator::update()
    {
        auto i = index;
        if (i >= str.size())
        {
            current = token();
            return;
        }

        if (str[i] == '\n' || str[i] == '\r')
        {
            i++;
        }
        else if (str[i] == '{' && i + 1 < str.size() && str[i + 1] == '{')
        {
            i += 2;
        }
        else if (str[i] == '}' && i + 1 < str.size() && str[i + 1] == '}')
        {
            i += 2;
        }
        else if (str[i] == '{' && i + 1 < str.size() && str[i + 1] != '{')
        {
            // Move to end brace
            auto startIndex = i;
            do
            {
                i++;
            } while (i < str.size() && str[i] != '}');
            if (i < str.size() && str[i] == '}')
            {
                i++;

                auto inner = str.substr(startIndex + 1, i - startIndex - 2);
                if (inner == "MOVE_X")
                {
                    uint32_t p = 0;
                    auto p0 = ParseNumericToken(str, i);
                    if (p0)
                    {
                        p = *p0;
                    }
                    current = token(FormatToken::Move, str.substr(startIndex, i - startIndex), p);
                    return;
                }

                if (inner == "INLINE_SPRITE")
                {
                    uint32_t p = 0;
                    auto p0 = ParseNumericToken(str, i);
                    auto p1 = ParseNumericToken(str, i);
                    auto p2 = ParseNumericToken(str, i);
                    auto p3 = ParseNumericToken(str, i);
                    if (p0 && p1 && p2 && p3)
                    {
                        p |= (*p0);
                        p |= (*p1) << 8;
                        p |= (*p2) << 16;
                        p |= (*p3) << 24;
                    }
                    current = token(FormatToken::InlineSprite, str.substr(startIndex, i - startIndex), p);
                    return;
                }
            }
        }
        else
        {
            do
            {
                i++;
            } while (i < str.size() && str[i] != '{' && str[i] != '}' && str[i] != '\n' && str[i] != '\r');
        }
        current = CreateToken(i - index);
    }

    bool FmtString::iterator::operator==(iterator& rhs)
    {
        return index == rhs.index;
    }

    bool FmtString::iterator::operator!=(iterator& rhs)
    {
        return index != rhs.index;
    }

    FmtString::token FmtString::iterator::CreateToken(size_t len)
    {
        std::string_view sztoken = str.substr(index, len);

        if (sztoken.size() >= 2 && ((sztoken[0] == '{' && sztoken[1] == '{') || (sztoken[0] == '}' && sztoken[1] == '}')))
        {
            return token(FormatToken::Escaped, sztoken);
        }
        if (sztoken.size() >= 2 && sztoken[0] == '{' && sztoken[1] != '{')
        {
            auto kind = FormatTokenFromString(sztoken.substr(1, len - 2));
            return token(kind, sztoken);
        }
        if (sztoken == "\n" || sztoken == "\r")
        {
            return token(FormatToken::Newline, sztoken);
        }
        return token(FormatToken::Literal, sztoken);
    }

    const FmtString::token* FmtString::iterator::operator->() const
    {
        return &current;
    }

    const FmtString::token& FmtString::iterator::operator*()
    {
        return current;
    }

    FmtString::iterator& FmtString::iterator::operator++()
    {
        if (index < str.size())
        {
            index += current.text.size();
            update();
        }
        return *this;
    }

    FmtString::iterator FmtString::iterator::operator++(int)
    {
        auto result = *this;
        if (index < str.size())
        {
            index += current.text.size();
            update();
        }
        return result;
    }

    bool FmtString::iterator::eol() const
    {
        return index >= str.size();
    }

    FmtString::FmtString(std::string&& s)
    {
        _strOwned = std::move(s);
        _str = _strOwned;
    }

    FmtString::FmtString(std::string_view s)
        : _str(s)
    {
    }

    FmtString::FmtString(const char* s)
        : FmtString(s == nullptr ? std::string_view() : std::string_view(s))
    {
    }

    FmtString::iterator FmtString::begin() const
    {
        return iterator(_str, 0);
    }

    FmtString::iterator FmtString::end() const
    {
        return iterator(_str, _str.size());
    }

    std::string FmtString::WithoutFormatTokens() const
    {
        std::string result;
        result.reserve(_str.size() * 4);
        for (const auto& t : *this)
        {
            if (t.IsLiteral())
            {
                result += t.text;
            }
        }
        return result;
    }

    static std::string_view GetDigitSeparator()
    {
        auto sz = LanguageGetString(STR_LOCALE_THOUSANDS_SEPARATOR);
        return sz != nullptr ? sz : std::string_view();
    }

    static std::string_view GetDecimalSeparator()
    {
        auto sz = LanguageGetString(STR_LOCALE_DECIMAL_POINT);
        return sz != nullptr ? sz : std::string_view();
    }

    void FormatRealName(FormatBuffer& ss, StringId id)
    {
        if (IsRealNameStringId(id))
        {
            auto realNameIndex = id - REAL_NAME_START;
            ss << real_names[realNameIndex % std::size(real_names)];
            ss << ' ';
            ss << real_name_initials[(realNameIndex >> 10) % std::size(real_name_initials)];
            ss << '.';
        }
    }

    template<size_t TSize, typename TIndex> static void AppendSeparator(char (&buffer)[TSize], TIndex& i, std::string_view sep)
    {
        if (i < TSize)
        {
            auto remainingLen = TSize - i;
            auto cpyLen = std::min(sep.size(), remainingLen);
            std::memcpy(&buffer[i], sep.data(), cpyLen);
            i += static_cast<TIndex>(cpyLen);
        }
    }

    template<size_t TDecimalPlace, bool TDigitSep, typename T> void FormatNumber(FormatBuffer& ss, T value)
    {
        char buffer[32];
        size_t i = 0;

        uint64_t num;
        if constexpr (std::is_signed<T>::value)
        {
            if (value < 0)
            {
                ss << '-';
                if (value == std::numeric_limits<int64_t>::min())
                {
                    // Edge case: int64_t can not store this number so manually assign num to (int64_t::max + 1)
                    num = static_cast<uint64_t>(std::numeric_limits<int64_t>::max()) + 1;
                }
                else
                {
                    // Cast negative number to int64_t and then reverse sign
                    num = -static_cast<int64_t>(value);
                }
            }
            else
            {
                num = value;
            }
        }
        else
        {
            num = value;
        }

        // Decimal digits
        if constexpr (TDecimalPlace > 0)
        {
            while (num != 0 && i < sizeof(buffer) && i < TDecimalPlace)
            {
                buffer[i++] = static_cast<char>('0' + (num % 10));
                num /= 10;
            }
            // handle case where value has fewer sig figs than required decimal places
            while (num == 0 && i < TDecimalPlace && i < sizeof(buffer))
            {
                buffer[i++] = '0';
            }

            auto decSep = GetDecimalSeparator();
            AppendSeparator(buffer, i, decSep);
        }

        // Whole digits
        [[maybe_unused]] auto digitSep = GetDigitSeparator();
        [[maybe_unused]] size_t groupLen = 0;
        do
        {
            if constexpr (TDigitSep)
            {
                if (groupLen >= 3)
                {
                    groupLen = 0;
                    AppendSeparator(buffer, i, digitSep);
                }
            }
            buffer[i++] = static_cast<char>('0' + (num % 10));
            num /= 10;
            if constexpr (TDigitSep)
            {
                groupLen++;
            }
        } while (num != 0 && i < sizeof(buffer));

        // Finally reverse append the string
        for (int32_t j = static_cast<int32_t>(i - 1); j >= 0; j--)
        {
            ss << buffer[j];
        }
    }

    template<size_t TDecimalPlace, bool TDigitSep, typename T> void FormatCurrency(FormatBuffer& ss, T rawValue)
    {
        auto currencyDesc = &CurrencyDescriptors[EnumValue(gConfigGeneral.CurrencyFormat)];
        auto value = static_cast<int64_t>(rawValue) * currencyDesc->rate;

        // Negative sign
        if (value < 0)
        {
            ss << '-';
            value = -value;
        }

        // Round the value away from zero
        if constexpr (TDecimalPlace < 2)
        {
            value = (value + 99) / 100;
        }

        // Currency symbol
        auto symbol = currencyDesc->symbol_unicode;
        auto affix = currencyDesc->affix_unicode;
        if (!FontSupportsString(symbol, FontStyle::Medium))
        {
            symbol = currencyDesc->symbol_ascii;
            affix = currencyDesc->affix_ascii;
        }

        // Currency symbol prefix
        if (affix == CurrencyAffix::Prefix)
        {
            ss << symbol;
        }

        // Drop the pennies for "large" currencies
        auto dropPennies = false;
        if constexpr (TDecimalPlace >= 2)
        {
            dropPennies = currencyDesc->rate >= 100;
        }
        if (dropPennies)
        {
            FormatNumber<0, TDigitSep>(ss, value / 100);
        }
        else
        {
            FormatNumber<TDecimalPlace, TDigitSep>(ss, value);
        }

        // Currency symbol suffix
        if (affix == CurrencyAffix::Suffix)
        {
            ss << symbol;
        }
    }

    template<typename T> static void FormatMinutesSeconds(FormatBuffer& ss, T value)
    {
        static constexpr const StringId Formats[][2] = {
            { STR_DURATION_SEC, STR_DURATION_SECS },
            { STR_DURATION_MIN_SEC, STR_DURATION_MIN_SECS },
            { STR_DURATION_MINS_SEC, STR_DURATION_MINS_SECS },
        };

        auto minutes = value / 60;
        auto seconds = value % 60;
        if (minutes == 0)
        {
            auto fmt = Formats[0][seconds == 1 ? 0 : 1];
            FormatStringID(ss, fmt, seconds);
        }
        else
        {
            auto fmt = Formats[minutes == 1 ? 1 : 2][seconds == 1 ? 0 : 1];
            FormatStringID(ss, fmt, minutes, seconds);
        }
    }

    template<typename T> static void FormatHoursMinutes(FormatBuffer& ss, T value)
    {
        static constexpr const StringId Formats[][2] = {
            { STR_REALTIME_MIN, STR_REALTIME_MINS },
            { STR_REALTIME_HOUR_MIN, STR_REALTIME_HOUR_MINS },
            { STR_REALTIME_HOURS_MIN, STR_REALTIME_HOURS_MINS },
        };

        auto hours = value / 60;
        auto minutes = value % 60;
        if (hours == 0)
        {
            auto fmt = Formats[0][minutes == 1 ? 0 : 1];
            FormatStringID(ss, fmt, minutes);
        }
        else
        {
            auto fmt = Formats[hours == 1 ? 1 : 2][minutes == 1 ? 0 : 1];
            FormatStringID(ss, fmt, hours, minutes);
        }
    }

    template<typename T> void FormatArgument(FormatBuffer& ss, FormatToken token, T arg)
    {
        switch (token)
        {
            case FormatToken::UInt16:
            case FormatToken::Int32:
                if constexpr (std::is_integral<T>())
                {
                    FormatNumber<0, false>(ss, arg);
                }
                break;
            case FormatToken::Comma16:
            case FormatToken::Comma32:
                if constexpr (std::is_integral<T>())
                {
                    FormatNumber<0, true>(ss, arg);
                }
                break;
            case FormatToken::Comma1dp16:
                if constexpr (std::is_integral<T>())
                {
                    FormatNumber<1, true>(ss, arg);
                }
                else if constexpr (std::is_floating_point<T>())
                {
                    FormatNumber<1, true>(ss, std::round(arg * 10));
                }
                break;
            case FormatToken::Comma2dp32:
                if constexpr (std::is_integral<T>())
                {
                    FormatNumber<2, true>(ss, arg);
                }
                else if constexpr (std::is_floating_point<T>())
                {
                    FormatNumber<2, true>(ss, std::round(arg * 100));
                }
                break;
            case FormatToken::Currency2dp:
                if constexpr (std::is_integral<T>())
                {
                    FormatCurrency<2, true>(ss, arg);
                }
                break;
            case FormatToken::Currency:
                if constexpr (std::is_integral<T>())
                {
                    FormatCurrency<0, true>(ss, arg);
                }
                break;
            case FormatToken::Velocity:
                if constexpr (std::is_integral<T>())
                {
                    switch (gConfigGeneral.MeasurementFormat)
                    {
                        default:
                        case MeasurementFormat::Imperial:
                            FormatStringID(ss, STR_UNIT_SUFFIX_MILES_PER_HOUR, arg);
                            break;
                        case MeasurementFormat::Metric:
                            FormatStringID(ss, STR_UNIT_SUFFIX_KILOMETRES_PER_HOUR, mph_to_kmph(arg));
                            break;
                        case MeasurementFormat::SI:
                            FormatStringID(ss, STR_UNIT_SUFFIX_METRES_PER_SECOND, mph_to_dmps(arg));
                            break;
                    }
                }
                break;
            case FormatToken::DurationShort:
                if constexpr (std::is_integral<T>())
                {
                    FormatMinutesSeconds(ss, arg);
                }
                break;
            case FormatToken::DurationLong:
                if constexpr (std::is_integral<T>())
                {
                    FormatHoursMinutes(ss, arg);
                }
                break;
            case FormatToken::Length:
                if constexpr (std::is_integral<T>())
                {
                    switch (gConfigGeneral.MeasurementFormat)
                    {
                        default:
                        case MeasurementFormat::Imperial:
                            FormatStringID(ss, STR_UNIT_SUFFIX_FEET, metres_to_feet(arg));
                            break;
                        case MeasurementFormat::Metric:
                        case MeasurementFormat::SI:
                            FormatStringID(ss, STR_UNIT_SUFFIX_METRES, arg);
                            break;
                    }
                }
                break;
            case FormatToken::MonthYear:
                if constexpr (std::is_integral<T>())
                {
                    auto month = DateGetMonth(arg);
                    auto year = DateGetYear(arg) + 1;
                    FormatMonthYear(ss, month, year);
                }
                break;
            case FormatToken::Month:
                if constexpr (std::is_integral<T>())
                {
                    auto szMonth = LanguageGetString(DateGameMonthNames[DateGetMonth(arg)]);
                    if (szMonth != nullptr)
                    {
                        ss << szMonth;
                    }
                }
                break;
            case FormatToken::String:
                ss << arg;
                break;
            case FormatToken::Sprite:
                if constexpr (std::is_integral<T>())
                {
                    auto idx = static_cast<uint32_t>(arg);
                    char inlineBuf[64];
                    size_t len = snprintf(
                        inlineBuf, sizeof(inlineBuf), "{INLINE_SPRITE}{%u}{%u}{%u}{%u}", ((idx >> 0) & 0xFF),
                        ((idx >> 8) & 0xFF), ((idx >> 16) & 0xFF), ((idx >> 24) & 0xFF));
                    ss.append(inlineBuf, len);
                }
                break;
            default:
                break;
        }
    }

    template void FormatArgument(FormatBuffer&, FormatToken, uint16_t);
    template void FormatArgument(FormatBuffer&, FormatToken, int16_t);
    template void FormatArgument(FormatBuffer&, FormatToken, int32_t);
    template void FormatArgument(FormatBuffer&, FormatToken, int64_t);
    template void FormatArgument(FormatBuffer&, FormatToken, uint32_t);
    template void FormatArgument(FormatBuffer&, FormatToken, uint64_t);
    template void FormatArgument(FormatBuffer&, FormatToken, const char*);
    template void FormatArgument(FormatBuffer&, FormatToken, std::string_view);

    bool IsRealNameStringId(StringId id)
    {
        return id >= REAL_NAME_START && id <= REAL_NAME_END;
    }

    FmtString GetFmtStringById(StringId id)
    {
        auto fmtc = LanguageGetString(id);
        return FmtString(fmtc);
    }

    FormatBuffer& GetThreadFormatStream()
    {
        thread_local FormatBuffer ss;
        ss.clear();
        return ss;
    }

    size_t CopyStringStreamToBuffer(char* buffer, size_t bufferLen, FormatBuffer& ss)
    {
        auto copyLen = std::min<size_t>(bufferLen - 1, ss.size());

        std::copy(ss.data(), ss.data() + copyLen, buffer);
        buffer[copyLen] = '\0';

        return ss.size();
    }

    static void FormatArgumentAny(FormatBuffer& ss, FormatToken token, const FormatArg_t& value)
    {
        if (std::holds_alternative<uint16_t>(value))
        {
            FormatArgument(ss, token, std::get<uint16_t>(value));
        }
        else if (std::holds_alternative<int32_t>(value))
        {
            FormatArgument(ss, token, std::get<int32_t>(value));
        }
        else if (std::holds_alternative<int64_t>(value))
        {
            FormatArgument(ss, token, std::get<int64_t>(value));
        }
        else if (std::holds_alternative<const char*>(value))
        {
            FormatArgument(ss, token, std::get<const char*>(value));
        }
        else if (std::holds_alternative<std::string>(value))
        {
            FormatArgument(ss, token, std::get<std::string>(value));
        }
        else
        {
            throw std::runtime_error("No support for format argument type.");
        }
    }

    static void FormatStringAny(FormatBuffer& ss, const FmtString& fmt, const std::vector<FormatArg_t>& args, size_t& argIndex)
    {
        for (const auto& token : fmt)
        {
            if (token.kind == FormatToken::StringById)
            {
                if (argIndex < args.size())
                {
                    const auto& arg = args[argIndex++];
                    std::optional<StringId> stringId;
                    if (auto value16 = std::get_if<uint16_t>(&arg))
                    {
                        stringId = *value16;
                    }
                    else if (auto value32 = std::get_if<int32_t>(&arg))
                    {
                        stringId = *value32;
                    }
                    if (stringId)
                    {
                        if (IsRealNameStringId(*stringId))
                        {
                            FormatRealName(ss, *stringId);
                        }
                        else
                        {
                            auto subfmt = GetFmtStringById(*stringId);
                            FormatStringAny(ss, subfmt, args, argIndex);
                        }
                    }
                }
                else
                {
                    argIndex++;
                }
            }
            else if (FormatTokenTakesArgument(token.kind))
            {
                if (argIndex < args.size())
                {
                    FormatArgumentAny(ss, token.kind, args[argIndex]);
                }
                argIndex++;
            }
            else if (token.kind != FormatToken::Push16 && token.kind != FormatToken::Pop16)
            {
                ss << token.text;
            }
        }
    }

    std::string FormatStringAny(const FmtString& fmt, const std::vector<FormatArg_t>& args)
    {
        auto& ss = GetThreadFormatStream();
        size_t argIndex = 0;
        FormatStringAny(ss, fmt, args, argIndex);
        return ss.data();
    }

    size_t FormatStringAny(char* buffer, size_t bufferLen, const FmtString& fmt, const std::vector<FormatArg_t>& args)
    {
        auto& ss = GetThreadFormatStream();
        size_t argIndex = 0;
        FormatStringAny(ss, fmt, args, argIndex);
        return CopyStringStreamToBuffer(buffer, bufferLen, ss);
    }

    template<typename T> static T ReadFromArgs(const void*& args)
    {
        T value;
        std::memcpy(&value, args, sizeof(T));
        args = reinterpret_cast<const char*>(reinterpret_cast<uintptr_t>(args) + sizeof(T));
        return value;
    }

    static void BuildAnyArgListFromLegacyArgBuffer(const FmtString& fmt, std::vector<FormatArg_t>& anyArgs, const void*& args)
    {
        for (const auto& t : fmt)
        {
            switch (t.kind)
            {
                case FormatToken::Comma32:
                case FormatToken::Int32:
                case FormatToken::Comma2dp32:
                case FormatToken::Sprite:
                    anyArgs.push_back(ReadFromArgs<int32_t>(args));
                    break;
                case FormatToken::Currency2dp:
                case FormatToken::Currency:
                    anyArgs.push_back(ReadFromArgs<int64_t>(args));
                    break;
                case FormatToken::UInt16:
                case FormatToken::MonthYear:
                case FormatToken::Month:
                case FormatToken::Velocity:
                case FormatToken::DurationShort:
                case FormatToken::DurationLong:
                    anyArgs.push_back(ReadFromArgs<uint16_t>(args));
                    break;
                case FormatToken::Comma16:
                case FormatToken::Length:
                case FormatToken::Comma1dp16:
                    anyArgs.push_back(ReadFromArgs<int16_t>(args));
                    break;
                case FormatToken::StringById:
                {
                    auto stringId = ReadFromArgs<StringId>(args);
                    anyArgs.push_back(stringId);
                    BuildAnyArgListFromLegacyArgBuffer(GetFmtStringById(stringId), anyArgs, args);
                    break;
                }
                case FormatToken::String:
                {
                    auto sz = ReadFromArgs<const char*>(args);
                    anyArgs.push_back(sz);
                    break;
                }
                case FormatToken::Pop16:
                    args = reinterpret_cast<const char*>(reinterpret_cast<uintptr_t>(args) + 2);
                    break;
                case FormatToken::Push16:
                    args = reinterpret_cast<const char*>(reinterpret_cast<uintptr_t>(args) - 2);
                    break;
                default:
                    break;
            }
        }
    }

    size_t FormatStringLegacy(char* buffer, size_t bufferLen, StringId id, const void* args)
    {
        thread_local std::vector<FormatArg_t> anyArgs;
        anyArgs.clear();
        auto fmt = GetFmtStringById(id);
        BuildAnyArgListFromLegacyArgBuffer(fmt, anyArgs, args);
        return FormatStringAny(buffer, bufferLen, fmt, anyArgs);
    }

    static void FormatMonthYear(FormatBuffer& ss, int32_t month, int32_t year)
    {
        thread_local std::vector<FormatArg_t> tempArgs;
        tempArgs.clear();

        auto fmt = GetFmtStringById(STR_DATE_FORMAT_MY);
        Formatter ft;
        ft.Add<uint16_t>(month);
        ft.Add<uint16_t>(year);
        const void* legacyArgs = ft.Data();
        BuildAnyArgListFromLegacyArgBuffer(fmt, tempArgs, legacyArgs);
        size_t argIndex = 0;
        FormatStringAny(ss, fmt, tempArgs, argIndex);
    }

} // namespace OpenRCT2

void format_string(utf8* dest, size_t size, StringId format, const void* args)
{
    OpenRCT2::FormatStringLegacy(dest, size, format, args);
}
