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
 * 
 *  rct2: 0x006752D5
 */
void config_load()
{
	HANDLE hFile;
	DWORD bytesRead;

	hFile = CreateFile(get_file_path(PATH_ID_GAMECFG), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING,
		FILE_FLAG_RANDOM_ACCESS | FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile != INVALID_HANDLE_VALUE) {
		// Read and check magic number
		ReadFile(hFile, RCT2_ADDRESS(0x013CE928, void), 4, &bytesRead, NULL);
		if (RCT2_GLOBAL(0x013CE928, int) == MagicNumber) {
			// Read options
			ReadFile(hFile, (void*)0x009AAC5C, 2155, &bytesRead, NULL);
			CloseHandle(hFile);
			if (RCT2_GLOBAL(0x009AB4C6, sint8) == 1)
				return;
			RCT2_GLOBAL(0x009AB4C6, sint8) = 1;
			RCT2_GLOBAL(RCT2_ADDRESS_CONFIG_METRIC, sint8) = 0;
			RCT2_GLOBAL(RCT2_ADDRESS_CONFIG_FAHRENHEIT, sint8) = 1;
			RCT2_GLOBAL(0x009AACBB, sint8) = 1;
			RCT2_GLOBAL(0x009AACBD, sint16) = 0;
			if (!(RCT2_GLOBAL(RCT2_ADDRESS_CONFIG_FLAGS, uint8) & CONFIG_FLAG_SHOW_HEIGHT_AS_UNITS))
				RCT2_GLOBAL(0x009AACBD, sint16) = (RCT2_GLOBAL(RCT2_ADDRESS_CONFIG_METRIC, sint8) + 1) * 256;
			RCT2_GLOBAL(0x009AA00D, sint8) = 1;
		}
	}

	RCT2_GLOBAL(0x009AAC77, sint8) = 0;
	if (RCT2_GLOBAL(RCT2_ADDRESS_MEM_TOTAL_PHYSICAL, uint32) > 0x4000000) {
		RCT2_GLOBAL(0x009AAC77, sint8) = 1;
		if (RCT2_GLOBAL(RCT2_ADDRESS_MEM_TOTAL_PHYSICAL, uint32) > 0x8000000)
			RCT2_GLOBAL(0x009AAC77, sint8) = 2;
	}

	RCT2_GLOBAL(0x009AAC75, sint8) = RCT2_ADDRESS(0x009AF601, sint8)[RCT2_GLOBAL(0x009AAC77, sint8)];
	RCT2_GLOBAL(0x009AAC76, sint8) = RCT2_ADDRESS(0x009AF604, sint8)[RCT2_GLOBAL(0x009AAC77, sint8)];
	RCT2_GLOBAL(0x009AACBD, sint16) = 0;
	if (!(RCT2_GLOBAL(RCT2_ADDRESS_CONFIG_FLAGS, uint8) & CONFIG_FLAG_SHOW_HEIGHT_AS_UNITS))
		RCT2_GLOBAL(0x009AACBD, sint16) = (RCT2_GLOBAL(RCT2_ADDRESS_CONFIG_METRIC, sint8) + 1) * 256;
	RCT2_GLOBAL(0x009AA00D, sint8) = 1;
}

/**
 * 
 *  rct2: 0x00675487
 */
void config_save()
{
	HANDLE hFile;
	DWORD bytesWritten;

	hFile = CreateFile(get_file_path(PATH_ID_GAMECFG), GENERIC_WRITE, NULL, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile != INVALID_HANDLE_VALUE) {
		WriteFile(hFile, &MagicNumber, 4, &bytesWritten, NULL);
		WriteFile(hFile, 0x009AAC5C, 2155, &bytesWritten, NULL);
		CloseHandle(hFile);
	}
}

// New config format

configuration_t gConfig;

static void config_parse_settings(FILE *fp);
static int config_get_line(FILE *fp, char *setting, char *value);
static void config_create_default(char *path);

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
				return NULL; // error creating path
			}
		}
		strcat(path, "\\config.ini");
		fp = fopen(path, "r");
		if (!fp) {
			config_create_default(path);
			fp = fopen(path, "r");
			if (!fp)
				return NULL;
		}

		config_parse_settings(fp);
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

	if (!config_find_rct2_path(gConfig.game_path)) {
		MessageBox(NULL, "Unable to find RCT2 installation directory. Please select the directoy where you installed RCT2!", "OpenRCT2", MB_OK);
		char *res = directory_browser();
		strcpy(gConfig.game_path, res);
	}

	fp = fopen(path, "w");
	fprintf(fp, "[general]\n");
	fprintf(fp, "game_path = %s\n", gConfig.game_path);
	fprintf(fp, "screenshot_format = PNG\n");
	fprintf(fp, "play_intro = false\n");
	fclose(fp);
}

//A directory browser allowing for semi-automatic config.ini for non standard installs
char* directory_browser()
{
	BROWSEINFO      bi;
	char            pszBuffer[MAX_PATH];
	LPITEMIDLIST    pidl;
	LPMALLOC        lpMalloc;

	// Initialize COM
	if (CoInitializeEx(0, COINIT_APARTMENTTHREADED) != S_OK)
	{
		MessageBox(NULL, _T("Error opening browse window"), _T("ERROR"), MB_OK);
		CoUninitialize();
		return 0;
	}

	// Get a pointer to the shell memory allocator
	if (SHGetMalloc(&lpMalloc) != S_OK)
	{
		MessageBox(NULL, _T("Error opening browse window"), _T("ERROR"), MB_OK);
		CoUninitialize();
		return 0;
	}

	bi.hwndOwner = NULL;
	bi.pidlRoot = NULL;
	bi.pszDisplayName = pszBuffer;
	bi.lpszTitle = _T("Select your RCT2 installation directory");
	bi.ulFlags = BIF_RETURNFSANCESTORS | BIF_RETURNONLYFSDIRS;
	bi.lpfn = NULL;
	bi.lParam = 0;

	char *outPath = "C:\\";

	if (pidl = SHBrowseForFolder(&bi))
	{
		// Copy the path directory to the buffer
		if (SHGetPathFromIDList(pidl, pszBuffer))
		{
			// store pszBuffer (and the path) in the outPath
			outPath = strcat("", pszBuffer);

		}

	}
	CoUninitialize();
	return outPath;
}

/**
 * Parse settings and set the game veriables
 * @param fp file pointer to the settings file
 */
static void config_parse_settings(FILE *fp)
{
	int c = NULL, pos = 0;
	char *setting;
	char *value;
	setting = (char *)malloc(128);
	value = (char *)malloc(128);
	
	int size = 256;
	
	while (config_get_line(fp, setting, value) > 0) {
		if (strcmp(setting, "game_path") == 0){
			strcpy(gConfig.game_path, value); // TODO: change to copy correct amount of bytes
		} else if(strcmp(setting, "screenshot_format") == 0) {
			if (strcmp(value, "png") == 0 || strcmp(value, "PNG") == 0) {
				gConfig.screenshot_format = SCREENSHOT_FORMAT_PNG;
			} else if (strcmp(value, "1") == 0) { // Maybe remove that? WARNING: Breaks existing config files
				gConfig.screenshot_format = SCREENSHOT_FORMAT_PNG;
			} else {
				gConfig.screenshot_format = SCREENSHOT_FORMAT_BMP;
			}
		} else if (strcmp(setting, "play_intro") == 0) {
			gConfig.play_intro = (strcmp(value, "true") == 0);
		}
	}
}

/**
 * Read one line in the settings file
 * @param fp filepointer to the settings file
 * @param setting pointer where to to store the setting
 * @param value pointer to where to store the value 
 * @return < 0 on error
 */
static int config_get_line(FILE *fp, char *setting, char *value)
{
	long start = ftell(fp);
	long end;
	int c;
	int pos = 0;
	long size;
	c = fgetc(fp);
	if (c == EOF)
		return -1;
	while (isalpha(c) || c == '_'){
		c = fgetc(fp); // find size of setting
		if (c == EOF)
			return -1;
	}

	end = ftell(fp);
	size = end - start;
	realloc(setting, size);
	fseek(fp, start, SEEK_SET);
	c = fgetc(fp);
	if (c == '[') {
		// TODO support categories
		setting[0] = '\0';
		value[0] = '\0';
		while (c != '\n' && c != EOF) {
			pos++;
			c = fgetc(fp);
		}

		return 1;
	}
	while (isalpha(c) || c == '_'){
		setting[pos] = (char)c;
		pos++;
		c = fgetc(fp);
	}
	setting[pos] = '\0';
	while (c != '=') {
		if (c == EOF || c == '\n') { // this is not a valid setting
			return -1;
		}
		c = fgetc(fp);
	}
	c = fgetc(fp);
	while (isspace(c)) {
		c = fgetc(fp);
	}

	start = ftell(fp);
	while (c != '\n' && c!= EOF) {
		c = fgetc(fp);
	}
	end = ftell(fp);
	size = end - start;
	realloc(value, size);
	fseek(fp, start - 1, SEEK_SET);
	pos = 0;
	c = fgetc(fp);
	while (c != '\n' && c != EOF) {
		value[pos] = (char)c;
		pos++;
		c = fgetc(fp);
	}
	value[pos] = '\0';
}