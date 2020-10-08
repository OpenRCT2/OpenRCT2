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
} // namespace OpenRCT2
