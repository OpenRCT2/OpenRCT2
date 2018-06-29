/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include <ctype.h>
#include <cstring>
#include <limits.h>

#ifdef _WIN32
#include <windows.h>
#elif defined(__ANDROID__)
#include <jni.h>
#include <SDL.h>

#else
#include <iconv.h>
#include <errno.h>
#endif // _WIN32

#include "../common.h"
#include "../config/Config.h"
#include "../core/Guard.hpp"
#include "../core/Math.hpp"
#include "../core/String.hpp"
#include "../core/Util.hpp"
#include "Date.h"
#include "../Game.h"
#include "Localisation.h"
#include "../management/Marketing.h"
#include "../ride/Ride.h"
#include "../util/Util.h"

char gCommonStringFormatBuffer[256];
uint8_t gCommonFormatArgs[80];
uint8_t gMapTooltipFormatArgs[40];

#ifdef DEBUG
    // Set to true before a string format call to see details of the formatting.
    // Set to false after the call.
    bool gDebugStringFormatting = false;
#endif

// clang-format off
const rct_string_id SpeedNames[] = {
    STR_SPEED_NORMAL,
    STR_SPEED_QUICK,
    STR_SPEED_FAST,
    STR_SPEED_TURBO,
    STR_SPEED_HYPER,
};

const rct_string_id ObjectiveNames[] = {
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

const rct_string_id ResearchFundingLevelNames[] = {
    STR_RESEARCH_FUNDING_NONE,
    STR_RESEARCH_FUNDING_MINIMUM,
    STR_RESEARCH_FUNDING_NORMAL,
    STR_RESEARCH_FUNDING_MAXIMUM,
};

const rct_string_id MarketingCampaignNames[ADVERTISING_CAMPAIGN_COUNT][3] = {
    { STR_MARKETING_VOUCHERS_FOR_FREE_ENTRY_TO_THE_PARK,            STR_VOUCHERS_FOR_FREE_ENTRY_TO,         STR_MARKETING_FINISHED_FREE_ENTRY },        // ADVERTISING_CAMPAIGN_PARK_ENTRY_FREE,
    { STR_MARKETING_VOUCHERS_FOR_FREE_RIDES_ON_A_PARTICULAR_RIDE,   STR_VOUCHERS_FOR_FREE_RIDE_ON,          STR_MARKETING_FINISHED_FREE_RIDES },        // ADVERTISING_CAMPAIGN_RIDE_FREE,
    { STR_MARKETING_VOUCHERS_FOR_HALF_PRICE_ENTRY_TO_THE_PARK,      STR_VOUCHERS_FOR_HALF_PRICE_ENTRY_TO,   STR_MARKETING_FINISHED_HALF_PRICE_ENTRY },  // ADVERTISING_CAMPAIGN_PARK_ENTRY_HALF_PRICE,
    { STR_MARKETING_VOUCHERS_FOR_FREE_FOOD_OR_DRINK,                STR_VOUCHERS_FOR_FREE,                  STR_MARKETING_FINISHED_FREE_RIDE },         // ADVERTISING_CAMPAIGN_FOOD_OR_DRINK_FREE,
    { STR_MARKETING_ADVERTISING_CAMPAIGN_FOR_THE_PARK,              STR_ADVERTISING_CAMPAIGN_FOR_1,         STR_MARKETING_FINISHED_PARK_ADS },          // ADVERTISING_CAMPAIGN_PARK,
    { STR_MARKETING_ADVERTISING_CAMPAIGN_FOR_A_PARTICULAR_RIDE,     STR_ADVERTISING_CAMPAIGN_FOR_2,         STR_MARKETING_FINISHED_RIDE_ADS },          // ADVERTISING_CAMPAIGN_RIDE,
};

const rct_string_id RideInspectionIntervalNames[] = {
    STR_EVERY_10_MINUTES,
    STR_EVERY_20_MINUTES,
    STR_EVERY_30_MINUTES,
    STR_EVERY_45_MINUTES,
    STR_EVERY_HOUR,
    STR_EVERY_2_HOURS,
    STR_NEVER,
    RIDE_SETTING_INSPECTION_INTERVAL
};

const rct_string_id PeepThoughts[] = {
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
    STR_PEEP_THOUGHT_TYPE_BATHROOM,
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

const rct_string_id DateDayNames[] = {
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

const rct_string_id DateGameMonthNames[MONTH_COUNT] = {
    STR_MONTH_MARCH,
    STR_MONTH_APRIL,
    STR_MONTH_MAY,
    STR_MONTH_JUNE,
    STR_MONTH_JULY,
    STR_MONTH_AUGUST,
    STR_MONTH_SEPTEMBER,
    STR_MONTH_OCTOBER,
};

const rct_string_id DateGameShortMonthNames[MONTH_COUNT] = {
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

#define format_push_char_safe(C) { *(*dest)++ = (C); --(*size); }
#define format_handle_overflow(X) if ((*size) <= (X)) { *(*dest) = '\0'; (*size) = 0; return; }
#define format_push_char(C) { format_handle_overflow(1); format_push_char_safe(C); }

#define format_push_wrap(C) { *ncur = (C); if (ncur == (*dest)) ncur = nbegin; }
#define reverse_string() while (nbegin < nend) { tmp = *nbegin; *nbegin++ = *nend; *nend-- = tmp; }

static void format_string_part_from_raw(char **dest, size_t *size, const char *src, char **args);
static void format_string_part(char **dest, size_t *size, rct_string_id format, char **args);

static void format_append_string(char **dest, size_t *size, const utf8 *string) {
    if ((*size) == 0) return;
    size_t length = strlen(string);
    if (length < (*size)) {
        memcpy((*dest), string, length);
        (*dest) += length;
        (*size) -= length;
    } else {
        memcpy((*dest), string, (*size) - 1);
        (*dest) += (*size) - 1;
        *(*dest)++ = '\0';
        (*size) = 0;
    }
}

static void format_append_string_n(char **dest, size_t *size, const utf8 *string, size_t maxlen) {
    if ((*size) == 0) return;
    size_t length = std::min(maxlen, strlen(string));
    if (length < (*size)) {
        memcpy((*dest), string, length);
        (*dest) += length;
        (*size) -= length;
    } else {
        memcpy((*dest), string, (*size) - 1);
        (*dest) += (*size) - 1;
        *(*dest)++ = '\0';
        (*size) = 0;
    }
}

static void format_integer(char **dest, size_t *size, int64_t value)
{
    int32_t digit;
    char *nbegin, *nend, *ncur;
    char tmp;

    if ((*size) == 0) return;

    // Negative sign
    if (value < 0) {
        format_push_char('-');
        value = -value;
    }

    if (value == 0) {
        format_push_char('0');
        return;
    }

    nbegin = (*dest);

    // Right to left
    while (value > 0 && (*size) > 1) {
        digit = value % 10;
        value /= 10;

        format_push_char_safe('0' + digit);
    }

    if (value > 0) {
        ncur = nbegin;

        while (value > 0) {
            digit = value % 10;
            value /= 10;

            format_push_wrap('0' + digit);
        }

        // Reverse first half of string
        nend = ncur - 1;
        reverse_string();

        // Reverse second half of string
        nbegin = ncur;
        nend = (*dest) - 1;
        reverse_string();

        format_push_char_safe('\0'); // Truncate overflowed string
    } else {
        // Reverse string
        nend = (*dest) - 1;
        reverse_string();
    }
}

static void format_comma_separated_integer(char **dest, size_t *size, int64_t value)
{
    int32_t digit, groupIndex;
    char *nbegin, *nend, *ncur;
    char tmp;
    const char *commaMark = language_get_string(STR_LOCALE_THOUSANDS_SEPARATOR);
    const char *ch = nullptr;

    if ((*size) == 0) return;

    // Negative sign
    if (value < 0) {
        format_push_char('-');
        value = -value;
    }

    if (value == 0) {
        format_push_char('0');
        return;
    }

    nbegin = *dest;

    // Groups of three digits, right to left
    groupIndex = 0;
    while (value > 0 && (*size) > 1) {
        // Append group separator
        if (groupIndex == 3) {
            groupIndex = 0;
            ch = commaMark;
        }

        if (ch != nullptr ) {
            format_push_char_safe(*ch++);
            if (*ch == '\0') ch = nullptr;
        } else {
            digit = value % 10;
            value /= 10;

            format_push_char_safe('0' + digit);
            groupIndex++;
        }
    }

    if (value > 0) {
        ncur = nbegin;

        while (value > 0) {
            // Append group separator
            if (groupIndex == 3) {
                groupIndex = 0;
                ch = commaMark;
            }

            if (ch != nullptr ) {
                format_push_wrap(*ch++);
                if (*ch == '\0') ch = nullptr;
            } else {
                digit = value % 10;
                value /= 10;

                format_push_wrap('0' + digit);
                groupIndex++;
            }
        }

        // Reverse first half of string
        nend = ncur - 1;
        reverse_string();

        // Reverse second half of string
        nbegin = ncur;
        nend = (*dest) - 1;
        reverse_string();

        format_push_char_safe('\0'); // Truncate overflowed string
    } else {
        // Reverse string
        nend = *dest - 1;
        reverse_string();
    }
}

static void format_comma_separated_fixed_1dp(char **dest, size_t *size, int64_t value)
{
    int32_t digit, groupIndex;
    char *nbegin, *nend, *ncur;
    char tmp;
    const char *commaMark = language_get_string(STR_LOCALE_THOUSANDS_SEPARATOR);
    const char *decimalMark = language_get_string(STR_LOCALE_DECIMAL_POINT);
    const char *ch = nullptr;
    int32_t zeroNeeded = 1;

    if ((*size) == 0) return;

    // Negative sign
    if (value < 0) {
        format_push_char('-');
        value = -value;
    }

    nbegin = (*dest);

    // In the case of buffers this small,
    // all of this would be truncated anyways.
    format_handle_overflow(1);
    if ((*size) > 2) {
        // One decimal place
        digit = value % 10;
        format_push_char_safe('0' + digit);

        ch = decimalMark;
    }
    value /= 10;

    groupIndex = 0;
    while ((zeroNeeded || value > 0) && (*size) > 1) {
        // Append group separator
        if (groupIndex == 3) {
            groupIndex = 0;
            ch = commaMark;
        }

        if (ch != nullptr ) {
            format_push_char_safe(*ch++);
            if (*ch == '\0') ch = nullptr;
        } else {
            zeroNeeded = 0;
            digit = value % 10;
            value /= 10;

            format_push_char_safe('0' + digit);
            groupIndex++;
        }
    }

    if (zeroNeeded || value > 0) {
        ncur = nbegin;

        while (zeroNeeded || value > 0) {
            // Append group separator
            if (groupIndex == 3) {
                groupIndex = 0;
                ch = commaMark;
            }

            if (ch != nullptr ) {
                format_push_wrap(*ch++);
                if (*ch == '\0') ch = nullptr;
            } else {
                zeroNeeded = 0;
                digit = value % 10;
                value /= 10;

                format_push_wrap('0' + digit);
                groupIndex++;
            }
        }

        // Reverse first half of string
        nend = ncur - 1;
        reverse_string();

        // Reverse second half of string
        nbegin = ncur;
        nend = (*dest) - 1;
        reverse_string();

        format_push_char_safe('\0'); // Truncate overflowed string
    } else {
        // Reverse string
        nend = *dest - 1;
        reverse_string();
    }
}

static void format_comma_separated_fixed_2dp(char **dest, size_t *size, int64_t value)
{
    int32_t digit, groupIndex;
    char *nbegin, *nend, *ncur;
    char tmp;
    const char *commaMark = language_get_string(STR_LOCALE_THOUSANDS_SEPARATOR);
    const char *decimalMark = language_get_string(STR_LOCALE_DECIMAL_POINT);
    const char *ch = nullptr;
    int32_t zeroNeeded = 1;

    if ((*size) == 0) return;

    // Negative sign
    if (value < 0) {
        format_push_char('-');
        value = -value;
    }

    nbegin = (*dest);

    // In the case of buffers this small,
    // all of this would be truncated anyways.
    format_handle_overflow(1);
    if ((*size) < 3) {
        value /= 100;
    } else {
        // Two decimal places
        digit = value % 10;
        value /= 10;
        format_push_char_safe('0' + digit);

        digit = value % 10;
        value /= 10;
        format_push_char_safe('0' + digit);

        ch = decimalMark;
    }

    groupIndex = 0;
    while ((zeroNeeded || value > 0) && (*size) > 1) {
        // Append group separator
        if (groupIndex == 3) {
            groupIndex = 0;
            ch = commaMark;
        }

        if (ch != nullptr ) {
            format_push_char_safe(*ch++);
            if (*ch == '\0') ch = nullptr;
        } else {
            zeroNeeded = 0;
            digit = value % 10;
            value /= 10;

            format_push_char_safe('0' + digit);
            groupIndex++;
        }
    }

    if (zeroNeeded || value > 0) {
        ncur = nbegin;

        while (zeroNeeded || value > 0) {
            // Append group separator
            if (groupIndex == 3) {
                groupIndex = 0;
                ch = commaMark;
            }

            if (ch != nullptr ) {
                format_push_wrap(*ch++);
                if (*ch == '\0') ch = nullptr;
            } else {
                zeroNeeded = 0;
                digit = value % 10;
                value /= 10;

                format_push_wrap('0' + digit);
                groupIndex++;
            }
        }

        // Reverse first half of string
        nend = ncur - 1;
        reverse_string();

        // Reverse second half of string
        nbegin = ncur;
        nend = (*dest) - 1;
        reverse_string();

        format_push_char_safe('\0'); // Truncate overflowed string
    } else {
        // Reverse string
        nend = *dest - 1;
        reverse_string();
    }
}

static void format_currency(char **dest, size_t *size, int64_t value)
{
    if ((*size) == 0) return;

    const currency_descriptor *currencyDesc = &CurrencyDescriptors[gConfigGeneral.currency_format];

    value *= currencyDesc->rate;

    // Negative sign
    if (value < 0) {
        format_push_char('-');
        value = -value;
    }

    // Round the value away from zero
    value = (value + 99) / 100;

    // Currency symbol
    const utf8 *symbol = currencyDesc->symbol_unicode;
    uint8_t affix = currencyDesc->affix_unicode;
    if (!font_supports_string(symbol, FONT_SIZE_MEDIUM)) {
        symbol = currencyDesc->symbol_ascii;
        affix = currencyDesc->affix_ascii;
    }

    // Prefix
    if (affix == CURRENCY_PREFIX)
        format_append_string(dest, size, symbol);
    if ((*size) == 0) return;

    format_comma_separated_integer(dest, size, value);
    if ((*size) == 0) return;

    // Currency symbol suffix
    if (affix == CURRENCY_SUFFIX)
        format_append_string(dest, size, symbol);
}

static void format_currency_2dp(char **dest, size_t *size, int64_t value)
{
    if ((*size) == 0) return;

    const currency_descriptor *currencyDesc = &CurrencyDescriptors[gConfigGeneral.currency_format];

    int32_t rate = currencyDesc->rate;
    value *= rate;

    // Negative sign
    if (value < 0) {
        format_push_char('-');
        value = -value;
    }

    // Currency symbol
    const utf8 *symbol = currencyDesc->symbol_unicode;
    uint8_t affix = currencyDesc->affix_unicode;
    if (!font_supports_string(symbol, FONT_SIZE_MEDIUM)) {
        symbol = currencyDesc->symbol_ascii;
        affix = currencyDesc->affix_ascii;
    }

    // Prefix
    if (affix == CURRENCY_PREFIX)
        format_append_string(dest, size, symbol);
    if ((*size) == 0) return;

    // Drop the pennies for "large" currencies
    if (rate >= 100) {
        format_comma_separated_integer(dest, size, value / 100);
    } else {
        format_comma_separated_fixed_2dp(dest, size, value);
    }
    if ((*size) == 0) return;

    // Currency symbol suffix
    if (affix == CURRENCY_SUFFIX)
        format_append_string(dest, size, symbol);
}

static void format_date(char **dest, size_t *size, uint16_t value)
{
    uint16_t args[] = { static_cast<uint16_t>(date_get_month(value)), static_cast<uint16_t>(date_get_year(value) + 1) };
    uint16_t *argsRef = args;
    format_string_part(dest, size, STR_DATE_FORMAT_MY, (char**)&argsRef);
}

static void format_length(char **dest, size_t *size, int16_t value)
{
    rct_string_id stringId = STR_UNIT_SUFFIX_METRES;

    if (gConfigGeneral.measurement_format == MEASUREMENT_FORMAT_IMPERIAL) {
        value = metres_to_feet(value);
        stringId = STR_UNIT_SUFFIX_FEET;
    }

    int16_t *argRef = &value;
    format_string_part(dest, size, stringId, (char**)&argRef);
}

static void format_velocity(char **dest, size_t *size, uint16_t value)
{
    rct_string_id stringId;

    switch (gConfigGeneral.measurement_format) {
    default:
        stringId = STR_UNIT_SUFFIX_MILES_PER_HOUR;
        break;
    case MEASUREMENT_FORMAT_METRIC:
        value = mph_to_kmph(value);
        stringId = STR_UNIT_SUFFIX_KILOMETRES_PER_HOUR;
        break;
    case MEASUREMENT_FORMAT_SI:
        value = mph_to_dmps(value);
        stringId = STR_UNIT_SUFFIX_METRES_PER_SECOND;
        break;
    }

    uint16_t *argRef = &value;
    format_string_part(dest, size, stringId, (char**)&argRef);
}

static constexpr const rct_string_id DurationFormats[][2] = {
    {STR_DURATION_SEC,      STR_DURATION_SECS},
    {STR_DURATION_MIN_SEC,  STR_DURATION_MIN_SECS},
    {STR_DURATION_MINS_SEC, STR_DURATION_MINS_SECS},
};

static void format_duration(char **dest, size_t *size, uint16_t value)
{
    uint16_t minutes = value / 60;
    uint16_t seconds = value % 60;
    uint16_t args[] = { minutes, seconds };
    uint16_t *argsRef = &args[1];

    int32_t minuteIndex = 0;
    if (minutes > 0) {
        minuteIndex = 1;
        if (minutes != 1) {
            minuteIndex = 2;
        }

        argsRef--;
    }

    int32_t secondsIndex = 0;
    if (seconds != 1) {
        secondsIndex = 1;
    }

    rct_string_id stringId = DurationFormats[minuteIndex][secondsIndex];

    format_string_part(dest, size, stringId, (char**)&argsRef);
}

static constexpr const rct_string_id RealtimeFormats[][2] = {
    {STR_REALTIME_MIN,       STR_REALTIME_MINS},
    {STR_REALTIME_HOUR_MIN,  STR_REALTIME_HOUR_MINS},
    {STR_REALTIME_HOURS_MIN, STR_REALTIME_HOURS_MINS},
};

static void format_realtime(char **dest, size_t *size, uint16_t value)
{
    uint16_t hours = value / 60;
    uint16_t minutes = value % 60;
    uint16_t args[] = { hours, minutes };
    uint16_t *argsRef = &args[1];

    int32_t hourIndex = 0;
    if (hours > 0) {
        hourIndex = 1;
        if (hours != 1) {
            hourIndex = 2;
        }

        argsRef--;
    }

    int32_t minuteIndex = 0;
    if (minutes != 1) {
        minuteIndex = 1;
    }

    rct_string_id stringId = RealtimeFormats[hourIndex][minuteIndex];

    format_string_part(dest, size, stringId, (char**)&argsRef);
}

static void format_string_code(uint32_t format_code, char **dest, size_t *size, char **args)
{
    intptr_t value;

    if ((*size) == 0) return;

#ifdef DEBUG
    if (gDebugStringFormatting) {
        printf("format_string_code(\"%s\")\n", format_get_token(format_code));
    }
#endif

    switch (format_code) {
    case FORMAT_COMMA32:
        // Pop argument
        value = *((int32_t*)*args);
        *args += 4;

        format_comma_separated_integer(dest, size, value);
        break;
    case FORMAT_INT32:
        // Pop argument
        value = *((int32_t*)*args);
        *args += 4;

        format_integer(dest, size, value);
        break;
    case FORMAT_COMMA2DP32:
        // Pop argument
        value = *((int32_t*)*args);
        *args += 4;

        format_comma_separated_fixed_2dp(dest, size, value);
        break;
    case FORMAT_COMMA1DP16:
        // Pop argument
        value = *((int16_t*)*args);
        *args += 2;

        format_comma_separated_fixed_1dp(dest, size, value);
        break;
    case FORMAT_COMMA16:
        // Pop argument
        value = *((int16_t*)*args);
        *args += 2;

        format_comma_separated_integer(dest, size, value);
        break;
    case FORMAT_UINT16:
        // Pop argument
        value = *((uint16_t*)*args);
        *args += 2;

        format_integer(dest, size, value);
        break;
    case FORMAT_CURRENCY2DP:
        // Pop argument
        value = *((int32_t*)*args);
        *args += 4;

        format_currency_2dp(dest, size, value);
        break;
    case FORMAT_CURRENCY:
        // Pop argument
        value = *((int32_t*)*args);
        *args += 4;

        format_currency(dest, size, value);
        break;
    case FORMAT_STRINGID:
    case FORMAT_STRINGID2:
        // Pop argument
        value = *((uint16_t*)*args);
        *args += 2;

        format_string_part(dest, size, (rct_string_id)value, args);
        break;
    case FORMAT_STRING:
        // Pop argument
        value = *((uintptr_t*)*args);
        *args += sizeof(uintptr_t);

        if (value != 0)
            format_append_string(dest, size, (char*)value);
        break;
    case FORMAT_MONTHYEAR:
        // Pop argument
        value = *((uint16_t*)*args);
        *args += 2;

        format_date(dest, size, (uint16_t)value);
        break;
    case FORMAT_MONTH:
        // Pop argument
        value = *((uint16_t*)*args);
        *args += 2;

        format_append_string(dest, size, language_get_string(DateGameMonthNames[date_get_month((int32_t)value)]));
        break;
    case FORMAT_VELOCITY:
        // Pop argument
        value = *((int16_t*)*args);
        *args += 2;

        format_velocity(dest, size, (uint16_t)value);
        break;
    case FORMAT_POP16:
        *args += 2;
        break;
    case FORMAT_PUSH16:
        *args -= 2;
        break;
    case FORMAT_DURATION:
        // Pop argument
        value = *((uint16_t*)*args);
        *args += 2;

        format_duration(dest, size, (uint16_t)value);
        break;
    case FORMAT_REALTIME:
        // Pop argument
        value = *((uint16_t*)*args);
        *args += 2;

        format_realtime(dest, size, (uint16_t)value);
        break;
    case FORMAT_LENGTH:
        // Pop argument
        value = *((int16_t*)*args);
        *args += 2;

        format_length(dest, size, (int16_t)value);
        break;
    case FORMAT_SPRITE:
        // Pop argument
        value = *((uint32_t*)*args);
        *args += 4;

        format_handle_overflow(1 + sizeof(uint32_t));

        format_push_char_safe('\x17');
        *((uint32_t*)(*dest)) = (uint32_t)value;
        (*dest) += sizeof(uint32_t);
        (*size) -= sizeof(uint32_t);
        break;
    }
}

static void format_string_part_from_raw(utf8 **dest, size_t *size, const utf8 *src, char **args)
{
#ifdef DEBUG
    if (gDebugStringFormatting) {
        printf("format_string_part_from_raw(\"%s\")\n", src);
    }
#endif

    while (*size > 1) {
        uint32_t code = utf8_get_next(src, &src);
        if (code < ' ') {
            if (code == 0) {
                break;
            } else if (code <= 4) {
                format_handle_overflow(2);
                format_push_char_safe(code);
                format_push_char_safe(*src++);
            } else if (code <= 16) {
                format_handle_overflow(1);
                format_push_char_safe(code);
            } else if (code <= 22) {
                format_handle_overflow(3);
                format_push_char_safe(code);
                format_push_char_safe(*src++);
                format_push_char_safe(*src++);
            } else {
                format_handle_overflow(5);
                format_push_char_safe(code);
                format_push_char_safe(*src++);
                format_push_char_safe(*src++);
                format_push_char_safe(*src++);
                format_push_char_safe(*src++);
            }
        } else if (code <= 'z') {
            format_push_char(code);
        } else if (code < FORMAT_COLOUR_CODE_START || code == FORMAT_COMMA1DP16) {
            format_string_code(code, dest, size, args);
        } else {
            size_t codepointLength = (size_t)utf8_get_codepoint_length(code);
            format_handle_overflow(codepointLength);
            if (*size > codepointLength) {
                *dest = utf8_write_codepoint(*dest, code);
                *size -= codepointLength;
            }
        }
    }
}

static void format_string_part(utf8 **dest, size_t *size, rct_string_id format, char **args)
{
    if (format == STR_NONE) {
        if (*size > 0) {
            *(*dest) = '\0';
        }
    } else if (format < USER_STRING_START) {
        // Language string
        const utf8 * rawString = language_get_string(format);
        format_string_part_from_raw(dest, size, rawString, args);
    } else if (format <= USER_STRING_END) {
        // Custom string
        format -= 0x8000;

        // Bits 10, 11 represent number of bytes to pop off arguments
        *args += (format & 0xC00) >> 9;
        format &= ~0xC00;

        format_append_string_n(dest, size, gUserStrings[format], USER_STRING_MAX_LENGTH);
        if ((*size) > 0) *(*dest) = '\0';
    } else if (format <= REAL_NAME_END) {
        // Real name
        auto realNameIndex = format - REAL_NAME_START;

        format_append_string(dest, size, real_names[realNameIndex % Util::CountOf(real_names)]);
        if ((*size) == 0) return;
        format_push_char(' ');
        format_push_char(real_name_initials[(realNameIndex >> 10) % Util::CountOf(real_name_initials)]);
        format_push_char('.');
        *(*dest) = '\0';

        *args += 4;
    } else {
        // ?
        log_error("Localisation CALLPROC reached. Please contact a dev");
        assert(false);
    }
}

/**
 * Writes a formatted string to a buffer.
 *  rct2: 0x006C2555
 * dest (edi)
 * format (ax)
 * args (ecx)
 */
void format_string(utf8 *dest, size_t size, rct_string_id format, void *args)
{
#ifdef DEBUG
    if (gDebugStringFormatting) {
        printf("format_string(%hu)\n", format);
    }
#endif

    if (size == 0) {
        return;
    }

    utf8 *end = dest;
    size_t left = size;
    format_string_part(&end, &left, format, (char**)&args);
    if (left == 0) {
        // Replace last character with null terminator
        *(end - 1) = '\0';
        log_warning("Truncating formatted string \"%s\" to %d bytes.", dest, size);
    } else {
        // Null terminate
        *end = '\0';
    }

#ifdef DEBUG
    // Check if characters were written past the end of the buffer
    assert(end <= dest + size);
#endif
}

void format_string_raw(utf8 *dest, size_t size, utf8 *src, void *args)
{
#ifdef DEBUG
    if (gDebugStringFormatting) {
        printf("format_string_raw(\"%s\")\n", src);
    }
#endif

    if (size == 0) {
        return;
    }

    utf8 *end = dest;
    size_t left = size;
    format_string_part_from_raw(&end, &left, src, (char**)&args);
    if (left == 0) {
        // Replace last character with null terminator
        *(end - 1) = '\0';
        log_warning("Truncating formatted string \"%s\" to %d bytes.", dest, size);
    } else {
        // Null terminate
        *end = '\0';
    }

#ifdef DEBUG
    // Check if characters were written past the end of the buffer
    assert(end <= dest + size);
#endif
}

/**
 * Writes a formatted string to a buffer and converts it to upper case.
 *  rct2: 0x006C2538
 * dest (edi)
 * format (ax)
 * args (ecx)
 */
void format_string_to_upper(utf8 *dest, size_t size, rct_string_id format, void *args)
{
#ifdef DEBUG
    if (gDebugStringFormatting) {
        printf("format_string_to_upper(%hu)\n", format);
    }
#endif

    if (size == 0) {
        return;
    }

    format_string(dest, size, format, args);

    std::string upperString = String::ToUpper(dest);

    if (upperString.size() + 1 >= size) {
        upperString.resize(size - 1);
        dest[size - 1] = '\0';
        log_warning("Truncating formatted string \"%s\" to %d bytes.", dest, size);
    }

    upperString.copy(dest, upperString.size());
    dest[upperString.size()] = '\0';
}

money32 string_to_money(const char* string_to_monetise)
{
    const char* decimal_char = language_get_string(STR_LOCALE_DECIMAL_POINT);
    const currency_descriptor* currencyDesc = &CurrencyDescriptors[gConfigGeneral.currency_format];
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
                return MONEY32_UNDEFINED;
            else
                hasDecSep = true;

            // Replace localised decimal separator with an English one.
            *dst_ptr++ = '.';
            src_ptr++;
            continue;
        }
        else if (*src_ptr == '-')
        {
            if (hasMinus)
                return MONEY32_UNDEFINED;
            else
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
        return MONEY32_UNDEFINED;

    int32_t sign = 1;
    if (hasMinus)
    {
        // If there is a minus sign, it has to be at position 0 in order to be valid.
        if (processedString[0] == '-')
            sign = -1;
        else
            return MONEY32_UNDEFINED;
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
    auto whole = static_cast<uint16_t>(number);
    auto fraction = static_cast<uint8_t>((number - whole) * 100);

    money32 result = MONEY(whole, fraction);
    // Check if MONEY resulted in overflow
    if ((whole > 0 && result < 0) || result / 10 < whole)
    {
        result = INT_MAX;
    }
    result *= sign;
    return result;
}

/**
 *
 * @param amount The amount in tens of pounds, e.g. 123 = £ 12.30
 * @param buffer_to_put_value_to Output parameter.
 * @param buffer_len Length of the buffer.
 * @param forceDecimals Show decimals, even if the amount does not have them. Will be ignored if the current exchange
 *                          rate is too big to have decimals.
 */
void money_to_string(money32 amount, char * buffer_to_put_value_to, size_t buffer_len, bool forceDecimals)
{
    if (amount == MONEY32_UNDEFINED)
    {
        snprintf(buffer_to_put_value_to, buffer_len, "0");
        return;
    }

    const currency_descriptor *currencyDesc = &CurrencyDescriptors[gConfigGeneral.currency_format];

    int sign = amount >= 0 ? 1 : -1;
    int a = abs(amount) * currencyDesc->rate;

    bool amountIsInteger = (a / 100 > 0) && (a % 100 == 0);

    // If whole and decimal exist
    if ((a / 100 > 0 && a % 100 > 0) || (amountIsInteger && forceDecimals && currencyDesc->rate < 100))
    {
        const char* decimal_char = language_get_string(STR_LOCALE_DECIMAL_POINT);
        auto decimalPart = a % 100;
        auto precedingZero = (decimalPart < 10) ? "0" : "";
        snprintf(buffer_to_put_value_to, buffer_len, "%d%s%s%d", (a / 100) * sign, decimal_char, precedingZero, decimalPart);
    }
    // If whole exists, but not decimal
    else if (amountIsInteger)
    {
        snprintf(buffer_to_put_value_to, buffer_len, "%d", (a / 100) * sign);
    }
    // If decimal exists, but not whole
    else if (a / 100 == 0 && a % 100 > 0)
    {
        const char* decimal_char = language_get_string(STR_LOCALE_DECIMAL_POINT);
        snprintf(buffer_to_put_value_to, buffer_len, "%s0%s%d", sign < 0 ? "-" : "", decimal_char, a % 100);
    }
    else
    {
        snprintf(buffer_to_put_value_to, buffer_len, "0");
    }
}
