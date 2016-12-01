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

#include "../localisation/string_ids.h"
#include "../rct2.h"
#include "../title.h"
#include "../util/util.h"
#include "title_sequences.h"
#include "window.h"


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
	const char *extension = path_get_extension(name);
	safe_strcpy(newName, name, MAX_PATH);
	if (_stricmp(extension, ".sv6") != 0 && _stricmp(extension, ".sc6") != 0)
		path_append_extension(newName, ".sv6", MAX_PATH);
	for (int i = 0; i < gConfigTitleSequences.presets[preset].num_saves; i++) {
		if (_stricmp(gConfigTitleSequences.presets[preset].saves[i], newName) == 0)
			return true;
	}
	return false;
}


void title_sequence_change_preset(int preset)
{
	if (preset >= 0 && preset < gConfigTitleSequences.num_presets) {
		SafeFree(gConfigInterface.current_title_sequence_preset);
		switch (preset) {
		case 0:
			gConfigInterface.current_title_sequence_preset = _strdup("*RCT1");
			break;
		case 1:
			gConfigInterface.current_title_sequence_preset = _strdup("*RCT1AA");
			break;
		case 2:
			gConfigInterface.current_title_sequence_preset = _strdup("*RCT1AALL");
			break;
		case 3:
			gConfigInterface.current_title_sequence_preset = _strdup("*RCT2");
			break;
		case 4:
			gConfigInterface.current_title_sequence_preset = _strdup("*OPENRCT2");
			break;
		default:
			gConfigInterface.current_title_sequence_preset = _strdup(gConfigTitleSequences.presets[preset].name);
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
		safe_strcpy(gConfigTitleSequences.presets[preset].name, name, TITLE_SEQUENCE_NAME_SIZE);
		gConfigTitleSequences.presets[preset].path[0] = 0;

		gConfigTitleSequences.presets[preset].saves = NULL;
		gConfigTitleSequences.presets[preset].commands = NULL;
		gConfigTitleSequences.presets[preset].num_saves = 0;
		gConfigTitleSequences.presets[preset].num_commands = 0;

		// Create the folder
		utf8 path[MAX_PATH];
		platform_get_user_directory(path, "title sequences", sizeof(path));
		safe_strcat_path(path, gConfigTitleSequences.presets[preset].name, MAX_PATH);
		platform_file_delete(path);
		platform_ensure_directory_exists(path);

		title_sequence_save_preset_script(preset);
		gCurrentTitleSequence = preset;
	}
}

void title_sequence_duplicate_preset(int duplicate, const char *name)
{
	if (duplicate >= 0 && duplicate < gConfigTitleSequences.num_presets && filename_valid_characters(name) && !title_sequence_name_exists(name)) {
		int preset = gConfigTitleSequences.num_presets;
		gConfigTitleSequences.num_presets++;
		gConfigTitleSequences.presets = realloc(gConfigTitleSequences.presets, sizeof(title_sequence) * (size_t)gConfigTitleSequences.num_presets);
		safe_strcpy(gConfigTitleSequences.presets[preset].name, name, TITLE_SEQUENCE_NAME_SIZE);
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
		platform_get_user_directory(path, "title sequences", sizeof(path));
		safe_strcat_path(path, gConfigTitleSequences.presets[preset].name, MAX_PATH);
		platform_file_delete(path);
		platform_ensure_directory_exists(path);

		// Copy the saves
		for (int i = 0; i < gConfigTitleSequences.presets[preset].num_saves; i++) {
			if (gConfigTitleSequences.presets[duplicate].path[0]) {
				safe_strcpy(srcPath, gConfigTitleSequences.presets[duplicate].path, MAX_PATH);
				safe_strcat_path(srcPath, gConfigTitleSequences.presets[duplicate].saves[i], MAX_PATH);
			}
			else {
				platform_get_user_directory(srcPath, "title sequences", sizeof(srcPath));
				safe_strcat_path(srcPath, gConfigTitleSequences.presets[duplicate].name, MAX_PATH);
				safe_strcat_path(srcPath, gConfigTitleSequences.presets[duplicate].saves[i], MAX_PATH);
			}
			platform_get_user_directory(path, "title sequences", sizeof(path));
			safe_strcat_path(path, gConfigTitleSequences.presets[preset].name, MAX_PATH);
			safe_strcat_path(path, gConfigTitleSequences.presets[preset].saves[i], MAX_PATH);

			platform_file_copy(srcPath, path, false);
		}

		if (loadmm) {
			title_sequence_add_save(preset, get_file_path(PATH_ID_SIXFLAGS_MAGICMOUNTAIN), "Six Flags Magic Mountain.SC6");
		}

		title_sequence_save_preset_script(preset);
		gCurrentTitleSequence = preset;
	}
}

void title_sequence_delete_preset(int preset)
{
	if (preset >= TITLE_SEQUENCE_DEFAULT_PRESETS && preset < gConfigTitleSequences.num_presets) {
		// Delete the folder
		utf8 path[MAX_PATH];
		platform_get_user_directory(path, "title sequences", sizeof(path));
		safe_strcat_path(path, gConfigTitleSequences.presets[preset].name, MAX_PATH);
		if (!platform_directory_delete(path)) {
			log_error("Failed to delete directory: \"%s\"", path);
		}

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
		platform_get_user_directory(src, "title sequences", sizeof(src));
		platform_get_user_directory(dest, "title sequences", sizeof(dest));
		safe_strcat_path(src, gConfigTitleSequences.presets[preset].name, sizeof(src));
		safe_strcat_path(dest, newName, sizeof(dest));
		platform_file_move(src, dest);

		safe_strcpy(gConfigTitleSequences.presets[preset].name, newName, TITLE_SEQUENCE_NAME_SIZE);

		// Rename the config preset name if needed
		if (preset == gCurrentPreviewTitleSequence) {
			gConfigInterface.current_title_sequence_preset = gConfigTitleSequences.presets[preset].name;
		}
	}
}


void title_sequence_add_save(int preset, const char *path, const char *newName)
{
	utf8 newPath[MAX_PATH];
	const char *extension = path_get_extension(newName);
	safe_strcpy(newPath, newName, MAX_PATH);
	if (_stricmp(extension, ".sv6") != 0 && _stricmp(extension, ".sc6") != 0)
		path_append_extension(newPath, ".sv6", MAX_PATH);
	if (preset >= TITLE_SEQUENCE_DEFAULT_PRESETS && preset < gConfigTitleSequences.num_presets && filename_valid_characters(newPath) && !title_sequence_save_exists(preset, newPath) && platform_file_exists(path)) {
		// Copy the save file
		platform_get_user_directory(newPath, "title sequences", sizeof(newPath));
		safe_strcat_path(newPath, gConfigTitleSequences.presets[preset].name, sizeof(newPath));
		safe_strcat_path(newPath, newName, sizeof(newPath));
		// Add the appropriate extension if needed
		if (_stricmp(extension, ".sv6") != 0 && _stricmp(extension, ".sc6") != 0)
			path_append_extension(newPath, ".sv6", sizeof(newPath));
		platform_file_copy(path, newPath, false);

		gConfigTitleSequences.presets[preset].num_saves++;
		gConfigTitleSequences.presets[preset].saves = realloc(gConfigTitleSequences.presets[preset].saves, sizeof(char[TITLE_SEQUENCE_MAX_SAVE_LENGTH]) * (size_t)gConfigTitleSequences.presets[preset].num_saves);

		safe_strcpy(gConfigTitleSequences.presets[preset].saves[gConfigTitleSequences.presets[preset].num_saves - 1], newName, TITLE_SEQUENCE_MAX_SAVE_LENGTH);
		// Add the appropriate extension if needed
		if (_stricmp(extension, ".sv6") != 0 && _stricmp(extension, ".sc6") != 0)
			path_append_extension(gConfigTitleSequences.presets[preset].saves[gConfigTitleSequences.presets[preset].num_saves - 1], ".sv6", TITLE_SEQUENCE_MAX_SAVE_LENGTH);
	}
}

void title_sequence_remove_save(int preset, int index)
{
	if (preset >= TITLE_SEQUENCE_DEFAULT_PRESETS && preset < gConfigTitleSequences.num_presets && index >= 0 && index < gConfigTitleSequences.presets[preset].num_saves) {
		// Delete the save file
		utf8 path[MAX_PATH];
		platform_get_user_directory(path, "title sequences", sizeof(path));
		safe_strcat_path(path, gConfigTitleSequences.presets[preset].name, sizeof(path));
		safe_strcat_path(path, gConfigTitleSequences.presets[preset].saves[index], sizeof(path));
		platform_file_delete(path);

		// Remove all references to this save in the commands and decrement save indices
		for (int i = 0; i < gConfigTitleSequences.presets[preset].num_commands; i++) {
			if (gConfigTitleSequences.presets[preset].commands[i].command == TITLE_SCRIPT_LOAD) {
				if (gConfigTitleSequences.presets[preset].commands[i].saveIndex == index)
					gConfigTitleSequences.presets[preset].commands[i].saveIndex = 0xFF;
				else if (gConfigTitleSequences.presets[preset].commands[i].saveIndex > index && gConfigTitleSequences.presets[preset].commands[i].saveIndex != 0xFF)
					gConfigTitleSequences.presets[preset].commands[i].saveIndex--;
			}
		}

		for (int i = index; i < gConfigTitleSequences.presets[preset].num_saves - 1; i++) {
			safe_strcpy(gConfigTitleSequences.presets[preset].saves[i], gConfigTitleSequences.presets[preset].saves[i + 1], TITLE_SEQUENCE_MAX_SAVE_LENGTH);
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
		utf8 src[MAX_PATH], dest[MAX_PATH];
		platform_get_user_directory(src, "title sequences", sizeof(src));
		platform_get_user_directory(dest, "title sequences", sizeof(dest));
		safe_strcat_path(src, gConfigTitleSequences.presets[preset].name, sizeof(src));
		safe_strcat_path(dest, gConfigTitleSequences.presets[preset].name, sizeof(dest));
		safe_strcat_path(src, gConfigTitleSequences.presets[preset].saves[index], sizeof(src));
		safe_strcat_path(dest, newName, sizeof(dest));
		// Add the appropriate extension if needed
		const char *extension = path_get_extension(newName);
		if (_stricmp(extension, ".sv6") != 0 && _stricmp(extension, ".sc6") != 0)
			path_append_extension(dest, ".sv6", sizeof(dest));
		platform_file_move(src, dest);

		safe_strcpy(gConfigTitleSequences.presets[preset].saves[index], newName, TITLE_SEQUENCE_MAX_SAVE_LENGTH);
		// Add the appropriate extension if needed
		if (_stricmp(extension, ".sv6") != 0 && _stricmp(extension, ".sc6") != 0)
			path_append_extension(gConfigTitleSequences.presets[preset].saves[index], ".sv6", TITLE_SEQUENCE_MAX_SAVE_LENGTH);
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
