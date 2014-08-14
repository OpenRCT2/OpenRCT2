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

#include <stdio.h>
#include <string.h>
#include "addresses.h"
#include "language.h"
#include "string_ids.h"

const char *language_names[LANGUAGE_COUNT] = {
	"",					// LANGUAGE_UNDEFINED
	"English (UK)",		// LANGUAGE_ENGLISH_UK
	"English (US)",		// LANGUAGE_ENGLISH_US
	"Nederlands",		// LANGUAGE_DUTCH
	"Fran\u00e7ais"		// LANGUAGE_FRENCH
};

const char *language_filenames[LANGUAGE_COUNT] = {
	"",					// LANGUAGE_UNDEFINED
	"english_uk",		// LANGUAGE_ENGLISH_UK
	"english_us",		// LANGUAGE_ENGLISH_US
	"dutch",			// LANGUAGE_DUTCH
	"french"			// LANGUAGE_FRENCH
};

int gCurrentLanguage = LANGUAGE_UNDEFINED;

// Buffer storing all the string data
long language_buffer_size = 0;
char *language_buffer = NULL;

// List of string pointers into the string data
int language_num_strings = 0;
char **language_strings = NULL;

static int language_open_file(const char *filename);

const char *language_get_string(rct_string_id id)
{
	const char *rct = RCT2_ADDRESS(0x009BF2D4, const char*)[id];
	const char *openrct = language_strings == NULL ? NULL : language_strings[id];
	const char *str = (openrct == NULL || strlen(openrct) == 0 ? rct : openrct);
	return str == NULL ? "" : str;
}

int language_open(int id)
{
	char filename[_MAX_PATH];

	language_close();
	if (id == LANGUAGE_UNDEFINED)
		return 1;

	sprintf(filename, "data/language/%s.txt", language_filenames[id]);
	if (language_open_file(filename)) {
		gCurrentLanguage = id;
		return 1;
	}

	return 0;
}

/**
 * Partial support to open a uncompiled language file which parses tokens and converts them to the corresponding character
 * code. Due to resource strings (strings in scenarios and objects) being written to the original game's string table,
 * get_string will use those if the same entry in the loaded language is empty.
 * 
 * Unsure at how the original game decides which entries to write resource strings to, but this could affect adding new
 * strings for the time being. Further investigation is required.
 *
 * Also note that all strings are currently still ASCII. It probably can't be converted to UTF-8 until all game functions that
 * read / write strings in some way is decompiled. The original game used a DIY extended 8-bit extended ASCII set for special
 * characters, format codes and accents.
 *
 * In terms of reading the language files, the STR_XXXX part is read and XXXX becomes the string id number. Everything after the
 * colon and before the new line will be saved as the string. Tokens are written with inside curly braces {TOKEN}.
 * Use # at the beginning of a line to leave a comment.
 */
static int language_open_file(const char *filename)
{
	FILE *f = fopen(filename, "rb");
	if (f == NULL)
		return 0;

	fseek(f, 0, SEEK_END);
	language_buffer_size = ftell(f);
	language_buffer = calloc(1, language_buffer_size);
	fseek(f, 0, SEEK_SET);
	fread(language_buffer, language_buffer_size, 1, f);
	fclose(f);

	language_strings = calloc(STR_COUNT, sizeof(char*));

	char *dst, *token;
	char tokenBuffer[64];
	int i, stringIndex = 0, mode = 0, string_no;
	for (i = 0; i < language_buffer_size; i++) {
		char *src = &language_buffer[i];

		switch (mode) {
		case 0:
			// Search for a comment
			if (*src == '#') {
				mode = 3;
			} else if (*src == ':' && string_no != -1) {
				// Search for colon
				dst = src + 1;
				language_strings[string_no] = dst;
				stringIndex++;
				mode = 1;
			} else if (!strncmp(src, "STR_", 4)){
				// Copy in the string number, 4 characters only
				if (sscanf(src, "STR_%4d", &string_no) != 1) {
					string_no = -1;
				}
			}
			break;
		case 1:
			// Copy string over, stop at line break
			if (*src == '{') {
				token = src + 1;
				mode = 2;
			} else if (*src == '\n' || *src == '\r') {
				*dst = 0;
				mode = 0;
			} else {
				*dst++ = *src;
			}
			break;
		case 2:
			// Read token, convert to code
			if (*src == '}') {
				int tokenLength = min(src - token, sizeof(tokenBuffer) - 1);
				memcpy(tokenBuffer, token, tokenLength);
				tokenBuffer[tokenLength] = 0;
				char code = format_get_code(tokenBuffer);
				if (code == 0)
					code = atoi(tokenBuffer);
				*dst++ = code;
				mode = 1;
			}
			break;
		case 3:
			if (*src == '\n' || *src == '\r') {
				mode = 0;
			}
		}
	}
	language_num_strings = stringIndex;

	return 1;
}

void language_close()
{
	if (language_buffer != NULL)
		free(language_buffer);
	language_buffer_size = 0;

	if (language_strings != NULL)
		free(language_strings);
	language_num_strings = 0;

	gCurrentLanguage = LANGUAGE_UNDEFINED;
}