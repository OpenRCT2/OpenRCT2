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

#include "LanguagePack.h"

extern "C" {

#include "../addresses.h"
#include "../drawing/drawing.h"
#include "../object.h"
#include "../openrct2.h"
#include "../util/util.h"
#include "localisation.h"

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
	RCT2_LANGUAGE_ID_CHINESE_SIMPLIFIED,
	RCT2_LANGUAGE_ID_CHINESE_TRADITIONAL,
	RCT2_LANGUAGE_ID_12,
	RCT2_LANGUAGE_ID_PORTUGESE,
	RCT2_LANGUAGE_ID_END = 255
};

static TTFFontSetDescriptor TTFFontMingLiu = {{
	{ "msjh.ttc",		9,		-1,		-3,		6,		NULL },
	{ "mingliu.ttc",	11,		1,		1,		12,		NULL },
	{ "mingliu.ttc",	12,		1,		0,		12,		NULL },
	{ "mingliu.ttc",	13,		1,		0,		20,		NULL },
}};

static TTFFontSetDescriptor TTFFontSimSun = {{
	{ "msyh.ttc",		9,		-1,		-3,		6,		NULL },
	{ "simsun.ttc",		11,		1,		-1,		14,		NULL },
	{ "simsun.ttc",		12,		1,		-2,		14,		NULL },
	{ "simsun.ttc",		13,		1,		0,		20,		NULL },
}};

static TTFFontSetDescriptor TTFFontMalgun = { {
	{ "malgun.ttf",		8,		-1,		-3,		6,		NULL },
	{ "malgun.ttf",		11,		1,		-2,		14,		NULL },
	{ "malgun.ttf",		12,		1,		-4,		14,		NULL },
	{ "malgun.ttf",		13,		1,		0,		20,		NULL },
} };

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
	{ "zh-Hant",	"Chinese (Traditional)",	"Chinese (Traditional)",	"chinese_traditional",		&TTFFontMingLiu,		RCT2_LANGUAGE_ID_CHINESE_TRADITIONAL	},	// LANGUAGE_CHINESE_TRADITIONAL
	{ "zh-Hans",	"Chinese (Simplified)",		"Chinese (Simplified)",		"chinese_simplified",		&TTFFontSimSun,			RCT2_LANGUAGE_ID_CHINESE_SIMPLIFIED		},	// LANGUAGE_CHINESE_SIMPLIFIED
	{ "fi-FI",		"Finnish",					"Suomi",					"finnish",					FONT_OPENRCT2_SPRITE,	RCT2_LANGUAGE_ID_ENGLISH_UK				},	// LANGUAGE_FINNISH
	{ "kr-KR",		"Korean",					"Korean",					"korean",					&TTFFontMalgun,			RCT2_LANGUAGE_ID_KOREAN					},	// LANGUAGE_KOREAN
};

int gCurrentLanguage = LANGUAGE_UNDEFINED;
bool gUseTrueTypeFont = false;

LanguagePack *_languageFallback = nullptr;
LanguagePack *_languageCurrent = nullptr;

const char **_languageOriginal = (const char**)0x009BF2D4;

const utf8 BlackUpArrowString[] =		{ (utf8)0xC2, (utf8)0x8E, (utf8)0xE2, (utf8)0x96, (utf8)0xB2, (utf8)0x00 };
const utf8 BlackDownArrowString[] =		{ (utf8)0xC2, (utf8)0x8E, (utf8)0xE2, (utf8)0x96, (utf8)0xBC, (utf8)0x00 };
const utf8 BlackLeftArrowString[] =		{ (utf8)0xC2, (utf8)0x8E, (utf8)0xE2, (utf8)0x97, (utf8)0x80, (utf8)0x00 };
const utf8 BlackRightArrowString[] =	{ (utf8)0xC2, (utf8)0x8E, (utf8)0xE2, (utf8)0x96, (utf8)0xB6, (utf8)0x00 };
const utf8 CheckBoxMarkString[] =		{ (utf8)0xE2, (utf8)0x9C, (utf8)0x93, (utf8)0x00 };

void utf8_remove_format_codes(utf8 *text)
{
	utf8 *dstCh = text;
	utf8 *ch = text;
	int codepoint;
	while ((codepoint = utf8_get_next(ch, (const utf8**)&ch)) != 0) {
		if (!utf8_is_format_code(codepoint)) {
			dstCh = utf8_write_codepoint(dstCh, codepoint);
		}
	}
	*dstCh = 0;
}

const char *language_get_string(rct_string_id id)
{
	const char *openrctString = NULL;

	if (id == (rct_string_id)STR_NONE)
		return NULL;

	if (_languageCurrent != nullptr)
		openrctString = _languageCurrent->GetString(id);
	if (openrctString == NULL && _languageFallback != nullptr)
		openrctString = _languageFallback->GetString(id);

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
		_languageFallback = LanguagePack::FromFile(LANGUAGE_ENGLISH_UK, filename);
	}

	sprintf(filename, languagePath, gExePath, LanguagesDescriptors[id].path);
	_languageCurrent = LanguagePack::FromFile(id, filename);
	if (_languageCurrent != NULL) {
		gCurrentLanguage = id;

		if (LanguagesDescriptors[id].font == FONT_OPENRCT2_SPRITE) {
			ttf_dispose();
			gUseTrueTypeFont = false;
			gCurrentTTFFontSet = NULL;
		} else {
			ttf_dispose();
			gUseTrueTypeFont = true;
			gCurrentTTFFontSet = LanguagesDescriptors[id].font;
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
	SafeDelete(_languageFallback);
	SafeDelete(_languageCurrent);
	gCurrentLanguage = LANGUAGE_UNDEFINED;
}

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
	while ((codepoint = utf8_get_next(src, (const utf8**)&src)) != 0) {
		if (codepoint != ' ') {
			dst = utf8_write_codepoint(dst, codepoint);
			last = dst;
			break;
		}
	}
	if (codepoint != 0) {
		// Trim right
		while ((codepoint = utf8_get_next(src, (const utf8**)&src)) != 0) {
			dst = utf8_write_codepoint(dst, codepoint);
			if (codepoint != ' ') {
				last = dst;
			}
		}
	}
	*last = 0;
}

static wchar_t convert_specific_language_character_to_unicode(int languageId, wchar_t codepoint)
{
	switch (languageId) {
	case RCT2_LANGUAGE_ID_KOREAN:
		return codepoint;
	case RCT2_LANGUAGE_ID_CHINESE_TRADITIONAL:
		return encoding_convert_big5_to_unicode(codepoint);
	case RCT2_LANGUAGE_ID_CHINESE_SIMPLIFIED:
		return encoding_convert_gb2312_to_unicode(codepoint);
	default:
		return codepoint;
	}
}

static utf8 *convert_multibyte_charset(const char *src, int languageId)
{
	int reservedLength = (strlen(src) * 4) + 1;
	utf8 *buffer = (utf8*)malloc(reservedLength);
	utf8 *dst = buffer;
	for (const uint8 *ch = (const uint8*)src; *ch != 0;) {
		if (*ch == 0xFF) {
			ch++;
			uint8 a = *ch++;
			uint8 b = *ch++;
			uint16 codepoint = (a << 8) | b;

			codepoint = convert_specific_language_character_to_unicode(languageId, codepoint);
			dst = utf8_write_codepoint(dst, codepoint);
		} else {
			dst = utf8_write_codepoint(dst, *ch++);
		}
	}
	*dst++ = 0;
	int actualLength = dst - buffer;
	buffer = (utf8*)realloc(buffer, actualLength);

	return buffer;
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
		for (char *ch = (char*)(*pStringTable); *ch != 0; ch++) {
			if (!isblank(*ch)) {
				isBlank = false;
				break;
			}
		}

		if (isBlank) languageId = 0xFE;

		// This is the ideal situation. Language found
		if (languageId == LanguagesDescriptors[gCurrentLanguage].rct2_original_id) {
			chosenLanguageId = languageId;
			pString = (char*)(*pStringTable);
			result |= 1;
		}

		// Just in case always load english into pString
		if (languageId == RCT2_LANGUAGE_ID_ENGLISH_UK && !(result & 1)) {
			chosenLanguageId = languageId;
			pString = (char*)(*pStringTable);
			result |= 2;
		}

		// Failing that fall back to whatever is first string
		if (!(result & 7)) {
			chosenLanguageId = languageId;
			pString = (char*)(*pStringTable);
			if (!isBlank) result |= 4;
		}

		// Skip over the actual string entry to get to the next entry
		while (*(*pStringTable)++ != 0);
	}

	char name[9];
	if (RCT2_GLOBAL(0x009ADAFC, uint8) == 0) {
		memcpy(name, object_entry_groups[type].entries[index].name, 8);
	} else {
		memcpy(name, gTempObjectLoadName, 8);
	}
	name[8] = 0;

	rct_string_id stringId = _languageCurrent->GetObjectOverrideStringId(name, tableindex);
	if (stringId != (rct_string_id)STR_NONE) {
		return stringId;
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
			*cacheString = convert_multibyte_charset(pString, chosenLanguageId);
		} else {
			*cacheString = win1252_to_utf8_alloc(pString);
		}
		utf8_trim_string(*cacheString);

		//put pointer in stringtable
		_languageCurrent->SetString(stringid, *cacheString);
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
			*cacheString = convert_multibyte_charset(pString, chosenLanguageId);
		} else {
			*cacheString = win1252_to_utf8_alloc(pString);
		}
		utf8_trim_string(*cacheString);

		//put pointer in stringtable
		_languageCurrent->SetString(stringid, *cacheString);
		// Until all string related functions are finished copy
		// to old array as well.
		_languageOriginal[stringid] = *cacheString;
		return stringid;
	}
}

}
