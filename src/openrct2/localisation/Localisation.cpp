/*****************************************************************************
 * Copyright (c) 2014-2023 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#ifdef _WIN32
#    include <windows.h>
#elif defined(__ANDROID__)
#    include <SDL.h>
#    include <jni.h>

#else
#    include <errno.h>
#    include <iconv.h>
#endif // _WIN32

#include "../Game.h"
#include "../common.h"
#include "../config/Config.h"
#include "../core/Guard.hpp"
#include "../core/String.hpp"
#include "../management/Marketing.h"
#include "../ride/Ride.h"
#include "../util/Util.h"
#include "Date.h"
#include "Formatting.h"
#include "Localisation.h"

#include <algorithm>
#include <cmath>
#include <cstring>
#include <ctype.h>
#include <iterator>
#include <limits.h>

thread_local char gCommonStringFormatBuffer[CommonTextBufferSize];

#ifdef DEBUG
// Set to true before a string format call to see details of the formatting.
// Set to false after the call.
bool gDebugStringFormatting = false;
#endif

// clang-format off
const StringId SpeedNames[] = {
    STR_SPEED_NORMAL,
    STR_SPEED_QUICK,
    STR_SPEED_FAST,
    STR_SPEED_TURBO,
    STR_SPEED_HYPER,
};

const StringId ObjectiveNames[] = {
    STR_OBJECTIVE_NONE,
    STR_OBJECTIVE_GUESTS_BY,
    STR_OBJECTIVE_PARK_VALUE_BY,
    STR_OBJECTIVE_HAVE_FUN,
    STR_OBJECTIVE_BUILD_THE_BEST,
    STR_OBJECTIVE_10_ROLLERCOASTERS,
    STR_OBJECTIVE_GUESTS_AND_RATING,
    STR_OBJECTIVE_MONTHLY_RIDE_INCOME,
    STR_OBJECTIVE_10_ROLLERCOASTERS_LENGTH,
    STR_OBJECTIVE_FINISH_5_ROLLERCOASTERS,
    STR_OBJECTIVE_REPLAY_LOAN_AND_PARK_VALUE,
    STR_OBJECTIVE_MONTHLY_FOOD_INCOME,
};

const StringId ResearchFundingLevelNames[] = {
    STR_RESEARCH_FUNDING_NONE,
    STR_RESEARCH_FUNDING_MINIMUM,
    STR_RESEARCH_FUNDING_NORMAL,
    STR_RESEARCH_FUNDING_MAXIMUM,
};

const StringId MarketingCampaignNames[ADVERTISING_CAMPAIGN_COUNT][3] = {
    { STR_MARKETING_VOUCHERS_FOR_FREE_ENTRY_TO_THE_PARK,            STR_VOUCHERS_FOR_FREE_ENTRY_TO,         STR_MARKETING_FINISHED_FREE_ENTRY },        // ADVERTISING_CAMPAIGN_PARK_ENTRY_FREE,
    { STR_MARKETING_VOUCHERS_FOR_FREE_RIDES_ON_A_PARTICULAR_RIDE,   STR_VOUCHERS_FOR_FREE_RIDE_ON,          STR_MARKETING_FINISHED_FREE_RIDES },        // ADVERTISING_CAMPAIGN_RIDE_FREE,
    { STR_MARKETING_VOUCHERS_FOR_HALF_PRICE_ENTRY_TO_THE_PARK,      STR_VOUCHERS_FOR_HALF_PRICE_ENTRY_TO,   STR_MARKETING_FINISHED_HALF_PRICE_ENTRY },  // ADVERTISING_CAMPAIGN_PARK_ENTRY_HALF_PRICE,
    { STR_MARKETING_VOUCHERS_FOR_FREE_FOOD_OR_DRINK,                STR_VOUCHERS_FOR_FREE,                  STR_MARKETING_FINISHED_FREE_RIDE },         // ADVERTISING_CAMPAIGN_FOOD_OR_DRINK_FREE,
    { STR_MARKETING_ADVERTISING_CAMPAIGN_FOR_THE_PARK,              STR_ADVERTISING_CAMPAIGN_FOR_1,         STR_MARKETING_FINISHED_PARK_ADS },          // ADVERTISING_CAMPAIGN_PARK,
    { STR_MARKETING_ADVERTISING_CAMPAIGN_FOR_A_PARTICULAR_RIDE,     STR_ADVERTISING_CAMPAIGN_FOR_2,         STR_MARKETING_FINISHED_RIDE_ADS },          // ADVERTISING_CAMPAIGN_RIDE,
};

const StringId RideInspectionIntervalNames[] = {
    STR_EVERY_10_MINUTES,
    STR_EVERY_20_MINUTES,
    STR_EVERY_30_MINUTES,
    STR_EVERY_45_MINUTES,
    STR_EVERY_HOUR,
    STR_EVERY_2_HOURS,
    STR_NEVER,
};

const StringId PeepThoughts[] = {
    STR_PEEP_THOUGHT_TYPE_CANT_AFFORD_0,
    STR_PEEP_THOUGHT_TYPE_SPENT_MONEY,
    STR_PEEP_THOUGHT_TYPE_SICK,
    STR_PEEP_THOUGHT_TYPE_VERY_SICK,
    STR_PEEP_THOUGHT_TYPE_MORE_THRILLING,
    STR_PEEP_THOUGHT_TYPE_INTENSE,
    STR_PEEP_THOUGHT_TYPE_HAVENT_FINISHED,
    STR_PEEP_THOUGHT_TYPE_SICKENING,
    STR_PEEP_THOUGHT_TYPE_BAD_VALUE,
    STR_PEEP_THOUGHT_TYPE_GO_HOME,
    STR_PEEP_THOUGHT_TYPE_GOOD_VALUE,
    STR_PEEP_THOUGHT_TYPE_ALREADY_GOT,
    STR_PEEP_THOUGHT_TYPE_CANT_AFFORD,
    STR_PEEP_THOUGHT_TYPE_NOT_HUNGRY,
    STR_PEEP_THOUGHT_TYPE_NOT_THIRSTY,
    STR_PEEP_THOUGHT_TYPE_DROWNING,
    STR_PEEP_THOUGHT_TYPE_LOST,
    STR_PEEP_THOUGHT_TYPE_WAS_GREAT,
    STR_PEEP_THOUGHT_TYPE_QUEUING_AGES,
    STR_PEEP_THOUGHT_TYPE_TIRED,
    STR_PEEP_THOUGHT_TYPE_HUNGRY,
    STR_PEEP_THOUGHT_TYPE_THIRSTY,
    STR_PEEP_THOUGHT_TYPE_TOILET,
    STR_PEEP_THOUGHT_TYPE_CANT_FIND,
    STR_PEEP_THOUGHT_TYPE_NOT_PAYING,
    STR_PEEP_THOUGHT_TYPE_NOT_WHILE_RAINING,
    STR_PEEP_THOUGHT_TYPE_BAD_LITTER,
    STR_PEEP_THOUGHT_TYPE_CANT_FIND_EXIT,
    STR_PEEP_THOUGHT_TYPE_GET_OFF,
    STR_PEEP_THOUGHT_TYPE_GET_OUT,
    STR_PEEP_THOUGHT_TYPE_NOT_SAFE,
    STR_PEEP_THOUGHT_TYPE_PATH_DISGUSTING,
    STR_PEEP_THOUGHT_TYPE_CROWDED,
    STR_PEEP_THOUGHT_TYPE_VANDALISM,
    STR_PEEP_THOUGHT_TYPE_SCENERY,
    STR_PEEP_THOUGHT_TYPE_VERY_CLEAN,
    STR_PEEP_THOUGHT_TYPE_FOUNTAINS,
    STR_PEEP_THOUGHT_TYPE_MUSIC,
    STR_PEEP_THOUGHT_TYPE_BALLOON,
    STR_PEEP_THOUGHT_TYPE_TOY,
    STR_PEEP_THOUGHT_TYPE_MAP,
    STR_PEEP_THOUGHT_TYPE_PHOTO,
    STR_PEEP_THOUGHT_TYPE_UMBRELLA,
    STR_PEEP_THOUGHT_TYPE_DRINK,
    STR_PEEP_THOUGHT_TYPE_BURGER,
    STR_PEEP_THOUGHT_TYPE_CHIPS,
    STR_PEEP_THOUGHT_TYPE_ICE_CREAM,
    STR_PEEP_THOUGHT_TYPE_CANDYFLOSS,
    0,
    0,
    0,
    STR_PEEP_THOUGHT_TYPE_PIZZA,
    0,
    STR_PEEP_THOUGHT_TYPE_POPCORN,
    STR_PEEP_THOUGHT_TYPE_HOT_DOG,
    STR_PEEP_THOUGHT_TYPE_TENTACLE,
    STR_PEEP_THOUGHT_TYPE_HAT,
    STR_PEEP_THOUGHT_TYPE_TOFFEE_APPLE,
    STR_PEEP_THOUGHT_TYPE_TSHIRT,
    STR_PEEP_THOUGHT_TYPE_DOUGHNUT,
    STR_PEEP_THOUGHT_TYPE_COFFEE,
    0,
    STR_PEEP_THOUGHT_TYPE_CHICKEN,
    STR_PEEP_THOUGHT_TYPE_LEMONADE,
    0,
    0,
    0,
    STR_PEEP_THOUGHT_TYPE_WOW,
    0,
    0,
    STR_PEEP_THOUGHT_TYPE_WOW2,
    STR_PEEP_THOUGHT_TYPE_WATCHED,
    STR_PEEP_THOUGHT_TYPE_BALLOON_MUCH,
    STR_PEEP_THOUGHT_TYPE_TOY_MUCH,
    STR_PEEP_THOUGHT_TYPE_MAP_MUCH,
    STR_PEEP_THOUGHT_TYPE_PHOTO_MUCH,
    STR_PEEP_THOUGHT_TYPE_UMBRELLA_MUCH,
    STR_PEEP_THOUGHT_TYPE_DRINK_MUCH,
    STR_PEEP_THOUGHT_TYPE_BURGER_MUCH,
    STR_PEEP_THOUGHT_TYPE_CHIPS_MUCH,
    STR_PEEP_THOUGHT_TYPE_ICE_CREAM_MUCH,
    STR_PEEP_THOUGHT_TYPE_CANDYFLOSS_MUCH,
    0,
    0,
    0,
    STR_PEEP_THOUGHT_TYPE_PIZZA_MUCH,
    0,
    STR_PEEP_THOUGHT_TYPE_POPCORN_MUCH,
    STR_PEEP_THOUGHT_TYPE_HOT_DOG_MUCH,
    STR_PEEP_THOUGHT_TYPE_TENTACLE_MUCH,
    STR_PEEP_THOUGHT_TYPE_HAT_MUCH,
    STR_PEEP_THOUGHT_TYPE_TOFFEE_APPLE_MUCH,
    STR_PEEP_THOUGHT_TYPE_TSHIRT_MUCH,
    STR_PEEP_THOUGHT_TYPE_DOUGHNUT_MUCH,
    STR_PEEP_THOUGHT_TYPE_COFFEE_MUCH,
    0,
    STR_PEEP_THOUGHT_TYPE_CHICKEN_MUCH,
    STR_PEEP_THOUGHT_TYPE_LEMONADE_MUCH,
    0,
    0,
    0,
    0,
    0,
    0,
    STR_PEEP_THOUGHT_TYPE_PHOTO2,
    STR_PEEP_THOUGHT_TYPE_PHOTO3,
    STR_PEEP_THOUGHT_TYPE_PHOTO4,
    STR_PEEP_THOUGHT_TYPE_PRETZEL,
    STR_PEEP_THOUGHT_TYPE_HOT_CHOCOLATE,
    STR_PEEP_THOUGHT_TYPE_ICED_TEA,
    STR_PEEP_THOUGHT_TYPE_FUNNEL_CAKE,
    STR_PEEP_THOUGHT_TYPE_SUNGLASSES,
    STR_PEEP_THOUGHT_TYPE_BEEF_NOODLES,
    STR_PEEP_THOUGHT_TYPE_FRIED_RICE_NOODLES,
    STR_PEEP_THOUGHT_TYPE_WONTON_SOUP,
    STR_PEEP_THOUGHT_TYPE_MEATBALL_SOUP,
    STR_PEEP_THOUGHT_TYPE_FRUIT_JUICE,
    STR_PEEP_THOUGHT_TYPE_SOYBEAN_MILK,
    STR_PEEP_THOUGHT_TYPE_SU_JONGKWA,
    STR_PEEP_THOUGHT_TYPE_SUB_SANDWICH,
    STR_PEEP_THOUGHT_TYPE_COOKIE,
    0,
    0,
    0,
    STR_PEEP_THOUGHT_TYPE_ROAST_SAUSAGE,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    STR_PEEP_THOUGHT_TYPE_PHOTO2_MUCH,
    STR_PEEP_THOUGHT_TYPE_PHOTO3_MUCH,
    STR_PEEP_THOUGHT_TYPE_PHOTO4_MUCH,
    STR_PEEP_THOUGHT_TYPE_PRETZEL_MUCH,
    STR_PEEP_THOUGHT_TYPE_HOT_CHOCOLATE_MUCH,
    STR_PEEP_THOUGHT_TYPE_ICED_TEA_MUCH,
    STR_PEEP_THOUGHT_TYPE_FUNNEL_CAKE_MUCH,
    STR_PEEP_THOUGHT_TYPE_SUNGLASSES_MUCH,
    STR_PEEP_THOUGHT_TYPE_BEEF_NOODLES_MUCH,
    STR_PEEP_THOUGHT_TYPE_FRIED_RICE_NOODLES_MUCH,
    STR_PEEP_THOUGHT_TYPE_WONTON_SOUP_MUCH,
    STR_PEEP_THOUGHT_TYPE_MEATBALL_SOUP_MUCH,
    STR_PEEP_THOUGHT_TYPE_FRUIT_JUICE_MUCH,
    STR_PEEP_THOUGHT_TYPE_SOYBEAN_MILK_MUCH,
    STR_PEEP_THOUGHT_TYPE_SU_JONGKWA_MUCH,
    STR_PEEP_THOUGHT_TYPE_SUB_SANDWICH_MUCH,
    STR_PEEP_THOUGHT_TYPE_COOKIE_MUCH,
    0,
    0,
    0,
    STR_PEEP_THOUGHT_TYPE_ROAST_SAUSAGE_MUCH,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    STR_PEEP_THOUGHT_TYPE_HELP,
    STR_PEEP_THOUGHT_TYPE_RUNNING_OUT,
    STR_PEEP_THOUGHT_TYPE_NEW_RIDE,
    STR_PEEP_THOUGHT_TYPE_NICE_RIDE_DEPRECATED,
    STR_PEEP_THOUGHT_TYPE_EXCITED_DEPRECATED,
    STR_PEEP_THOUGHT_TYPE_HERE_WE_ARE,
};

const StringId DateDayNames[] = {
    STR_DATE_DAY_1,
    STR_DATE_DAY_2,
    STR_DATE_DAY_3,
    STR_DATE_DAY_4,
    STR_DATE_DAY_5,
    STR_DATE_DAY_6,
    STR_DATE_DAY_7,
    STR_DATE_DAY_8,
    STR_DATE_DAY_9,
    STR_DATE_DAY_10,
    STR_DATE_DAY_11,
    STR_DATE_DAY_12,
    STR_DATE_DAY_13,
    STR_DATE_DAY_14,
    STR_DATE_DAY_15,
    STR_DATE_DAY_16,
    STR_DATE_DAY_17,
    STR_DATE_DAY_18,
    STR_DATE_DAY_19,
    STR_DATE_DAY_20,
    STR_DATE_DAY_21,
    STR_DATE_DAY_22,
    STR_DATE_DAY_23,
    STR_DATE_DAY_24,
    STR_DATE_DAY_25,
    STR_DATE_DAY_26,
    STR_DATE_DAY_27,
    STR_DATE_DAY_28,
    STR_DATE_DAY_29,
    STR_DATE_DAY_30,
    STR_DATE_DAY_31,
};

const StringId DateGameMonthNames[MONTH_COUNT] = {
    STR_MONTH_MARCH,
    STR_MONTH_APRIL,
    STR_MONTH_MAY,
    STR_MONTH_JUNE,
    STR_MONTH_JULY,
    STR_MONTH_AUGUST,
    STR_MONTH_SEPTEMBER,
    STR_MONTH_OCTOBER,
};

const StringId DateGameShortMonthNames[MONTH_COUNT] = {
    STR_MONTH_SHORT_MAR,
    STR_MONTH_SHORT_APR,
    STR_MONTH_SHORT_MAY,
    STR_MONTH_SHORT_JUN,
    STR_MONTH_SHORT_JUL,
    STR_MONTH_SHORT_AUG,
    STR_MONTH_SHORT_SEP,
    STR_MONTH_SHORT_OCT,
};
// clang-format on

std::string FormatStringID(StringId format, const void* args)
{
    std::string buffer(256, 0);
    size_t len{};
    for (;;)
    {
        OpenRCT2::FormatStringLegacy(buffer.data(), buffer.size(), format, args);
        len = buffer.find('\0');
        if (len == std::string::npos)
        {
            len = buffer.size();
        }
        if (len >= buffer.size() - 1)
        {
            // Null terminator to close to end of buffer, grow buffer and try again
            buffer.resize(buffer.size() * 2);
        }
        else
        {
            buffer.resize(len);
            break;
        }
    }
    return buffer;
}

/**
 * Writes a formatted string to a buffer and converts it to upper case.
 *  rct2: 0x006C2538
 * dest (edi)
 * format (ax)
 * args (ecx)
 */
void FormatStringToUpper(utf8* dest, size_t size, StringId format, const void* args)
{
#ifdef DEBUG
    if (gDebugStringFormatting)
    {
        printf("FormatStringToUpper(%hu)\n", format);
    }
#endif

    if (size == 0)
    {
        return;
    }

    OpenRCT2::FormatStringLegacy(dest, size, format, args);

    std::string upperString = String::ToUpper(dest);

    if (upperString.size() + 1 >= size)
    {
        upperString.resize(size - 1);
        dest[size - 1] = '\0';
        LOG_WARNING("Truncating formatted string \"%s\" to %d bytes.", dest, size);
    }

    upperString.copy(dest, upperString.size());
    dest[upperString.size()] = '\0';
}

void FormatReadableSize(char* buf, size_t bufSize, uint64_t sizeBytes)
{
    constexpr uint32_t SizeTable[] = {
        STR_SIZE_BYTE, STR_SIZE_KILOBYTE, STR_SIZE_MEGABYTE, STR_SIZE_GIGABYTE, STR_SIZE_TERABYTE,
    };

    double size = sizeBytes;
    size_t idx = 0;
    while (size >= 1024.0)
    {
        size /= 1024.0;
        idx++;
    }

    char sizeType[128] = {};
    OpenRCT2::FormatStringLegacy(sizeType, sizeof(sizeType), SizeTable[idx], nullptr);

    snprintf(buf, bufSize, "%.03f %s", size, sizeType);
}

void FormatReadableSpeed(char* buf, size_t bufSize, uint64_t sizeBytes)
{
    char sizeText[128] = {};
    FormatReadableSize(sizeText, sizeof(sizeText), sizeBytes);

    const char* args[1] = {
        sizeText,
    };
    OpenRCT2::FormatStringLegacy(buf, bufSize, STR_NETWORK_SPEED_SEC, args);
}

money64 StringToMoney(const char* string_to_monetise)
{
    const char* decimal_char = LanguageGetString(STR_LOCALE_DECIMAL_POINT);
    const CurrencyDescriptor* currencyDesc = &CurrencyDescriptors[EnumValue(gConfigGeneral.CurrencyFormat)];
    char processedString[128] = {};

    Guard::Assert(strlen(string_to_monetise) < sizeof(processedString));

    uint32_t numNumbers = 0;
    bool hasMinus = false;
    bool hasDecSep = false;
    const char* src_ptr = string_to_monetise;
    char* dst_ptr = processedString;

    // Process the string, keeping only numbers decimal, and minus sign(s).
    while (*src_ptr != '\0')
    {
        if (*src_ptr >= '0' && *src_ptr <= '9')
        {
            numNumbers++;
        }
        else if (*src_ptr == decimal_char[0])
        {
            if (hasDecSep)
                return MONEY64_UNDEFINED;
            hasDecSep = true;

            // Replace localised decimal separator with an English one.
            *dst_ptr++ = '.';
            src_ptr++;
            continue;
        }
        else if (*src_ptr == '-')
        {
            if (hasMinus)
                return MONEY64_UNDEFINED;
            hasMinus = true;
        }
        else
        {
            // Skip invalid characters.
            src_ptr++;
            continue;
        }

        // Copy numeric values.
        *dst_ptr++ = *src_ptr;
        src_ptr++;
    }

    // Terminate destination string.
    *dst_ptr = '\0';

    if (numNumbers == 0)
        return MONEY64_UNDEFINED;

    int64_t sign = 1;
    if (hasMinus)
    {
        // If there is a minus sign, it has to be at position 0 in order to be valid.
        if (processedString[0] == '-')
            sign = -1;
        else
            return MONEY64_UNDEFINED;
    }

    // Due to the nature of strstr and strtok, decimals at the very beginning will be ignored, causing
    // ".1" to be interpreted as "1". To prevent this, prefix with "0" if decimal is at the beginning.
    if (processedString[0] == decimal_char[0])
    {
        for (size_t i = strlen(processedString); i >= 1; i--)
            processedString[i] = processedString[i - 1];
        processedString[0] = '0';
    }

    auto number = std::stod(processedString, nullptr);
    number /= (currencyDesc->rate / 10.0);

    return ToMoney64FromGBP(number) * sign;
}

/**
 *
 * @param amount The amount in tens of pounds, e.g. 123 = £ 12.30
 * @param buffer_to_put_value_to Output parameter.
 * @param buffer_len Length of the buffer.
 * @param forceDecimals Show decimals, even if the amount does not have them. Will be ignored if the current exchange
 *                          rate is too big to have decimals.
 */
void MoneyToString(money64 amount, char* buffer_to_put_value_to, size_t buffer_len, bool forceDecimals)
{
    if (amount == MONEY64_UNDEFINED)
    {
        snprintf(buffer_to_put_value_to, buffer_len, "0");
        return;
    }

    const CurrencyDescriptor& currencyDesc = CurrencyDescriptors[EnumValue(gConfigGeneral.CurrencyFormat)];

    const char* sign = amount >= 0 ? "" : "-";
    const uint64_t a = std::abs(amount) * currencyDesc.rate;
    const unsigned long long whole = a / 100;
    const unsigned long long decimal = a % 100;

    bool amountIsInteger = (whole > 0) && decimal == 0;

    // If whole and decimal exist
    if ((whole > 0 && decimal > 0) || (amountIsInteger && forceDecimals && currencyDesc.rate < 100))
    {
        const char* decimalChar = LanguageGetString(STR_LOCALE_DECIMAL_POINT);
        auto precedingZero = (decimal < 10) ? "0" : "";
        snprintf(buffer_to_put_value_to, buffer_len, "%s%llu%s%s%llu", sign, whole, decimalChar, precedingZero, decimal);
    }
    // If whole exists, but not decimal
    else if (amountIsInteger)
    {
        snprintf(buffer_to_put_value_to, buffer_len, "%s%llu", sign, whole);
    }
    // If decimal exists, but not whole
    else if (whole == 0 && decimal > 0)
    {
        const char* decimalChar = LanguageGetString(STR_LOCALE_DECIMAL_POINT);
        snprintf(buffer_to_put_value_to, buffer_len, "%s0%s%llu", sign, decimalChar, decimal);
    }
    else
    {
        snprintf(buffer_to_put_value_to, buffer_len, "0");
    }
}
