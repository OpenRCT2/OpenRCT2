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

#include "addresses.h"
#include "config.h"
#include "localisation/language.h"
#include "localisation/localisation.h"
#include "util/util.h"
#include "interface/themes.h"

// Magic number for original game cfg file
static const int MagicNumber = 0x0003113A;

enum {
	CONFIG_VALUE_TYPE_BOOLEAN,
	CONFIG_VALUE_TYPE_UINT8,
	CONFIG_VALUE_TYPE_UINT16,
	CONFIG_VALUE_TYPE_UINT32,
	CONFIG_VALUE_TYPE_SINT8,
	CONFIG_VALUE_TYPE_SINT16,
	CONFIG_VALUE_TYPE_SINT32,
	CONFIG_VALUE_TYPE_FLOAT,
	CONFIG_VALUE_TYPE_DOUBLE,
	CONFIG_VALUE_TYPE_STRING
};

size_t _configValueTypeSize[] = {
	sizeof(bool),
	sizeof(uint8),
	sizeof(uint16),
	sizeof(uint32),
	sizeof(sint8),
	sizeof(sint16),
	sizeof(sint32),
	sizeof(float),
	sizeof(double),
	sizeof(utf8string)
};

typedef union {
	sint32 value_sint32;

	bool value_boolean;
	sint8 value_sint8;
	sint16 value_sint16;
	uint8 value_uint8;
	uint16 value_uint16;
	uint32 value_uint32;
	float value_float;
	double value_double;
	utf8string value_string;
} value_union;

typedef struct {
	const_utf8string key;
	value_union value;
} config_enum_definition;

#define END_OF_ENUM { NULL, 0 }

typedef struct {
	size_t offset;
	const_utf8string property_name;
	uint8 type;
	value_union default_value;
	config_enum_definition *enum_definitions;
} config_property_definition;

typedef struct {
	void *base_address;
	const_utf8string section_name;
	config_property_definition *property_definitions;
	int property_definitions_count;
} config_section_definition;

#pragma region Enum definitions

config_enum_definition _screenShotFormatEnum[] = {
	{ "BMP", SCREENSHOT_FORMAT_BMP },
	{ "PNG", SCREENSHOT_FORMAT_PNG },
	END_OF_ENUM
};

config_enum_definition _measurementFormatEnum[] = {
	{ "IMPERIAL", MEASUREMENT_FORMAT_IMPERIAL },
	{ "METRIC", MEASUREMENT_FORMAT_METRIC },
	END_OF_ENUM
};

config_enum_definition _temperatureFormatEnum[] = {
	{ "CELSIUS", TEMPERATURE_FORMAT_C },
	{ "FAHRENHEIT", TEMPERATURE_FORMAT_F },
	END_OF_ENUM
};

config_enum_definition _currencyEnum[] = {
	{ "GBP", CURRENCY_POUNDS },
	{ "USD", CURRENCY_DOLLARS },
	{ "FRF", CURRENCY_FRANC },
	{ "DEM", CURRENCY_DEUTSCHMARK },
	{ "JPY", CURRENCY_YEN },
	{ "ESP", CURRENCY_PESETA },
	{ "ITL", CURRENCY_LIRA },
	{ "NLG", CURRENCY_GUILDERS },
	{ "SEK", CURRENCY_KRONA },
	{ "EUR", CURRENCY_EUROS },
	END_OF_ENUM
};

config_enum_definition _languageEnum[] = {
	{ "en-GB", LANGUAGE_ENGLISH_UK },
	{ "en-US", LANGUAGE_ENGLISH_US },
	{ "de-DE", LANGUAGE_GERMAN },
	{ "nl-NL", LANGUAGE_DUTCH },
	{ "fr-FR", LANGUAGE_FRENCH },
	{ "hu-HU", LANGUAGE_HUNGARIAN },
	{ "pl-PL", LANGUAGE_POLISH },
	{ "es-ES", LANGUAGE_SPANISH },
	{ "sv-SE", LANGUAGE_SWEDISH },
	END_OF_ENUM
};

config_enum_definition _dateFormatEnum[] = {
	{ "DD/MM/YY", DATE_FORMAT_DMY },
	{ "MM/DD/YY", DATE_FORMAT_MDY },
	END_OF_ENUM
};

#pragma endregion

#pragma region Section / property definitions

config_property_definition _generalDefinitions[] = {
	{ offsetof(general_configuration, always_show_gridlines),			"always_show_gridlines",		CONFIG_VALUE_TYPE_BOOLEAN,		false,							NULL					},
	{ offsetof(general_configuration, autosave_frequency),				"autosave",						CONFIG_VALUE_TYPE_UINT8,		AUTOSAVE_EVERY_MONTH,			NULL					},
	{ offsetof(general_configuration, confirmation_prompt),				"confirmation_prompt",			CONFIG_VALUE_TYPE_UINT8,		0,								NULL					},
	{ offsetof(general_configuration, construction_marker_colour),		"construction_marker_colour",	CONFIG_VALUE_TYPE_UINT8,		false,							NULL					},
	{ offsetof(general_configuration, currency_format),					"currency_format",				CONFIG_VALUE_TYPE_UINT8,		CURRENCY_POUNDS,				_currencyEnum			},
	{ offsetof(general_configuration, edge_scrolling),					"edge_scrolling",				CONFIG_VALUE_TYPE_BOOLEAN,		true,							NULL					},
	{ offsetof(general_configuration, fullscreen_mode),					"fullscreen_mode",				CONFIG_VALUE_TYPE_UINT8,		0,								NULL					},
	{ offsetof(general_configuration, fullscreen_height),				"fullscreen_height",			CONFIG_VALUE_TYPE_SINT32,		-1,								NULL					},
	{ offsetof(general_configuration, fullscreen_width),				"fullscreen_width",				CONFIG_VALUE_TYPE_SINT32,		-1,								NULL					},
	{ offsetof(general_configuration, game_path),						"game_path",					CONFIG_VALUE_TYPE_STRING,		{ .value_string = NULL },		NULL					},
	{ offsetof(general_configuration, landscape_smoothing),				"landscape_smoothing",			CONFIG_VALUE_TYPE_BOOLEAN,		true,							NULL					},
	{ offsetof(general_configuration, language),						"language",						CONFIG_VALUE_TYPE_UINT16,		LANGUAGE_ENGLISH_UK,			_languageEnum			},
	{ offsetof(general_configuration, measurement_format),				"measurement_format",			CONFIG_VALUE_TYPE_UINT8,		MEASUREMENT_FORMAT_IMPERIAL,	_measurementFormatEnum	},
	{ offsetof(general_configuration, play_intro),						"play_intro",					CONFIG_VALUE_TYPE_BOOLEAN,		false,							NULL					},
	{ offsetof(general_configuration, save_plugin_data),				"save_plugin_data",				CONFIG_VALUE_TYPE_BOOLEAN,		false,							NULL					},
	{ offsetof(general_configuration, debugging_tools),					"debugging_tools",				CONFIG_VALUE_TYPE_BOOLEAN,		false,							NULL					},
	{ offsetof(general_configuration, screenshot_format),				"screenshot_format",			CONFIG_VALUE_TYPE_UINT8,		SCREENSHOT_FORMAT_PNG,			_screenShotFormatEnum	},
	{ offsetof(general_configuration, show_height_as_units),			"show_height_as_units",			CONFIG_VALUE_TYPE_BOOLEAN,		false,							NULL					},
	{ offsetof(general_configuration, temperature_format),				"temperature_format",			CONFIG_VALUE_TYPE_UINT8,		TEMPERATURE_FORMAT_C,			_temperatureFormatEnum	},
	{ offsetof(general_configuration, window_height),					"window_height",				CONFIG_VALUE_TYPE_SINT32,		-1,								NULL					},
	{ offsetof(general_configuration, window_snap_proximity),			"window_snap_proximity",		CONFIG_VALUE_TYPE_UINT8,		5,								NULL					},
	{ offsetof(general_configuration, window_width),					"window_width",					CONFIG_VALUE_TYPE_SINT32,		-1,								NULL					},
	{ offsetof(general_configuration, hardware_display),				"hardware_display",				CONFIG_VALUE_TYPE_BOOLEAN,		false,							NULL					},
	{ offsetof(general_configuration, test_unfinished_tracks),			"test_unfinished_tracks",		CONFIG_VALUE_TYPE_BOOLEAN,		false,							NULL					},
	{ offsetof(general_configuration, no_test_crashes),					"no_test_crashes",				CONFIG_VALUE_TYPE_BOOLEAN,		false,							NULL					},
	{ offsetof(general_configuration, date_format),						"date_format",					CONFIG_VALUE_TYPE_UINT8,		DATE_FORMAT_DMY,				_dateFormatEnum			},
};

config_property_definition _interfaceDefinitions[] = {
	{ offsetof(interface_configuration, toolbar_show_finances),			"toolbar_show_finances",		CONFIG_VALUE_TYPE_BOOLEAN,		false,							NULL					},
	{ offsetof(interface_configuration, toolbar_show_research),			"toolbar_show_research",		CONFIG_VALUE_TYPE_BOOLEAN,		true,							NULL					},
	{ offsetof(interface_configuration, toolbar_show_cheats),			"toolbar_show_cheats",			CONFIG_VALUE_TYPE_BOOLEAN,		false,							NULL					},
	{ offsetof(interface_configuration, allow_subtype_switching),		"allow_subtype_switching",		CONFIG_VALUE_TYPE_BOOLEAN,		false,							NULL					},
	{ offsetof(interface_configuration, console_small_font),			"console_small_font",			CONFIG_VALUE_TYPE_BOOLEAN,		false,							NULL					},
	{ offsetof(interface_configuration, current_theme_preset),			"current_theme",				CONFIG_VALUE_TYPE_STRING, { .value_string = "*RCT2" }, NULL },
};

config_property_definition _soundDefinitions[] = {
	{ offsetof(sound_configuration, forced_software_buffering),			"forced_software_buffering",	CONFIG_VALUE_TYPE_BOOLEAN,		false,							NULL					},
	{ offsetof(sound_configuration, sound_quality),						"sound_quality",				CONFIG_VALUE_TYPE_UINT8,		2,								NULL					},
	{ offsetof(sound_configuration, title_music),						"title_music",					CONFIG_VALUE_TYPE_UINT8,		2,								NULL					},
	{ offsetof(sound_configuration, sound),								"sound",						CONFIG_VALUE_TYPE_BOOLEAN,		true,							NULL					},
	{ offsetof(sound_configuration, ride_music),						"ride_music",					CONFIG_VALUE_TYPE_BOOLEAN,		true,							NULL					},
};

config_property_definition _cheatDefinitions[] = {
	{ offsetof(cheat_configuration, fast_lift_hill),					"fast_lift_hill",				CONFIG_VALUE_TYPE_BOOLEAN,		false,							NULL					},
	{ offsetof(cheat_configuration, disable_brakes_failure),			"disable_brakes_failure",		CONFIG_VALUE_TYPE_BOOLEAN,		false,							NULL					},
	{ offsetof(cheat_configuration, disable_all_breakdowns),			"disable_all_breakdowns",		CONFIG_VALUE_TYPE_BOOLEAN,		false,							NULL					},
	{ offsetof(cheat_configuration, unlock_all_prices),					"unlock_all_prices",			CONFIG_VALUE_TYPE_BOOLEAN,		false,							NULL					},
};

config_property_definition _twitchDefinitions[] = {
	{ offsetof(twitch_configuration, channel),							"channel",						CONFIG_VALUE_TYPE_STRING,		{ .value_string = NULL },		NULL					},
	{ offsetof(twitch_configuration, enable_follower_peep_names),		"follower_peep_names",			CONFIG_VALUE_TYPE_BOOLEAN,		true,							NULL					},
	{ offsetof(twitch_configuration, enable_follower_peep_tracking),	"follower_peep_tracking",		CONFIG_VALUE_TYPE_BOOLEAN,		false,							NULL					},
	{ offsetof(twitch_configuration, enable_chat_peep_names),			"chat_peep_names",				CONFIG_VALUE_TYPE_BOOLEAN,		true,							NULL					},
	{ offsetof(twitch_configuration, enable_chat_peep_tracking),		"chat_peep_tracking",			CONFIG_VALUE_TYPE_BOOLEAN,		true,							NULL					},
	{ offsetof(twitch_configuration, enable_news),						"news",							CONFIG_VALUE_TYPE_BOOLEAN,		false,							NULL					}
};

config_section_definition _sectionDefinitions[] = {
	{ &gConfigGeneral, "general", _generalDefinitions, countof(_generalDefinitions) },
	{ &gConfigInterface, "interface", _interfaceDefinitions, countof(_interfaceDefinitions) },
	{ &gConfigSound, "sound", _soundDefinitions, countof(_soundDefinitions) },
	{ &gConfigCheat, "cheat", _cheatDefinitions, countof(_cheatDefinitions) },
	{ &gConfigTwitch, "twitch", _twitchDefinitions, countof(_twitchDefinitions) }
};

#pragma endregion

general_configuration gConfigGeneral;
interface_configuration gConfigInterface;
sound_configuration gConfigSound;
cheat_configuration gConfigCheat;
twitch_configuration gConfigTwitch;
themes_configuration gConfigThemes;

bool config_open(const utf8string path);
bool config_save(const utf8string path);
static void config_read_properties(config_section_definition **currentSection, const_utf8string line);
static void config_save_property_value(FILE *file, uint8 type, value_union *value);
static bool config_read_enum(void *dest, int destSize, const utf8 *key, int keySize, config_enum_definition *enumDefinitions);
static void config_write_enum(FILE *file, uint8 type, value_union *value, config_enum_definition *enumDefinitions);

static int utf8_read(utf8 **outch);
static void utf8_skip_whitespace(utf8 **outch);
static void utf8_skip_non_whitespace(utf8 **outch);

void config_apply_to_old_addresses();

void config_set_defaults()
{
	int i, j;

	for (i = 0; i < countof(_sectionDefinitions); i++) {
		config_section_definition *section = &_sectionDefinitions[i];
		for (j = 0; j < section->property_definitions_count; j++) {
			config_property_definition *property = &section->property_definitions[j];

			value_union *destValue = (value_union*)((size_t)section->base_address + (size_t)property->offset);

			if (strcmp(property->property_name, "language") == 0){
				destValue->value_uint16 = platform_get_locale_language();
				if (destValue->value_uint16 == LANGUAGE_UNDEFINED)
					memcpy(destValue, &property->default_value, _configValueTypeSize[property->type]);
			}
			else if (strcmp(property->property_name, "currency_format") == 0){
				destValue->value_uint8 = platform_get_locale_currency();
			}
			else if (strcmp(property->property_name, "measurement_format") == 0){
				destValue->value_uint8 = platform_get_locale_measurement_format();
			}
			else if (strcmp(property->property_name, "temperature_format") == 0){
				destValue->value_uint8 = platform_get_locale_temperature_format();
			}
			else {
				memcpy(destValue, &property->default_value, _configValueTypeSize[property->type]);
			}
		}
	}
}

bool config_open_default()
{
	utf8 path[MAX_PATH];

	platform_get_user_directory(path, NULL);
	strcat(path, "config.ini");
	if (config_open(path)) {
		config_apply_to_old_addresses();
		return true;
	}

	return false;
}

bool config_save_default()
{
	utf8 path[MAX_PATH];

	platform_get_user_directory(path, NULL);
	strcat(path, "config.ini");
	if (config_save(path)) {
		config_apply_to_old_addresses();
		return true;
	}

	return false;
}

bool config_open(const utf8string path)
{
	FILE *file;
	uint8 *lineBuffer;
	size_t lineBufferCapacity;
	size_t lineLength;
	int c;
	config_section_definition *currentSection;

	file = fopen(path, "rb");
	if (file == NULL)
		return false;

	currentSection = NULL;
	lineBufferCapacity = 64;
	lineBuffer = malloc(lineBufferCapacity);
	lineLength = 0;

	// Skim UTF-8 byte order mark
	fread(lineBuffer, 3, 1, file);
	if (!(lineBuffer[0] == 0xEF && lineBuffer[1] == 0xBB && lineBuffer[2] == 0xBF))
		fseek(file, 0, SEEK_SET);

	while ((c = fgetc(file)) != EOF) {
		if (c == '\n' || c == '\r') {
			lineBuffer[lineLength++] = 0;
			config_read_properties(&currentSection, (const_utf8string)lineBuffer);
			lineLength = 0;
		} else {
			lineBuffer[lineLength++] = c;
		}

		if (lineLength >= lineBufferCapacity) {
			lineBufferCapacity *= 2;
			lineBuffer = realloc(lineBuffer, lineBufferCapacity);
		}
	}

	if (lineLength > 0) {
		lineBuffer[lineLength++] = 0;
		config_read_properties(&currentSection, lineBuffer);
	}

	free(lineBuffer);
	fclose(file);
	return true;
}

bool config_save(const utf8string path)
{
	FILE *file;
	int i, j;
	value_union *value;

	file = fopen(path, "wb");
	if (file == NULL) {
		log_error("Unable to write to config file.");
		return false;
	}

	for (i = 0; i < countof(_sectionDefinitions); i++) {
		config_section_definition *section = &_sectionDefinitions[i];

		fputc('[', file);
		fwrite(section->section_name, strlen(section->section_name), 1, file);
		fputc(']', file);
		fputc('\n', file);

		for (j = 0; j < section->property_definitions_count; j++) {
			config_property_definition *property = &section->property_definitions[j];

			fwrite(property->property_name, strlen(property->property_name), 1, file);
			fwrite(" = ", 3, 1, file);

			value = (value_union*)((size_t)section->base_address + (size_t)property->offset);
			if (property->enum_definitions != NULL)
				config_write_enum(file, property->type, value, property->enum_definitions);
			else
				config_save_property_value(file, property->type, value);
			fputc('\n', file);
		}
		fputc('\n', file);
	}

	fclose(file);
	return true;
}

static void config_save_property_value(FILE *file, uint8 type, value_union *value)
{
	switch (type) {
	case CONFIG_VALUE_TYPE_BOOLEAN:
		if (value->value_boolean) fwrite("true", 4, 1, file);
		else fwrite("false", 5, 1, file);
		break;
	case CONFIG_VALUE_TYPE_UINT8:
		fprintf(file, "%u", value->value_uint8);
		break;
	case CONFIG_VALUE_TYPE_UINT16:
		fprintf(file, "%u", value->value_uint16);
		break;
	case CONFIG_VALUE_TYPE_UINT32:
		fprintf(file, "%u", value->value_uint32);
		break;
	case CONFIG_VALUE_TYPE_SINT8:
		fprintf(file, "%d", value->value_sint8);
		break;
	case CONFIG_VALUE_TYPE_SINT16:
		fprintf(file, "%d", value->value_sint16);
		break;
	case CONFIG_VALUE_TYPE_SINT32:
		fprintf(file, "%d", value->value_sint32);
		break;
	case CONFIG_VALUE_TYPE_FLOAT:
		fprintf(file, "%.3f", value->value_float);
		break;
	case CONFIG_VALUE_TYPE_DOUBLE:
		fprintf(file, "%.6f", value->value_double);
		break;
	case CONFIG_VALUE_TYPE_STRING:
		fputc('"', file);
		if (value->value_string != NULL)
			fwrite(value->value_string, strlen(value->value_string), 1, file);
		fputc('"', file);
		break;
	}
}

bool config_get_section(const utf8string line, const utf8 **sectionName, int *sectionNameSize)
{
	utf8 *ch;
	int c;

	ch = line;
	utf8_skip_whitespace(&ch);
	if (*ch != '[') return false;
	*sectionName = ++ch;

	while (*ch != 0) {
		c = utf8_read(&ch);
		if (c == '#') return false;
		if (c == '[') return false;
		if (c == ' ') break;
		if (c == ']') break;
	}

	*sectionNameSize = ch - *sectionName - 1;
	return true;
}

bool config_get_property_name_value(const utf8string line, utf8 **propertyName, int *propertyNameSize, utf8 **value, int *valueSize)
{
	utf8 *ch;
	int c, lastC;
	bool quotes;

	ch = line;
	utf8_skip_whitespace(&ch);
	
	if (*ch == 0) return false;
	*propertyName = ch;

	while (*ch != 0) {
		c = utf8_read(&ch);
		if (isspace(c) || c == '=') {
			*propertyNameSize = ch - *propertyName - 1;
			break;
		} else if (c == '#') {
			return false;
		}
	}

	if (*ch == 0) return false;
	utf8_skip_whitespace(&ch);
	if (*ch != '=') return false;
	ch++;
	utf8_skip_whitespace(&ch);
	if (*ch == 0) return false;

	if (*ch == '"') {
		ch++;
		quotes = true;
	} else {
		quotes = false;
	}
	*value = ch;

	while (*ch != 0) {
		c = utf8_read(&ch);
		if (isspace(c) || c == '#') {
			if (!quotes) break;
		}
		lastC = c;
	}
	*valueSize = ch - *value - 1;
	return true;
}

config_section_definition *config_get_section_def(const utf8 *name, int size)
{
	int i;

	for (i = 0; i < countof(_sectionDefinitions); i++) {
		const_utf8string sectionName = _sectionDefinitions[i].section_name;
		if (sectionName[size] == 0 && _strnicmp(sectionName, name, size) == 0)
			return &_sectionDefinitions[i];
	}

	return NULL;
}

config_property_definition *config_get_property_def(config_section_definition *section, const utf8 *name, int size)
{
	int i;

	for (i = 0; i < section->property_definitions_count; i++) {
		const_utf8string propertyName = section->property_definitions[i].property_name;
		if (propertyName[size] == 0 && _strnicmp(propertyName, name, size) == 0)
			return &section->property_definitions[i];
	}

	return NULL;
}

void config_set_property(const config_section_definition *section, const config_property_definition *property, const utf8 *value, int valueSize)
{
	value_union *destValue = (value_union*)((size_t)section->base_address + (size_t)property->offset);

	if (property->enum_definitions != NULL)
		if (config_read_enum(destValue, _configValueTypeSize[property->type], value, valueSize, property->enum_definitions))
			return;

	switch (property->type) {
	case CONFIG_VALUE_TYPE_BOOLEAN:
		if (_strnicmp(value, "false", valueSize) == 0) destValue->value_boolean = false;
		else if (_strnicmp(value, "true", valueSize) == 0) destValue->value_boolean = true;
		else destValue->value_boolean = strtol(value, NULL, 0) != 0;
		break;
	case CONFIG_VALUE_TYPE_UINT8:
		destValue->value_uint8 = (uint8)strtol(value, NULL, 0);
		break;
	case CONFIG_VALUE_TYPE_UINT16:
		destValue->value_uint16 = (uint16)strtol(value, NULL, 0);
		break;
	case CONFIG_VALUE_TYPE_UINT32:
		destValue->value_uint32 = (uint32)strtol(value, NULL, 0);
		break;
	case CONFIG_VALUE_TYPE_SINT8:
		destValue->value_sint8 = (sint8)strtol(value, NULL, 0);
		break;
	case CONFIG_VALUE_TYPE_SINT16:
		destValue->value_sint16 = (sint16)strtol(value, NULL, 0);
		break;
	case CONFIG_VALUE_TYPE_SINT32:
		destValue->value_sint32 = (sint32)strtol(value, NULL, 0);
		break;
	case CONFIG_VALUE_TYPE_FLOAT:
		destValue->value_float = strtof(value, NULL);
		break;
	case CONFIG_VALUE_TYPE_DOUBLE:
		destValue->value_double = strtod(value, NULL);
		break;
	case CONFIG_VALUE_TYPE_STRING:
		SafeFree(destValue->value_string);
		destValue->value_string = malloc(valueSize + 1);
		memcpy(destValue->value_string, value, valueSize);
		destValue->value_string[valueSize] = 0;
		break;
	}
}

static void config_read_properties(config_section_definition **currentSection, const_utf8string line)
{
	utf8 *ch = (utf8*)line;
	utf8_skip_whitespace(&ch);

	if (*ch == '[') {
		const utf8 *sectionName;
		int sectionNameSize;
		if (config_get_section(ch, &sectionName, &sectionNameSize))
			*currentSection = config_get_section_def(sectionName, sectionNameSize);
	} else {
		if (*currentSection != NULL) {
			utf8 *propertyName, *value;
			int propertyNameSize, valueSize;
			if (config_get_property_name_value(ch, &propertyName, &propertyNameSize, &value, &valueSize)) {
				config_property_definition *property;
				property = config_get_property_def(*currentSection, propertyName, propertyNameSize);
				if (property != NULL)
					config_set_property(*currentSection, property, value, valueSize);
			}
		}
	}
}

static bool config_read_enum(void *dest, int destSize, const utf8 *key, int keySize, config_enum_definition *enumDefinitions)
{
	while (enumDefinitions->key != NULL) {
		if (_strnicmp(enumDefinitions->key, key, keySize) == 0) {
			memcpy(dest, &enumDefinitions->value.value_uint32, destSize);
			return true;
		}
		enumDefinitions++;
	}
	return false;
}

static void config_write_enum(FILE *file, uint8 type, value_union *value, config_enum_definition *enumDefinitions)
{
	uint32 enumValue = (value->value_uint32) & ((1 << (_configValueTypeSize[type] * 8)) - 1);
	while (enumDefinitions->key != NULL) {
		if (enumDefinitions->value.value_uint32 == enumValue) {
			fwrite(enumDefinitions->key, strlen(enumDefinitions->key), 1, file);
			return;
		}
		enumDefinitions++;
	}
	config_save_property_value(file, type, value);
}

static int utf8_read(utf8 **outch)
{
	int result;
	int numBytes;

	utf8 *ch = *outch;
	if (!(ch[0] & 0x80)) {
		result = ch[0];
		numBytes = 1;
	} else if (!(ch[0] & 0x20)) {
		result = ((ch[0] & 0x1F) << 6) | (ch[1] & 0x3F);
		numBytes = 2;
	} else {
		numBytes = 1;
	}

	*outch = ch + numBytes;
	return result;
}

static void utf8_skip_whitespace(utf8 **outch)
{
	utf8 *ch;
	while (**outch != 0) {
		ch = *outch;
		if (!isspace(utf8_read(outch))) {
			*outch = ch;
			break;
		}
	}
}

static void utf8_skip_non_whitespace(utf8 **outch)
{
	while (**outch != 0) {
		if (isspace(utf8_read(outch)))
			break;
	}
}

/*

Code reserved for when we want more intelligent saving of config file which preserves comments and layout

enum {
	CONFIG_LINE_TYPE_WHITESPACE,
	CONFIG_LINE_TYPE_COMMENT,
	CONFIG_LINE_TYPE_SECTION,
	CONFIG_LINE_TYPE_PROPERTY,
	CONFIG_LINE_TYPE_INVALID
};

typedef struct {
	uint8 type;
	utf8string line;
} config_line;

static config_line *_configLines = NULL;

*/

/**
 * Attempts to find the RCT2 installation directory.
 * This should be created from some other resource when OpenRCT2 grows.
 * @param resultPath Pointer to where the absolute path of the RCT2 installation directory will be copied to.
 * @returns 1 if successful, otherwise 0.
 */
static bool config_find_rct2_path(char *resultPath)
{
	int i;

	log_verbose("searching common installation locations.");

	const char *searchLocations[] = {
		"C:\\Program Files\\Infogrames\\RollerCoaster Tycoon 2",
		"C:\\Program Files (x86)\\Infogrames\\RollerCoaster Tycoon 2",
		"C:\\Program Files\\Infogrames Interactive\\RollerCoaster Tycoon 2",
		"C:\\Program Files (x86)\\Infogrames Interactive\\RollerCoaster Tycoon 2",
		"C:\\Program Files\\Atari\\RollerCoaster Tycoon 2",
		"C:\\Program Files (x86)\\Atari\\RollerCoaster Tycoon 2",
		"C:\\GOG Games\\RollerCoaster Tycoon 2 Triple Thrill Pack"
	};

	for (i = 0; i < countof(searchLocations); i++) {
		if (platform_directory_exists(searchLocations[i]) ) {
			strcpy(resultPath, searchLocations[i]);
			return true;
		}
	}

	return false;
}

bool config_find_or_browse_install_directory()
{
	char path[MAX_PATH];
	char *installPath;

	if (config_find_rct2_path(path)) {
		SafeFree(gConfigGeneral.game_path);
		gConfigGeneral.game_path = malloc(strlen(path) + 1);
		strcpy(gConfigGeneral.game_path, path);
	} else {
		platform_show_messagebox("Unable to find RCT2 installation directory. Please select the directory where you installed RCT2!");
		installPath = platform_open_directory_browser("Please select your RCT2 directory");
		if (installPath == NULL)
			return false;

		SafeFree(gConfigGeneral.game_path);
		gConfigGeneral.game_path = installPath;
	}

	return true;
}

#pragma region Obsolete

/**
 * Any code not implemented in OpenRCT2 will still uses the old configuration option addresses. This function copies all the
 * OpenRCT2 configuration options to those addresses until the process is no longer necessary.
 */
void config_apply_to_old_addresses()
{
	RCT2_GLOBAL(RCT2_ADDRESS_CONFIG_EDGE_SCROLLING, sint8) = gConfigGeneral.edge_scrolling;
	RCT2_GLOBAL(RCT2_ADDRESS_CONFIG_CURRENCY, sint8) = gConfigGeneral.currency_format; 
	RCT2_GLOBAL(RCT2_ADDRESS_CONFIG_METRIC, sint8) = gConfigGeneral.measurement_format;
	RCT2_GLOBAL(RCT2_ADDRESS_CONFIG_TEMPERATURE, sint8) = gConfigGeneral.temperature_format;
	RCT2_GLOBAL(RCT2_ADDRESS_CONFIG_CONSTRUCTION_MARKER, uint8) = gConfigGeneral.construction_marker_colour;
	RCT2_GLOBAL(RCT2_ADDRESS_CONFIG_SOUND_QUALITY, sint8) = gConfigSound.sound_quality;
	RCT2_GLOBAL(RCT2_ADDRESS_CONFIG_SOUND_SW_BUFFER, sint8) = gConfigSound.forced_software_buffering; 
	RCT2_GLOBAL(RCT2_ADDRESS_CONFIG_HEIGHT_MARKERS, sint16) = (gConfigGeneral.measurement_format + 1) * 256;
	if (gConfigGeneral.show_height_as_units)
		RCT2_GLOBAL(RCT2_ADDRESS_CONFIG_HEIGHT_MARKERS, sint16) = 0;

	int configFlags = 0;
	if (gConfigGeneral.always_show_gridlines)
		configFlags |= CONFIG_FLAG_ALWAYS_SHOW_GRIDLINES;
	if (!gConfigGeneral.landscape_smoothing)
		configFlags |= CONFIG_FLAG_DISABLE_SMOOTH_LANDSCAPE;
	if (gConfigGeneral.show_height_as_units)
		configFlags |= CONFIG_FLAG_SHOW_HEIGHT_AS_UNITS;
	if (gConfigGeneral.save_plugin_data)
		configFlags |= CONFIG_FLAG_SAVE_PLUGIN_DATA;

	RCT2_GLOBAL(RCT2_ADDRESS_CONFIG_FLAGS, uint8) = configFlags;
}

// The following functions are related to the original configuration file. This has now been replaced with a new configuration
// INI file located in the user's OpenRCT2 home directory.

/**
 *  Reads the config file data/config.cfg
 *  rct2: 0x006752D5
 */
void config_dat_load()
{
	FILE *fp=NULL;

	const char *path = get_file_path(PATH_ID_GAMECFG);

	fp = fopen(path, "rb");

	if (fp != NULL) {
		// Read and check magic number
		fread(RCT2_ADDRESS(0x013CE928, void), 1, 4, fp);

		if (RCT2_GLOBAL(0x013CE928, int) == MagicNumber) {
			// Read options
			fread((void*)0x009AAC5C, 1, 2155, fp);
			fclose(fp);

			//general configuration
			RCT2_GLOBAL(RCT2_ADDRESS_CONFIG_EDGE_SCROLLING, sint8) = gConfigGeneral.edge_scrolling;
			RCT2_GLOBAL(RCT2_ADDRESS_CONFIG_CURRENCY, sint8) = gConfigGeneral.currency_format; 
			RCT2_GLOBAL(RCT2_ADDRESS_CONFIG_METRIC, sint8) = gConfigGeneral.measurement_format;
			RCT2_GLOBAL(RCT2_ADDRESS_CONFIG_TEMPERATURE, sint8) = gConfigGeneral.temperature_format;
			
			// always show gridlines
			if (gConfigGeneral.always_show_gridlines){
				RCT2_GLOBAL(RCT2_ADDRESS_CONFIG_FLAGS, uint8) |= CONFIG_FLAG_ALWAYS_SHOW_GRIDLINES;
			}
			else {
				RCT2_GLOBAL(RCT2_ADDRESS_CONFIG_FLAGS, uint8) &= !CONFIG_FLAG_ALWAYS_SHOW_GRIDLINES;
			}

			// landscape smoothing
			if (!gConfigGeneral.landscape_smoothing){
				RCT2_GLOBAL(RCT2_ADDRESS_CONFIG_FLAGS, uint8) |= CONFIG_FLAG_DISABLE_SMOOTH_LANDSCAPE;
			}
			else {
				RCT2_GLOBAL(RCT2_ADDRESS_CONFIG_FLAGS, uint8) &= !CONFIG_FLAG_DISABLE_SMOOTH_LANDSCAPE;
			}
			
			// show height as units
			if (gConfigGeneral.show_height_as_units){
				RCT2_GLOBAL(RCT2_ADDRESS_CONFIG_FLAGS, uint8) |= CONFIG_FLAG_SHOW_HEIGHT_AS_UNITS;
			}
			else {
				RCT2_GLOBAL(RCT2_ADDRESS_CONFIG_FLAGS, uint8) &= !CONFIG_FLAG_SHOW_HEIGHT_AS_UNITS;
			}

			// save plugin data
			if (gConfigGeneral.save_plugin_data){
				RCT2_GLOBAL(RCT2_ADDRESS_CONFIG_FLAGS, uint8) |= CONFIG_FLAG_SAVE_PLUGIN_DATA;
			}
			else {
				RCT2_GLOBAL(RCT2_ADDRESS_CONFIG_FLAGS, uint8) &= !CONFIG_FLAG_SAVE_PLUGIN_DATA;
			}

			//sound configuration
			RCT2_GLOBAL(RCT2_ADDRESS_CONFIG_SOUND_QUALITY, sint8) = gConfigSound.sound_quality;
			RCT2_GLOBAL(RCT2_ADDRESS_CONFIG_SOUND_SW_BUFFER, sint8) = gConfigSound.forced_software_buffering; 

			// Line below is temporaraly disabled until all config is in the new format.
			//if (RCT2_GLOBAL(0x009AB4C6, sint8) == 1) 
			//	return;
			
			
			RCT2_GLOBAL(RCT2_ADDRESS_CONFIG_FIRST_TIME_LOAD_CONFIG, sint8) = 1; // Marks config as first time loaded

			RCT2_GLOBAL(RCT2_ADDRESS_CONFIG_HEIGHT_MARKERS, sint16) = (RCT2_GLOBAL(RCT2_ADDRESS_CONFIG_METRIC, sint8) + 2) * 256;
			if (!(RCT2_GLOBAL(RCT2_ADDRESS_CONFIG_FLAGS, uint8) & CONFIG_FLAG_SHOW_HEIGHT_AS_UNITS))
				RCT2_GLOBAL(RCT2_ADDRESS_CONFIG_HEIGHT_MARKERS, sint16) = (RCT2_GLOBAL(RCT2_ADDRESS_CONFIG_METRIC, sint8) + 1) * 256;
			// No longer used (controls first time object load)
			//RCT2_GLOBAL(RCT2_ADDRESS_CONFIG_FIRST_TIME_LOAD_OBJECTS, sint8) = 0;
		}
	
	}
	
	/* TODO: CLEANUP

			if (RCT2_GLOBAL(RCT2_ADDRESS_CONFIG_FIRST_TIME_LOAD_CONFIG, sint8) == 1)
				return;
			RCT2_GLOBAL(RCT2_ADDRESS_CONFIG_FIRST_TIME_LOAD_CONFIG, sint8) = 1;
			RCT2_GLOBAL(RCT2_ADDRESS_CONFIG_METRIC, sint8) = 0; 
			RCT2_GLOBAL(RCT2_ADDRESS_CONFIG_TEMPERATURE, sint8) = 1; 
			RCT2_GLOBAL(RCT2_ADDRESS_CONFIG_CURRENCY, sint8) = 1;
			RCT2_GLOBAL(RCT2_ADDRESS_CONFIG_HEIGHT_MARKERS, sint16) = 0;
			if (!(RCT2_GLOBAL(RCT2_ADDRESS_CONFIG_FLAGS, uint8) & CONFIG_FLAG_SHOW_HEIGHT_AS_UNITS))
				RCT2_GLOBAL(RCT2_ADDRESS_CONFIG_HEIGHT_MARKERS, sint16) = (RCT2_GLOBAL(RCT2_ADDRESS_CONFIG_METRIC, sint8) + 1) * 256;
			RCT2_GLOBAL(RCT2_ADDRESS_CONFIG_FIRST_TIME_LOAD_OBJECTS, sint8) = 1;
		}
	
	}

	RCT2_GLOBAL(RCT2_ADDRESS_CONFIG_SOUND_QUALITY, sint8) = 0;
	if (RCT2_GLOBAL(RCT2_ADDRESS_MEM_TOTAL_PHYSICAL, uint32) > 0x4000000) {
		RCT2_GLOBAL(RCT2_ADDRESS_CONFIG_SOUND_QUALITY, sint8) = 1;
		if (RCT2_GLOBAL(RCT2_ADDRESS_MEM_TOTAL_PHYSICAL, uint32) > 0x8000000)
			RCT2_GLOBAL(RCT2_ADDRESS_CONFIG_SOUND_QUALITY, sint8) = 2;
	}
	*/


	RCT2_GLOBAL(RCT2_ADDRESS_CONFIG_MAX_VEHICLE_SOUNDS, sint8) = RCT2_ADDRESS(0x009AF601, sint8)[RCT2_GLOBAL(RCT2_ADDRESS_CONFIG_SOUND_QUALITY, sint8)];
	RCT2_GLOBAL(RCT2_ADDRESS_CONFIG_MAX_NO_SOUNDS, sint8) = RCT2_ADDRESS(0x009AF604, sint8)[RCT2_GLOBAL(RCT2_ADDRESS_CONFIG_SOUND_QUALITY, sint8)];
	RCT2_GLOBAL(RCT2_ADDRESS_CONFIG_HEIGHT_MARKERS, sint16) = 0;
	if (!(RCT2_GLOBAL(RCT2_ADDRESS_CONFIG_FLAGS, uint8) & CONFIG_FLAG_SHOW_HEIGHT_AS_UNITS))
		RCT2_GLOBAL(RCT2_ADDRESS_CONFIG_HEIGHT_MARKERS, sint16) = (RCT2_GLOBAL(RCT2_ADDRESS_CONFIG_METRIC, sint8) + 1) * 256;
	RCT2_GLOBAL(RCT2_ADDRESS_CONFIG_FIRST_TIME_LOAD_OBJECTS, sint8) = 1;
}

/**
 *  Save configuration to the data/config.cfg file
 *  rct2: 0x00675487
 */
void config_dat_save()
{
	FILE *fp = fopen(get_file_path(PATH_ID_GAMECFG), "wb");
	if (fp != NULL){
		fwrite(&MagicNumber, 4, 1, fp);
		fwrite((void*)0x009AAC5C, 2155, 1, fp);
		fclose(fp);
	}
}

#pragma endregion

#pragma region Shortcuts

// Current keyboard shortcuts
uint16 gShortcutKeys[SHORTCUT_COUNT];

// Default keyboard shortcuts
static const uint16 _defaultShortcutKeys[SHORTCUT_COUNT] = {
	SDL_SCANCODE_BACKSPACE,				// SHORTCUT_CLOSE_TOP_MOST_WINDOW
	0x0100 | SDL_SCANCODE_BACKSPACE,	// SHORTCUT_CLOSE_ALL_FLOATING_WINDOWS
	SDL_SCANCODE_ESCAPE,				// SHORTCUT_CANCEL_CONSTRUCTION_MODE
	SDL_SCANCODE_PAUSE,					// SHORTCUT_PAUSE_GAME
	SDL_SCANCODE_PAGEUP,				// SHORTCUT_ZOOM_VIEW_OUT
	SDL_SCANCODE_PAGEDOWN,				// SHORTCUT_ZOOM_VIEW_IN
	SDL_SCANCODE_RETURN,				// SHORTCUT_ROTATE_VIEW
	SDL_SCANCODE_Z,						// SHORTCUT_ROTATE_CONSTRUCTION_OBJECT
	SDL_SCANCODE_1,						// SHORTCUT_UNDERGROUND_VIEW_TOGGLE
	SDL_SCANCODE_H,						// SHORTCUT_REMOVE_BASE_LAND_TOGGLE
	SDL_SCANCODE_V,						// SHORTCUT_REMOVE_VERTICAL_LAND_TOGGLE
	SDL_SCANCODE_3,						// SHORTCUT_SEE_THROUGH_RIDES_TOGGLE
	SDL_SCANCODE_4,						// SHORTCUT_SEE_THROUGH_SCENERY_TOGGLE
	SDL_SCANCODE_5,						// SHORTCUT_INVISIBLE_SUPPORTS_TOGGLE
	SDL_SCANCODE_6,						// SHORTCUT_INVISIBLE_PEOPLE_TOGGLE
	SDL_SCANCODE_8,						// SHORTCUT_HEIGHT_MARKS_ON_LAND_TOGGLE
	SDL_SCANCODE_9,						// SHORTCUT_HEIGHT_MARKS_ON_RIDE_TRACKS_TOGGLE
	SDL_SCANCODE_0,						// SHORTCUT_HEIGHT_MARKS_ON_PATHS_TOGGLE
	SDL_SCANCODE_F1,					// SHORTCUT_ADJUST_LAND
	SDL_SCANCODE_F2,					// SHORTCUT_ADJUST_WATER
	SDL_SCANCODE_F3,					// SHORTCUT_BUILD_SCENERY
	SDL_SCANCODE_F4,					// SHORTCUT_BUILD_PATHS
	SDL_SCANCODE_F5,					// SHORTCUT_BUILD_NEW_RIDE
	SDL_SCANCODE_F,						// SHORTCUT_SHOW_FINANCIAL_INFORMATION
	SDL_SCANCODE_D,						// SHORTCUT_SHOW_RESEARCH_INFORMATION
	SDL_SCANCODE_R,						// SHORTCUT_SHOW_RIDES_LIST
	SDL_SCANCODE_P,						// SHORTCUT_SHOW_PARK_INFORMATION
	SDL_SCANCODE_G,						// SHORTCUT_SHOW_GUEST_LIST
	SDL_SCANCODE_S,						// SHORTCUT_SHOW_STAFF_LIST
	SDL_SCANCODE_M,						// SHORTCUT_SHOW_RECENT_MESSAGES
	SDL_SCANCODE_TAB,					// SHORTCUT_SHOW_MAP
	0x0200 | SDL_SCANCODE_S,			// SHORTCUT_SCREENSHOT

	// New
	SDL_SCANCODE_MINUS,					// SHORTCUT_REDUCE_GAME_SPEED,
	SDL_SCANCODE_EQUALS,				// SHORTCUT_INCREASE_GAME_SPEED,
	0x0200 | 0x0400 | SDL_SCANCODE_C 	// SHORTCUT_OPEN_CHEAT_WINDOW,
};

/**
 * 
 *  rct2: 0x006E3604
 */
void config_reset_shortcut_keys()
{
	memcpy(gShortcutKeys, _defaultShortcutKeys, sizeof(gShortcutKeys));
}

void config_shortcut_keys_get_path(char *outPath)
{
	platform_get_user_directory(outPath, NULL);
	strcat(outPath, "hotkeys.cfg");
}

bool config_shortcut_keys_load()
{
	char path[MAX_PATH];
	FILE *file;
	int result;

	config_shortcut_keys_get_path(path);

	file = fopen(path, "rb");
	if (file != NULL) {
		result = fread(gShortcutKeys, sizeof(gShortcutKeys), 1, file) == 1;
		fclose(file);
	} else {
		result = false;
	}

	return result;
}

bool config_shortcut_keys_save()
{
	char path[MAX_PATH];
	FILE *file;
	int result;

	config_shortcut_keys_get_path(path);

	file = fopen(path, "wb");
	if (file != NULL) {
		result = fwrite(gShortcutKeys, sizeof(gShortcutKeys), 1, file) == 1;
		fclose(file);
	} else {
		result = false;
	}

	return result;
}

#pragma endregion


#pragma region Themes

static void themes_remove_extension(char *path);
static bool themes_open(const utf8string path);
static bool themes_save(const utf8string path, int preset);
static void themes_read_properties(int preset, window_colours **current_window_colours, const_utf8string line);
static void themes_set_property(window_colours *colour_scheme, utf8string name, utf8string value);


void themes_set_default()
{

	utf8 path[MAX_PATH];

	platform_get_user_directory(path, "themes");
	platform_ensure_directory_exists(path);

	gConfigThemes.num_presets = 2;
	gConfigThemes.presets = malloc(sizeof(theme_preset) * gConfigThemes.num_presets);

	// Set RCT2 colour scheme
	strcpy(gConfigThemes.presets[0].name, language_get_string(2741));
	gConfigThemes.presets[0].colour_schemes = malloc(sizeof(window_colours) * gNumColourSchemeWindows);
	for (int i = 0; i < (int)gNumColourSchemeWindows; i++) {
		for (int j = 0; j < 6; j++)
			gConfigThemes.presets[0].colour_schemes[i].colours[j] = gColourSchemes[i].colours[j];
	}

	// Set RCT1 colour scheme
	strcpy(gConfigThemes.presets[1].name, language_get_string(2740));
	gConfigThemes.presets[1].colour_schemes = malloc(sizeof(window_colours) * gNumColourSchemeWindows);
	for (int i = 0; i < (int)gNumColourSchemeWindows; i++) {
		uint8 changed_colour = 0xFF;
		for (int k = 0; gColourSchemesRCT1[k].classification != 0xFF; k++) {
			if (gColourSchemesRCT1[k].classification == gColourSchemes[i].classification) {
				changed_colour = (uint8)k;
				break;
			}
		}
		for (int j = 0; j < 6; j++) {
			gConfigThemes.presets[1].colour_schemes[i].colours[j] = (changed_colour != 0xFF ? gColourSchemesRCT1[changed_colour].colours[j] : gColourSchemes[i].colours[j]);
		}
	}
}

void themes_load_presets()
{
	utf8 path[MAX_PATH];
	file_info file;
	int fileEnumHandle, i;

	platform_get_user_directory(path, "themes");
	strcat(path, "*.ini");
	fileEnumHandle = platform_enumerate_files_begin(path);
	while (platform_enumerate_files_next(fileEnumHandle, &file)) {
		platform_get_user_directory(path, "themes");
		strcat(path, file.path);
		themes_open(path);
	}
	platform_enumerate_files_end(fileEnumHandle);

	if (strcmp(gConfigInterface.current_theme_preset, "*RCT2") == 0) {
		theme_change_preset(0);
	}
	else if (strcmp(gConfigInterface.current_theme_preset, "*RCT1") == 0) {
		theme_change_preset(1);
	}
	else {
		for (i = 2; i < gConfigThemes.num_presets; i++) {
			if (strcmp(gConfigInterface.current_theme_preset, gConfigThemes.presets[i].name) == 0) {
				theme_change_preset(i);
				break;
			}
		}
		if (i == gConfigThemes.num_presets) {
			theme_change_preset(0);
		}
	}
}

bool themes_save_preset(int preset)
{
	utf8 path[MAX_PATH];
	
	platform_get_user_directory(path, "themes");
	strcat(path, gConfigThemes.presets[preset].name);
	strcat(path, ".ini");
	if (themes_save(path, preset)) {
		return true;
	}

	return false;
}

bool themes_open(const utf8string path)
{
	FILE *file;
	uint8 *lineBuffer;
	size_t lineBufferCapacity;
	size_t lineLength;
	int c, preset;
	window_colours *currentColourScheme;

	file = fopen(path, "rb");
	if (file == NULL)
		return false;

	// Check if the colour scheme is already loaded
	// No nead to read the first two presets as they're hardcoded in
	for (preset = 2; preset < gConfigThemes.num_presets; preset++) {
		if (strcmp(path, gConfigThemes.presets[preset].name) == 0) {
			break;
		}
	}
	// Otherwise allocate one
	if (preset == gConfigThemes.num_presets) {
		gConfigThemes.num_presets++;
		gConfigThemes.presets = realloc(gConfigThemes.presets, sizeof(theme_preset) * gConfigThemes.num_presets);
		strcpy(gConfigThemes.presets[preset].name, path_get_filename(path));
		themes_remove_extension(gConfigThemes.presets[preset].name);
		gConfigThemes.presets[preset].colour_schemes = malloc(sizeof(window_colours) * gNumColourSchemeWindows);
		for (int i = 0; i < (int)gNumColourSchemeWindows; i++) {
			for (int j = 0; j < 6; j++)
				gConfigThemes.presets[preset].colour_schemes[i].colours[j] = gColourSchemes[i].colours[j];
		}
	}

	currentColourScheme = NULL;
	lineBufferCapacity = 64;
	lineBuffer = malloc(lineBufferCapacity);
	lineLength = 0;

	// Skim UTF-8 byte order mark
	fread(lineBuffer, 3, 1, file);
	if (!(lineBuffer[0] == 0xEF && lineBuffer[1] == 0xBB && lineBuffer[2] == 0xBF))
		fseek(file, 0, SEEK_SET);

	while ((c = fgetc(file)) != EOF) {
		if (c == '\n' || c == '\r') {
			lineBuffer[lineLength++] = 0;
			themes_read_properties(preset, &currentColourScheme, (const_utf8string)lineBuffer);
			lineLength = 0;
		}
		else {
			lineBuffer[lineLength++] = c;
		}

		if (lineLength >= lineBufferCapacity) {
			lineBufferCapacity *= 2;
			lineBuffer = realloc(lineBuffer, lineBufferCapacity);
		}
	}

	if (lineLength > 0) {
		lineBuffer[lineLength++] = 0;
		themes_read_properties(preset, &currentColourScheme, lineBuffer);
	}

	free(lineBuffer);
	fclose(file);
	return true;
}

static bool themes_save(const utf8string path, int preset)
{
	FILE *file;
	int i, j;

	file = fopen(path, "wb");
	if (file == NULL) {
		log_error("Unable to write to colour scheme file.");
		return false;
	}

	for (i = 0; i < (int)gNumColourSchemeWindows; i++) {
		window_colour_scheme* colour_scheme_info = &gColourSchemes[i];
		window_colours* colour_scheme = &gConfigThemes.presets[preset].colour_schemes[i];

		fputc('[', file);
		fwrite(colour_scheme_info->section_name, strlen(colour_scheme_info->section_name), 1, file);
		fputc(']', file);
		fputc('\n', file);

		for (j = 0; j < colour_scheme_info->num_colours; j++) {

			fprintf(file, "colour_%d", j);
			fwrite(" = ", 3, 1, file);
			fprintf(file, "%u", colour_scheme->colours[j]);
			fputc('\n', file);
		}
		fputc('\n', file);
	}

	fclose(file);
	return true;
}

static void themes_read_properties(int preset, window_colours **colour_scheme, const_utf8string line)
{
	utf8string ch = (utf8string)line;
	utf8_skip_whitespace(&ch);

	if (*ch == '[') {
		utf8string sectionName;
		int sectionNameSize;
		if (config_get_section(ch, &sectionName, &sectionNameSize)) {
			sectionName[strlen(sectionName) - 1] = '\0';
			for (int i = 0; i < (int)gNumColourSchemeWindows; i++) {
				if (strcmp(sectionName, gColourSchemes[i].section_name) == 0) {
					*colour_scheme = &(gConfigThemes.presets[preset].colour_schemes[i]);
					break;
				}
			}
		}
	}
	else {
		if (*colour_scheme != NULL) {
			utf8string propertyName, value;
			int propertyNameSize, valueSize;
			if (config_get_property_name_value(ch, &propertyName, &propertyNameSize, &value, &valueSize)) {
				propertyName[propertyNameSize] = '\0';
				themes_set_property(*colour_scheme, propertyName, value);
			}
		}
	}
}

static void themes_set_property(window_colours *colour_scheme, utf8string name, utf8string value)
{
	const_utf8string colour_names[] = { "colour_0", "colour_1", "colour_2", "colour_3", "colour_4", "colour_5" };

	for (int i = 0; i < 6; i++) {
		if (strcmp(name, colour_names[i]) == 0) {
			colour_scheme->colours[i] = (uint8)strtol(value, NULL, 0);
		}
	}
}

static void themes_remove_extension(char *path)
{
	char *ch;

	for (ch = path; *ch != 0; ch++) {
		if (*ch == '.') {
			*ch = '\0';
			break;
		}
	}
}

#pragma endregion