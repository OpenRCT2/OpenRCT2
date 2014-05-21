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
#include <shlobj.h>
#include <windows.h>
#include <SDL_keycode.h>
#include "addresses.h"
#include "config.h"
#include "rct2.h"
#include <tchar.h>

#include "osinterface.h"

// Current keyboard shortcuts
uint16 gShortcutKeys[SHORTCUT_COUNT];

// Magic number for original game cfg file
static const int MagicNumber = 0x0003113A;

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
	0x0200 | SDL_SCANCODE_S				// SHORTCUT_SCREENSHOT
};

/**
 * 
 *  rct2: 0x006E3604
 */
void config_reset_shortcut_keys()
{
	memcpy(gShortcutKeys, _defaultShortcutKeys, sizeof(gShortcutKeys));
}

/**
 *  Reads the config file data/config.cfg
 *  rct2: 0x006752D5
 */
void config_load()
{
	HANDLE hFile;
	DWORD bytesRead;

	char* path = get_file_path(PATH_ID_GAMECFG);
	hFile = CreateFile(get_file_path(PATH_ID_GAMECFG), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING,
		FILE_FLAG_RANDOM_ACCESS | FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile != INVALID_HANDLE_VALUE) {
		// Read and check magic number
		ReadFile(hFile, RCT2_ADDRESS(0x013CE928, void), 4, &bytesRead, NULL);
		if (RCT2_GLOBAL(0x013CE928, int) == MagicNumber) {
			// Read options
			ReadFile(hFile, (void*)0x009AAC5C, 2155, &bytesRead, NULL);
			CloseHandle(hFile);


			//general configuration
			RCT2_GLOBAL(RCT2_ADDRESS_CONFIG_EDGE_SCROLLING, sint8) = gGeneral_config.edge_scrolling;
			RCT2_GLOBAL(RCT2_ADDRESS_CONFIG_CURRENCY, sint8) = gGeneral_config.currency_format; 
			RCT2_GLOBAL(RCT2_ADDRESS_CONFIG_METRIC, sint8) = gGeneral_config.measurement_format;
			RCT2_GLOBAL(RCT2_ADDRESS_CONFIG_TEMPERATURE, sint8) = gGeneral_config.temperature_format;
			

			//sound configuration
			RCT2_GLOBAL(RCT2_ADDRESS_CONFIG_SOUND_QUALITY, sint8) = gSound_config.sound_quality;
			RCT2_GLOBAL(RCT2_ADDRESS_CONFIG_SOUND_SW_BUFFER, sint8) = gSound_config.forced_software_buffering; 

			// Line below is temporaraly disabled until all config is in the new format.
			//if (RCT2_GLOBAL(0x009AB4C6, sint8) == 1) 
			//	return;
			
			
			RCT2_GLOBAL(0x009AB4C6, sint8) = 1; // no idea on what this does

			RCT2_GLOBAL(0x009AACBD, sint16) = (RCT2_GLOBAL(RCT2_ADDRESS_CONFIG_METRIC, sint8) + 2) * 256;
			if (!(RCT2_GLOBAL(RCT2_ADDRESS_CONFIG_FLAGS, uint8) & CONFIG_FLAG_SHOW_HEIGHT_AS_UNITS))
				RCT2_GLOBAL(0x009AACBD, sint16) = (RCT2_GLOBAL(RCT2_ADDRESS_CONFIG_METRIC, sint8) + 1) * 256;
			RCT2_GLOBAL(0x009AA00D, sint8) = 0;
		}
	
	}
	
	/* TODO: CLEANUP

			if (RCT2_GLOBAL(0x009AB4C6, sint8) == 1)
				return;
			RCT2_GLOBAL(0x009AB4C6, sint8) = 1;
			RCT2_GLOBAL(RCT2_ADDRESS_CONFIG_METRIC, sint8) = 0; 
			RCT2_GLOBAL(RCT2_ADDRESS_CONFIG_TEMPERATURE, sint8) = 1; 
			RCT2_GLOBAL(RCT2_ADDRESS_CONFIG_CURRENCY, sint8) = 1;
			RCT2_GLOBAL(RCT2_ADDRESS_CONFIG_HEIGHT_MARKERS, sint16) = 0;
			if (!(RCT2_GLOBAL(RCT2_ADDRESS_CONFIG_FLAGS, uint8) & CONFIG_FLAG_SHOW_HEIGHT_AS_UNITS))
				RCT2_GLOBAL(RCT2_ADDRESS_CONFIG_HEIGHT_MARKERS, sint16) = (RCT2_GLOBAL(RCT2_ADDRESS_CONFIG_METRIC, sint8) + 1) * 256;
			RCT2_GLOBAL(0x009AA00D, sint8) = 1;
		}
	
	}

	RCT2_GLOBAL(RCT2_ADDRESS_CONFIG_SOUND_QUALITY, sint8) = 0;
	if (RCT2_GLOBAL(RCT2_ADDRESS_MEM_TOTAL_PHYSICAL, uint32) > 0x4000000) {
		RCT2_GLOBAL(RCT2_ADDRESS_CONFIG_SOUND_QUALITY, sint8) = 1;
		if (RCT2_GLOBAL(RCT2_ADDRESS_MEM_TOTAL_PHYSICAL, uint32) > 0x8000000)
			RCT2_GLOBAL(RCT2_ADDRESS_CONFIG_SOUND_QUALITY, sint8) = 2;
	}
	*/


	RCT2_GLOBAL(0x009AAC75, sint8) = RCT2_ADDRESS(0x009AF601, sint8)[RCT2_GLOBAL(RCT2_ADDRESS_CONFIG_SOUND_QUALITY, sint8)];
	RCT2_GLOBAL(0x009AAC76, sint8) = RCT2_ADDRESS(0x009AF604, sint8)[RCT2_GLOBAL(RCT2_ADDRESS_CONFIG_SOUND_QUALITY, sint8)];
	RCT2_GLOBAL(RCT2_ADDRESS_CONFIG_HEIGHT_MARKERS, sint16) = 0;
	if (!(RCT2_GLOBAL(RCT2_ADDRESS_CONFIG_FLAGS, uint8) & CONFIG_FLAG_SHOW_HEIGHT_AS_UNITS))
		RCT2_GLOBAL(RCT2_ADDRESS_CONFIG_HEIGHT_MARKERS, sint16) = (RCT2_GLOBAL(RCT2_ADDRESS_CONFIG_METRIC, sint8) + 1) * 256;
	RCT2_GLOBAL(0x009AA00D, sint8) = 1;
}

/**
 *  Save configuration to the data/config.cfg file
 *  rct2: 0x00675487
 */
void config_save()
{
	HANDLE hFile;
	DWORD bytesWritten;

	hFile = CreateFile(get_file_path(PATH_ID_GAMECFG), GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile != INVALID_HANDLE_VALUE) {
		WriteFile(hFile, &MagicNumber, 4, &bytesWritten, NULL);
		WriteFile(hFile, (LPCVOID)0x009AAC5C, 2155, &bytesWritten, NULL);
		CloseHandle(hFile);
	}
}

// New config format

general_configuration_t gGeneral_config;
sound_configuration_t gSound_config;

static char *config_show_directory_browser();
static void config_parse_settings(FILE *fp);
static void config_general(char *setting, char *value);
static void config_sound(char *setting, char *value);
static int config_get_line(FILE *fp, char *setting, char *value);
static int config_parse_setting(FILE *fp, char *setting);
static int config_parse_value(FILE *fp, char *value);
static int config_parse_section(FILE *fp, char *setting, char *value);
static void config_create_default(char *path);
static int config_parse_currency(char* currency);
static void config_error(char *msg);

/**
 * Initilise the settings.
 * It checks if the OpenRCT2 folder exists and creates it if it does not
 * parsing of the config file is done in config_parse_settings
 */
void config_init()
{	
	TCHAR path[MAX_PATH];
	FILE* fp;

	if (SUCCEEDED(SHGetFolderPath(NULL, CSIDL_PERSONAL | CSIDL_FLAG_CREATE, NULL, 0, path))) { // find home folder
		strcat(path, "\\OpenRCT2");
		DWORD dwAttrib = GetFileAttributes(path);
		if (dwAttrib == INVALID_FILE_ATTRIBUTES || !(dwAttrib & FILE_ATTRIBUTE_DIRECTORY)) { // folder does not exist
			if (!CreateDirectory(path, NULL)) {
				config_error("Could not create config file (do you have write access to your documents folder?)");
			}
		}
		strcat(path, "\\config.ini");
		fp = fopen(path, "r");
		if (!fp) {
			config_create_default(path);
			fp = fopen(path, "r");
			if (!fp)
				config_error("Could not create config file");
		}

		config_parse_settings(fp);

		fclose(fp);
	}
}

/**
 * Attempts to find the RCT2 installation directory.
 * This should be created from some other resource when OpenRCT2 grows.
 * @param resultPath Pointer to where the absolute path of the RCT2 installation directory will be copied to.
 * @returns 1 if successful, otherwise 0.
 */
static int config_find_rct2_path(char *resultPath)
{
	int i;
	DWORD dwAttrib;
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
		dwAttrib = GetFileAttributes(searchLocations[i]);
		if (dwAttrib != INVALID_FILE_ATTRIBUTES && (dwAttrib & FILE_ATTRIBUTE_DIRECTORY)) {
			strcpy(resultPath, searchLocations[i]);
			return 1;
		}
	}

	return 0;
}

/**
 * Create a new default settings file.
 * This should be created from some other resource when openRCT2 grows
 * @param path The aboslute path of the config file
 */
static void config_create_default(char *path)
{
	FILE* fp;


	if (!config_find_rct2_path(gGeneral_config.game_path)) {
		osinterface_show_messagebox("Unable to find RCT2 installation directory. Please select the directory where you installed RCT2!");
		char *res = osinterface_open_directory_browser("Please select your RCT2 directory");
		strcpy(gGeneral_config.game_path, res);
	}

	fp = fopen(path, "w");
	fprintf(fp, "[general]\n");
	fprintf(fp, "game_path = %s\n", gGeneral_config.game_path);
	fprintf(fp, "screenshot_format = PNG\n");
	fprintf(fp, "play_intro = false\n");
	fprintf(fp, "edge_scrolling = true\n");
	fprintf(fp, "currency = GBP\n");
	fprintf(fp, "use_imperial = false\n");
	fprintf(fp, "use_farenheit = false\n");
	fprintf(fp, "[sound]\n");
	fprintf(fp, "sound_quality = high\n");
	fprintf(fp, "forced_software_buffering = false\n");
	fclose(fp);
}


/**
 * Parse settings and set the game veriables
 * @param fp file pointer to the settings file
 */
static void config_parse_settings(FILE *fp)
{
	int pos = 0;
	char *setting;
	char *value;
	char *section;
	setting = (char *)malloc(MAX_CONFIG_LENGTH);
	value = (char *)malloc(MAX_CONFIG_LENGTH);
	section = (char*)malloc(MAX_CONFIG_LENGTH);
	
	while (config_get_line(fp, setting, value) > 0) {
		if (strcmp(setting, "section") == 0){ 
			strcpy(section, value);
			continue;
		}
		
		if (strcmp(section, "sound") == 0){
			config_sound(setting, value);
		}
		else if (strcmp(section, "general") == 0){
			config_general(setting, value);
		}
		
	
		
	}
	//RCT2_GLOBAL(0x009AACBC, sint8) = CURRENCY_KRONA;
	
	
	
	free(setting);
	free(value);
	free(section);
}


static void config_sound(char *setting, char *value){
	if (strcmp(setting, "sound_quality") == 0){
		if (strcmp(value, "low") == 0){
			gSound_config.sound_quality = SOUND_QUALITY_LOW;
		}
		else if (strcmp(value, "medium") == 0){
			gSound_config.sound_quality = SOUND_QUALITY_MEDIUM;
		}
		else{
			gSound_config.sound_quality = SOUND_QUALITY_HIGH;
		}
	}
	else if (strcmp(setting, "forced_software_buffering") == 0){
		if (strcmp(value, "true") == 0){
			gSound_config.forced_software_buffering = 1;
		}
		else{
			gSound_config.forced_software_buffering = 0;
		}
	}

}

static void config_general(char *setting, char *value){
	if (strcmp(setting, "game_path") == 0){
		strcpy(gGeneral_config.game_path, value);
	}
	else if (strcmp(setting, "screenshot_format") == 0) {
		if (strcmp(value, "png") == 0 || strcmp(value, "PNG") == 0) {
			gGeneral_config.screenshot_format = SCREENSHOT_FORMAT_PNG;
		}
		else if (strcmp(value, "1") == 0) { //TODO: REMOVE LINE AT LATER DATE WHEN EVERYONE HAS NEW CONFIG FORMAT
			gGeneral_config.screenshot_format = SCREENSHOT_FORMAT_PNG;
		}
		else {
			gGeneral_config.screenshot_format = SCREENSHOT_FORMAT_BMP;
		}
	}
	else if (strcmp(setting, "play_intro") == 0) {
		gGeneral_config.play_intro = (strcmp(value, "true") == 0);
	}
	else if (strcmp(setting, "edge_scrolling") == 0){
		if (strcmp(value, "true") == 0){
			gGeneral_config.edge_scrolling = 1;
		}
		else {
			gGeneral_config.edge_scrolling = 0;
		}
	}
	else if (strcmp(setting, "use_imperial") == 0){
		if (strcmp(value, "true") == 0){
			gGeneral_config.measurement_format = MEASUREMENT_FORMAT_IMPERIAL;
		}
		else{
			gGeneral_config.measurement_format = MEASUREMENT_FORMAT_METRIC;
		}
	}
	else if (strcmp(setting, "use_farenheit") == 0){
		if (strcmp(value, "true") == 0){
			gGeneral_config.temperature_format = TEMPERATURE_FORMAT_F;
		}
		else{
			gGeneral_config.temperature_format = TEMPERATURE_FORMAT_C;
		}
	}
	else if (strcmp(setting, "currency") == 0){
		config_parse_currency(value);
	}

}

/**
 * Read one line in the settings file
 * @param fp filepointer to the config file
 * @param setting pointer where to to store the setting
 * @param value pointer to where to store the value 
 * @return < 0 if EOF file is reached or other error
 */
static int config_get_line(FILE *fp, char *setting, char *value)
{
	int c;
	
	c = fgetc(fp);
	while (isspace(c)){
		c = getc(fp);
	}

	if (c == '['){
		return config_parse_section(fp, setting, value);
	}
	else if(c == '#'){
		while (c != '\n'){
			c = fgetc(fp);
		}
		return 1;
	}
	if (c == EOF){
		return -1;
	}
	
	while (!isalpha(c)){
		c = fgetc(fp);
	}
	
	//if the first char is not the '[' char, it belongs to the setting name. We want to leave that for the next fn
	fseek(fp, -1, SEEK_CUR);
	
	config_parse_setting(fp, setting);

	c = fgetc(fp);
	while (isspace(c)){
		c = getc(fp);
	}
	
	if (c != '='){
		config_error("There is an error in your configuration file");
		return -1;
	}

	config_parse_value(fp, value);
	return 1;

	
}

/**
* Parse the value of a setting
* @param fp a filepointer to the config file
* @param value a pointer to where to store the setting
* @return < 0 if EOF is reached
*/
static int config_parse_setting(FILE *fp, char *setting){
	long start, end;
	int size, c, pos = 0;
	
	
	start = ftell(fp);
	c = fgetc(fp);
	
	while (isspace(c)){
		start = ftell(fp);
		c = fgetc(fp);

	}
	if (c == EOF){
		return -1;
	}

	while (isalpha(c) || c == '_'){
		c = fgetc(fp);
	}

	end = ftell(fp);
	size = end - start;
	
	
	fseek(fp, start, SEEK_SET);
	c = fgetc(fp);
	while (isalpha(c) || c == '_'){
		setting[pos] = (char)c;
		c = fgetc(fp);
		pos++;
	}
	setting[pos] = '\0';
	return 1;
}

/**
 * Parse the value of a setting
 * @param fp a filepointer to the config file
 * @param value a pointer to where to store the value
 * @return < 0 if EOF is reached
 */
static int config_parse_value(FILE *fp, char *value)
{
	long start, end;
	int size, c, pos = 0;

	start = ftell(fp);
	c = fgetc(fp);
	while (isspace(c)) {
		start = ftell(fp);
		c = fgetc(fp);
	}
	
	while (c != EOF && c != '\n') {
		c = fgetc(fp);		
	}

	end = ftell(fp);
	size = end - start;
	if (size > MAX_CONFIG_LENGTH)
		config_error("One of your settings is too long");
	
	fseek(fp, start, SEEK_SET);
	c = fgetc(fp);
	while (c != EOF && c != '\n') {
		value[pos] = (char)c;
		c = fgetc(fp);
		pos++;
	}
	value[pos] = '\0';
	return 0;
}

/**
 * Parse the current section
 * @param fp Filepointer to the config file
 * @param setting This is set to contain the string "section"
 * @param value Pointer to where the section name should be put
 * @return < 0 if EOF is reached
 */
static int config_parse_section(FILE *fp, char *setting, char *value){
	int size, c, pos = 0;
	long start, end;
		
	strcpy(setting, "section\0");
	c = fgetc(fp);
	start = ftell(fp);
	while (c != ']' && c != EOF){
		c = fgetc(fp);
	}
	end = ftell(fp);
	size = end - start;
	fseek(fp, start - 1, SEEK_SET);
	c = fgetc(fp);
	while (c != ']' && c != EOF){
		value[pos] = (char)c;
		c = fgetc(fp);
		pos++;
	}
	
	value[pos] = '\0';
	if (c != ']'){
		config_error("There is an error with the section headers");
	}
	c = fgetc(fp); //devour ']'	
	
	return 1;
}

static const struct { char *key; int value; } _currencyLookupTable[] = {
	{ "GBP", CURRENCY_POUNDS },
	{ "USD", CURRENCY_DOLLARS },
	{ "FRF", CURRENCY_FRANC },
	{ "DEM", CURRENCY_DEUTSCHMARK },
	{ "YEN", CURRENCY_YEN },
	{ "ESP", CURRENCY_PESETA },
	{ "ITL", CURRENCY_LIRA },
	{ "NLG", CURRENCY_GUILDERS },
	{ "NOK", CURRENCY_KRONA },
	{ "SEK", CURRENCY_KRONA },
	{ "DEK", CURRENCY_KRONA },
	{ "EUR", CURRENCY_EUROS },

	{ "£", CURRENCY_POUNDS },
	{ "$", CURRENCY_DOLLARS },
	{ "€", CURRENCY_EUROS }
};

static int config_parse_currency(char *currency)
{
	int i;
	for (i = 0; i < countof(_currencyLookupTable); i++) {
		if (_strcmpi(currency, _currencyLookupTable[i].key) == 0) {
			gGeneral_config.currency_format = _currencyLookupTable[i].value;
			return 1;
		}
	}

	config_error("Invalid currency set in config file");
	return -1;
}
/**
 * Error with config file. Print error message an quit the game
 * @param msg Message to print in message box
 */
static void config_error(char *msg){

	osinterface_show_messagebox(msg);
	//TODO:SHUT DOWN EVERYTHING!

}

