#pragma region Copyright (c) 2014-2016 OpenRCT2 Developers
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

#ifdef __WINDOWS__
#include <windows.h>
#else
#include <iconv.h>
#include <errno.h>
#endif // __WINDOWS__

#include "../config.h"
#include "../game.h"
#include "../util/util.h"
#include "date.h"
#include "localisation.h"
#include "../management/marketing.h"

char gCommonStringFormatBuffer[256];
uint8 gCommonFormatArgs[80];
uint8 gMapTooltipFormatArgs[40];

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
	{ STR_MARKETING_VOUCHERS_FOR_FREE_ENTRY_TO_THE_PARK,			STR_VOUCHERS_FOR_FREE_ENTRY_TO,			STR_MARKETING_FINISHED_FREE_ENTRY }, 		// ADVERTISING_CAMPAIGN_PARK_ENTRY_FREE,
	{ STR_MARKETING_VOUCHERS_FOR_FREE_RIDES_ON_A_PARTICULAR_RIDE,	STR_VOUCHERS_FOR_FREE_RIDE_ON,			STR_MARKETING_FINISHED_FREE_RIDES },		// ADVERTISING_CAMPAIGN_RIDE_FREE,
	{ STR_MARKETING_VOUCHERS_FOR_HALF_PRICE_ENTRY_TO_THE_PARK,		STR_VOUCHERS_FOR_HALF_PRICE_ENTRY_TO,	STR_MARKETING_FINISHED_HALF_PRICE_ENTRY },	// ADVERTISING_CAMPAIGN_PARK_ENTRY_HALF_PRICE,
	{ STR_MARKETING_VOUCHERS_FOR_FREE_FOOD_OR_DRINK,				STR_VOUCHERS_FOR_FREE,					STR_MARKETING_FINISHED_FREE_RIDE },				// ADVERTISING_CAMPAIGN_FOOD_OR_DRINK_FREE,
	{ STR_MARKETING_ADVERTISING_CAMPAIGN_FOR_THE_PARK,				STR_ADVERTISING_CAMPAIGN_FOR_1,			STR_MARKETING_FINISHED_PARK_ADS },		// ADVERTISING_CAMPAIGN_PARK,
	{ STR_MARKETING_ADVERTISING_CAMPAIGN_FOR_A_PARTICULAR_RIDE,		STR_ADVERTISING_CAMPAIGN_FOR_2,			STR_MARKETING_FINISHED_RIDE_ADS },		// ADVERTISING_CAMPAIGN_RIDE,
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
	STR_PEEP_THOUGHT_TYPE_FRIES,
	STR_PEEP_THOUGHT_TYPE_ICE_CREAM,
	STR_PEEP_THOUGHT_TYPE_COTTON_CANDY,
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
	STR_PEEP_THOUGHT_TYPE_FRIES_MUCH,
	STR_PEEP_THOUGHT_TYPE_ICE_CREAM_MUCH,
	STR_PEEP_THOUGHT_TYPE_COTTON_CANDY_MUCH,
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

#pragma region Format codes

typedef struct format_code_token {
	uint32 code;
	const char *token;
} format_code_token;

format_code_token format_code_tokens[] = {
	{ FORMAT_MOVE_X,					"MOVE_X"				},
	{ FORMAT_ADJUST_PALETTE,			"ADJUST_PALETTE"		},
	{ FORMAT_NEWLINE,					"NEWLINE"				},
	{ FORMAT_NEWLINE_SMALLER,			"NEWLINE_SMALLER"		},
	{ FORMAT_TINYFONT,					"TINYFONT"				},
	{ FORMAT_BIGFONT,					"BIGFONT"				},
	{ FORMAT_MEDIUMFONT,				"MEDIUMFONT"			},
	{ FORMAT_SMALLFONT,					"SMALLFONT"				},
	{ FORMAT_OUTLINE,					"OUTLINE"				},
	{ FORMAT_OUTLINE_OFF,				"OUTLINE_OFF"			},
	{ FORMAT_WINDOW_COLOUR_1,			"WINDOW_COLOUR_1"		},
	{ FORMAT_WINDOW_COLOUR_2,			"WINDOW_COLOUR_2"		},
	{ FORMAT_WINDOW_COLOUR_3,			"WINDOW_COLOUR_3"		},
	{ FORMAT_NEWLINE_X_Y,				"NEWLINE_X_Y"			},
	{ FORMAT_INLINE_SPRITE,				"INLINE_SPRITE"			},
	{ FORMAT_ENDQUOTES,					"ENDQUOTES"				},
	{ FORMAT_COMMA32,					"COMMA32"				},
	{ FORMAT_INT32,						"INT32"					},
	{ FORMAT_COMMA2DP32,				"COMMA2DP32"			},
	{ FORMAT_COMMA16,					"COMMA16"				},
	{ FORMAT_UINT16,					"UINT16"				},
	{ FORMAT_CURRENCY2DP,				"CURRENCY2DP"			},
	{ FORMAT_CURRENCY,					"CURRENCY"				},
	{ FORMAT_STRINGID,					"STRINGID"				},
	{ FORMAT_STRINGID2,					"STRINGID2"				},
	{ FORMAT_STRING,					"STRING"				},
	{ FORMAT_MONTHYEAR,					"MONTHYEAR"				},
	{ FORMAT_MONTH,						"MONTH"					},
	{ FORMAT_VELOCITY,					"VELOCITY"				},
	{ FORMAT_POP16,						"POP16"					},
	{ FORMAT_PUSH16,					"PUSH16"				},
	{ FORMAT_DURATION,					"DURATION"				},
	{ FORMAT_REALTIME,					"REALTIME"				},
	{ FORMAT_LENGTH,					"LENGTH"				},
	{ FORMAT_SPRITE,					"SPRITE"				},
	{ FORMAT_BLACK,						"BLACK"					},
	{ FORMAT_GREY,						"GREY"					},
	{ FORMAT_WHITE,						"WHITE"					},
	{ FORMAT_RED,						"RED"					},
	{ FORMAT_GREEN,						"GREEN"					},
	{ FORMAT_YELLOW,					"YELLOW"				},
	{ FORMAT_TOPAZ,						"TOPAZ"					},
	{ FORMAT_CELADON,					"CELADON"				},
	{ FORMAT_BABYBLUE,					"BABYBLUE"				},
	{ FORMAT_PALELAVENDER,				"PALELAVENDER"			},
	{ FORMAT_PALEGOLD,					"PALEGOLD"				},
	{ FORMAT_LIGHTPINK,					"LIGHTPINK"				},
	{ FORMAT_PEARLAQUA,					"PEARLAQUA"				},
	{ FORMAT_PALESILVER,				"PALESILVER"			},
	{ FORMAT_AMINUSCULE,				"AMINUSCULE"			},
	{ FORMAT_UP,						"UP"					},
	{ FORMAT_POUND,						"POUND"					},
	{ FORMAT_YEN,						"YEN"					},
	{ FORMAT_COPYRIGHT,					"COPYRIGHT"				},
	{ FORMAT_DOWN,						"DOWN"					},
	{ FORMAT_LEFTGUILLEMET,				"LEFTGUILLEMET"			},
	{ FORMAT_TICK,						"TICK"					},
	{ FORMAT_CROSS,						"CROSS"					},
	{ FORMAT_RIGHT,						"RIGHT"					},
	{ FORMAT_DEGREE,					"DEGREE"				},
	{ FORMAT_SQUARED,					"SQUARED"				},
	{ FORMAT_OPENQUOTES,				"OPENQUOTES"			},
	{ FORMAT_EURO,						"EURO"					},
	{ FORMAT_APPROX,					"APPROX"				},
	{ FORMAT_POWERNEGATIVEONE,			"POWERNEGATIVEONE"		},
	{ FORMAT_BULLET,					"BULLET"				},
	{ FORMAT_RIGHTGUILLEMET,			"RIGHTGUILLEMET"		},
	{ FORMAT_SMALLUP,					"SMALLUP"				},
	{ FORMAT_SMALLDOWN,					"SMALLDOWN"				},
	{ FORMAT_LEFT,						"LEFT"					},
	{ FORMAT_INVERTEDQUESTION,			"INVERTEDQUESTION"		},
	{ FORMAT_COMMA1DP16,				"COMMA1DP16"			}
};

uint32 format_get_code(const char *token)
{
	int i;
	for (i = 0; i < countof(format_code_tokens); i++)
		if (_strcmpi(token, format_code_tokens[i].token) == 0)
			return format_code_tokens[i].code;
	return 0;
}

const char *format_get_token(uint32 code)
{
	int i;
	for (i = 0; i < countof(format_code_tokens); i++)
		if (code == format_code_tokens[i].code)
			return format_code_tokens[i].token;
	return 0;
}

bool utf8_is_format_code(int codepoint)
{
	if (codepoint < 32) return true;
	if (codepoint >= FORMAT_ARGUMENT_CODE_START && codepoint <= FORMAT_ARGUMENT_CODE_END) return true;
	if (codepoint >= FORMAT_COLOUR_CODE_START && codepoint <= FORMAT_COLOUR_CODE_END) return true;
	if (codepoint == FORMAT_COMMA1DP16) return true;
	return false;
}

bool utf8_is_colour_code(int codepoint)
{
	if (codepoint >= FORMAT_COLOUR_CODE_START && codepoint <= FORMAT_COLOUR_CODE_END) return true;
	return false;
}

bool utf8_should_use_sprite_for_codepoint(int codepoint)
{
	switch (codepoint) {
	case FORMAT_UP:
	case FORMAT_DOWN:
	case FORMAT_LEFTGUILLEMET:
	case FORMAT_TICK:
	case FORMAT_CROSS:
	case FORMAT_RIGHT:
	case FORMAT_RIGHTGUILLEMET:
	case FORMAT_SMALLUP:
	case FORMAT_SMALLDOWN:
	case FORMAT_LEFT:
	case FORMAT_OPENQUOTES:
	case FORMAT_ENDQUOTES:
		return true;
	default:
		return false;
	}
}

int utf8_get_format_code_arg_length(int codepoint)
{
	switch (codepoint) {
	case FORMAT_MOVE_X:
	case FORMAT_ADJUST_PALETTE:
	case 3:
	case 4:
		return 1;
	case FORMAT_NEWLINE_X_Y:
		return 2;
	case FORMAT_INLINE_SPRITE:
		return 4;
	default:
		return 0;
	}
}

void utf8_remove_formatting(utf8* string, bool allowColours) {
	utf8* readPtr = string;
	utf8* writePtr = string;

	while (true) {
		uint32 code = utf8_get_next(readPtr, (const utf8**)&readPtr);

		if (code == 0) {
			*writePtr = 0;
			break;
		} else if (!utf8_is_format_code(code) || (allowColours && utf8_is_colour_code(code))) {
			writePtr = utf8_write_codepoint(writePtr, code);
		}
	}
}

#pragma endregion

void format_string_part_from_raw(char **dest, const char *src, char **args);
void format_string_part(char **dest, rct_string_id format, char **args);

static void format_integer(char **dest, long long value)
{
	int digit;
	char *dst = *dest;
	char *finish;
	char tmp;

	// Negative sign
	if (value < 0) {
		*dst++ = '-';
		value = -value;
	}

	*dest = dst;

	if (value == 0) {
		*dst++ = '0';
	} else {
		// Right to left
		while (value > 0) {
			digit = value % 10;
			value /= 10;

			*dst++ = '0' + digit;
		}
	}
	finish = dst;

	// Reverse string
	dst--;
	while (*dest < dst) {
		tmp = **dest;
		**dest = *dst;
		*dst = tmp;
		(*dest)++;
		dst--;
	}
	*dest = finish;
}

static void format_comma_separated_integer(char **dest, long long value)
{
	int digit, groupIndex;
	char *dst = *dest;
	char *finish;
	char tmp;
	const char *commaMark = language_get_string(STR_LOCALE_THOUSANDS_SEPARATOR);
	const char *ch;

	// Negative sign
	if (value < 0) {
		*dst++ = '-';
		value = -value;
	}

	*dest = dst;

	if (value == 0) {
		*dst++ = '0';
	} else {
		// Groups of three digits, right to left
		groupIndex = 0;
		while (value > 0) {
			// Append group separator
			if (groupIndex == 3) {
				groupIndex = 0;

				ch = commaMark;
				while (*ch != 0) {
					*dst++ = *ch++;
				}
			}

			digit = value % 10;
			value /= 10;

			*dst++ = '0' + digit;
			groupIndex++;
		}
	}
	finish = dst;

	// Reverse string
	dst--;
	while (*dest < dst) {
		tmp = **dest;
		**dest = *dst;
		*dst = tmp;
		(*dest)++;
		dst--;
	}
	*dest = finish;
}

static void format_comma_separated_fixed_1dp(char **dest, long long value)
{
	int digit, groupIndex;
	char *dst = *dest;
	char *finish;
	char tmp;
	const char *commaMark = language_get_string(STR_LOCALE_THOUSANDS_SEPARATOR);
	const char *decimalMark = language_get_string(STR_LOCALE_DECIMAL_POINT);
	const char *ch;

	// Negative sign
	if (value < 0) {
		*dst++ = '-';
		value = -value;
	}

	*dest = dst;

	// One decimal place
	digit = value % 10;
	value /= 10;
	*dst++ = '0' + digit;

	ch = decimalMark;
	while (*ch != 0) {
		*dst++ = *ch++;
	}

	if (value == 0) {
		*dst++ = '0';
	} else {
		// Groups of three digits, right to left
		groupIndex = 0;
		while (value > 0) {
			// Append group separator
			if (groupIndex == 3) {
				groupIndex = 0;

				ch = commaMark;
				while (*ch != 0) {
					*dst++ = *ch++;
				}
			}

			digit = value % 10;
			value /= 10;

			*dst++ = '0' + digit;
			groupIndex++;
		}
	}
	finish = dst;

	// Reverse string
	dst--;
	while (*dest < dst) {
		tmp = **dest;
		**dest = *dst;
		*dst = tmp;
		(*dest)++;
		dst--;
	}
	*dest = finish;
}

static void format_comma_separated_fixed_2dp(char **dest, long long value)
{
	int digit, groupIndex;
	char *dst = *dest;
	char *finish;
	char tmp;
	const char *commaMark = language_get_string(STR_LOCALE_THOUSANDS_SEPARATOR);
	const char *decimalMark = language_get_string(STR_LOCALE_DECIMAL_POINT);
	const char *ch;

	// Negative sign
	if (value < 0) {
		*dst++ = '-';
		value = -value;
	}

	*dest = dst;

	// Two decimal places
	digit = value % 10;
	value /= 10;
	*dst++ = '0' + digit;
	digit = value % 10;
	value /= 10;
	*dst++ = '0' + digit;

	ch = decimalMark;
	while (*ch != 0) {
		*dst++ = *ch++;
	}

	if (value == 0) {
		*dst++ = '0';
	} else {
		// Groups of three digits, right to left
		groupIndex = 0;
		while (value > 0) {
			// Append group separator
			if (groupIndex == 3) {
				groupIndex = 0;

				ch = commaMark;
				while (*ch != 0) {
					*dst++ = *ch++;
				}
			}

			digit = value % 10;
			value /= 10;

			*dst++ = '0' + digit;
			groupIndex++;
		}
	}
	finish = dst;

	// Reverse string
	dst--;
	while (*dest < dst) {
		tmp = **dest;
		**dest = *dst;
		*dst = tmp;
		(*dest)++;
		dst--;
	}
	*dest = finish;
}

static void format_currency(char **dest, long long value)
{
	const currency_descriptor *currencyDesc = &CurrencyDescriptors[gConfigGeneral.currency_format];

	int rate = currencyDesc->rate;
	value *= rate;

	// Negative sign
	if (value < 0) {
		// Round the value away from zero
		value = (value - 99) / 100;
		*(*dest)++ = '-';
		value = -value;
	}
	else{
		//Round the value away from zero
		value = (value + 99) / 100;
	}

	// Currency symbol
	const utf8 *symbol = currencyDesc->symbol_unicode;
	uint8 affix = currencyDesc->affix_unicode;
	if (!font_supports_string(symbol, FONT_SIZE_MEDIUM)) {
		symbol = currencyDesc->symbol_ascii;
		affix = currencyDesc->affix_ascii;
	}

	// Prefix
	if (affix == CURRENCY_PREFIX) {
		safe_strcpy(*dest, symbol, CURRENCY_SYMBOL_MAX_SIZE);
		*dest += strlen(*dest);
	}

	format_comma_separated_integer(dest, value);

	// Currency symbol suffix
	if (affix == CURRENCY_SUFFIX) {
		safe_strcpy(*dest, symbol, CURRENCY_SYMBOL_MAX_SIZE);
		*dest += strlen(*dest);
	}
}

static void format_currency_2dp(char **dest, long long value)
{
	const currency_descriptor *currencyDesc = &CurrencyDescriptors[gConfigGeneral.currency_format];

	int rate = currencyDesc->rate;
	value *= rate;

	// Negative sign
	if (value < 0) {
		*(*dest)++ = '-';
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
	if (affix == CURRENCY_PREFIX) {
		safe_strcpy(*dest, symbol, CURRENCY_SYMBOL_MAX_SIZE);
		*dest += strlen(*dest);
	}

	// Drop the pennies for "large" currencies
	if (rate >= 100) {
		format_comma_separated_integer(dest, value / 100);
	} else {
		format_comma_separated_fixed_2dp(dest, value);
	}

	// Currency symbol suffix
	if (affix == CURRENCY_SUFFIX) {
		safe_strcpy(*dest, symbol, CURRENCY_SYMBOL_MAX_SIZE);
		*dest += strlen(*dest);
	}
}

static void format_date(char **dest, uint16 value)
{
	uint16 args[] = { date_get_month(value), date_get_year(value) + 1 };
	uint16 *argsRef = args;
	format_string_part(dest, STR_DATE_FORMAT_MY, (char**)&argsRef);
	(*dest)--;
}

static void format_length(char **dest, sint16 value)
{
	rct_string_id stringId = STR_UNIT_SUFFIX_METRES;

	if (gConfigGeneral.measurement_format == MEASUREMENT_FORMAT_IMPERIAL) {
		value = metres_to_feet(value);
		stringId = STR_UNIT_SUFFIX_FEET;
	}

	sint16 *argRef = &value;
	format_string_part(dest, stringId, (char**)&argRef);
	(*dest)--;
}

static void format_velocity(char **dest, uint16 value)
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
	format_string_part(dest, stringId, (char**)&argRef);
	(*dest)--;
}

static const rct_string_id DurationFormats[][2] = {
	{STR_DURATION_SEC,      STR_DURATION_SECS},
	{STR_DURATION_MIN_SEC,  STR_DURATION_MIN_SECS},
	{STR_DURATION_MINS_SEC, STR_DURATION_MINS_SECS},
};

static void format_duration(char **dest, uint16 value)
{
	uint16 minutes = value / 60;
	uint16 seconds = value % 60;
	uint16 args[] = { minutes, seconds };
	uint16 *argsRef = &args[1];

	int minuteIndex = 0;
	if (minutes > 0) {
		minuteIndex = 1;
		if (minutes != 1) {
			minuteIndex = 2;
		}
		
		argsRef--;
	}

	int secondsIndex = 0;
	if (seconds != 1) {
		secondsIndex = 1;
	}

	rct_string_id stringId = DurationFormats[minuteIndex][secondsIndex];
	
	format_string_part(dest, stringId, (char**)&argsRef);
	(*dest)--;
}

static const rct_string_id RealtimeFormats[][2] = {
	{STR_REALTIME_MIN,       STR_REALTIME_MINS},
	{STR_REALTIME_HOUR_MIN,  STR_REALTIME_HOUR_MINS},
	{STR_REALTIME_HOURS_MIN, STR_REALTIME_HOURS_MINS},
};

static void format_realtime(char ** dest, uint16 value)
{
	uint16 hours = value / 60;
	uint16 minutes = value % 60;
	uint16 args[] = { hours, minutes };
	uint16 *argsRef = &args[1];

	int hourIndex = 0;
	if (hours > 0) {
		hourIndex = 1;
		if (hours != 1) {
			hourIndex = 2;
		}

		argsRef--;
	}

	int minuteIndex = 0;
	if (minutes != 1) {
		minuteIndex = 1;
	}

	rct_string_id stringId = RealtimeFormats[hourIndex][minuteIndex];

	format_string_part(dest, stringId, (char**)&argsRef);
	(*dest)--;
}

static void format_string_code(unsigned int format_code, char **dest, char **args)
{
	intptr_t value;

	switch (format_code) {
	case FORMAT_COMMA32:
		// Pop argument
		value = *((sint32*)*args);
		*args += 4;

		format_comma_separated_integer(dest, value);
		break;
	case FORMAT_INT32:
		// Pop argument
		value = *((sint32*)*args);
		*args += 4;

		format_integer(dest, value);
		break;
	case FORMAT_COMMA2DP32:
		// Pop argument
		value = *((sint32*)*args);
		*args += 4;

		format_comma_separated_fixed_2dp(dest, value);
		break;
		case FORMAT_COMMA1DP16:
		// Pop argument
		value = *((sint16*)*args);
		*args += 2;

		format_comma_separated_fixed_1dp(dest, value);
		break;
	case FORMAT_COMMA16:
		// Pop argument
		value = *((sint16*)*args);
		*args += 2;

		format_comma_separated_integer(dest, value);
		break;
	case FORMAT_UINT16:
		// Pop argument
		value = *((uint16*)*args);
		*args += 2;

		format_integer(dest, value);
		break;
	case FORMAT_CURRENCY2DP:
		// Pop argument
		value = *((sint32*)*args);
		*args += 4;

		format_currency_2dp(dest, value);
		break;
	case FORMAT_CURRENCY:
		// Pop argument
		value = *((sint32*)*args);
		*args += 4;

		format_currency(dest, value);
		break;
	case FORMAT_STRINGID:
	case FORMAT_STRINGID2:
		// Pop argument
		value = *((uint16*)*args);
		*args += 2;

		format_string_part(dest, (rct_string_id)value, args);
		(*dest)--;
		break;
	case FORMAT_STRING:
		// Pop argument
		value = *((uintptr_t*)*args);
		*args += sizeof(uintptr_t);

		if (value != 0) {
			strcpy(*dest, (char*)value);
			*dest += strlen(*dest);
		}
		break;
	case FORMAT_MONTHYEAR:
		// Pop argument
		value = *((uint16*)*args);
		*args += 2;

		format_date(dest, (uint16)value);
		break;
	case FORMAT_MONTH:
		// Pop argument
		value = *((uint16*)*args);
		*args += 2;

		strcpy(*dest, language_get_string(DateGameMonthNames[date_get_month((int)value)]));
		*dest += strlen(*dest);
		break;
	case FORMAT_VELOCITY:
		// Pop argument
		value = *((sint16*)*args);
		*args += 2;

		format_velocity(dest, (uint16)value);
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

		format_duration(dest, (uint16)value);
		break;
	case FORMAT_REALTIME:
		// Pop argument
		value = *((uint16*)*args);
		*args += 2;

		format_realtime(dest, (uint16)value);
		break;
	case FORMAT_LENGTH:
		// Pop argument
		value = *((sint16*)*args);
		*args += 2;

		format_length(dest, (sint16)value);
		break;
	case FORMAT_SPRITE:
		// Pop argument
		value = *((uint32*)*args);
		*args += 4;

		*(*dest)++ = 23;
		*((intptr_t*)(*dest)) = value;
		*dest += 4;
		break;
	}
}

void format_string_part_from_raw(utf8 **dest, const utf8 *src, char **args)
{
	unsigned int code;
	while (1) {
		code = utf8_get_next(src, &src);
		if (code < ' ') {
			if (code == 0) {
				*(*dest)++ = code;
				break;
			} else if (code <= 4) {
				*(*dest)++ = code;
				*(*dest)++ = *src++;
			} else if (code <= 16) {
				*(*dest)++ = code;
			} else if (code <= 22) {
				*(*dest)++ = code;
				*(*dest)++ = *src++;
				*(*dest)++ = *src++;
			} else {
				*(*dest)++ = code;
				*(*dest)++ = *src++;
				*(*dest)++ = *src++;
				*(*dest)++ = *src++;
				*(*dest)++ = *src++;
			}
		} else if (code <= 'z') {
			*(*dest)++ = code;
		} else if (code < FORMAT_COLOUR_CODE_START || code == FORMAT_COMMA1DP16) {
			format_string_code(code, dest, args);
		} else {
			*dest = utf8_write_codepoint(*dest, code);
		}
	}
}

void format_string_part(utf8 **dest, rct_string_id format, char **args)
{
	if (format == STR_NONE) {
		**dest = 0;
	} else if (format < 0x8000) {
		// Language string
		format_string_part_from_raw(dest, language_get_string(format), args);
	} else if (format < 0x9000) {
		// Custom string
		format -= 0x8000;

		// Bits 10, 11 represent number of bytes to pop off arguments
		*args += (format & 0xC00) >> 9;
		format &= ~0xC00;

		safe_strcpy(*dest, &gUserStrings[format * 32], 32);
		*dest = strchr(*dest, 0) + 1;
	} else if (format < 0xE000) {
		// Real name
		format -= -0xA000;
		sprintf(*dest, "%s %c.",
			real_names[format % countof(real_names)],
			real_name_initials[(format >> 10) % countof(real_name_initials)]
		);
		*dest = strchr(*dest, 0) + 1;

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
void format_string(utf8 *dest, rct_string_id format, void *args)
{
	format_string_part(&dest, format, (char**)&args);
}

void format_string_raw(utf8 *dest, utf8 *src, void *args)
{
	format_string_part_from_raw(&dest, src, (char**)&args);
}

/**
 * Writes a formatted string to a buffer and converts it to upper case.
 *  rct2: 0x006C2538
 * dest (edi)
 * format (ax)
 * args (ecx)
 */
void format_string_to_upper(utf8 *dest, rct_string_id format, void *args)
{
	format_string(dest, format, args);

	char *ch = dest;
	while (*ch != 0) {
		*ch = toupper(*ch);
		ch++;
	}
}

/**
 * Returns a pointer to the null terminator of the given UTF-8 string.
 */
utf8 *get_string_end(const utf8 *text)
{
	int codepoint;
	const utf8 *ch = text;

	while ((codepoint = utf8_get_next(ch, &ch)) != 0) {
		int argLength = utf8_get_format_code_arg_length(codepoint);
		ch += argLength;
	}
	return (utf8*)(ch - 1);
}

/**
 * Return the number of bytes (including the null terminator) in the given UTF-8 string.
 */
size_t get_string_size(const utf8 *text)
{
	return get_string_end(text) - text + 1;
}

/**
 * Return the number of visible characters (excludes format codes) in the given UTF-8 string.
 */
int get_string_length(const utf8 *text)
{
	int codepoint;
	const utf8 *ch = text;

	int count = 0;
	while ((codepoint = utf8_get_next(ch, &ch)) != 0) {
		if (utf8_is_format_code(codepoint)) {
			ch += utf8_get_format_code_arg_length(codepoint);
		} else {
			count++;
		}
	}
	return count;
}

utf8 *win1252_to_utf8_alloc(const char *src)
{
	size_t reservedSpace = (strlen(src) * 4) + 1;
	utf8 *result = malloc(reservedSpace);
	int actualSpace = win1252_to_utf8(result, src, reservedSpace);
	return (utf8*)realloc(result, actualSpace);
}

int win1252_to_utf8(utf8string dst, const char *src, size_t maxBufferLength)
{
	size_t srcLength = strlen(src);

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
	MultiByteToWideChar(CP_ACP, 0, src, -1, intermediateBuffer, (int)bufferCount);
	int result = WideCharToMultiByte(CP_UTF8, 0, intermediateBuffer, -1, dst, (int)maxBufferLength, NULL, NULL);

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
		int error = errno;
		switch (error)
		{
			case EINVAL:
				log_error("Unsupported conversion from %s to %s, errno = %d", from_charset, to_charset, error);
				break;
			default:
				log_error("Unknown error while initializing iconv, errno = %d", error);
		}
		return 0;
	}
	size_t obl = maxBufferLength;
	char *outBuf = dst;
	size_t conversion_result = iconv(cd, &buffer_conv, &srcLength, &outBuf, &obl);
	if (conversion_result == (size_t)-1)
	{
		int error = errno;
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
	int close_result = iconv_close(cd);
	if (close_result == -1)
	{
		log_error("Failed to close iconv, errno = %d", errno);
	}
	size_t byte_diff = maxBufferLength - obl + 1;
	dst[byte_diff - 1] = '\0';
	//log_warning("converted %s of size %d, %d", dst, byte_diff, strlen(dst));
	int result = byte_diff;
	free(buffer_orig);
#endif // __WINDOWS__

	return result;
}
