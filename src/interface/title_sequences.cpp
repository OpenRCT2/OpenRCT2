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

#include "../core/Json.hpp"
#include "../core/StringBuilder.hpp"
#include "../core/String.hpp"

extern "C" {
    #include "title_sequences.h"
    #include "../localisation/string_ids.h"
    #include "window.h"
    #include "../title.h"
    #include "../util/util.h"
}

// The index of the current title sequence being edited
uint16 gCurrentTitleSequence;
// The index of the current title sequence being shown
uint16 gCurrentPreviewTitleSequence;

title_command TitleScriptMakeCommand(int command, int parameter1, int parameter2)
{
    title_command titleCommand = { (uint8)command, (uint8)parameter1, (uint8)parameter2 };
    return titleCommand;
}

bool title_sequence_name_exists(const char *name)
{
    for (int i = 0; i < gConfigTitleSequences.num_presets; i++) {
        if (String::Equals(gConfigTitleSequences.presets[i].name, name, true))
            return true;
    }
    return false;
}

bool title_sequence_save_exists(int preset, const char *name)
{
    utf8 newName[MAX_PATH];
    const char *extension = path_get_extension(name);
    String::Set(newName, MAX_PATH, name);
    if (!String::Equals(extension, ".sv6", true) && !String::Equals(extension, ".sc6", true))
        path_append_extension(newName, ".sv6", MAX_PATH);
    for (int i = 0; i < gConfigTitleSequences.presets[preset].num_saves; i++) {
        if (String::Equals(gConfigTitleSequences.presets[preset].saves[i], newName, true))
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
        gConfigTitleSequences.presets = (title_sequence*) realloc(gConfigTitleSequences.presets, sizeof(title_sequence) * (size_t)gConfigTitleSequences.num_presets);
        String::Set(gConfigTitleSequences.presets[preset].name, TITLE_SEQUENCE_NAME_SIZE, name);
        gConfigTitleSequences.presets[preset].path[0] = 0;

        gConfigTitleSequences.presets[preset].saves = Memory::AllocateArray<char[TITLE_SEQUENCE_MAX_SAVE_LENGTH]>(0);
        gConfigTitleSequences.presets[preset].commands = Memory::AllocateArray<title_command>(0);
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
        gConfigTitleSequences.presets = (title_sequence*) realloc(gConfigTitleSequences.presets, sizeof(title_sequence) * (size_t)gConfigTitleSequences.num_presets);
        String::Set(gConfigTitleSequences.presets[preset].name, TITLE_SEQUENCE_NAME_SIZE, name);
        gConfigTitleSequences.presets[preset].path[0] = 0;

        gConfigTitleSequences.presets[preset].saves = Memory::Duplicate(gConfigTitleSequences.presets[duplicate].saves, gConfigTitleSequences.presets[duplicate].num_saves * sizeof(char[TITLE_SEQUENCE_MAX_SAVE_LENGTH]));
        gConfigTitleSequences.presets[preset].commands = Memory::DuplicateArray(gConfigTitleSequences.presets[duplicate].commands, gConfigTitleSequences.presets[duplicate].num_commands);
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
                String::Set(srcPath, MAX_PATH, gConfigTitleSequences.presets[duplicate].path);
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
        gConfigTitleSequences.presets = (title_sequence*) realloc(gConfigTitleSequences.presets, sizeof(title_sequence) * (size_t)gConfigTitleSequences.num_presets);

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

        String::Set(gConfigTitleSequences.presets[preset].name, TITLE_SEQUENCE_NAME_SIZE, newName);

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
    String::Set(newPath, MAX_PATH, newName);
    if (String::Equals(extension, ".sv6", true) && String::Equals(extension, ".sc6", true))
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
        gConfigTitleSequences.presets[preset].saves = Memory::ReallocateArray(gConfigTitleSequences.presets[preset].saves, gConfigTitleSequences.presets[preset].num_saves);

        String::Set(gConfigTitleSequences.presets[preset].saves[gConfigTitleSequences.presets[preset].num_saves - 1], TITLE_SEQUENCE_MAX_SAVE_LENGTH, newName);
        // Add the appropriate extension if needed
        if (!String::Equals(extension, ".sv6", true) && !String::Equals(extension, ".sc6", true))
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
            String::Set(gConfigTitleSequences.presets[preset].saves[i], TITLE_SEQUENCE_MAX_SAVE_LENGTH, gConfigTitleSequences.presets[preset].saves[i + 1]);
        }
        gConfigTitleSequences.presets[preset].num_saves--;
        gConfigTitleSequences.presets[preset].saves = Memory::ReallocateArray(gConfigTitleSequences.presets[preset].saves, gConfigTitleSequences.presets[preset].num_saves);
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

        String::Set(gConfigTitleSequences.presets[preset].saves[index], TITLE_SEQUENCE_MAX_SAVE_LENGTH, newName);
        // Add the appropriate extension if needed
        if (!String::Equals(extension, ".sv6", true) && !String::Equals(extension, ".sc6", true))
            path_append_extension(gConfigTitleSequences.presets[preset].saves[index], ".sv6", TITLE_SEQUENCE_MAX_SAVE_LENGTH);
        title_sequence_save_preset_script(preset);
    }
}


void title_sequence_add_command(int preset, title_command command)
{
    if (preset >= TITLE_SEQUENCE_DEFAULT_PRESETS && preset < gConfigTitleSequences.num_presets) {
        gConfigTitleSequences.presets[preset].commands = Memory::ReallocateArray(gConfigTitleSequences.presets[preset].commands, gConfigTitleSequences.presets[preset].num_commands + 1);
        gConfigTitleSequences.presets[preset].commands[gConfigTitleSequences.presets[preset].num_commands] = command;
        gConfigTitleSequences.presets[preset].num_commands++;
        title_sequence_save_preset_script(preset);
    }
}
void title_sequence_insert_command(int preset, int index, title_command command)
{
    if (preset >= TITLE_SEQUENCE_DEFAULT_PRESETS && preset < gConfigTitleSequences.num_presets && index >= 0 && index <= gConfigTitleSequences.presets[preset].num_commands) {
        gConfigTitleSequences.presets[preset].commands = Memory::ReallocateArray(gConfigTitleSequences.presets[preset].commands, gConfigTitleSequences.presets[preset].num_commands + 1);
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
        gConfigTitleSequences.presets[preset].commands = Memory::ReallocateArray(gConfigTitleSequences.presets[preset].commands, gConfigTitleSequences.presets[preset].num_commands);
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

// Parses a command struct from a json struct
//		json: The json of the command
//		sequence: A pointer to the sequence to update
static title_command title_sequence_command_from_json(json_t* json, title_sequence* sequence)
{
    title_command command;
    const utf8* command_text = json_string_value(json_object_get(json, "command"));
    if (String::Equals(command_text, "load", true))
    {
        command.command = TITLE_SCRIPT_LOAD;
        // Find the index of the save
        const utf8* save_location = json_string_value(json_object_get(json, "file"));
        command.saveIndex = 0xFF;
        for (int i = 0; i < sequence->num_saves && command.saveIndex == 0xFF; i++)
            if (String::Equals(save_location, sequence->saves[i], true))
                command.saveIndex = i;
    }
    else if (String::Equals(command_text, "location", true))
    {
        command.command = TITLE_SCRIPT_LOCATION;
        command.x = (uint8)json_integer_value(json_object_get(json, "x"));
        command.y = (uint8)json_integer_value(json_object_get(json, "y"));
    }
    else if (String::Equals(command_text, "rotate", true))
    {
        command.command = TITLE_SCRIPT_ROTATE;
        command.rotations = (uint8)json_integer_value(json_object_get(json, "rotations"));
    }
    else if (String::Equals(command_text, "zoom", true))
    {
        command.command = TITLE_SCRIPT_ZOOM;
        command.zoom = (uint8)json_integer_value(json_object_get(json, "zoom"));
    }
    else if (String::Equals(command_text, "speed", true))
    {
        command.command = TITLE_SCRIPT_SPEED;
        command.speed = Math::Max((uint8) 1, Math::Min((uint8) 4, (uint8)json_integer_value(json_object_get(json, "speed"))));
    }
    else if (String::Equals(command_text, "wait", true))
    {
        command.command = TITLE_SCRIPT_WAIT;
        command.seconds = (uint8) json_integer_value(json_object_get(json, "seconds"));
    }
    else if (String::Equals(command_text, "restart", true))
    {
        command.command = TITLE_SCRIPT_RESTART;
    }
    else if (String::Equals(command_text, "end", true))
    {
        command.command = TITLE_SCRIPT_END;
    }
    else if (String::Equals(command_text, "loadmm", true))
    {
        command.command = TITLE_SCRIPT_LOADMM;
    }
    else if (String::Equals(command_text, "loadrct1", true))
    {
        command.command = TITLE_SCRIPT_LOADRCT1;
        command.saveIndex = (uint8)json_integer_value(json_object_get(json, "saveIndex"));
    }
    return command;
}

// Parses a title sequence from json
//		json: The json to parse
//		title_sequence: A pointer to the sequence to update
static void title_sequence_from_json(json_t* json, title_sequence* sequence)
{
    sequence->num_commands = (uint16) json_array_size(json);
    sequence->commands = Memory::AllocateArray<title_command>(sequence->num_commands);

    for (int i = 0; i < sequence->num_commands; i++)
        sequence->commands[i] = title_sequence_command_from_json(json_array_get(json, i), sequence);
}

void title_sequence_open(const char *path, const char *customName)
{
    utf8 scriptPath[MAX_PATH], titlePath[MAX_PATH];
    file_info fileInfo;
    int fileEnumHandle, preset;

    // Check for the script file
    String::Set(scriptPath, MAX_PATH, path);
    safe_strcat_path(scriptPath, "script.json", MAX_PATH);

    if (!platform_file_exists(scriptPath))
    {
        // No script file, title sequence is invalid
        log_error("Title sequence script file doesn't exist: %s\n", scriptPath);
        return;
    }

    // Check if the preset is already loaded
    // No need to read the first two presets as they're hardcoded in
    for (preset = 0; preset < gConfigTitleSequences.num_presets; preset++)
        if (String::Equals(path, gConfigTitleSequences.presets[preset].name, true))
            return;

    // Otherwise allocate one
    if (preset == gConfigTitleSequences.num_presets)
    {
        gConfigTitleSequences.num_presets++;
        gConfigTitleSequences.presets = Memory::ReallocateArray(gConfigTitleSequences.presets, (size_t) gConfigTitleSequences.num_presets);

        if (customName == NULL)
        {
            StringBuilder* nameBuffer = new StringBuilder();
            nameBuffer->Append(path);
            // Get folder name
            // First find the last index of the separator
            size_t folderNameEnd = String::LastIndexOf(nameBuffer->GetBuffer(), *PATH_SEPARATOR);
            // Remove everything after that
            StringBuilder* nameBuffer2 = nameBuffer->SubString(0, folderNameEnd);
            // Find the separator before it
            size_t folderNameStart = String::LastIndexOf(nameBuffer2->GetBuffer(), *PATH_SEPARATOR);
            // Remove everything befoe that
            StringBuilder* nameBuffer3 = nameBuffer2->SubString(folderNameStart + 1, nameBuffer2->GetLength());
            
            Memory::CopyArray<char>(gConfigTitleSequences.presets[preset].name, nameBuffer3->GetBuffer(), TITLE_SEQUENCE_NAME_SIZE);
            gConfigTitleSequences.presets[preset].path[0] = '\0';
            delete nameBuffer;
            delete nameBuffer2;
            delete nameBuffer3;
        }
        else
        {
            Memory::CopyArray<char>(gConfigTitleSequences.presets[preset].name, customName, TITLE_SEQUENCE_NAME_SIZE);
            Memory::CopyArray<char>(gConfigTitleSequences.presets[preset].path, path, MAX_PATH);
        }

        gConfigTitleSequences.presets[preset].saves = Memory::AllocateArray<char[TITLE_SEQUENCE_MAX_SAVE_LENGTH]>(0);
        gConfigTitleSequences.presets[preset].num_saves = 0;
        gConfigTitleSequences.presets[preset].num_commands = 0;
    }

    // Get the save file list
    // Load sv6 files
    String::Set(titlePath, MAX_PATH, path);
    safe_strcat_path(titlePath, "*.sv6", MAX_PATH);
    fileEnumHandle = platform_enumerate_files_begin(titlePath);
    while (platform_enumerate_files_next(fileEnumHandle, &fileInfo))
    {
        gConfigTitleSequences.presets[preset].num_saves++;
        gConfigTitleSequences.presets[preset].saves = Memory::ReallocateArray(gConfigTitleSequences.presets[preset].saves, gConfigTitleSequences.presets[preset].num_saves);
        Memory::Copy(gConfigTitleSequences.presets[preset].saves[gConfigTitleSequences.presets[preset].num_saves - 1], fileInfo.path, sizeof(char[TITLE_SEQUENCE_MAX_SAVE_LENGTH]));
        gConfigTitleSequences.presets[preset].saves[gConfigTitleSequences.presets[preset].num_saves - 1][TITLE_SEQUENCE_MAX_SAVE_LENGTH - 1] = '\0';
    }
    platform_enumerate_files_end(fileEnumHandle);

    // Load sc6 files
    String::Set(titlePath, MAX_PATH, path);
    safe_strcat_path(titlePath, "*.sc6", MAX_PATH);
    fileEnumHandle = platform_enumerate_files_begin(titlePath);
    while (platform_enumerate_files_next(fileEnumHandle, &fileInfo))
    {
        gConfigTitleSequences.presets[preset].num_saves++;
        gConfigTitleSequences.presets[preset].saves = Memory::ReallocateArray(gConfigTitleSequences.presets[preset].saves, gConfigTitleSequences.presets[preset].num_saves);
        Memory::Copy(gConfigTitleSequences.presets[preset].saves[gConfigTitleSequences.presets[preset].num_saves - 1], fileInfo.path, sizeof(char[TITLE_SEQUENCE_MAX_SAVE_LENGTH]));
        gConfigTitleSequences.presets[preset].saves[gConfigTitleSequences.presets[preset].num_saves - 1][TITLE_SEQUENCE_MAX_SAVE_LENGTH - 1] = '\0';
    }
    platform_enumerate_files_end(fileEnumHandle);

    // Open the json and parse the sequence
    try
    {
        json_t* json = Json::ReadFromFile(scriptPath);
        title_sequence_from_json(json, &gConfigTitleSequences.presets[preset]);
        SafeFree(json);
    }
    catch (Exception ex)
    {
        log_error("Unable to load title sequence %s: %s", scriptPath, ex.GetMessage());
    }
}

// Save the preset with index preset to the corresponding JSON file
void title_sequence_save_preset_script(int preset)
{
    utf8 path[MAX_PATH];
    int i;
    json_t* jsonCommands;

    platform_get_user_directory(path, "title sequences", MAX_PATH);
    safe_strcat_path(path, gConfigTitleSequences.presets[preset].name, MAX_PATH);
    safe_strcat_path(path, "script.json", MAX_PATH);

    // Create a new json array
    jsonCommands = json_array();

    // Add all commands
    for (i = 0; i < gConfigTitleSequences.presets[preset].num_commands; i++)
    {
        title_command *command = &gConfigTitleSequences.presets[preset].commands[i];
        json_t* jsonCommand = json_object();
        switch (command->command)
        {
        case TITLE_SCRIPT_LOAD:
            json_object_set_new(jsonCommand, "command", json_string("load"));
            if (command->saveIndex != 0xFF)
                json_object_set_new(jsonCommand, "file", json_string(gConfigTitleSequences.presets[preset].saves[command->saveIndex]));
            break;
        case TITLE_SCRIPT_LOCATION:
            json_object_set_new(jsonCommand, "command", json_string("location"));
            json_object_set_new(jsonCommand, "x", json_integer(command->x));
            json_object_set_new(jsonCommand, "y", json_integer(command->y));
            break;
        case TITLE_SCRIPT_ROTATE:
            json_object_set_new(jsonCommand, "command", json_string("rotate"));
            json_object_set_new(jsonCommand, "rotations", json_integer(command->rotations));
            break;
        case TITLE_SCRIPT_ZOOM:
            json_object_set_new(jsonCommand, "command", json_string("zoom"));
            json_object_set_new(jsonCommand, "zoom", json_integer(command->zoom));
            break;
        case TITLE_SCRIPT_SPEED:
            json_object_set_new(jsonCommand, "command", json_string("speed"));
            json_object_set_new(jsonCommand, "speed", json_integer(command->speed));
            break;
        case TITLE_SCRIPT_WAIT:
            json_object_set_new(jsonCommand, "command", json_string("wait"));
            json_object_set_new(jsonCommand, "seconds", json_integer(command->seconds));
            break;
        case TITLE_SCRIPT_RESTART:
            json_object_set_new(jsonCommand, "command", json_string("restart"));
            break;
        case TITLE_SCRIPT_END:
            json_object_set_new(jsonCommand, "command", json_string("end"));
            break;
        }
        json_array_append_new(jsonCommands, jsonCommand);
    }

    // Write to file
    try
    {
        Json::WriteToFile(path, jsonCommands, JSON_INDENT(4) | JSON_PRESERVE_ORDER);
    }
    catch (Exception ex)
    {
        log_error("Unable to save title sequence %s: %s", path, ex.GetMessage());
    }

    SafeFree(jsonCommands);
}
