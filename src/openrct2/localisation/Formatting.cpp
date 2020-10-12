/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "Formatting.h"

#include "../config/Config.h"
#include "../util/Util.h"
#include "FormatCodes.h"
#include "Language.h"
#include "Localisation.h"
#include "StringIds.h"

#include <cstdint>

namespace OpenRCT2
{
    FmtString::token::token(FormatToken k, std::string_view s)
        : kind(k)
        , text(s)
    {
    }

    bool FmtString::token::IsLiteral() const
    {
        return kind == 0;
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

        if (str[i] == '{' && i + 1 < str.size() && str[i + 1] != '{')
        {
            // Move to end brace
            do
            {
                i++;
            } while (i < str.size() && str[i] != '}');
            if (i < str.size() && str[i] == '}')
                i++;
        }
        else
        {
            do
            {
                i++;
            } while (i < str.size() && str[i] != '{');
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
        if (sztoken.size() >= 2 && sztoken[0] == '{' && sztoken[1] != '{')
        {
            auto kind = format_get_code(sztoken.substr(1, len - 2));
            return token(kind, sztoken);
        }
        return token(0, sztoken);
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
        : FmtString(std::string_view(s))
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

    std::string_view GetDigitSeperator()
    {
        auto sz = language_get_string(STR_LOCALE_THOUSANDS_SEPARATOR);
        return sz != nullptr ? sz : std::string_view();
    }

    std::string_view GetDecimalSeperator()
    {
        auto sz = language_get_string(STR_LOCALE_DECIMAL_POINT);
        return sz != nullptr ? sz : std::string_view();
    }

    template<size_t TSize, typename TIndex> static void AppendSeperator(char (&buffer)[TSize], TIndex& i, std::string_view sep)
    {
        if (i < TSize)
        {
            auto remainingLen = TSize - i;
            auto cpyLen = std::min(sep.size(), remainingLen);
            std::memcpy(&buffer[i], sep.data(), cpyLen);
            i += static_cast<TIndex>(cpyLen);
        }
    }

    template<size_t TDecimalPlace, bool TDigitSep, typename T> void FormatNumber(std::stringstream& ss, T value)
    {
        char buffer[32];
        int32_t i = 0;

        size_t num;
        if (value < 0)
        {
            // TODO handle edge case: std::numeric_limits<int64_t>::min();
            num = -value;
            ss << '-';
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
                buffer[i++] = (char)('0' + (num % 10));
                num /= 10;
            }

            auto decSep = GetDecimalSeperator();
            AppendSeperator(buffer, i, decSep);
        }

        // Whole digits
        auto digitSep = GetDigitSeperator();
        size_t groupLen = 0;
        do
        {
            if constexpr (TDigitSep)
            {
                if (groupLen >= 3)
                {
                    groupLen = 0;
                    AppendSeperator(buffer, i, digitSep);
                }
            }
            buffer[i++] = (char)('0' + (num % 10));
            num /= 10;
            if constexpr (TDigitSep)
            {
                groupLen++;
            }
        } while (num != 0 && i < sizeof(buffer));

        // Finally reverse append the string
        for (int32_t j = i - 1; j >= 0; j--)
        {
            ss << buffer[j];
        }
    }

    template<size_t TDecimalPlace, bool TDigitSep, typename T> void FormatCurrency(std::stringstream& ss, T rawValue)
    {
        auto currencyDesc = &CurrencyDescriptors[gConfigGeneral.currency_format];
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
        if (!font_supports_string(symbol, FONT_SIZE_MEDIUM))
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

    template<typename T> static void FormatMinutesSeconds(std::stringstream& ss, T value)
    {
        static constexpr const rct_string_id Formats[][2] = {
            { STR_DURATION_SEC, STR_DURATION_SECS },
            { STR_DURATION_MIN_SEC, STR_DURATION_MIN_SECS },
            { STR_DURATION_MINS_SEC, STR_DURATION_MINS_SECS },
        };

        auto minutes = value / 60;
        auto seconds = value % 60;
        if (minutes == 0)
        {
            auto fmt = Formats[0][seconds == 1 ? 0 : 1];
            FormatStringId(ss, fmt, seconds);
        }
        else
        {
            auto fmt = Formats[minutes == 1 ? 1 : 2][seconds == 1 ? 0 : 1];
            FormatStringId(ss, fmt, minutes, seconds);
        }
    }

    template<typename T> static void FormatHoursMinutes(std::stringstream& ss, T value)
    {
        static constexpr const rct_string_id Formats[][2] = {
            { STR_REALTIME_MIN, STR_REALTIME_MINS },
            { STR_REALTIME_HOUR_MIN, STR_REALTIME_HOUR_MINS },
            { STR_REALTIME_HOURS_MIN, STR_REALTIME_HOURS_MINS },
        };

        auto hours = value / 60;
        auto minutes = value % 60;
        if (hours == 0)
        {
            auto fmt = Formats[0][minutes == 1 ? 0 : 1];
            FormatStringId(ss, fmt, minutes);
        }
        else
        {
            auto fmt = Formats[hours == 1 ? 1 : 2][minutes == 1 ? 0 : 1];
            FormatStringId(ss, fmt, hours, minutes);
        }
    }

    template<typename T> void FormatArgument(std::stringstream& ss, FormatToken token, T arg)
    {
        switch (token)
        {
            case FORMAT_UINT16:
            case FORMAT_INT32:
                if constexpr (std::is_integral<T>())
                {
                    FormatNumber<0, false>(ss, arg);
                }
                break;
            case FORMAT_COMMA16:
            case FORMAT_COMMA32:
                if constexpr (std::is_integral<T>())
                {
                    FormatNumber<0, true>(ss, arg);
                }
                break;
            case FORMAT_COMMA1DP16:
                if constexpr (std::is_integral<T>())
                {
                    FormatNumber<1, true>(ss, arg);
                }
                else if constexpr (std::is_floating_point<T>())
                {
                    FormatNumber<1, true>(ss, std::round(arg * 10));
                }
                break;
            case FORMAT_COMMA2DP32:
                if constexpr (std::is_integral<T>())
                {
                    FormatNumber<2, true>(ss, arg);
                }
                else if constexpr (std::is_floating_point<T>())
                {
                    FormatNumber<2, true>(ss, std::round(arg * 100));
                }
                break;
            case FORMAT_CURRENCY2DP:
                if constexpr (std::is_integral<T>())
                {
                    FormatCurrency<2, true>(ss, arg);
                }
                break;
            case FORMAT_CURRENCY:
                if constexpr (std::is_integral<T>())
                {
                    FormatCurrency<0, true>(ss, arg);
                }
                break;
            case FORMAT_VELOCITY:
                if constexpr (std::is_integral<T>())
                {
                    switch (gConfigGeneral.measurement_format)
                    {
                        default:
                        case MeasurementFormat::Imperial:
                            FormatStringId(ss, STR_UNIT_SUFFIX_MILES_PER_HOUR, arg);
                            break;
                        case MeasurementFormat::Metric:
                            FormatStringId(ss, STR_UNIT_SUFFIX_KILOMETRES_PER_HOUR, mph_to_kmph(arg));
                            break;
                        case MeasurementFormat::SI:
                            FormatStringId(ss, STR_UNIT_SUFFIX_METRES_PER_SECOND, mph_to_dmps(arg));
                            break;
                    }
                }
                break;
            case FORMAT_DURATION:
                if constexpr (std::is_integral<T>())
                {
                    FormatMinutesSeconds(ss, arg);
                }
                break;
            case FORMAT_REALTIME:
                if constexpr (std::is_integral<T>())
                {
                    FormatHoursMinutes(ss, arg);
                }
                break;
            case FORMAT_LENGTH:
                if constexpr (std::is_integral<T>())
                {
                    switch (gConfigGeneral.measurement_format)
                    {
                        default:
                        case MeasurementFormat::Imperial:
                            FormatStringId(ss, STR_UNIT_SUFFIX_FEET, metres_to_feet(arg));
                            break;
                        case MeasurementFormat::Metric:
                        case MeasurementFormat::SI:
                            FormatStringId(ss, STR_UNIT_SUFFIX_METRES, arg);
                            break;
                    }
                }
                break;
            case FORMAT_MONTHYEAR:
                if constexpr (std::is_integral<T>())
                {
                    auto month = date_get_month(arg);
                    auto year = date_get_year(arg) + 1;
                    FormatStringId(ss, STR_DATE_FORMAT_MY, month, year);
                }
                break;
            case FORMAT_MONTH:
                if constexpr (std::is_integral<T>())
                {
                    auto szMonth = language_get_string(DateGameMonthNames[date_get_month(arg)]);
                    if (szMonth != nullptr)
                    {
                        ss << szMonth;
                    }
                }
                break;
            case FORMAT_STRING:
                if constexpr (std::is_same<T, const char*>())
                {
                    ss << arg;
                }
                else if constexpr (std::is_same<T, const std::string&>())
                {
                    ss << arg.c_str();
                }
                break;
            case FORMAT_STRINGID:
            case FORMAT_STRINGID2:
                if constexpr (std::is_integral<T>())
                {
                    ss << language_get_string(arg);
                }
                break;
        }
    }

    template void FormatArgument(std::stringstream&, uint32_t, uint16_t);
    template void FormatArgument(std::stringstream&, uint32_t, int16_t);
    template void FormatArgument(std::stringstream&, uint32_t, int32_t);
    template void FormatArgument(std::stringstream&, uint32_t, const char*);

    bool CanFormatToken(FormatToken t)
    {
        return t == FORMAT_COMMA1DP16 || (t >= FORMAT_COMMA32 && t <= FORMAT_LENGTH);
    }

    FmtString GetFmtStringById(rct_string_id id)
    {
        auto lang = language_get_string(id);
        auto fmtc = language_convert_string_to_tokens(lang);
        return FmtString(std::move(fmtc));
    }

    std::stringstream& GetThreadFormatStream()
    {
        thread_local std::stringstream ss;
        // Reset the buffer (reported as most efficient way)
        std::stringstream().swap(ss);
        return ss;
    }

    size_t CopyStringStreamToBuffer(char* buffer, size_t bufferLen, std::stringstream& ss)
    {
        auto stringLen = ss.tellp();
        auto copyLen = std::min<size_t>(bufferLen - 1, stringLen);

        ss.seekg(0, std::ios::beg);
        ss.read(buffer, copyLen);
        buffer[copyLen] = '\0';

        return stringLen;
    }

    void FormatArgumentAny(std::stringstream& ss, FormatToken token, const FormatArg_t& value)
    {
        if (std::holds_alternative<uint16_t>(value))
        {
            FormatArgument(ss, token, std::get<uint16_t>(value));
        }
        else if (std::holds_alternative<int32_t>(value))
        {
            FormatArgument(ss, token, std::get<int32_t>(value));
        }
        else if (std::holds_alternative<const char*>(value))
        {
            FormatArgument(ss, token, std::get<const char*>(value));
        }
        else
        {
            throw std::runtime_error("No support for format argument type.");
        }
    }

    static void FormatStringAny(
        std::stringstream& ss, const FmtString& fmt, const std::vector<FormatArg_t>& args, size_t& argIndex)
    {
        for (const auto& token : fmt)
        {
            if (token.kind == FORMAT_STRINGID || token.kind == FORMAT_STRINGID2)
            {
                if (argIndex < args.size())
                {
                    const auto& arg = args[argIndex++];
                    if (auto stringid = std::get_if<uint16_t>(&arg))
                    {
                        auto subfmt = GetFmtStringById(*stringid);
                        FormatStringAny(ss, subfmt, args, argIndex);
                    }
                }
                else
                {
                    argIndex++;
                }
            }
            else if (CanFormatToken(token.kind))
            {
                if (argIndex < args.size())
                {
                    FormatArgumentAny(ss, token.kind, args[argIndex]);
                }
                argIndex++;
            }
            else
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
        return ss.str();
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

    static void BuildAnyArgListFromLegacyArgBuffer(const FmtString& fmt, std::vector<FormatArg_t>& anyArgs, const void* args)
    {
        for (const auto& t : fmt)
        {
            switch (t.kind)
            {
                case FORMAT_COMMA32:
                case FORMAT_INT32:
                case FORMAT_COMMA2DP32:
                case FORMAT_CURRENCY2DP:
                case FORMAT_CURRENCY:
                    anyArgs.push_back(ReadFromArgs<int32_t>(args));
                    break;
                case FORMAT_COMMA16:
                case FORMAT_UINT16:
                case FORMAT_MONTHYEAR:
                case FORMAT_MONTH:
                case FORMAT_VELOCITY:
                case FORMAT_DURATION:
                case FORMAT_REALTIME:
                case FORMAT_LENGTH:
                    anyArgs.push_back(ReadFromArgs<uint16_t>(args));
                    break;
                case FORMAT_STRINGID:
                case FORMAT_STRINGID2:
                {
                    auto stringId = ReadFromArgs<rct_string_id>(args);
                    anyArgs.push_back(stringId);
                    BuildAnyArgListFromLegacyArgBuffer(GetFmtStringById(stringId), anyArgs, args);
                    break;
                }
                case FORMAT_STRING:
                {
                    auto sz = ReadFromArgs<const char*>(args);
                    anyArgs.push_back(sz);
                    BuildAnyArgListFromLegacyArgBuffer(sz, anyArgs, args);
                    break;
                }
                case FORMAT_POP16:
                    args = reinterpret_cast<const char*>(reinterpret_cast<uintptr_t>(args) + 2);
                    break;
                case FORMAT_PUSH16:
                    args = reinterpret_cast<const char*>(reinterpret_cast<uintptr_t>(args) - 2);
                    break;
            }
        }
    }

    size_t FormatStringLegacy(char* buffer, size_t bufferLen, rct_string_id id, const void* args)
    {
        std::vector<FormatArg_t> anyArgs;
        auto fmt = GetFmtStringById(id);
        BuildAnyArgListFromLegacyArgBuffer(fmt, anyArgs, args);
        return FormatStringAny(buffer, bufferLen, fmt, anyArgs);
    }
} // namespace OpenRCT2
