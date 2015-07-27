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

enum {
	RCT2_LANGUAGE_ID_ENGLISH_UK,
	RCT2_LANGUAGE_ID_ENGLISH_US,
	RCT2_LANGUAGE_ID_FRENCH,
	RCT2_LANGUAGE_ID_GERMAN,
	RCT2_LANGUAGE_ID_SPANISH,
	RCT2_LANGUAGE_ID_ITALIAN,
	RCT2_LANGUAGE_ID_DUTCH,
	RCT2_LANGUAGE_ID_SWEDISH,
	RCT2_LANGUAGE_ID_8,
	RCT2_LANGUAGE_ID_KOREAN,
	RCT2_LANGUAGE_ID_CHINESE_TRADITIONAL,
	RCT2_LANGUAGE_ID_CHINESE_SIMPLIFIED,
	RCT2_LANGUAGE_ID_12,
	RCT2_LANGUAGE_ID_PORTUGESE,
	RCT2_LANGUAGE_ID_END = 255
};

const language_descriptor LanguagesDescriptors[LANGUAGE_COUNT] = {
	{ "",			 "",						 "",						"",							FONT_OPENRCT2_SPRITE,	RCT2_LANGUAGE_ID_ENGLISH_UK				},	// LANGUAGE_UNDEFINED
	{ "en-GB",		"English (UK)",				"English (UK)",				"english_uk",				FONT_OPENRCT2_SPRITE,	RCT2_LANGUAGE_ID_ENGLISH_UK				},	// LANGUAGE_ENGLISH_UK
	{ "en-US",		"English (US)",				"English (US)",				"english_us",				FONT_OPENRCT2_SPRITE,	RCT2_LANGUAGE_ID_ENGLISH_US				},	// LANGUAGE_ENGLISH_US
	{ "de-DE",		"German",					"Deutsch",					"german",					FONT_OPENRCT2_SPRITE,	RCT2_LANGUAGE_ID_GERMAN					},	// LANGUAGE_GERMAN
	{ "nl-NL",		"Dutch",					"Nederlands",				"dutch",					FONT_OPENRCT2_SPRITE,	RCT2_LANGUAGE_ID_DUTCH					},	// LANGUAGE_DUTCH
	{ "fr-FR",		"French",					"Fran\xC3\xA7" "ais",		"french",					FONT_OPENRCT2_SPRITE,	RCT2_LANGUAGE_ID_FRENCH					},	// LANGUAGE_FRENCH
	{ "hu-HU",		"Hungarian",				"Magyar",					"hungarian",				FONT_OPENRCT2_SPRITE,	RCT2_LANGUAGE_ID_ENGLISH_UK				},	// LANGUAGE_HUNGARIAN
	{ "pl-PL",		"Polish",					"Polski",					"polish",					FONT_OPENRCT2_SPRITE,	RCT2_LANGUAGE_ID_ENGLISH_UK				},	// LANGUAGE_POLISH
	{ "es-ES",		"Spanish",					"Espa\xC3\xB1ol",			"spanish_sp",				FONT_OPENRCT2_SPRITE,	RCT2_LANGUAGE_ID_SPANISH				},	// LANGUAGE_SPANISH
	{ "sv-SE",		"Swedish",					"Svenska",					"swedish",					FONT_OPENRCT2_SPRITE,	RCT2_LANGUAGE_ID_SWEDISH				},	// LANGUAGE_SWEDISH
	{ "it-IT",		"Italian",					"Italiano",					"italian",					FONT_OPENRCT2_SPRITE,	RCT2_LANGUAGE_ID_ITALIAN				},	// LANGUAGE_ITALIAN
	{ "pt-BR",		"Portuguese (BR)",			"Portug\xC3\xAAs (BR)",		"portuguese_br",			FONT_OPENRCT2_SPRITE,	RCT2_LANGUAGE_ID_PORTUGESE				},	// LANGUAGE_PORTUGUESE_BR
	{ "zh-Hant",	"Chinese (Traditional)",	"Chinese (Traditional)",	"chinese_traditional",		"msjh.ttc",				RCT2_LANGUAGE_ID_CHINESE_TRADITIONAL	},	// LANGUAGE_CHINESE_TRADITIONAL
};

int gCurrentLanguage = LANGUAGE_UNDEFINED;
bool gUseTrueTypeFont = false;
const utf8 *gTrueTypeFontPath;

language_data _languageFallback = { 0 };
language_data _languageCurrent = { 0 };

const char **_languageOriginal = (char**)0x009BF2D4;

const utf8 BlackUpArrowString[] = { 0xC2, 0x8E, 0xE2, 0x96, 0xB2, 0x00 };
const utf8 BlackDownArrowString[] = { 0xC2, 0x8E, 0xE2, 0x96, 0xBC, 0x00 };
const utf8 BlackLeftArrowString[] = { 0xC2, 0x8E, 0xE2, 0x97, 0x80, 0x00 };
const utf8 BlackRightArrowString[] = { 0xC2, 0x8E, 0xE2, 0x96, 0xB6, 0x00 };
const utf8 CheckBoxMarkString[] = { 0xE2, 0x9C, 0x93, 0x00 };

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
		sprintf(filename, languagePath, gExePath, LanguagesDescriptors[LANGUAGE_ENGLISH_UK].path);
		if (language_open_file(filename, &_languageFallback)) {
			_languageFallback.id = LANGUAGE_ENGLISH_UK;
		}
	}

	sprintf(filename, languagePath, gExePath, LanguagesDescriptors[id].path);
	if (language_open_file(filename, &_languageCurrent)) {
		_languageCurrent.id = id;
		gCurrentLanguage = id;

		if (LanguagesDescriptors[id].font == FONT_OPENRCT2_SPRITE) {
			gUseTrueTypeFont = false;
			gTrueTypeFontPath = NULL;
			ttf_dispose();
		} else {
			gUseTrueTypeFont = true;
			gTrueTypeFontPath = LanguagesDescriptors[id].font;
			if (!ttf_initialise()) {
				log_warning("Unable to initialise TrueType fonts.");
			}
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
				uint32 code = format_get_code(tokenBuffer);
				if (code == 0) {
					code = atoi(tokenBuffer);
					*dst++ = code & 0xFF;
				} else {
					dst = utf8_write_codepoint(dst, code);
				}
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
	0,
	0,
	1,
	3,
	6,
	2,
	0,
	0,
	4,
	7,
	5,
	13
};

#define STEX_BASE_STRING_ID			3447
#define NONSTEX_BASE_STRING_ID		3463
#define MAX_OBJECT_CACHED_STRINGS	2048

/* rct2: 0x0098DA16 */
uint16 ObjectTypeStringTableCount[] = { 3, 1, 1, 1, 1, 1, 1, 1, 1, 1, 3 };

utf8 *_cachedObjectStrings[MAX_OBJECT_CACHED_STRINGS] = { NULL };

void utf8_trim_string(utf8 *text)
{
	utf8 *src = text;
	utf8 *dst = text;
	utf8 *last = text;
	int codepoint;

	// Trim left
	while ((codepoint = utf8_get_next(src, &src)) != 0) {
		if (codepoint != ' ') {
			dst = utf8_write_codepoint(dst, codepoint);
			last = dst;
			break;
		}
	}
	if (codepoint != 0) {
		// Trim right
		while ((codepoint = utf8_get_next(src, &src)) != 0) {
			dst = utf8_write_codepoint(dst, codepoint);
			if (codepoint != ' ') {
				last = dst;
			}
		}
	}
	*last = 0;
}

static utf8 *convert_multibyte_charset(const char *src)
{
	int reservedLength = (strlen(src) * 4) + 1;
	utf8 *buffer = malloc(reservedLength);
	utf8 *dst = buffer;
	for (const uint8 *ch = src; *ch != 0;) {
		if (*ch == 0xFF) {
			ch++;
			uint8 a = *ch++;
			uint8 b = *ch++;
			uint16 codepoint = (a << 8) | b;
			dst = utf8_write_codepoint(dst, codepoint);
		} else {
			*dst++ = *ch++;
		}
	}
	*dst++ = 0;
	int actualLength = dst - buffer;
	return realloc(buffer, actualLength);
}

static bool rct2_language_is_multibyte_charset(int languageId)
{
	switch (languageId) {
	case RCT2_LANGUAGE_ID_KOREAN:
	case RCT2_LANGUAGE_ID_CHINESE_TRADITIONAL:
	case RCT2_LANGUAGE_ID_CHINESE_SIMPLIFIED:
		return true;
	default:
		return false;
	}
}

/* rct2: 0x006A9E24*/
rct_string_id object_get_localised_text(uint8_t** pStringTable/*ebp*/, int type/*ecx*/, int index/*ebx*/, int tableindex/*edx*/)
{
	uint8 languageId, chosenLanguageId;
	char *pString = NULL;
	int result = 0;
	bool isBlank;
	
	while ((languageId = *(*pStringTable)++) != RCT2_LANGUAGE_ID_END) {
		isBlank = true;

		// Strings that are just ' ' are set as invalid langauges.
		// But if there is no real string then it will set the string as
		// the blank string
		for (char *ch = *pStringTable; *ch != 0; ch++) {
			if (!isblank(*ch)) {
				isBlank = false;
				break;
			}
		}

		if (isBlank) languageId = 0xFE;

		// This is the ideal situation. Language found
		if (languageId == LanguagesDescriptors[gCurrentLanguage].rct2_original_id) {
			chosenLanguageId = languageId;
			pString = *pStringTable;
			result |= 1;
		}

		// Just in case always load english into pString
		if (languageId == RCT2_LANGUAGE_ID_ENGLISH_UK && !(result & 1)) {
			chosenLanguageId = languageId;
			pString = *pStringTable;
			result |= 2;
		}

		// Failing that fall back to whatever is first string
		if (!(result & 7)) {
			chosenLanguageId = languageId;
			pString = *pStringTable;
			if (!isBlank) result |= 4;
		}

		// Skip over the actual string entry to get to the next entry
		while (*(*pStringTable)++ != 0);
	}

	// If not scenario text
	if (RCT2_GLOBAL(0x009ADAFC, uint8) == 0) {
		int stringid = NONSTEX_BASE_STRING_ID;
		for (int i = 0; i < type; i++) {
			int nrobjects = object_entry_group_counts[i];
			int nrstringtables = ObjectTypeStringTableCount[i];
			stringid += nrobjects * nrstringtables;
		}
		stringid += index * ObjectTypeStringTableCount[type];
		// Used by the object list to allocate name in plugin.dat
		RCT2_GLOBAL(RCT2_ADDRESS_CURR_OBJECT_BASE_STRING_ID, uint32) = stringid;
		stringid += tableindex;

		// cache UTF-8 string
		int cacheStringOffset = stringid - STEX_BASE_STRING_ID;
		utf8 **cacheString = &_cachedObjectStrings[cacheStringOffset];
		if (*cacheString != NULL) {
			free(*cacheString);
		}
		if (rct2_language_is_multibyte_charset(chosenLanguageId)) {
			*cacheString = convert_multibyte_charset(pString);
		} else {
			*cacheString = win1252_to_utf8_alloc(pString);
		}
		utf8_trim_string(*cacheString);

		//put pointer in stringtable
		if (_languageCurrent.num_strings > stringid)
			_languageCurrent.strings[stringid] = *cacheString;
		// Until all string related functions are finished copy
		// to old array as well.
		_languageOriginal[stringid] = *cacheString;
		return stringid;
	} else {
		int stringid = STEX_BASE_STRING_ID + tableindex;

		// cache UTF-8 string
		int cacheStringOffset = stringid - STEX_BASE_STRING_ID;
		utf8 **cacheString = &_cachedObjectStrings[cacheStringOffset];
		if (*cacheString != NULL) {
			free(*cacheString);
		}
		if (rct2_language_is_multibyte_charset(chosenLanguageId)) {
			*cacheString = convert_multibyte_charset(pString);
		} else {
			*cacheString = win1252_to_utf8_alloc(pString);
		}
		utf8_trim_string(*cacheString);

		//put pointer in stringtable
		if (_languageCurrent.num_strings > stringid)
			_languageCurrent.strings[stringid] = *cacheString;
		// Until all string related functions are finished copy
		// to old array as well.
		_languageOriginal[stringid] = *cacheString;
		return stringid;
	}
}
