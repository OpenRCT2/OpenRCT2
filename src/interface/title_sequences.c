/*****************************************************************************
* Copyright (c) 2014 Ted John, Peter Hill
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

#include "../localisation/string_ids.h"
#include "window.h"
#include "title_sequences.h"
#include "../title.h"
#include "../util/util.h"


uint16 gCurrentTitleSequence;
uint16 gCurrentPreviewTitleSequence;

title_command TitleScriptMakeCommand(int command, int parameter1, int parameter2)
{
	title_command titleCommand = { (uint8)command, (uint8)parameter1, (uint8)parameter2 };
	return titleCommand;
}

bool title_sequence_name_exists(const char *name)
{
	for (int i = 0; i < gConfigTitleSequences.num_presets; i++) {
		if (_stricmp(gConfigTitleSequences.presets[i].name, name) == 0)
			return true;
	}
	return false;
}

bool title_sequence_save_exists(int preset, const char *name)
{
	utf8 newName[MAX_PATH];
	char *extension = (char*)path_get_extension(name);
	strcpy(newName, name);
	if (_stricmp(extension, ".sv6") != 0 && _stricmp(extension, ".sc6") != 0)
		strcat(newName, ".sv6");
	for (int i = 0; i < gConfigTitleSequences.presets[preset].num_saves; i++) {
		if (_stricmp(gConfigTitleSequences.presets[preset].saves[i], newName) == 0)
			return true;
	}
	return false;
}


void title_sequence_change_preset(int preset)
{
	if (preset >= 0 && preset < gConfigTitleSequences.num_presets) {
		switch (preset) {
		case 0:
			gConfigInterface.current_title_sequence_preset = "*RCT2";
			break;
		case 1:
			gConfigInterface.current_title_sequence_preset = "*OPENRCT2";
			break;
		default:
			gConfigInterface.current_title_sequence_preset = gConfigTitleSequences.presets[preset].name;
			break;
		}
		gCurrentPreviewTitleSequence = preset;
	}
	window_invalidate_all();

	// Switch to (and restart) this title sequence if it's valid
	title_refresh_sequence();
}

void title_sequence_create_preset(const char *name)
{
	if (filename_valid_characters(name) && !title_sequence_name_exists(name)) {
		int preset = gConfigTitleSequences.num_presets;
		gConfigTitleSequences.num_presets++;
		gConfigTitleSequences.presets = realloc(gConfigTitleSequences.presets, sizeof(title_sequence) * (size_t)gConfigTitleSequences.num_presets);
		strcpy(gConfigTitleSequences.presets[preset].name, name);
		gConfigTitleSequences.presets[preset].path[0] = 0;

		gConfigTitleSequences.presets[preset].saves = malloc(0);
		gConfigTitleSequences.presets[preset].commands = malloc(0);
		gConfigTitleSequences.presets[preset].num_saves = 0;
		gConfigTitleSequences.presets[preset].num_commands = 0;

		// Create the folder
		utf8 path[MAX_PATH];
		platform_get_user_directory(path, "title sequences");
		strcat(path, gConfigTitleSequences.presets[preset].name);
		platform_file_delete(path);
		platform_ensure_directory_exists(path);
		
		title_sequence_save_preset_script(preset);
		title_sequence_change_preset(preset);
	}
}

void title_sequence_duplicate_preset(int duplicate, const char *name)
{
	if (duplicate >= 0 && duplicate < gConfigTitleSequences.num_presets && filename_valid_characters(name) && !title_sequence_name_exists(name)) {
		int preset = gConfigTitleSequences.num_presets;
		gConfigTitleSequences.num_presets++;
		gConfigTitleSequences.presets = realloc(gConfigTitleSequences.presets, sizeof(title_sequence) * (size_t)gConfigTitleSequences.num_presets);
		strcpy(gConfigTitleSequences.presets[preset].name, name);
		gConfigTitleSequences.presets[preset].path[0] = 0;

		size_t savesSize = sizeof(char[TITLE_SEQUENCE_MAX_SAVE_LENGTH]) * gConfigTitleSequences.presets[duplicate].num_saves;
		size_t commandsSize = sizeof(title_command) * gConfigTitleSequences.presets[duplicate].num_commands;
		gConfigTitleSequences.presets[preset].saves = malloc(savesSize);
		gConfigTitleSequences.presets[preset].commands = malloc(commandsSize);
		memcpy(gConfigTitleSequences.presets[preset].saves, gConfigTitleSequences.presets[duplicate].saves, savesSize);
		memcpy(gConfigTitleSequences.presets[preset].commands, gConfigTitleSequences.presets[duplicate].commands, commandsSize);
		gConfigTitleSequences.presets[preset].num_saves = gConfigTitleSequences.presets[duplicate].num_saves;
		gConfigTitleSequences.presets[preset].num_commands = gConfigTitleSequences.presets[duplicate].num_commands;

		bool loadmm = false;
		for (int i = 0; i < gConfigTitleSequences.presets[preset].num_commands; i++) {
			if (gConfigTitleSequences.presets[preset].commands[i].command == TITLE_SCRIPT_LOADMM) {
				loadmm = true;
				gConfigTitleSequences.presets[preset].commands[i].command = TITLE_SCRIPT_LOAD;
				gConfigTitleSequences.presets[preset].commands[i].saveIndex = gConfigTitleSequences.presets[duplicate].num_saves;
			}
		}

		// Create the folder
		utf8 path[MAX_PATH], srcPath[MAX_PATH];
		platform_get_user_directory(path, "title sequences");
		strcat(path, gConfigTitleSequences.presets[preset].name);
		platform_file_delete(path);
		platform_ensure_directory_exists(path);

		// Copy the saves
		char separator = platform_get_path_separator();
		for (int i = 0; i < gConfigTitleSequences.presets[preset].num_saves; i++) {
			if (gConfigTitleSequences.presets[duplicate].path[0]) {
				strcpy(srcPath, gConfigTitleSequences.presets[duplicate].path);
				strcat(srcPath, gConfigTitleSequences.presets[duplicate].saves[i]);
			}
			else {
				platform_get_user_directory(srcPath, "title sequences");
				strcat(srcPath, gConfigTitleSequences.presets[duplicate].name);
				strncat(srcPath, &separator, 1);
				strcat(srcPath, gConfigTitleSequences.presets[duplicate].saves[i]);
			}
			platform_get_user_directory(path, "title sequences");
			strcat(path, gConfigTitleSequences.presets[preset].name);
			strncat(path, &separator, 1);
			strcat(path, gConfigTitleSequences.presets[preset].saves[i]);

			platform_file_copy(srcPath, path);
		}

		if (loadmm) {
			title_sequence_add_save(preset, get_file_path(PATH_ID_SIXFLAGS_MAGICMOUNTAIN), "Six Flags Magic Mountain.SC6");
		}
		
		title_sequence_save_preset_script(preset);
		//title_sequence_change_preset(preset);
		gCurrentTitleSequence = preset;
	}
}

void title_sequence_delete_preset(int preset)
{
	if (preset >= TITLE_SEQUENCE_DEFAULT_PRESETS && preset < gConfigTitleSequences.num_presets) {
		// Delete the folder
		utf8 path[MAX_PATH];
		platform_get_user_directory(path, "title sequences");
		strcat(path, gConfigTitleSequences.presets[preset].name);
		platform_directory_delete(path);

		free(gConfigTitleSequences.presets[preset].saves);
		free(gConfigTitleSequences.presets[preset].commands);

		for (int i = preset; i < gConfigTitleSequences.num_presets - 1; i++) {
			gConfigTitleSequences.presets[i] = gConfigTitleSequences.presets[i + 1];
		}
		gConfigTitleSequences.num_presets--;
		gConfigTitleSequences.presets = realloc(gConfigTitleSequences.presets, sizeof(title_sequence) * (size_t)gConfigTitleSequences.num_presets);

		gCurrentTitleSequence--;
		if (gCurrentPreviewTitleSequence > preset)
			title_sequence_change_preset(gCurrentPreviewTitleSequence - 1);
		else if (gCurrentPreviewTitleSequence == preset)
			title_sequence_change_preset(0);
	}
}

void title_sequence_rename_preset(int preset, const char *newName)
{
	if (preset >= TITLE_SEQUENCE_DEFAULT_PRESETS && preset < gConfigTitleSequences.num_presets && filename_valid_characters(newName) && !title_sequence_name_exists(newName)) {
		// Rename the folder
		utf8 src[MAX_PATH], dest[MAX_PATH];
		platform_get_user_directory(src, "title sequences");
		platform_get_user_directory(dest, "title sequences");
		strcat(src, gConfigTitleSequences.presets[preset].name);
		strcat(dest, newName);
		platform_file_move(src, dest);

		strcpy(gConfigTitleSequences.presets[preset].name, newName);

		// Rename the config preset name if needed
		if (preset == gCurrentPreviewTitleSequence) {
			gConfigInterface.current_title_sequence_preset = gConfigTitleSequences.presets[preset].name;
		}
	}
}


void title_sequence_add_save(int preset, const char *path, const char *newName)
{
	utf8 newPath[MAX_PATH];
	char *extension = (char*)path_get_extension(newName);
	strcpy(newPath, newName);
	if (_stricmp(extension, ".sv6") != 0 && _stricmp(extension, ".sc6") != 0)
		strcat(newPath, ".sv6");
	if (preset >= TITLE_SEQUENCE_DEFAULT_PRESETS && preset < gConfigTitleSequences.num_presets && filename_valid_characters(newPath) && !title_sequence_save_exists(preset, newPath) && platform_file_exists(path)) {
		// Copy the save file
		char separator = platform_get_path_separator();
		platform_get_user_directory(newPath, "title sequences");
		strcat(newPath, gConfigTitleSequences.presets[preset].name);
		strncat(newPath, &separator, 1);
		strcat(newPath, newName);
		// Add the appropriate extension if needed
		if (_stricmp(extension, ".sv6") != 0 && _stricmp(extension, ".sc6") != 0)
			strcat(newPath, ".sv6");
		platform_file_copy(path, newPath);

		gConfigTitleSequences.presets[preset].num_saves++;
		gConfigTitleSequences.presets[preset].saves = realloc(gConfigTitleSequences.presets[preset].saves, sizeof(char[TITLE_SEQUENCE_MAX_SAVE_LENGTH]) * (size_t)gConfigTitleSequences.presets[preset].num_saves);
		
		strcpy(gConfigTitleSequences.presets[preset].saves[gConfigTitleSequences.presets[preset].num_saves - 1], newName);
		// Add the appropriate extension if needed
		if (_stricmp(extension, ".sv6") != 0 && _stricmp(extension, ".sc6") != 0)
			strcat(gConfigTitleSequences.presets[preset].saves[gConfigTitleSequences.presets[preset].num_saves - 1], ".sv6");
	}
}

void title_sequence_remove_save(int preset, int index)
{
	if (preset >= TITLE_SEQUENCE_DEFAULT_PRESETS && preset < gConfigTitleSequences.num_presets && index >= 0 && index < gConfigTitleSequences.presets[preset].num_saves) {
		// Delete the save file
		char separator = platform_get_path_separator();
		utf8 path[MAX_PATH];
		platform_get_user_directory(path, "title sequences");
		strcat(path, gConfigTitleSequences.presets[preset].name);
		strncat(path, &separator, 1);
		strcat(path, gConfigTitleSequences.presets[preset].saves[index]);
		platform_file_delete(path);
		
		// Remove all references to this save in the commands and decrement save indecies
		for (int i = 0; i < gConfigTitleSequences.presets[preset].num_commands; i++) {
			if (gConfigTitleSequences.presets[preset].commands[i].command == TITLE_SCRIPT_LOAD) {
				if (gConfigTitleSequences.presets[preset].commands[i].saveIndex == index)
					gConfigTitleSequences.presets[preset].commands[i].saveIndex = 0xFF;
				else if (gConfigTitleSequences.presets[preset].commands[i].saveIndex > index)
					gConfigTitleSequences.presets[preset].commands[i].saveIndex--;
			}
		}
		
		for (int i = index; i < gConfigTitleSequences.presets[preset].num_saves - 1; i++) {
			strcpy(gConfigTitleSequences.presets[preset].saves[i], gConfigTitleSequences.presets[preset].saves[i + 1]);
		}
		gConfigTitleSequences.presets[preset].num_saves--;
		gConfigTitleSequences.presets[preset].saves = realloc(gConfigTitleSequences.presets[preset].saves, sizeof(char[TITLE_SEQUENCE_MAX_SAVE_LENGTH]) * (size_t)gConfigTitleSequences.presets[preset].num_saves);
		title_sequence_save_preset_script(preset);
	}
}

void title_sequence_rename_save(int preset, int index, const char *newName)
{
	if (preset >= TITLE_SEQUENCE_DEFAULT_PRESETS && preset < gConfigTitleSequences.num_presets && index >= 0 && index < gConfigTitleSequences.presets[preset].num_saves &&
		filename_valid_characters(newName) && !title_sequence_save_exists(preset, newName)) {

		// Rename the save file
		char separator = platform_get_path_separator();
		utf8 src[MAX_PATH], dest[MAX_PATH];
		platform_get_user_directory(src, "title sequences");
		platform_get_user_directory(dest, "title sequences");
		strcat(src, gConfigTitleSequences.presets[preset].name);
		strcat(dest, gConfigTitleSequences.presets[preset].name);
		strncat(src, &separator, 1);
		strncat(dest, &separator, 1);
		strcat(src, gConfigTitleSequences.presets[preset].saves[index]);
		strcat(dest, newName);
		// Add the appropriate extension if needed
		char *extension = (char*)path_get_extension(newName);
		if (_stricmp(extension, ".sv6") != 0 && _stricmp(extension, ".sc6") != 0)
			strcat(dest, ".sv6");
		platform_file_move(src, dest);

		strcpy(gConfigTitleSequences.presets[preset].saves[index], newName);
		// Add the appropriate extension if needed
		if (_stricmp(extension, ".sv6") != 0 && _stricmp(extension, ".sc6") != 0)
			strcat(gConfigTitleSequences.presets[preset].saves[index], ".sv6");
		title_sequence_save_preset_script(preset);
	}
}


void title_sequence_add_command(int preset, title_command command)
{
	if (preset >= TITLE_SEQUENCE_DEFAULT_PRESETS && preset < gConfigTitleSequences.num_presets) {
		gConfigTitleSequences.presets[preset].commands = realloc(gConfigTitleSequences.presets[preset].commands, sizeof(title_command) * (size_t)(gConfigTitleSequences.presets[preset].num_commands + 1));
		gConfigTitleSequences.presets[preset].commands[gConfigTitleSequences.presets[preset].num_commands] = command;
		gConfigTitleSequences.presets[preset].num_commands++;
		title_sequence_save_preset_script(preset);
	}
}
void title_sequence_insert_command(int preset, int index, title_command command)
{
	if (preset >= TITLE_SEQUENCE_DEFAULT_PRESETS && preset < gConfigTitleSequences.num_presets && index >= 0 && index <= gConfigTitleSequences.presets[preset].num_commands) {
		gConfigTitleSequences.presets[preset].commands = realloc(gConfigTitleSequences.presets[preset].commands, sizeof(title_command) * (size_t)(gConfigTitleSequences.presets[preset].num_commands + 1));
		for (int i = gConfigTitleSequences.presets[preset].num_commands; i > index; i--) {
			gConfigTitleSequences.presets[preset].commands[i] = gConfigTitleSequences.presets[preset].commands[i - 1];
		}
		gConfigTitleSequences.presets[preset].commands[index] = command;
		gConfigTitleSequences.presets[preset].num_commands++;
		title_sequence_save_preset_script(preset);
	}
}

void title_sequence_delete_command(int preset, int index)
{
	if (preset >= TITLE_SEQUENCE_DEFAULT_PRESETS && preset < gConfigTitleSequences.num_presets && index >= 0 && index < gConfigTitleSequences.presets[preset].num_commands) {
		for (int i = index; i < gConfigTitleSequences.presets[preset].num_commands - 1; i++) {
			gConfigTitleSequences.presets[preset].commands[i] = gConfigTitleSequences.presets[preset].commands[i + 1];
		}
		gConfigTitleSequences.presets[preset].num_commands--;
		gConfigTitleSequences.presets[preset].commands = realloc(gConfigTitleSequences.presets[preset].commands, sizeof(title_command) * (size_t)gConfigTitleSequences.presets[preset].num_commands);
		title_sequence_save_preset_script(preset);
	}
}

void title_sequence_move_down_command(int preset, int index)
{
	if (preset >= TITLE_SEQUENCE_DEFAULT_PRESETS && preset < gConfigTitleSequences.num_presets && index >= 0 && index + 1 < gConfigTitleSequences.presets[preset].num_commands) {
		title_command command = gConfigTitleSequences.presets[preset].commands[index];
		gConfigTitleSequences.presets[preset].commands[index] = gConfigTitleSequences.presets[preset].commands[index + 1];
		gConfigTitleSequences.presets[preset].commands[index + 1] = command;
		title_sequence_save_preset_script(preset);
	}
}

void title_sequence_move_up_command(int preset, int index)
{
	if (preset >= TITLE_SEQUENCE_DEFAULT_PRESETS && preset < gConfigTitleSequences.num_presets && index > 0 && index < gConfigTitleSequences.presets[preset].num_commands) {
		title_command command = gConfigTitleSequences.presets[preset].commands[index];
		gConfigTitleSequences.presets[preset].commands[index] = gConfigTitleSequences.presets[preset].commands[index - 1];
		gConfigTitleSequences.presets[preset].commands[index - 1] = command;
		title_sequence_save_preset_script(preset);
	}
}
