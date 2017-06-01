#pragma region Copyright (c) 2014-2017 OpenRCT2 Developers
/*****************************************************************************
 * OpenRCT2, an open source clone of Roller Coaster Tycoon 2.
 *
 * OpenRCT2 is the work of many authors, a full list can be found in contributors.md
 * For more information, visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * A full copy of the GNU General Public License can be found in licence.txt
 *****************************************************************************/
#pragma endregion

#include "../common.h"

#include <ctype.h>

#ifdef __WINDOWS__
#include <windows.h>
#else
#include <iconv.h>
#include <errno.h>
#endif // __WINDOWS__

#include "../config/Config.h"
#include "../game.h"
#include "../util/util.h"
#include "date.h"
#include "localisation.h"
#include "../management/marketing.h"

char gCommonStringFormatBuffer[256];
uint8 gCommonFormatArgs[80];
uint8 gMapTooltipFormatArgs[40];

#ifdef DEBUG
	// Set to true before a string format call to see details of the formatting.
	// Set to false after the call.
	bool gDebugStringFormatting = false;
#endif

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
	{ STR_MARKETING_VOUCHERS_FOR_FREE_ENTRY_TO_THE_PARK,			STR_VOUCHERS_FOR_FREE_ENTRY_TO,			STR_MARKETING_FINISHED_FREE_ENTRY },		// ADVERTISING_CAMPAIGN_PARK_ENTRY_FREE,
	{ STR_MARKETING_VOUCHERS_FOR_FREE_RIDES_ON_A_PARTICULAR_RIDE,	STR_VOUCHERS_FOR_FREE_RIDE_ON,			STR_MARKETING_FINISHED_FREE_RIDES },		// ADVERTISING_CAMPAIGN_RIDE_FREE,
	{ STR_MARKETING_VOUCHERS_FOR_HALF_PRICE_ENTRY_TO_THE_PARK,		STR_VOUCHERS_FOR_HALF_PRICE_ENTRY_TO,	STR_MARKETING_FINISHED_HALF_PRICE_ENTRY },	// ADVERTISING_CAMPAIGN_PARK_ENTRY_HALF_PRICE,
	{ STR_MARKETING_VOUCHERS_FOR_FREE_FOOD_OR_DRINK,				STR_VOUCHERS_FOR_FREE,					STR_MARKETING_FINISHED_FREE_RIDE },			// ADVERTISING_CAMPAIGN_FOOD_OR_DRINK_FREE,
	{ STR_MARKETING_ADVERTISING_CAMPAIGN_FOR_THE_PARK,				STR_ADVERTISING_CAMPAIGN_FOR_1,			STR_MARKETING_FINISHED_PARK_ADS },			// ADVERTISING_CAMPAIGN_PARK,
	{ STR_MARKETING_ADVERTISING_CAMPAIGN_FOR_A_PARTICULAR_RIDE,		STR_ADVERTISING_CAMPAIGN_FOR_2,			STR_MARKETING_FINISHED_RIDE_ADS },			// ADVERTISING_CAMPAIGN_RIDE,
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
	STR_PEEP_THOUGHT_TYPE_CANDY_APPLE,
	STR_PEEP_THOUGHT_TYPE_TSHIRT,
	STR_PEEP_THOUGHT_TYPE_DONUT,
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
	STR_PEEP_THOUGHT_TYPE_CANDY_APPLE_MUCH,
	STR_PEEP_THOUGHT_TYPE_TSHIRT_MUCH,
	STR_PEEP_THOUGHT_TYPE_DONUT_MUCH,
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
	size_t length = min(maxlen, strlen(string));
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

static void format_integer(char **dest, size_t *size, sint64 value)
{
	sint32 digit;
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

static void format_comma_separated_integer(char **dest, size_t *size, sint64 value)
{
	sint32 digit, groupIndex;
	char *nbegin, *nend, *ncur;
	char tmp;
	const char *commaMark = language_get_string(STR_LOCALE_THOUSANDS_SEPARATOR);
	const char *ch = NULL;

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

		if (ch != NULL ) {
			format_push_char_safe(*ch++);
			if (*ch == '\0') ch = NULL;
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

			if (ch != NULL ) {
				format_push_wrap(*ch++);
				if (*ch == '\0') ch = NULL;
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

static void format_comma_separated_fixed_1dp(char **dest, size_t *size, sint64 value)
{
	sint32 digit, groupIndex;
	char *nbegin, *nend, *ncur;
	char tmp;
	const char *commaMark = language_get_string(STR_LOCALE_THOUSANDS_SEPARATOR);
	const char *decimalMark = language_get_string(STR_LOCALE_DECIMAL_POINT);
	const char *ch = NULL;
	sint32 zeroNeeded = 1;

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

		if (ch != NULL ) {
			format_push_char_safe(*ch++);
			if (*ch == '\0') ch = NULL;
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

			if (ch != NULL ) {
				format_push_wrap(*ch++);
				if (*ch == '\0') ch = NULL;
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

static void format_comma_separated_fixed_2dp(char **dest, size_t *size, sint64 value)
{
	sint32 digit, groupIndex;
	char *nbegin, *nend, *ncur;
	char tmp;
	const char *commaMark = language_get_string(STR_LOCALE_THOUSANDS_SEPARATOR);
	const char *decimalMark = language_get_string(STR_LOCALE_DECIMAL_POINT);
	const char *ch = NULL;
	sint32 zeroNeeded = 1;

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

		if (ch != NULL ) {
			format_push_char_safe(*ch++);
			if (*ch == '\0') ch = NULL;
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

			if (ch != NULL ) {
				format_push_wrap(*ch++);
				if (*ch == '\0') ch = NULL;
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

static void format_currency(char **dest, size_t *size, sint64 value)
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
	uint8 affix = currencyDesc->affix_unicode;
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

static void format_currency_2dp(char **dest, size_t *size, sint64 value)
{
	if ((*size) == 0) return;

	const currency_descriptor *currencyDesc = &CurrencyDescriptors[gConfigGeneral.currency_format];

	sint32 rate = currencyDesc->rate;
	value *= rate;

	// Negative sign
	if (value < 0) {
		format_push_char('-');
		value = -value;
	}

	// Currency symbol
	const utf8 *symbol = currencyDesc->symbol_unicode;
	uint8 affix = currencyDesc->affix_unicode;
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

static void format_date(char **dest, size_t *size, uint16 value)
{
	uint16 args[] = { date_get_month(value), date_get_year(value) + 1 };
	uint16 *argsRef = args;
	format_string_part(dest, size, STR_DATE_FORMAT_MY, (char**)&argsRef);
}

static void format_length(char **dest, size_t *size, sint16 value)
{
	rct_string_id stringId = STR_UNIT_SUFFIX_METRES;

	if (gConfigGeneral.measurement_format == MEASUREMENT_FORMAT_IMPERIAL) {
		value = metres_to_feet(value);
		stringId = STR_UNIT_SUFFIX_FEET;
	}

	sint16 *argRef = &value;
	format_string_part(dest, size, stringId, (char**)&argRef);
}

static void format_velocity(char **dest, size_t *size, uint16 value)
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

	uint16 *argRef = &value;
	format_string_part(dest, size, stringId, (char**)&argRef);
}

static const rct_string_id DurationFormats[][2] = {
	{STR_DURATION_SEC,      STR_DURATION_SECS},
	{STR_DURATION_MIN_SEC,  STR_DURATION_MIN_SECS},
	{STR_DURATION_MINS_SEC, STR_DURATION_MINS_SECS},
};

static void format_duration(char **dest, size_t *size, uint16 value)
{
	uint16 minutes = value / 60;
	uint16 seconds = value % 60;
	uint16 args[] = { minutes, seconds };
	uint16 *argsRef = &args[1];

	sint32 minuteIndex = 0;
	if (minutes > 0) {
		minuteIndex = 1;
		if (minutes != 1) {
			minuteIndex = 2;
		}

		argsRef--;
	}

	sint32 secondsIndex = 0;
	if (seconds != 1) {
		secondsIndex = 1;
	}

	rct_string_id stringId = DurationFormats[minuteIndex][secondsIndex];

	format_string_part(dest, size, stringId, (char**)&argsRef);
}

static const rct_string_id RealtimeFormats[][2] = {
	{STR_REALTIME_MIN,       STR_REALTIME_MINS},
	{STR_REALTIME_HOUR_MIN,  STR_REALTIME_HOUR_MINS},
	{STR_REALTIME_HOURS_MIN, STR_REALTIME_HOURS_MINS},
};

static void format_realtime(char **dest, size_t *size, uint16 value)
{
	uint16 hours = value / 60;
	uint16 minutes = value % 60;
	uint16 args[] = { hours, minutes };
	uint16 *argsRef = &args[1];

	sint32 hourIndex = 0;
	if (hours > 0) {
		hourIndex = 1;
		if (hours != 1) {
			hourIndex = 2;
		}

		argsRef--;
	}

	sint32 minuteIndex = 0;
	if (minutes != 1) {
		minuteIndex = 1;
	}

	rct_string_id stringId = RealtimeFormats[hourIndex][minuteIndex];

	format_string_part(dest, size, stringId, (char**)&argsRef);
}

static void format_string_code(uint32 format_code, char **dest, size_t *size, char **args)
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
		value = *((sint32*)*args);
		*args += 4;

		format_comma_separated_integer(dest, size, value);
		break;
	case FORMAT_INT32:
		// Pop argument
		value = *((sint32*)*args);
		*args += 4;

		format_integer(dest, size, value);
		break;
	case FORMAT_COMMA2DP32:
		// Pop argument
		value = *((sint32*)*args);
		*args += 4;

		format_comma_separated_fixed_2dp(dest, size, value);
		break;
	case FORMAT_COMMA1DP16:
		// Pop argument
		value = *((sint16*)*args);
		*args += 2;

		format_comma_separated_fixed_1dp(dest, size, value);
		break;
	case FORMAT_COMMA16:
		// Pop argument
		value = *((sint16*)*args);
		*args += 2;

		format_comma_separated_integer(dest, size, value);
		break;
	case FORMAT_UINT16:
		// Pop argument
		value = *((uint16*)*args);
		*args += 2;

		format_integer(dest, size, value);
		break;
	case FORMAT_CURRENCY2DP:
		// Pop argument
		value = *((sint32*)*args);
		*args += 4;

		format_currency_2dp(dest, size, value);
		break;
	case FORMAT_CURRENCY:
		// Pop argument
		value = *((sint32*)*args);
		*args += 4;

		format_currency(dest, size, value);
		break;
	case FORMAT_STRINGID:
	case FORMAT_STRINGID2:
		// Pop argument
		value = *((uint16*)*args);
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
		value = *((uint16*)*args);
		*args += 2;

		format_date(dest, size, (uint16)value);
		break;
	case FORMAT_MONTH:
		// Pop argument
		value = *((uint16*)*args);
		*args += 2;

		format_append_string(dest, size, language_get_string(DateGameMonthNames[date_get_month((sint32)value)]));
		break;
	case FORMAT_VELOCITY:
		// Pop argument
		value = *((sint16*)*args);
		*args += 2;

		format_velocity(dest, size, (uint16)value);
		break;
	case FORMAT_POP16:
		*args += 2;
		break;
	case FORMAT_PUSH16:
		*args -= 2;
		break;
	case FORMAT_DURATION:
		// Pop argument
		value = *((uint16*)*args);
		*args += 2;

		format_duration(dest, size, (uint16)value);
		break;
	case FORMAT_REALTIME:
		// Pop argument
		value = *((uint16*)*args);
		*args += 2;

		format_realtime(dest, size, (uint16)value);
		break;
	case FORMAT_LENGTH:
		// Pop argument
		value = *((sint16*)*args);
		*args += 2;

		format_length(dest, size, (sint16)value);
		break;
	case FORMAT_SPRITE:
		// Pop argument
		value = *((uint32*)*args);
		*args += 4;

		format_handle_overflow(1 + sizeof(uint32));

		format_push_char_safe('\x17');
		*((uint32*)(*dest)) = (uint32)value;
		(*dest) += sizeof(uint32);
		(*size) -= sizeof(uint32);
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
		uint32 code = utf8_get_next(src, &src);
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
	} else if (format < 0x8000) {
		// Language string
		const utf8 * rawString = language_get_string(format);
		format_string_part_from_raw(dest, size, rawString, args);
	} else if (format < 0x9000) {
		// Custom string
		format -= 0x8000;

		// Bits 10, 11 represent number of bytes to pop off arguments
		*args += (format & 0xC00) >> 9;
		format &= ~0xC00;

		format_append_string_n(dest, size, &gUserStrings[format * 32], 32);
		if ((*size) > 0) *(*dest) = '\0';
	} else if (format < 0xE000) {
		// Real name
		format -= -0xA000;

		format_append_string(dest, size, real_names[format % countof(real_names)]);
		if ((*size) == 0) return;
		format_push_char(' ');
		format_push_char(real_name_initials[(format >> 10) % countof(real_name_initials)]);
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

	// Convert to upper case
	utf8 *ch = dest;
	while (*ch != '\0') {
		*ch = toupper(*ch);
		ch++;
	}
}

money32 string_to_money(char * string_to_monetise)
{
	const char* decimal_char = language_get_string(STR_LOCALE_DECIMAL_POINT);
	char * text_ptr = string_to_monetise;
	int i, j, sign;
	// Remove everything except numbers decimal, and minus sign(s)
	for (i = 0; text_ptr[i] != '\0'; ++i) {
		while (!(
			(text_ptr[i] >= '0' && text_ptr[i] <= '9') ||
			(text_ptr[i] == decimal_char[0]) ||
			(text_ptr[i] == '-') ||
			(text_ptr[i] == '\0')
		)) {
			// Move everything over to the left by one
			for (j = i; text_ptr[j] != '\0'; ++j) {
				text_ptr[j] = text_ptr[j + 1];
			}
			text_ptr[j] = '\0';
		}
	}

	// If first character of shortened string is a minus, consider number negative
	if (text_ptr[0] == '-') {
		sign = -1;
	}
	else {
		sign = 1;
	}

	// Now minus signs can be removed from string
	for (i = 0; text_ptr[i] != '\0'; ++i) {
		if (text_ptr[i] == '-') {
			for (j = i; text_ptr[j] != '\0'; ++j) {
				text_ptr[j] = text_ptr[j + 1];
			}
			text_ptr[j] = '\0';
		}
	}

	// Due to the nature of strstr and strtok, decimals at the very beginning will be ignored, causing
	// ".1" to be interpreted as "1". To prevent this, prefix with "0" if decimal is at the beginning.
	char * buffer = (char *)malloc(strlen(string_to_monetise) + 4);
	if (string_to_monetise[0] == decimal_char[0]) {
		strcpy(buffer, "0");
		strcpy(buffer + 1, string_to_monetise);
	}
	else {
		strcpy(buffer, string_to_monetise);
	}

	int number = 0, decimal = 0;
	if (strstr(buffer, decimal_char) == NULL) {
		// If decimal char does not exist, no tokenising is needed.
		number = atoi(buffer);
	}
	else {
		char *numberText = strtok(buffer, decimal_char);
		char *decimalText = strtok(NULL, decimal_char);

		if (numberText != NULL) number = atoi(numberText);
		if (decimalText != NULL) decimal = atoi(decimalText);

		// The second parameter in MONEY must be two digits in length, while the game only ever uses
		// the first of the two digits.
		// Convert invalid numbers, such as ".6", ".234", ".05", to ".60", ".20", ".00" (respectively)
		while (decimal > 10) decimal /= 10;
		if (decimal < 10) decimal *= 10;
	}
	free(buffer);

	money32 result = MONEY(number, decimal);
	// Check if MONEY resulted in overflow
	if ((number > 0 && result < 0) || result / 10 < number) {
		result = INT_MAX;
	}
	result *= sign;
	return result;
}

void money_to_string(money32 amount, char * buffer_to_put_value_to, size_t buffer_len)
{
	if (amount == MONEY32_UNDEFINED) {
		snprintf(buffer_to_put_value_to, buffer_len, "0");
		return;
	}
	int sign = amount >= 0 ? 1 : -1;
	int a = abs(amount);
	if (a / 10 > 0 && a % 10 > 0) { // If whole and decimal exist
		const char* decimal_char = language_get_string(STR_LOCALE_DECIMAL_POINT);
		snprintf(buffer_to_put_value_to, buffer_len, "%d%s%d0", (a / 10) * sign, decimal_char, a % 10);
	}
	else if (a / 10 > 0 && a % 10 == 0) { // If whole exists, but not decimal
		snprintf(buffer_to_put_value_to, buffer_len, "%d", (a / 10) * sign);
	}
	else if (a / 10 == 0 && a % 10 > 0) { // If decimal exists, but not whole
		const char* decimal_char = language_get_string(STR_LOCALE_DECIMAL_POINT);
		snprintf(buffer_to_put_value_to, buffer_len, "%s0%s%d0", sign < 0 ? "-" : "", decimal_char, a % 10);
	}
	else {
		snprintf(buffer_to_put_value_to, buffer_len, "0");
	}
}

utf8 *win1252_to_utf8_alloc(const char *src, size_t srcMaxSize)
{
	size_t stringLength = strnlen(src, srcMaxSize);
	size_t reservedSpace = (stringLength * 4) + 1;
	utf8 *result = malloc(reservedSpace);
	sint32 actualSpace = win1252_to_utf8(result, src, stringLength, reservedSpace);
	return (utf8*)realloc(result, actualSpace);
}

sint32 win1252_to_utf8(utf8string dst, const char *src, size_t srcLength, size_t maxBufferLength)
{
#ifdef __WINDOWS__
	utf16 stackBuffer[256];
	utf16 *heapBuffer = NULL;
	utf16 *intermediateBuffer = stackBuffer;
	size_t bufferCount = countof(stackBuffer);
	if (maxBufferLength > bufferCount) {
		if (srcLength > bufferCount) {
			bufferCount = srcLength + 4;
			heapBuffer = malloc(bufferCount * sizeof(utf16));
			assert(heapBuffer != NULL);
			intermediateBuffer = heapBuffer;
		}
	}
	MultiByteToWideChar(CP_ACP, 0, src, -1, intermediateBuffer, (sint32)bufferCount);
	sint32 result = WideCharToMultiByte(CP_UTF8, 0, intermediateBuffer, -1, dst, (sint32)maxBufferLength, NULL, NULL);

	free(heapBuffer);
#else
	//log_warning("converting %s of size %d", src, srcLength);
	char *buffer_conv = strndup(src, srcLength);
	char *buffer_orig = buffer_conv;
	const char *to_charset = "UTF8";
	const char *from_charset = "CP1252";
	iconv_t cd = iconv_open(to_charset, from_charset);
	if ((iconv_t)-1 == cd)
	{
		sint32 error = errno;
		switch (error)
		{
			case EINVAL:
				log_error("Unsupported conversion from %s to %s, errno = %d", from_charset, to_charset, error);
				break;
			default:
				log_error("Unknown error while initialising iconv, errno = %d", error);
		}
		return 0;
	}
	size_t obl = maxBufferLength;
	char *outBuf = dst;
	size_t conversion_result = iconv(cd, &buffer_conv, &srcLength, &outBuf, &obl);
	if (conversion_result == (size_t)-1)
	{
		sint32 error = errno;
		switch (error)
		{
			case EILSEQ:
				log_error("Encountered invalid sequence");
				break;
			case EINVAL:
				log_error("Encountered incomplete sequence");
				break;
			case E2BIG:
				log_error("Ran out of space");
				break;
			default:
				log_error("Unknown error encountered, errno = %d", error);
		}
	}
	sint32 close_result = iconv_close(cd);
	if (close_result == -1)
	{
		log_error("Failed to close iconv, errno = %d", errno);
	}
	size_t byte_diff = maxBufferLength - obl + 1;
	dst[byte_diff - 1] = '\0';
	//log_warning("converted %s of size %d, %d", dst, byte_diff, strlen(dst));
	sint32 result = byte_diff;
	free(buffer_orig);
#endif // __WINDOWS__

	return result;
}
