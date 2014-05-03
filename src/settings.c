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

#include <windows.h>
#include <ShlObj.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include "settings.h"
#include "screenshot.h"

settings_t settings;


/**
 * Initilize the settings.
 * It checks if the openRCT2 folder exists and creates it if it does not
 * parsing of the config file is done in settings_parse_settings
 */
void settings_init(){
	
	TCHAR path[MAX_PATH];
	FILE* fp;

	if (SUCCEEDED(SHGetFolderPath(NULL, CSIDL_PERSONAL | CSIDL_FLAG_CREATE, NULL, 0, path))){ //find home folder
		strcat(path, "\\OpenRCT2");
		DWORD dwAttrib = GetFileAttributes(path);
		if (!(dwAttrib != INVALID_FILE_ATTRIBUTES && dwAttrib & FILE_ATTRIBUTE_DIRECTORY)){ //folder does not exist
			if (!CreateDirectory(path, NULL)){
				return NULL; //error createing path
			}
		}
		strcat(path, "\\config.ini");
		fp = fopen(path, "r");
		if (!fp){
			settings_create_default(path);
			fp = fopen(path, "r");
			if (!fp){
				return NULL;
			}
		}

		settings_parse_settings(fp);
		
	}
}

/**
 * Create a new default settings file.
 * This should be created from some other resource when openRCT2 grows
 * @param path The aboslute path of the config file
 */
void settings_create_default(char *path){
	FILE* fp = fopen(path, "w");
	fprintf(fp, "GAME_PATH = C:\\GOG Games\\RollerCoaster Tycoon 2 Triple Thrill Pack\n");
	fprintf(fp, "SCREENSHOT_AS_PNG = TRUE\n");
	fclose(fp);
}

/**
 * Parse settings and set the game veriables
 * @param fp file pointer to the settings file
 */
void settings_parse_settings(FILE *fp){
	int c = NULL, pos = 0;
	char *setting;
	char *value;
	setting = (char *)malloc(128);
	value = (char *)malloc(128);
	
	int size = 256;
	
	while (settings_get_line(fp, setting, value) > 0){

		if (strcmp(setting, "GAME_PATH") == 0){
			strcpy(settings.GAME_PATH, value); //TODO: change to copy correct amount of bytes
		}
		else if(strcmp(setting, "SCREENSHOT_AS_PNG") == 0){
		
			if (value == '1'){
				settings.SCREENSHOT_AS_PNG = 1;
			}
			else{
				settings.SCREENSHOT_AS_PNG = 0;
			}
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
int settings_get_line(FILE *fp, char *setting, char *value){
	long start = ftell(fp);
	long end;
	int c;
	int pos = 0;
	long size;
	c = fgetc(fp);
	if(c == EOF) return -1;
	while (isalpha(c) || c == '_'){
		c = fgetc(fp); //find size of setting
		if (c == EOF) return -1;
	}

	end = ftell(fp);
	size = end - start;
	realloc(setting, size);
	fseek(fp, start, SEEK_SET);
	c = fgetc(fp);
	while (isalpha(c) || c == '_'){
		setting[pos] = (char)c;
		pos++;
		c = fgetc(fp);
	}
	setting[pos] = '\0';
	while (c != '='){
		if (c == EOF || c == '\n'){ // this is not a valid setting
			return -1;
		}
		c = fgetc(fp);
	}
	c = fgetc(fp);
	while (isspace(c)){
		c = fgetc(fp);
	}

	start = ftell(fp);
	while (c != '\n' && c!= EOF){
		c = fgetc(fp);
	}
	end = ftell(fp);
	size = end - start;
	realloc(value, size);
	fseek(fp, start - 1, SEEK_SET);
	pos = 0;
	c = fgetc(fp);
	while (c != '\n' && c != EOF){
		value[pos] = (char)c;
		pos++;
		c = fgetc(fp);
	}
	value[pos] = '\0';

}