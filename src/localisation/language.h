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

#ifndef _LANGUAGE_H_
#define _LANGUAGE_H_

#include "../common.h"

enum {
	LANGUAGE_UNDEFINED,
	LANGUAGE_ENGLISH_UK,
	LANGUAGE_ENGLISH_US,
	LANGUAGE_GERMAN,
	LANGUAGE_DUTCH,
	LANGUAGE_FRENCH,
	LANGUAGE_HUNGARIAN,
	LANGUAGE_POLISH,
	LANGUAGE_SPANISH,
	LANGUAGE_SWEDISH,
	LANGUAGE_ITALIAN,
	LANGUAGE_PORTUGUESE_BR,
	LANGUAGE_CHINESE_TRADITIONAL,
	LANGUAGE_COUNT
};

#define FONT_OPENRCT2_SPRITE NULL

typedef struct {
	const char *locale;
	const utf8 *english_name;
	const utf8 *native_name;
	const utf8 *path;
	const utf8 *font;
	uint8 rct2_original_id;
} language_descriptor;

extern const language_descriptor LanguagesDescriptors[LANGUAGE_COUNT];

extern int gCurrentLanguage;
extern bool gUseTrueTypeFont;
extern const utf8 *gTrueTypeFontPath;

extern const utf8 BlackUpArrowString[];
extern const utf8 BlackDownArrowString[];
extern const utf8 BlackLeftArrowString[];
extern const utf8 BlackRightArrowString[];
extern const utf8 CheckBoxMarkString[];

const char *language_get_string(rct_string_id id);
int language_open(int id);
void language_close_all();

rct_string_id object_get_localised_text(uint8_t** pStringTable/*ebp*/, int type/*ecx*/, int index/*ebx*/, int tableindex/*edx*/);

uint32 utf8_get_next(const utf8 *char_ptr, const utf8 **nextchar_ptr);
utf8 *utf8_write_codepoint(utf8 *dst, uint32 codepoint);

#endif
