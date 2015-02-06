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

#include <SDL_keycode.h>
#include "addresses.h"
#include "config.h"
#include "localisation/localisation.h"
#include "platform/osinterface.h"
#include "platform/platform.h"

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
	0x0200 | SDL_SCANCODE_S,			// SHORTCUT_SCREENSHOT

	// New
	SDL_SCANCODE_MINUS,					// SHORTCUT_REDUCE_GAME_SPEED,
	SDL_SCANCODE_EQUALS,				// SHORTCUT_INCREASE_GAME_SPEED,
	0x0200 | 0x0400 | SDL_SCANCODE_C 	// SHORTCUT_OPEN_CHEAT_WINDOW,
};

general_configuration_t gGeneral_config;
general_configuration_t gGeneral_config_default = {
	0,								// play_intro
	0,								// confirmation_prompt
	SCREENSHOT_FORMAT_PNG,			// screenshot_format
	"",								// game_path
	MEASUREMENT_FORMAT_IMPERIAL,	// measurement_format
	TEMPERATURE_FORMAT_C,			// temperature_format
	CURRENCY_POUNDS,				// currency_format
	0,								// construction_marker_colour
	1,								// edge_scrolling
	0,								// always_show_gridlines
	1,								// landscape_smoothing
	0,								// show_height_as_units
	1,								// save_plugin_data
	0,								// fullscreen mode (default: windowed)
	-1,								// fullscreen_width
	-1,								// fullscreen_height
	-1,								// window_width
	-1,								// window_height
	LANGUAGE_ENGLISH_UK,			// language
	5								// window_snap_proximity
};
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

void config_save_ini(char *path);
void config_write_ini_general(FILE *fp);
void config_write_ini_sound(FILE *fp);

/**
 * 
 *  rct2: 0x006E3604
 */
void config_reset_shortcut_keys()
{
	memcpy(gShortcutKeys, _defaultShortcutKeys, sizeof(gShortcutKeys));
}

void config_save_ini(char *path)
{
	FILE *fp = NULL;


	fp = fopen(path, "wt+");

	config_write_ini_general(fp);
	config_write_ini_sound(fp);

	fclose(fp);
}

void config_write_ini_sound(FILE *fp)
{
	fprintf(fp, "[sound]\n");
	if (gSound_config.sound_quality == SOUND_QUALITY_LOW) {
		fprintf(fp, "sound_quality = low\n");
	}
	else if (gSound_config.sound_quality == SOUND_QUALITY_MEDIUM) {
		fprintf(fp, "sound_quality = medium\n");
	}
	else{
		fprintf(fp, "sound_quality = high\n");
	}
	
	if (gSound_config.forced_software_buffering){
		fprintf(fp, "forced_software_buffering = true\n");
	}
	else {
		fprintf(fp, "forced_software_buffering = false\n");
	}
}

void config_write_ini_general(FILE *fp)
{
	int currencyIterator = 0;

	fprintf(fp, "[general]\n");
	fprintf(fp, "game_path = %s\n", gGeneral_config.game_path);

	switch (gGeneral_config.screenshot_format)
	{
	case SCREENSHOT_FORMAT_BMP:
		fprintf(fp, "screenshot_format = BMP\n");
		break;
	case SCREENSHOT_FORMAT_PNG:
		fprintf(fp, "screenshot_format = PNG\n");
		break;
	default:
		config_error("error saving config.ini: wrong screenshot_format");
		break;
	}

	if (gGeneral_config.play_intro){
		fprintf(fp, "play_intro = true\n");
	}
	else {
		fprintf(fp, "play_intro = false\n");
	}

	if (gGeneral_config.confirmation_prompt){
		fprintf(fp, "confirmation_prompt = true\n");
	}
	else {
		fprintf(fp, "confirmation_prompt = false\n");
	}

	if (gGeneral_config.edge_scrolling){
		fprintf(fp, "edge_scrolling = true\n");
	}
	else {
		fprintf(fp, "edge_scrolling = false\n");
	}

	for (currencyIterator = 0; currencyIterator < countof(_currencyLookupTable); currencyIterator++) {
		if (_currencyLookupTable[currencyIterator].value == gGeneral_config.currency_format) {
			gGeneral_config.currency_format = _currencyLookupTable[currencyIterator].value;
			fprintf(fp, "currency = %s\n", _currencyLookupTable[currencyIterator].key);
			break; // There are more than one valid item for Pound, Euro and Dollar ...
		}
	}

	if (gGeneral_config.measurement_format == MEASUREMENT_FORMAT_IMPERIAL) {
		fprintf(fp, "measurement_format = imperial\n");
	}
	else {
		fprintf(fp, "measurement_format = metric\n");
	}

	if (gGeneral_config.temperature_format == TEMPERATURE_FORMAT_F) {
		fprintf(fp, "temperature_format = fahrenheit\n");
	}
	else {
		fprintf(fp, "temperature_format = celsius\n");
	}

	if (gGeneral_config.always_show_gridlines){
		fprintf(fp, "always_show_gridlines = true\n");
	}
	else {
		fprintf(fp, "always_show_gridlines = false\n");
	}

	if (gGeneral_config.landscape_smoothing){
		fprintf(fp, "landscape_smoothing = true\n");
	}
	else {
		fprintf(fp, "landscape_smoothing = false\n");
	}

	if (gGeneral_config.show_height_as_units){
		fprintf(fp, "show_height_as_units = true\n");
	}
	else {
		fprintf(fp, "show_height_as_units = false\n");
	}

	if (gGeneral_config.save_plugin_data){
		fprintf(fp, "save_plugin_data = true\n");
	}
	else {
		fprintf(fp, "save_plugin_data = false\n");
	}

	if (gGeneral_config.fullscreen_mode == 0)
		fprintf(fp, "fullscreen_mode = window\n");
	else if (gGeneral_config.fullscreen_mode == 1)
		fprintf(fp, "fullscreen_mode = fullscreen\n");
	else
		fprintf(fp, "fullscreen_mode = borderless_fullscreen\n");

	if (gGeneral_config.fullscreen_width != -1)
		fprintf(fp, "fullscreen_width = %d\n", gGeneral_config.fullscreen_width);
	if (gGeneral_config.fullscreen_height != -1)
		fprintf(fp, "fullscreen_height = %d\n", gGeneral_config.fullscreen_height);

	if (gGeneral_config.window_width != -1)
		fprintf(fp, "window_width = %d\n", gGeneral_config.window_width);
	if (gGeneral_config.window_height != -1)
		fprintf(fp, "window_height = %d\n", gGeneral_config.window_height);

	fprintf(fp, "language = %d\n", gGeneral_config.language);

	fprintf(fp, "window_snap_proximity = %d\n", gGeneral_config.window_snap_proximity);
}

/**
 * Any code not implemented in OpenRCT2 will still uses the old configuration option addresses. This function copies all the
 * OpenRCT2 configuration options to those addresses until the process is no longer necessary.
 */
void config_apply_to_old_addresses()
{
	RCT2_GLOBAL(RCT2_ADDRESS_CONFIG_EDGE_SCROLLING, sint8) = gGeneral_config.edge_scrolling;
	RCT2_GLOBAL(RCT2_ADDRESS_CONFIG_CURRENCY, sint8) = gGeneral_config.currency_format; 
	RCT2_GLOBAL(RCT2_ADDRESS_CONFIG_METRIC, sint8) = gGeneral_config.measurement_format;
	RCT2_GLOBAL(RCT2_ADDRESS_CONFIG_TEMPERATURE, sint8) = gGeneral_config.temperature_format;
	RCT2_GLOBAL(RCT2_ADDRESS_CONFIG_CONSTRUCTION_MARKER, uint8) = gGeneral_config.construction_marker_colour;
	RCT2_GLOBAL(RCT2_ADDRESS_CONFIG_SOUND_QUALITY, sint8) = gSound_config.sound_quality;
	RCT2_GLOBAL(RCT2_ADDRESS_CONFIG_SOUND_SW_BUFFER, sint8) = gSound_config.forced_software_buffering; 
	RCT2_GLOBAL(RCT2_ADDRESS_CONFIG_HEIGHT_MARKERS, sint16) = (gGeneral_config.measurement_format + 1) * 256;
	if (gGeneral_config.show_height_as_units)
		RCT2_GLOBAL(RCT2_ADDRESS_CONFIG_HEIGHT_MARKERS, sint16) = 0;

	int configFlags = 0;
	if (gGeneral_config.always_show_gridlines)
		configFlags |= CONFIG_FLAG_ALWAYS_SHOW_GRIDLINES;
	if (!gGeneral_config.landscape_smoothing)
		configFlags |= CONFIG_FLAG_DISABLE_SMOOTH_LANDSCAPE;
	if (gGeneral_config.show_height_as_units)
		configFlags |= CONFIG_FLAG_SHOW_HEIGHT_AS_UNITS;
	if (gGeneral_config.save_plugin_data)
		configFlags |= CONFIG_FLAG_SAVE_PLUGIN_DATA;

	RCT2_GLOBAL(RCT2_ADDRESS_CONFIG_FLAGS, uint8) = configFlags;
}

/**
 * Initialise the settings.
 * It checks if the OpenRCT2 folder exists and creates it if it does not
 * parsing of the config file is done in config_parse_settings
 */
void config_load()
{	
	char *path = osinterface_get_orct2_homefolder();
	FILE* fp;

	memcpy(&gGeneral_config, &gGeneral_config_default, sizeof(general_configuration_t));

	if (strcmp(path, "") != 0){
		if (!platform_ensure_directory_exists(path)) {
			config_error("Could not create config file (do you have write access to your documents folder?)");
			return;
		}
		
		sprintf(path, "%s%c%s", path, osinterface_get_path_separator(), "config.ini");
		
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

	free(path);

	config_apply_to_old_addresses();
}

void config_save()
{
	char *configIniPath = osinterface_get_orct2_homefolder();;

	sprintf(configIniPath, "%s%c%s", configIniPath, osinterface_get_path_separator(), "config.ini");
	config_save_ini(configIniPath);

	config_apply_to_old_addresses();
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
			return 1;
		}
	}

	return 0;
}

int config_find_or_browse_install_directory()
{
	char *installPath;

	if (!config_find_rct2_path(gGeneral_config.game_path)) {
		osinterface_show_messagebox("Unable to find RCT2 installation directory. Please select the directory where you installed RCT2!");
		installPath = osinterface_open_directory_browser("Please select your RCT2 directory");
		if (installPath == NULL)
			return 0;

		strcpy(gGeneral_config.game_path, installPath);
	}

	config_save();
	return 1;
}

/**
 * Create a new default settings file.
 * This should be created from some other resource when openRCT2 grows
 * @param path The aboslute path of the config file
 */
static void config_create_default(char *path)
{
	gGeneral_config = gGeneral_config_default;
	if (!config_find_or_browse_install_directory()) {
		log_fatal("An RCT2 install directory must be specified!");
		exit(-1);
	}
	
	config_save_ini(path);
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
		if (strcmp(value, "png") == 0) {
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
	else if (strcmp(setting, "confirmation_prompt") == 0) {
		gGeneral_config.confirmation_prompt = (strcmp(value, "true") == 0);
	}
	else if (strcmp(setting, "edge_scrolling") == 0){
		if (strcmp(value, "true") == 0){
			gGeneral_config.edge_scrolling = 1;
		}
		else {
			gGeneral_config.edge_scrolling = 0;
		}
	}
	else if (strcmp(setting, "measurement_format") == 0){
		if (strcmp(value, "imperial") == 0){
			gGeneral_config.measurement_format = MEASUREMENT_FORMAT_IMPERIAL;
		}
		else{
			gGeneral_config.measurement_format = MEASUREMENT_FORMAT_METRIC;
		}
	}
	else if (strcmp(setting, "temperature_format") == 0){
		if (strcmp(value, "fahrenheit") == 0){
			gGeneral_config.temperature_format = TEMPERATURE_FORMAT_F;
		}
		else{
			gGeneral_config.temperature_format = TEMPERATURE_FORMAT_C;
		}
	}
	else if (strcmp(setting, "currency") == 0){
		config_parse_currency(value);
	}
	else if (strcmp(setting, "always_show_gridlines") == 0){
		if (strcmp(value, "true") == 0){
			gGeneral_config.always_show_gridlines = 1;
		}
		else {
			gGeneral_config.always_show_gridlines = 0;
		}
	}
	else if (strcmp(setting, "landscape_smoothing") == 0){
		if (strcmp(value, "true") == 0){
			gGeneral_config.landscape_smoothing = 1;
		}
		else {
			gGeneral_config.landscape_smoothing = 0;
		}
	}
	else if (strcmp(setting, "show_height_as_units") == 0){
		if (strcmp(value, "true") == 0){
			gGeneral_config.show_height_as_units = 1;
		}
		else {
			gGeneral_config.show_height_as_units = 0;
		}
	}
	else if (strcmp(setting, "save_plugin_data") == 0){
		if (strcmp(value, "true") == 0){
			gGeneral_config.save_plugin_data = 1;
		}
		else {
			gGeneral_config.save_plugin_data = 0;
		}
	}
	else if (strcmp(setting, "fullscreen_mode") == 0){
		if (strcmp(value, "window") == 0){
			gGeneral_config.fullscreen_mode = 0;
		}
		else if (strcmp(value, "fullscreen") == 0){
			gGeneral_config.fullscreen_mode = 1;
		}
		else
			gGeneral_config.fullscreen_mode = 2;
	}
	else if (strcmp(setting, "fullscreen_width") == 0) {
		gGeneral_config.fullscreen_width = atoi(value);
	}
	else if (strcmp(setting, "fullscreen_height") == 0) {
		gGeneral_config.fullscreen_height = atoi(value);
	}
	else if (strcmp(setting, "window_width") == 0) {
		gGeneral_config.window_width = atoi(value);
	}
	else if (strcmp(setting, "window_height") == 0) {
		gGeneral_config.window_height = atoi(value);
	}
	else if (strcmp(setting, "language") == 0) {
		gGeneral_config.language = atoi(value);
	}
	else if (strcmp(setting, "window_snap_proximity") == 0) {
		gGeneral_config.window_snap_proximity = clamp(0, atoi(value), 255);
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
		value[pos] = (char)tolower(c);
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

#pragma region Obsolete

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
			RCT2_GLOBAL(RCT2_ADDRESS_CONFIG_EDGE_SCROLLING, sint8) = gGeneral_config.edge_scrolling;
			RCT2_GLOBAL(RCT2_ADDRESS_CONFIG_CURRENCY, sint8) = gGeneral_config.currency_format; 
			RCT2_GLOBAL(RCT2_ADDRESS_CONFIG_METRIC, sint8) = gGeneral_config.measurement_format;
			RCT2_GLOBAL(RCT2_ADDRESS_CONFIG_TEMPERATURE, sint8) = gGeneral_config.temperature_format;
			
			// always show gridlines
			if (gGeneral_config.always_show_gridlines){
				RCT2_GLOBAL(RCT2_ADDRESS_CONFIG_FLAGS, uint8) |= CONFIG_FLAG_ALWAYS_SHOW_GRIDLINES;
			}
			else {
				RCT2_GLOBAL(RCT2_ADDRESS_CONFIG_FLAGS, uint8) &= !CONFIG_FLAG_ALWAYS_SHOW_GRIDLINES;
			}

			// landscape smoothing
			if (!gGeneral_config.landscape_smoothing){
				RCT2_GLOBAL(RCT2_ADDRESS_CONFIG_FLAGS, uint8) |= CONFIG_FLAG_DISABLE_SMOOTH_LANDSCAPE;
			}
			else {
				RCT2_GLOBAL(RCT2_ADDRESS_CONFIG_FLAGS, uint8) &= !CONFIG_FLAG_DISABLE_SMOOTH_LANDSCAPE;
			}
			
			// show height as units
			if (gGeneral_config.show_height_as_units){
				RCT2_GLOBAL(RCT2_ADDRESS_CONFIG_FLAGS, uint8) |= CONFIG_FLAG_SHOW_HEIGHT_AS_UNITS;
			}
			else {
				RCT2_GLOBAL(RCT2_ADDRESS_CONFIG_FLAGS, uint8) &= !CONFIG_FLAG_SHOW_HEIGHT_AS_UNITS;
			}

			// save plugin data
			if (gGeneral_config.save_plugin_data){
				RCT2_GLOBAL(RCT2_ADDRESS_CONFIG_FLAGS, uint8) |= CONFIG_FLAG_SAVE_PLUGIN_DATA;
			}
			else {
				RCT2_GLOBAL(RCT2_ADDRESS_CONFIG_FLAGS, uint8) &= !CONFIG_FLAG_SAVE_PLUGIN_DATA;
			}

			//sound configuration
			RCT2_GLOBAL(RCT2_ADDRESS_CONFIG_SOUND_QUALITY, sint8) = gSound_config.sound_quality;
			RCT2_GLOBAL(RCT2_ADDRESS_CONFIG_SOUND_SW_BUFFER, sint8) = gSound_config.forced_software_buffering; 

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