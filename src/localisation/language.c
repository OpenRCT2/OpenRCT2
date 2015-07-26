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

#include "../addresses.h"
#include "../drawing/drawing.h"
#include "../object.h"
#include "../openrct2.h"
#include "../util/util.h"
#include "localisation.h"

typedef struct {
	int id;
	int num_strings;
	char **strings;
	size_t string_data_size;
	char *string_data;
} language_data;

const char *language_names[LANGUAGE_COUNT] = {
	"",						// LANGUAGE_UNDEFINED
	"English (UK)",			// LANGUAGE_ENGLISH_UK
	"English (US)",			// LANGUAGE_ENGLISH_US
	"Deutsch",				// LANGUAGE_GERMAN
	"Nederlands",			// LANGUAGE_DUTCH
	"Fran\u00E7ais",		// LANGUAGE_FRENCH
	"Magyar",				// LANGUAGE_HUNGARIAN
	"Polski",				// LANGUAGE_POLISH
	"Espa\u00F1ol",			// LANGUAGE_SPANISH
	"Svenska",				// LANGUAGE_SWEDISH
	"Italiano",				// LANGUAGE_ITALIAN
	"Portug\u00CAs (BR)",	// LANGUAGE_PORTUGUESE_BR
	"Chinese Traditional"	// LANGUAGE_CHINESE_TRADITIONAL
};

const char *language_filenames[LANGUAGE_COUNT] = {
	"",						// LANGUAGE_UNDEFINED
	"english_uk",			// LANGUAGE_ENGLISH_UK
	"english_us",			// LANGUAGE_ENGLISH_US
	"german", 				// LANGUAGE_GERMAN
	"dutch",				// LANGUAGE_DUTCH
	"french",				// LANGUAGE_FRENCH
	"hungarian",			// LANGUAGE_HUNGARIAN
	"polish",				// LANGUAGE_POLISH
	"spanish_sp",			// LANGUAGE_SPANISH
	"swedish",				// LANGUAGE_SWEDISH
	"italian",				// LANGUAGE_ITALIAN
	"portuguese_br",		// LANGUAGE_PORTUGUESE_BR
	"chinese_traditional"	// LANGUAGE_CHINESE_TRADITIONAL
};

int gCurrentLanguage = LANGUAGE_UNDEFINED;

language_data _languageFallback = { 0 };
language_data _languageCurrent = { 0 };

const char **_languageOriginal = (char**)0x009BF2D4;

static int language_open_file(const char *filename, language_data *language);
static void language_close(language_data *language);

uint32 utf8_get_next(const utf8 *char_ptr, const utf8 **nextchar_ptr)
{
	int result;
	int numBytes;

	if (!(char_ptr[0] & 0x80)) {
		result = char_ptr[0];
		numBytes = 1;
	} else if ((char_ptr[0] & 0xE0) == 0xC0) {
		result = ((char_ptr[0] & 0x1F) << 6) | (char_ptr[1] & 0x3F);
		numBytes = 2;
	} else if ((char_ptr[0] & 0xF0) == 0xE0) {
		result = ((char_ptr[0] & 0x0F) << 12) | ((char_ptr[1] & 0x3F) << 6) | (char_ptr[2] & 0x3F);
		numBytes = 3;
	} else {
		// TODO 4 bytes
		result = ' ';
		numBytes = 1;
	}

	if (nextchar_ptr != NULL)
		*nextchar_ptr = char_ptr + numBytes;
	return result;
}

utf8 *utf8_write_codepoint(utf8 *dst, uint32 codepoint)
{
	if (codepoint <= 0x7F) {
		dst[0] = (utf8)codepoint;
		return dst + 1;
	} else if (codepoint <= 0x7FF) {
		dst[0] = 0xC0 | ((codepoint >> 6) & 0x1F);
		dst[1] = 0x80 | (codepoint & 0x3F);
		return dst + 2;
	} else if (codepoint <= 0xFFFF) {
		dst[0] = 0xE0 | ((codepoint >> 12) & 0x0F);
		dst[1] = 0x80 | ((codepoint >> 6) & 0x3F);
		dst[2] = 0x80 | (codepoint & 0x3F);
		return dst + 3;
	} else {
		dst[0] = 0xF0 | ((codepoint >> 18) & 0x07);
		dst[1] = 0x80 | ((codepoint >> 12) & 0x3F);
		dst[2] = 0x80 | ((codepoint >> 6) & 0x3F);
		dst[3] = 0x80 | (codepoint & 0x3F);
		return dst + 4;
	}
}

const char *language_get_string(rct_string_id id)
{
	const char *openrctString = NULL;

	if (id == (rct_string_id)STR_NONE)
		return NULL;

	if (_languageCurrent.num_strings > id)
		openrctString = _languageCurrent.strings[id];
	else if (_languageFallback.num_strings > id)
		openrctString = _languageFallback.strings[id];

	if (id >= STR_OPENRCT2_BEGIN_STRING_ID) {
		return openrctString != NULL ? openrctString : "(undefined string)";
	} else {
		const char *rct = _languageOriginal[id];
		const char *str = (openrctString == NULL || strlen(openrctString) == 0 ? rct : openrctString);
		return str == NULL ? "" : str;
	}
}

int language_open(int id)
{
	static const char *languagePath = "%s/data/language/%s.txt";
	char filename[MAX_PATH];

	language_close_all();
	if (id == LANGUAGE_UNDEFINED)
		return 1;

	if (id != LANGUAGE_ENGLISH_UK) {
		sprintf(filename, languagePath, gExePath, language_filenames[LANGUAGE_ENGLISH_UK]);
		if (language_open_file(filename, &_languageFallback)) {
			_languageFallback.id = LANGUAGE_ENGLISH_UK;
		}
	}

	sprintf(filename, languagePath, gExePath, language_filenames[id]);
	if (language_open_file(filename, &_languageCurrent)) {
		_languageCurrent.id = id;
		gCurrentLanguage = id;

		if (!ttf_initialise()) {
			log_warning("Unable to initialise TrueType fonts.");
		}

		return 1;
	}

	return 0;
}

void language_close_all()
{
	language_close(&_languageFallback);
	language_close(&_languageCurrent);
	_languageFallback.id = LANGUAGE_UNDEFINED;
	_languageCurrent.id = LANGUAGE_UNDEFINED;
	gCurrentLanguage = LANGUAGE_UNDEFINED;
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
static int language_open_file(const char *filename, language_data *language)
{
	assert(filename != NULL);
	assert(language != NULL);

	FILE *f = fopen(filename, "rb");
	if (f == NULL)
		return 0;

	fseek(f, 0, SEEK_END);
	language->string_data_size = ftell(f) + 1;
	language->string_data = calloc(1, language->string_data_size);
	fseek(f, 0, SEEK_SET);
	fread(language->string_data, language->string_data_size, 1, f);
	fclose(f);

	language->strings = calloc(STR_COUNT, sizeof(char*));

	char *dst = NULL;
	char *token = NULL;
	char tokenBuffer[64];
	int stringIndex = 0, mode = 0, stringId, maxStringId = 0;
	size_t i = 0;

	// Skim UTF-8 byte order mark
	if (utf8_is_bom(language->string_data))
		i += 3;

	for (; i < language->string_data_size; i++) {
		char *src = &language->string_data[i];

		// Handle UTF-8
		char *srcNext;
		uint32 utf8Char = utf8_get_next(src, &srcNext);
		i += srcNext - src - 1;

		switch (mode) {
		case 0:
			// Search for a comment
			if (utf8Char == '#') {
				mode = 3;
			} else if (utf8Char == ':' && stringId != -1) {
				// Search for colon
				dst = src + 1;
				language->strings[stringId] = dst;
				stringIndex++;
				mode = 1;
			} else if (!strncmp(src, "STR_", 4)){
				// Copy in the string number, 4 characters only
				if (sscanf(src, "STR_%4d", &stringId) != 1) {
					stringId = -1;
				} else {
					maxStringId = max(maxStringId, stringId);
				}
			}
			break;
		case 1:
			// Copy string over, stop at line break
			if (utf8Char == '{') {
				token = src + 1;
				mode = 2;
			} else if (utf8Char == '\n' || *src == '\r') {
				*dst = 0;
				mode = 0;
			} else {
				dst = utf8_write_codepoint(dst, utf8Char);
			}
			break;
		case 2:
			// Read token, convert to code
			if (utf8Char == '}') {
				int tokenLength = min(src - token, sizeof(tokenBuffer) - 1);
				memcpy(tokenBuffer, token, tokenLength);
				tokenBuffer[tokenLength] = 0;
				uint8 code = (uint8)format_get_code(tokenBuffer);
				if (code == 0)
					code = atoi(tokenBuffer);
				dst = utf8_write_codepoint(dst, code);
				mode = 1;
			}
			break;
		case 3:
			if (utf8Char == '\n' || utf8Char == '\r') {
				mode = 0;
			}
		}
	}
	language->num_strings = maxStringId + 1;
	language->strings = realloc(language->strings, language->num_strings * sizeof(char*));

	return 1;
}

static void language_close(language_data *language)
{
	SafeFree(language->strings);
	SafeFree(language->string_data);
	language->num_strings = 0;
	language->string_data_size = 0;
}

const int OpenRCT2LangIdToObjectLangId[] = {
	0, 0, 1, 3, 6, 2, 0, 0, 4, 7, 5, 13
};

/* rct2: 0x0098DA16 */
uint16 ObjectTypeStringTableCount[] = { 3, 1, 1, 1, 1, 1, 1, 1, 1, 1, 3 };

/* rct2: 0x006A9E24*/
rct_string_id object_get_localised_text(uint8_t** pStringTable/*ebp*/, int type/*ecx*/, int index/*ebx*/, int tableindex/*edx*/)
{
	char* pString = NULL;
	int result = 0;
	while (true)
	{
		uint8_t language_code = *(*pStringTable)++;
		
		if (language_code == 0xFF) //end of string table
			break;

		// This is the ideal situation. Language found
		if (language_code == OpenRCT2LangIdToObjectLangId[gCurrentLanguage])//1)
		{
			pString = *pStringTable;
			result |= 1;
		}

		// Just in case always load english into pString
		if (language_code == 0 && !(result & 1))
		{
			pString = *pStringTable;
			result |= 2;
		}

		// Failing that fall back to whatever is first string
		if (!(result & 7))
		{
			pString = *pStringTable;
			result |= 4;
		}

		// Skip over the actual string entry to get to the next
		// entry
		while (*(*pStringTable)++ != 0);
	}

	// If not scenario text
	if (RCT2_GLOBAL(0x9ADAFC, uint8_t) == 0)
	{
		int stringid = 3463;
		for (int i = 0; i < type; i++)
		{
			int nrobjects = object_entry_group_counts[i];
			int nrstringtables = ObjectTypeStringTableCount[i];
			stringid += nrobjects * nrstringtables;
		}
		stringid += index * ObjectTypeStringTableCount[type];
		// Used by the object list to allocate name in plugin.dat
		RCT2_GLOBAL(RCT2_ADDRESS_CURR_OBJECT_BASE_STRING_ID, uint32) = stringid;
		stringid += tableindex;

		//put pointer in stringtable
		if (_languageCurrent.num_strings > stringid)
			_languageCurrent.strings[stringid] = pString;
		// Until all string related functions are finished copy
		// to old array as well.
		_languageOriginal[stringid] = pString;
		return stringid;
	}
	else
	{
		int stringid = 3447 + tableindex;
		//put pointer in stringtable
		if (_languageCurrent.num_strings > stringid)
			_languageCurrent.strings[stringid] = pString;
		// Until all string related functions are finished copy
		// to old array as well.
		_languageOriginal[stringid] = pString;
		return stringid;
	}
}
