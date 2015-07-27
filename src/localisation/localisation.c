/*****************************************************************************
 * Copyright (c) 2014 Ted John
 * OpenRCT2, an open source clone of Roller Coaster Tycoon 2.
 *
 * This file is part of OpenRCT2.
 *
 * OpenRCT2 is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *****************************************************************************/

#include <windows.h>
#include "../addresses.h"
#include "../config.h"
#include "../game.h"
#include "../util/util.h"
#include "date.h"
#include "localisation.h"

#pragma region Format codes

typedef struct {
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
	{ FORMAT_INVERTEDQUESTION,			"INVERTEDQUESTION"		}
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
	if (codepoint >= 123 && codepoint <= 155) return true;
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

int utf8_get_sprite_offset_for_codepoint(int codepoint)
{
	switch (codepoint) {
	case FORMAT_AMINUSCULE: return 159 - 32;
	case FORMAT_POUND: return 163 - 32;
	case FORMAT_YEN: return 165 - 32;
	case FORMAT_COPYRIGHT: return 169 - 32;
	case FORMAT_LEFTGUILLEMET: return 171 - 32;
	case FORMAT_DEGREE: return 176 - 32;
	case FORMAT_SQUARED: return 178 - 32;
	case FORMAT_RIGHTGUILLEMET: return 187 - 32;
	case FORMAT_INVERTEDQUESTION: return 191 - 32;
	case FORMAT_OPENQUOTES: return 180 - 32;
	case FORMAT_ENDQUOTES: return 34 - 32;
	case FORMAT_BULLET: return 186 - 32;
	case FORMAT_POWERNEGATIVEONE: return 185 - 32;
	case FORMAT_EURO: return 181 - 32;
	case FORMAT_APPROX: return 184 - 32;
	case FORMAT_UP: return 160 - 32;
	case FORMAT_RIGHT: return 175 - 32;
	case FORMAT_DOWN: return 170 - 32;
	case FORMAT_LEFT: return 190 - 32;
	case FORMAT_SMALLUP: return 188 - 32;
	case FORMAT_SMALLDOWN: return 189 - 32;
	case FORMAT_TICK: return 172 - 32;
	case FORMAT_CROSS: return 173 - 32;
	default:
		if (codepoint > 224) codepoint = 'W';
		return codepoint - 32;
	}
}

#pragma endregion

void format_string_part_from_raw(char **dest, const char *src, char **args);
void format_string_part(char **dest, rct_string_id format, char **args);

void format_integer(char **dest, long long value)
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

void format_comma_separated_integer(char **dest, long long value)
{
	int digit, groupIndex;
	char *dst = *dest;
	char *finish;
	char tmp;
	const char *commaMark = language_get_string(5151);
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

void format_comma_separated_fixed_2dp(char **dest, long long value)
{
	int digit, groupIndex;
	char *dst = *dest;
	char *finish;
	char tmp;
	const char *commaMark = language_get_string(5151);
	const char *decimalMark = language_get_string(5152);
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

void format_currency(char **dest, long long value)
{
	const rct_currency_spec *currencySpec = &g_currency_specs[gConfigGeneral.currency_format];

	int rate = currencySpec->rate;
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
	const utf8 *symbol = currencySpec->symbol;

	// Prefix
	if (currencySpec->affix == CURRENCY_PREFIX) {
		strcpy(*dest, symbol);
		*dest += strlen(*dest);
	}

	format_comma_separated_integer(dest, value);

	// Currency symbol suffix
	if (currencySpec->affix == CURRENCY_SUFFIX) {
		strcpy(*dest, symbol);
		*dest += strlen(*dest);
	}
}

void format_currency_2dp(char **dest, long long value)
{
	const rct_currency_spec *currencySpec = &g_currency_specs[gConfigGeneral.currency_format];

	int rate = currencySpec->rate;
	value *= rate;

	// Negative sign
	if (value < 0) {
		*(*dest)++ = '-';
		value = -value;
	}

	// Currency symbol
	const utf8 *symbol = currencySpec->symbol;

	// Prefix
	if (currencySpec->affix == CURRENCY_PREFIX) {
		strcpy(*dest, symbol);
		*dest += strlen(*dest);
	}

	// Drop the pennies for "large" currencies
	if (rate > 10) {
		format_comma_separated_integer(dest, value / 100);
	} else {
		format_comma_separated_fixed_2dp(dest, value);
	}

	// Currency symbol suffix
	if (currencySpec->affix == CURRENCY_SUFFIX) {
		strcpy(*dest, symbol);
		*dest += strlen(*dest);
	}
}

void format_date(char **dest, uint16 value)
{
	uint16 args[] = { date_get_month(value), date_get_year(value) + 1 };
	uint16 *argsRef = args;
	format_string_part(dest, 2736, (char**)&argsRef);
	(*dest)--;
}

void format_length(char **dest, sint16 value)
{
	rct_string_id stringId = 2733;

	if (gConfigGeneral.measurement_format == MEASUREMENT_FORMAT_IMPERIAL) {
		value = metres_to_feet(value);
		stringId--;
	}

	uint16 *argRef = &value;
	format_string_part(dest, stringId, (char**)&argRef);
	(*dest)--;
}

void format_velocity(char **dest, uint16 value)
{
	rct_string_id stringId = 2734;

	if (gConfigGeneral.measurement_format == MEASUREMENT_FORMAT_METRIC) {
		value = mph_to_kmph(value);
		stringId++;
	}

	uint16 *argRef = &value;
	format_string_part(dest, stringId, (char**)&argRef);
	(*dest)--;
}

void format_duration(char **dest, uint16 value)
{
	uint16 minutes = value / 60;
	uint16 seconds = value % 60;
	uint16 args[] = { minutes, seconds };
	uint16 *argsRef = &args[1];
	rct_string_id stringId = 2720;

	if (minutes > 0) {
		stringId += 2;
		if (minutes != 1)
			stringId += 2;

		argsRef--;
	}

	if (seconds != 1)
		stringId++;

	format_string_part(dest, stringId, (char**)&argsRef);
	(*dest)--;
}

void format_realtime(char **dest, uint16 value)
{
	uint16 hours = value / 60;
	uint16 minutes = value % 60;
	uint16 args[] = { hours, minutes };
	uint16 *argsRef = &args[1];
	rct_string_id stringId = 2726;

	if (hours > 0) {
		stringId += 2;
		if (hours != 1)
			stringId += 2;

		argsRef--;
	}

	if (minutes != 1)
		stringId++;

	format_string_part(dest, stringId, (char**)&argsRef);
	(*dest)--;
}

void format_string_code(unsigned char format_code, char **dest, char **args)
{
	int value;

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

		format_string_part(dest, value, args);
		(*dest)--;
		break;
	case FORMAT_STRING:
		// Pop argument
		value = *((uint32*)*args);
		*args += 4;

		strcpy(*dest, (char*)value);
		*dest += strlen(*dest);
		break;
	case FORMAT_MONTHYEAR:
		// Pop argument
		value = *((uint16*)*args);
		*args += 2;

		format_date(dest, value);
		break;
	case FORMAT_MONTH:
		// Pop argument
		value = *((uint16*)*args);
		*args += 2;

		strcpy(*dest, language_get_string(STR_MONTH_MARCH + date_get_month(value)));
		*dest += strlen(*dest);
		break;
	case FORMAT_VELOCITY:
		// Pop argument
		value = *((sint16*)*args);
		*args += 2;

		format_velocity(dest, value);
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

		format_duration(dest, value);
		break;
	case FORMAT_REALTIME:
		// Pop argument
		value = *((uint16*)*args);
		*args += 2;

		format_realtime(dest, value);
		break;
	case FORMAT_LENGTH:
		// Pop argument
		value = *((sint16*)*args);
		*args += 2;

		format_length(dest, value);
		break;
	case FORMAT_SPRITE:
		// Pop argument
		value = *((uint32*)*args);
		*args += 4;

		*(*dest)++ = 23;
		*((uint32*)(*dest)) = value;
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
		} else if (code < 142) {
			format_string_code(code, dest, args);
		} else {
			*dest = utf8_write_codepoint(*dest, code);
		}
	}
}

void format_string_part(utf8 **dest, rct_string_id format, char **args)
{
	if (format == (rct_string_id)STR_NONE) {
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

		strcpy(*dest, RCT2_ADDRESS(0x135A8F4 + (format * 32), char));
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
		RCT2_CALLPROC_EBPSAFE(RCT2_ADDRESS(0x0095AFB8, uint32)[format]);
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
 *  rct2: 0x006E37F7
 *  error  (eax)
 *  format (bx)
 */
void error_string_quit(int error, rct_string_id format){
	RCT2_GLOBAL(0x14241A0, uint32) = error;
	RCT2_GLOBAL(0x9E2DA0, uint32) = 1;

	char* error_string = RCT2_ADDRESS(0x1424080, char);
	void* args = RCT2_ADDRESS(0x13CE952, void);
	*error_string = 0;

	if (format != 0xFFFF){
		format_string(error_string, format, args);
	}
	RCT2_GLOBAL(0x9E2D9C, uint32) = 1;
	rct2_exit();
	rct2_endupdate();
}

void generate_string_file()
{
	FILE* f;
	uint8** str;
	uint8* c;
	int i;

	f = fopen("english.txt", "w");

	for (i = 0; i < 4442; i++) {
		str = ((uint8**)(0x009BF2D4 + (i * 4)));
		if (*str == (uint8*)0xFFFFFFFF)
			continue;
		c = *str;

		fprintf(f, "STR_%04d    :", i);
		while (*c != '\0') {
			const char *token = format_get_token(*c);
			if (token != NULL) {
				fprintf(f, "{%s}", token);
			} else {
				if (*c < 32 || *c > 127)
					fprintf(f, "{%d}", *c);
				else
					fputc(*c, f);
			}
			c++;
		}
		fputc('\n', f);
	}

	fclose(f);
}

utf8 *get_string_end(utf8 *text)
{
	int codepoint;
	const utf8 *ch = text;

	while ((codepoint = utf8_get_next(ch, &ch)) != 0) {
		if (utf8_is_format_code(codepoint)) {
			switch (codepoint) {
			case FORMAT_MOVE_X:
			case FORMAT_ADJUST_PALETTE:
			case 3:
			case 4:
				ch++;
				break;
			case FORMAT_INLINE_SPRITE:
				ch += 4;
				break;
			default:
				if (codepoint <= 22) {
					ch += 2;
				} else {
					ch += 4;
				}
				break;
			}
		}
	}
	return ch - 1;
}

/**
*  Return the length of the string in buffer.
*  note you can't use strlen as there can be inline sprites!
*
* buffer (esi)
*/
int get_string_length(const utf8* text)
{
	int codepoint;
	const utf8 *ch = text;

	while ((codepoint = utf8_get_next(ch, &ch)) != 0) {
		if (utf8_is_format_code(codepoint)) {
			switch (codepoint) {
			case FORMAT_MOVE_X:
			case FORMAT_ADJUST_PALETTE:
			case 3:
			case 4:
				ch++;
				break;
			case FORMAT_INLINE_SPRITE:
				ch += 4;
				break;
			default:
				if (codepoint <= 22) {
					ch += 2;
				} else {
					ch += 4;
				}
				break;
			}
		}
	}
	return ch - text - 1;
}

utf8 *win1252_to_utf8_alloc(const char *src)
{
	int reservedSpace = (strlen(src) * 4) + 1;
	utf8 *result = malloc(reservedSpace);
	int actualSpace = win1252_to_utf8(result, src, reservedSpace);
	return (utf8*)realloc(result, actualSpace);
}

int win1252_to_utf8(utf8string dst, const char *src, int maxBufferLength)
{
	utf16 stackBuffer[256];
	utf16 *heapBuffer = NULL;
	utf16 *intermediateBuffer = stackBuffer;
	int bufferCount = countof(stackBuffer);

	if (maxBufferLength > bufferCount) {
		int srcLength = strlen(src);
		if (srcLength > bufferCount) {
			bufferCount = srcLength + 4;
			heapBuffer = malloc(bufferCount * sizeof(utf16));
			intermediateBuffer = heapBuffer;
		}
	}

	MultiByteToWideChar(CP_ACP, 0, src, -1, intermediateBuffer, bufferCount);
	int result = WideCharToMultiByte(CP_UTF8, 0, intermediateBuffer, -1, dst, maxBufferLength, NULL, NULL);

	if (heapBuffer != NULL) {
		free(heapBuffer);
	}

	return result;
}
