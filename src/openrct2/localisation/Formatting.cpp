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

    char GetDigitSeperator()
    {
        return ',';
    }

    char GetDecimalSeperator()
    {
        return '.';
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
            buffer[i++] = GetDecimalSeperator();
        }

        // Whole digits
        size_t groupLen = 0;
        do
        {
            if constexpr (TDigitSep)
            {
                if (groupLen >= 3)
                {
                    groupLen = 0;
                    buffer[i++] = GetDigitSeperator();
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

    std::pair<std::string_view, uint32_t> FormatNextPart(std::string_view& fmt)
    {
        if (fmt.size() > 0)
        {
            for (size_t i = 0; i < fmt.size() - 1; i++)
            {
                if (fmt[i] == '{' && fmt[i + 1] != '}')
                {
                    if (i == 0)
                    {
                        // Find end brace
                        for (size_t j = i + 1; j < fmt.size(); j++)
                        {
                            if (fmt[j] == '}')
                            {
                                auto result = fmt.substr(0, j + 1);
                                fmt = fmt.substr(j + 1);
                                return { result, format_get_code(result.substr(1, result.size() - 2)) };
                            }
                        }
                    }
                    else
                    {
                        auto result = fmt.substr(0, i);
                        fmt = fmt.substr(i);
                        return { result, 0 };
                    }
                }
            }
        }
        {
            auto result = fmt;
            fmt = {};
            return { result, 0 };
        }
    }

    bool CanFormatToken(FormatToken t)
    {
        return t == FORMAT_COMMA1DP16 || (t >= FORMAT_COMMA32 && t <= FORMAT_LENGTH);
    }

    template void FormatArgument(std::stringstream&, uint32_t, int32_t);
    template void FormatArgument(std::stringstream&, uint32_t, const char*);

    void FormatArgumentAny(std::stringstream& ss, FormatToken token, const std::any& value)
    {
        if (value.type() == typeid(int32_t))
        {
            FormatArgument(ss, token, std::any_cast<int32_t>(value));
        }
        else if (value.type() == typeid(const char*))
        {
            FormatArgument(ss, token, std::any_cast<const char*>(value));
        }
        else
        {
            throw std::runtime_error("No support for format argument type.");
        }
    }

    std::string FormatStringAny(std::string_view fmt, const std::vector<std::any>& args)
    {
        thread_local std::stringstream ss;
        // Reset the buffer (reported as most efficient way)
        std::stringstream().swap(ss);

        size_t argIndex = 0;
        auto fmtc = fmt;
        while (!fmtc.empty())
        {
            auto [part, token] = FormatNextPart(fmtc);
            if (CanFormatToken(token))
            {
                if (argIndex < args.size())
                {
                    FormatArgumentAny(ss, token, args[argIndex]);
                }
                argIndex++;
            }
            else
            {
                ss << part;
            }
        }
        return ss.str();
    }
} // namespace OpenRCT2
