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

#include <stack>
#include "../core/String.hpp"
#include "../object/ObjectManager.h"
#include "LanguagePack.h"

extern "C" {

#include "../config.h"
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
	RCT2_LANGUAGE_ID_PORTUGUESE,
	RCT2_LANGUAGE_ID_END = 255
};

static TTFFontSetDescriptor TTFFontMSGothic = {{
	{ "msgothic.ttc",	"MS PGothic",	9,		1,		0,		15,		nullptr },
	{ "msgothic.ttc",	"MS PGothic",	12,		1,		0,		17,		nullptr },
	{ "msgothic.ttc",	"MS PGothic",	12,		1,		0,		17,		nullptr },
	{ "msgothic.ttc",	"MS PGothic",	13,		1,		0,		20,		nullptr },
}};

static TTFFontSetDescriptor TTFFontMingLiu = {{
	{ "msjh.ttc",		"JhengHei",	9,		-1,		-3,		6,		nullptr },
	{ "mingliu.ttc",	"MingLiU",	11,		1,		1,		12,		nullptr },
	{ "mingliu.ttc",	"MingLiU",	12,		1,		0,		12,		nullptr },
	{ "mingliu.ttc",	"MingLiU",	13,		1,		0,		20,		nullptr },
}};

static TTFFontSetDescriptor TTFFontSimSun = {{
	{ "msyh.ttc",		"YaHei",	9,		-1,		-3,		6,		nullptr },
	{ "simsun.ttc",		"SimSun",	11,		1,		-1,		14,		nullptr },
	{ "simsun.ttc",		"SimSun",	12,		1,		-2,		14,		nullptr },
	{ "simsun.ttc",		"SimSun",	13,		1,		0,		20,		nullptr },
}};

static TTFFontSetDescriptor TTFFontGulim = {{
	{ "gulim.ttc",		"Gulim",	11,		1,		0,		15,		nullptr },
	{ "gulim.ttc",		"Gulim",	12,		1,		0,		17,		nullptr },
	{ "gulim.ttc",		"Gulim",	12,		1,		0,		17,		nullptr },
	{ "gulim.ttc",		"Gulim",	13,		1,		0,		20,		nullptr },
}};

static TTFFontSetDescriptor TTFFontArial = {{
	{ "arial.ttf",		"Arial",	8,		0,		-1,		6,		nullptr },
	{ "arial.ttf",		"Arial",	10,		0,		-1,		12,		nullptr },
	{ "arial.ttf",		"Arial",	11,		0,		-1,		12,		nullptr },
	{ "arial.ttf",		"Arial",	12,		0,		-1,		20,		nullptr },
}};

const language_descriptor LanguagesDescriptors[LANGUAGE_COUNT] = {
	{ "",			 "",						 "",						FONT_OPENRCT2_SPRITE,	RCT2_LANGUAGE_ID_ENGLISH_UK				},	// LANGUAGE_UNDEFINED
	{ "en-GB",		"English (UK)",				"English (UK)",				FONT_OPENRCT2_SPRITE,	RCT2_LANGUAGE_ID_ENGLISH_UK				},	// LANGUAGE_ENGLISH_UK
	{ "en-US",		"English (US)",				"English (US)",				FONT_OPENRCT2_SPRITE,	RCT2_LANGUAGE_ID_ENGLISH_US				},	// LANGUAGE_ENGLISH_US
	{ "de-DE",		"German",					"Deutsch",					FONT_OPENRCT2_SPRITE,	RCT2_LANGUAGE_ID_GERMAN					},	// LANGUAGE_GERMAN
	{ "nl-NL",		"Dutch",					"Nederlands",				FONT_OPENRCT2_SPRITE,	RCT2_LANGUAGE_ID_DUTCH					},	// LANGUAGE_DUTCH
	{ "fr-FR",		"French",					"Fran\xC3\xA7" "ais",		FONT_OPENRCT2_SPRITE,	RCT2_LANGUAGE_ID_FRENCH					},	// LANGUAGE_FRENCH
	{ "hu-HU",		"Hungarian",				"Magyar",					FONT_OPENRCT2_SPRITE,	RCT2_LANGUAGE_ID_ENGLISH_UK				},	// LANGUAGE_HUNGARIAN
	{ "pl-PL",		"Polish",					"Polski",					&TTFFontArial,			RCT2_LANGUAGE_ID_ENGLISH_UK				},	// LANGUAGE_POLISH
	{ "es-ES",		"Spanish",					"Espa\xC3\xB1ol",			FONT_OPENRCT2_SPRITE,	RCT2_LANGUAGE_ID_SPANISH				},	// LANGUAGE_SPANISH
	{ "sv-SE",		"Swedish",					"Svenska",					FONT_OPENRCT2_SPRITE,	RCT2_LANGUAGE_ID_SWEDISH				},	// LANGUAGE_SWEDISH
	{ "it-IT",		"Italian",					"Italiano",					FONT_OPENRCT2_SPRITE,	RCT2_LANGUAGE_ID_ITALIAN				},	// LANGUAGE_ITALIAN
	{ "pt-BR",		"Portuguese (BR)",			"Portugu\xC3\xAAs (BR)",	FONT_OPENRCT2_SPRITE,	RCT2_LANGUAGE_ID_PORTUGUESE				},	// LANGUAGE_PORTUGUESE_BR
	{ "zh-TW",		"Chinese (Traditional)",	"Chinese (Traditional)",	&TTFFontMingLiu,		RCT2_LANGUAGE_ID_CHINESE_TRADITIONAL	},	// LANGUAGE_CHINESE_TRADITIONAL
	{ "zh-CN",		"Chinese (Simplified)",		"Chinese (Simplified)",		&TTFFontSimSun,			RCT2_LANGUAGE_ID_CHINESE_SIMPLIFIED		},	// LANGUAGE_CHINESE_SIMPLIFIED
	{ "fi-FI",		"Finnish",					"Suomi",					FONT_OPENRCT2_SPRITE,	RCT2_LANGUAGE_ID_ENGLISH_UK				},	// LANGUAGE_FINNISH
	{ "ko-KR",		"Korean",					"Korean",					&TTFFontGulim,			RCT2_LANGUAGE_ID_KOREAN					},	// LANGUAGE_KOREAN
	{ "ru-RU",		"Russian",					"Russian",					&TTFFontArial,			RCT2_LANGUAGE_ID_ENGLISH_UK				},	// LANGUAGE_RUSSIAN
	{ "cs-CZ",		"Czech",					"Czech",					&TTFFontArial,			RCT2_LANGUAGE_ID_ENGLISH_UK				},	// LANGUAGE_CZECH
	{ "ja-JP",		"Japanese",					"Japanese",					&TTFFontMSGothic,		RCT2_LANGUAGE_ID_ENGLISH_UK				},	// LANGUAGE_JAPANESE
	{ "nb-NO",		"Norwegian",				"Norsk",					FONT_OPENRCT2_SPRITE,	RCT2_LANGUAGE_ID_ENGLISH_UK,			},	// LANGUAGE_NORWEGIAN
};

int gCurrentLanguage = LANGUAGE_UNDEFINED;
bool gUseTrueTypeFont = false;

ILanguagePack *_languageFallback = nullptr;
ILanguagePack *_languageCurrent = nullptr;

const utf8 BlackUpArrowString[] =		{ (utf8)0xC2, (utf8)0x8E, (utf8)0xE2, (utf8)0x96, (utf8)0xB2, (utf8)0x00 };
const utf8 BlackDownArrowString[] =		{ (utf8)0xC2, (utf8)0x8E, (utf8)0xE2, (utf8)0x96, (utf8)0xBC, (utf8)0x00 };
const utf8 BlackLeftArrowString[] =		{ (utf8)0xC2, (utf8)0x8E, (utf8)0xE2, (utf8)0x97, (utf8)0x80, (utf8)0x00 };
const utf8 BlackRightArrowString[] =	{ (utf8)0xC2, (utf8)0x8E, (utf8)0xE2, (utf8)0x96, (utf8)0xB6, (utf8)0x00 };
const utf8 CheckBoxMarkString[] =		{ (utf8)0xE2, (utf8)0x9C, (utf8)0x93, (utf8)0x00 };

void utf8_remove_format_codes(utf8 *text, bool allowcolours)
{
	utf8 *dstCh = text;
	utf8 *ch = text;
	int codepoint;
	while ((codepoint = utf8_get_next(ch, (const utf8**)&ch)) != 0) {
		if (!utf8_is_format_code(codepoint) || (allowcolours && utf8_is_colour_code(codepoint))) {
			dstCh = utf8_write_codepoint(dstCh, codepoint);
		}
	}
	*dstCh = 0;
}

const char *language_get_string(rct_string_id id)
{
	const char *openrctString = nullptr;

	if (id == STR_NONE)
		return nullptr;

	if (_languageCurrent != nullptr)
		openrctString = _languageCurrent->GetString(id);
	if (openrctString == nullptr && _languageFallback != nullptr)
		openrctString = _languageFallback->GetString(id);
	if (openrctString == nullptr)
		openrctString = "(undefined string)";

	return openrctString;
}

bool language_open(int id)
{
	static const char *languagePath = "%s/language/%s.txt";
	char filename[MAX_PATH];
	char dataPath[MAX_PATH];

	language_close_all();
	if (id == LANGUAGE_UNDEFINED)
		return false;

	platform_get_openrct_data_path(dataPath);
	if (id != LANGUAGE_ENGLISH_UK) {
		sprintf(filename, languagePath, dataPath, LanguagesDescriptors[LANGUAGE_ENGLISH_UK].locale);
		_languageFallback = LanguagePackFactory::FromFile(LANGUAGE_ENGLISH_UK, filename);
	}

	sprintf(filename, languagePath, dataPath, LanguagesDescriptors[id].locale);
	_languageCurrent = LanguagePackFactory::FromFile(id, filename);
	if (_languageCurrent != nullptr) {
		gCurrentLanguage = id;

		if (LanguagesDescriptors[id].font == FONT_OPENRCT2_SPRITE) {
			ttf_dispose();
			gUseTrueTypeFont = false;
			gCurrentTTFFontSet = nullptr;
		} else {
			if (!String::IsNullOrEmpty(gConfigFonts.file_name)) {
				static TTFFontSetDescriptor TTFFontCustom = {{
					{ gConfigFonts.file_name,	gConfigFonts.font_name,	gConfigFonts.size_tiny,		gConfigFonts.x_offset,	gConfigFonts.y_offset,	gConfigFonts.height_tiny,	nullptr },
					{ gConfigFonts.file_name,	gConfigFonts.font_name,	gConfigFonts.size_small,	gConfigFonts.x_offset,	gConfigFonts.y_offset,	gConfigFonts.height_small,	nullptr },
					{ gConfigFonts.file_name,	gConfigFonts.font_name,	gConfigFonts.size_medium,	gConfigFonts.x_offset,	gConfigFonts.y_offset,	gConfigFonts.height_medium,	nullptr },
					{ gConfigFonts.file_name,	gConfigFonts.font_name,	gConfigFonts.size_big,		gConfigFonts.x_offset,	gConfigFonts.y_offset,	gConfigFonts.height_big,	nullptr },
				}};
				ttf_dispose();
				gUseTrueTypeFont = true;
				gCurrentTTFFontSet = &TTFFontCustom;
				
				bool font_initialised = ttf_initialise();
				if (!font_initialised) {
					log_warning("Unable to initialise configured TrueType font -- falling back to Language default.");
				} else {
					// Objects and their localized strings need to be refreshed
					GetObjectManager()->ResetObjects();
					return true;
				}
			}
			ttf_dispose();
			gUseTrueTypeFont = true;
			gCurrentTTFFontSet = LanguagesDescriptors[id].font;
			bool font_initialised = ttf_initialise();

			// Have we tried Arial yet?
			if (!font_initialised && gCurrentTTFFontSet != &TTFFontArial) {
				log_warning("Unable to initialise prefered TrueType font -- falling back to Arial.");
				gCurrentTTFFontSet = &TTFFontArial;
				font_initialised = ttf_initialise();
			}

			// Fall back to sprite font.
			if (!font_initialised) {
				log_warning("Falling back to sprite font.");
				gUseTrueTypeFont = false;
				gCurrentTTFFontSet = nullptr;
				return false;
			}
		}

		// Objects and their localized strings need to be refreshed
		GetObjectManager()->ResetObjects();
		return true;
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

utf8 *_cachedObjectStrings[MAX_OBJECT_CACHED_STRINGS] = { nullptr };

static void utf8_trim_string(utf8 *text)
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
	size_t reservedLength = (strlen(src) * 4) + 1;
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
	size_t actualLength = (size_t)(dst - buffer);
	buffer = (utf8*)realloc(buffer, actualLength);

	return buffer;
}

static bool rct2_language_is_multibyte_charset(int languageId)
{
	switch (languageId) {
	case RCT2_LANGUAGE_ID_KOREAN:
	case RCT2_LANGUAGE_ID_CHINESE_TRADITIONAL:
	case RCT2_LANGUAGE_ID_CHINESE_SIMPLIFIED:
	case RCT2_LANGUAGE_ID_8:
		return true;
	default:
		return false;
	}
}

utf8 *rct2_language_string_to_utf8(const char *src, int languageId)
{
    if (rct2_language_is_multibyte_charset(languageId)) {
        return convert_multibyte_charset(src, languageId);
    } else {
        return win1252_to_utf8_alloc(src);
    }
}

bool language_get_localised_scenario_strings(const utf8 *scenarioFilename, rct_string_id *outStringIds)
{
	outStringIds[0] = _languageCurrent->GetScenarioOverrideStringId(scenarioFilename, 0);
	outStringIds[1] = _languageCurrent->GetScenarioOverrideStringId(scenarioFilename, 1);
	outStringIds[2] = _languageCurrent->GetScenarioOverrideStringId(scenarioFilename, 2);
	return
		outStringIds[0] != STR_NONE ||
		outStringIds[1] != STR_NONE ||
		outStringIds[2] != STR_NONE;
}

static bool							_availableObjectStringIdsInitialised = false;
static std::stack<rct_string_id>	_availableObjectStringIds;

rct_string_id language_allocate_object_string(const utf8 * target)
{
	if (!_availableObjectStringIdsInitialised)
	{
		_availableObjectStringIdsInitialised = true;
		for (rct_string_id stringId = NONSTEX_BASE_STRING_ID + MAX_OBJECT_CACHED_STRINGS; stringId >= NONSTEX_BASE_STRING_ID; stringId--)
		{
			_availableObjectStringIds.push(stringId);
		}
	}
	
	rct_string_id stringId = _availableObjectStringIds.top();
	_availableObjectStringIds.pop();
	_languageCurrent->SetString(stringId, target);
	return stringId;
}

void language_free_object_string(rct_string_id stringId)
{
	if (stringId != 0)
	{
		if (_languageCurrent != nullptr)
		{
			_languageCurrent->SetString(stringId, nullptr);
		}
		_availableObjectStringIds.push(stringId);
	}
}

rct_string_id language_get_object_override_string_id(const char * identifier, uint8 index)
{
	if (_languageCurrent == nullptr)
	{
		return STR_NONE;
	}
	return _languageCurrent->GetObjectOverrideStringId(identifier, index);
}

}
