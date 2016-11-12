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

#include "../common.h"
#include <SDL.h>
#include <zip.h>
#include <vector>
#include "../core/Collections.hpp"
#include "../core/Guard.hpp"
#include "../core/Math.hpp"
#include "../core/Memory.hpp"
#include "../core/Path.hpp"
#include "../core/String.hpp"
#include "TitleSequence.h"

static std::vector<utf8 *> GetSaves(zip_t * zip);
static std::vector<TitleCommand> LegacyScriptRead(utf8 * script, size_t scriptLength, std::vector<utf8 *> saves);
static void LegacyScriptGetLine(SDL_RWops * file, char * parts);
static void * GetZipFileData(zip_t * zip, const char * name, size_t * outSize);

extern "C"
{
    TitleSequence * LoadTitleSequence(const utf8 * path)
    {
        int error;
        zip_t * zip = zip_open(path, ZIP_RDONLY, &error);
        if (zip == nullptr)
        {
            // Unable to open zip
            return nullptr;
        }

        size_t scriptLength;
        char * script = (char *)GetZipFileData(zip, "script.txt", &scriptLength);
        if (script == NULL) {
            // Unable to open script
            zip_close(zip);
            return nullptr;
        }

        std::vector<utf8 *> saves = GetSaves(zip);
        std::vector<TitleCommand> commands = LegacyScriptRead(script, scriptLength, saves);

        TitleSequence * seq = Memory::Allocate<TitleSequence>();
        seq->Name = Path::GetFileNameWithoutExtension(path);
        seq->Path = String::Duplicate(path);
        seq->NumSaves = saves.size();
        seq->Saves = Collections::ToArray(saves);
        seq->NumCommands = commands.size();
        seq->Commands = Collections::ToArray(commands);
        return seq;
    }

    void FreeTitleSequence(TitleSequence * seq)
    {
        if (seq != nullptr)
        {
            Memory::Free(seq->Name);
            Memory::Free(seq->Path);
            Memory::Free(seq->Commands);
            for (size_t i = 0; i < seq->NumSaves; i++)
            {
                Memory::Free(seq->Saves[i]);
            }
            Memory::Free(seq->Saves);
        }
    }

    TitleSequenceParkHandle * TitleSequenceGetParkHandle(TitleSequence * seq, size_t index)
    {
        int error;
        zip_t * zip = zip_open(seq->Path, ZIP_RDONLY, &error);
        if (zip == nullptr)
        {
            // Unable to open zip
            return nullptr;
        }

        Guard::Assert(index < seq->NumSaves, "Invalid park save index.");
        utf8 * filename = seq->Saves[index];

        auto handle = Memory::Allocate<TitleSequenceParkHandle>();
        handle->Data = GetZipFileData(zip, filename, &handle->DataSize);
        handle->RWOps = SDL_RWFromMem(handle->Data, (int)handle->DataSize);
        handle->IsScenario = String::Equals(Path::GetExtension(filename), ".sc6", true);
        return handle;
    }

    void TitleSequenceCloseParkHandle(TitleSequenceParkHandle * handle)
    {
        SDL_RWclose(handle->RWOps);
        Memory::Free(handle->Data);
        Memory::Free(handle);
    }
}

static std::vector<utf8 *> GetSaves(zip_t * zip)
{
    std::vector<utf8 *> saves;
    int numFiles = zip_get_num_files(zip);
    for (int i = 0; i < numFiles; i++)
    {
        const utf8 * name = zip_get_name(zip, i, ZIP_FL_ENC_GUESS);
        const utf8 * ext = Path::GetExtension(name);
        if (String::Equals(ext, ".sv6", true) ||
            String::Equals(ext, ".sc6", true))
        {
            saves.push_back(String::Duplicate(name));
        }
    }
    return saves;
}

static std::vector<TitleCommand> LegacyScriptRead(utf8 * script, size_t scriptLength, std::vector<utf8 *> saves)
{
    std::vector<TitleCommand> commands;
    SDL_RWops * file = SDL_RWFromMem(script, (int)scriptLength);
    do {
        char parts[3 * 128], *token, *part1, *part2;
        LegacyScriptGetLine(file, parts);

        token = &parts[0 * 128];
        part1 = &parts[1 * 128];
        part2 = &parts[2 * 128];
        TitleCommand command = { 0 };
        command.Type = TITLE_SCRIPT_UNDEFINED;

        if (token[0] != 0)
        {
            if (_stricmp(token, "LOAD") == 0)
            {
                command.Type = TITLE_SCRIPT_LOAD;
                command.SaveIndex = UINT8_MAX;
                for (size_t i = 0; i < saves.size(); i++)
                {
                    if (String::Equals(part1, saves[i], true))
                    {
                        command.SaveIndex = (uint8)i;
                        break;
                    }
                }
            }
            else if (_stricmp(token, "LOCATION") == 0)
            {
                command.Type = TITLE_SCRIPT_LOCATION;
                command.X = atoi(part1) & 0xFF;
                command.Y = atoi(part2) & 0xFF;
            }
            else if (_stricmp(token, "ROTATE") == 0)
            {
                command.Type = TITLE_SCRIPT_ROTATE;
                command.Rotations = atoi(part1) & 0xFF;
            }
            else if (_stricmp(token, "ZOOM") == 0)
            {
                command.Type = TITLE_SCRIPT_ZOOM;
                command.Zoom = atoi(part1) & 0xFF;
            }
            else if (_stricmp(token, "SPEED") == 0)
            {
                command.Type = TITLE_SCRIPT_SPEED;
                command.Speed = Math::Max(1, Math::Min(4, atoi(part1) & 0xFF));
            }
            else if (_stricmp(token, "WAIT") == 0)
            {
                command.Type = TITLE_SCRIPT_WAIT;
                command.Seconds = atoi(part1) & 0xFF;
            }
            else if (_stricmp(token, "RESTART") == 0)
            {
                command.Type = TITLE_SCRIPT_RESTART;
            }
            else if (_stricmp(token, "END") == 0)
            {
                command.Type = TITLE_SCRIPT_END;
            }
            else if (_stricmp(token, "LOADMM") == 0)
            {
                command.Type = TITLE_SCRIPT_LOADMM;
            }
            else if (_stricmp(token, "LOADRCT1") == 0)
            {
                command.Type = TITLE_SCRIPT_LOADRCT1;
                command.SaveIndex = atoi(part1) & 0xFF;
            }
        }
        if (command.Type != TITLE_SCRIPT_UNDEFINED)
        {
            commands.push_back(command);
        }
    } while (SDL_RWtell(file) < (int)scriptLength);
    SDL_RWclose(file);

    return commands;
}

static void LegacyScriptGetLine(SDL_RWops * file, char * parts)
{
    for (int i = 0; i < 3; i++)
    {
        parts[i * 128] = 0;
    }
    int part = 0;
    int cindex = 0;
    int whitespace = 1;
    int comment = 0;
    int load = 0;
    for (; part < 3;)
    {
        int c = 0;
        if (SDL_RWread(file, &c, 1, 1) != 1)
        {
            c = EOF;
        }
        if (c == '\n' || c == '\r' || c == EOF)
        {
            parts[part * 128 + cindex] = 0;
            return;
        }
        else if (c == '#')
        {
            parts[part * 128 + cindex] = 0;
            comment = 1;
        }
        else if (c == ' ' && !comment && !load)
        {
            if (!whitespace)
            {
                if (part == 0 && cindex == 4 && _strnicmp(parts, "LOAD", 4) == 0)
                {
                    load = true;
                }
                parts[part * 128 + cindex] = 0;
                part++;
                cindex = 0;
            }
        }
        else if (!comment)
        {
            whitespace = 0;
            if (cindex < 127)
            {
                parts[part * 128 + cindex] = c;
                cindex++;
            }
            else
            {
                parts[part * 128 + cindex] = 0;
                part++;
                cindex = 0;
            }
        }
    }
}

static void * GetZipFileData(zip_t * zip, const char * name, size_t * outSize)
{
    void * data = nullptr;
    size_t dataSize = 0;

    zip_stat_t zipFileStat;
    if (zip_stat(zip, name, 0, &zipFileStat) == ZIP_ER_OK)
    {
        zip_file_t * zipFile = zip_fopen(zip, name, 0);
        if (zipFile != nullptr)
        {
            if (zipFileStat.size < SIZE_MAX)
            {
                dataSize = zipFileStat.size;
                data = malloc(dataSize);
                size_t readBytes = zip_fread(zipFile, data, dataSize);
                if (readBytes != dataSize)
                {
                    free(data);
                    data = NULL;
                    dataSize = 0;
                }
                zip_fclose(zipFile);
            }
        }
    }

    if (outSize != NULL) *outSize = dataSize;
    return data;
}

/*
void title_sequence_save_preset_script(int preset)
{
    utf8 path[MAX_PATH];
    SDL_RWops *file;
    int i;


    platform_get_user_directory(path, "title sequences", sizeof(path));
    safe_strcat_path(path, gConfigTitleSequences.presets[preset].name, MAX_PATH);
    safe_strcat_path(path, "script.txt", MAX_PATH);

    file = SDL_RWFromFile(path, "wb");
    if (file == NULL) {
        log_error("Unable to write to script file.");
        return;
    }

    for (i = 0; i < gConfigTitleSequences.presets[preset].num_commands; i++) {
        title_command *command = &gConfigTitleSequences.presets[preset].commands[i];
        switch (command->command) {
        case TITLE_SCRIPT_LOAD:
            if (command->saveIndex == 0xFF)
                rwopsprintf(file, "LOAD <No save file>");
            else
                rwopsprintf(file, "LOAD %s", gConfigTitleSequences.presets[preset].saves[command->saveIndex]);
            break;
        case TITLE_SCRIPT_LOCATION:
            rwopsprintf(file, "LOCATION %i %i", command->x, command->y);
            break;
        case TITLE_SCRIPT_ROTATE:
            rwopsprintf(file, "ROTATE %i", command->rotations);
            break;
        case TITLE_SCRIPT_ZOOM:
            rwopsprintf(file, "ZOOM %i", command->zoom);
            break;
        case TITLE_SCRIPT_SPEED:
            rwopsprintf(file, "SPEED %i", command->speed);
            break;
        case TITLE_SCRIPT_WAIT:
            rwopsprintf(file, "WAIT %i", command->seconds);
            rwopswritenewline(file);
            break;
        case TITLE_SCRIPT_RESTART:
            rwopsprintf(file, "RESTART");
            break;
        case TITLE_SCRIPT_END:
            rwopsprintf(file, "END");
            break;
        }
        rwopswritenewline(file);
    }

    SDL_RWclose(file);
}
*/
