#pragma region Copyright (c) 2014-2017 OpenRCT2 Developers
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

#include <vector>
#include "../common.h"
#include "../core/Collections.hpp"
#include "../core/Console.hpp"
#include "../core/File.h"
#include "../core/FileScanner.h"
#include "../core/FileStream.hpp"
#include "../core/Guard.hpp"
#include "../core/Math.hpp"
#include "../core/Memory.hpp"
#include "../core/MemoryStream.h"
#include "../core/Path.hpp"
#include "../core/String.hpp"
#include "../core/StringBuilder.hpp"
#include "../core/Zip.h"
#include "TitleSequence.h"

#ifndef MAX_PATH
    #define MAX_PATH 260
#endif

static std::vector<utf8 *> GetSaves(const utf8 * path);
static std::vector<utf8 *> GetSaves(IZipArchive * zip);
static std::vector<TitleCommand> LegacyScriptRead(utf8 * script, size_t scriptLength, std::vector<utf8 *> saves);
static void LegacyScriptGetLine(IStream * stream, char * parts);
static void * ReadScriptFile(const utf8 * path, size_t * outSize);
static utf8 * LegacyScriptWrite(TitleSequence * seq);

extern "C"
{
    TitleSequence * CreateTitleSequence()
    {
        TitleSequence * seq = Memory::Allocate<TitleSequence>();
        Memory::Set(seq, 0, sizeof(TitleSequence));
        return seq;
    }

    TitleSequence * LoadTitleSequence(const utf8 * path)
    {
        size_t scriptLength;
        utf8 * script;
        std::vector<utf8 *> saves;
        bool isZip;

        log_verbose("Loading title sequence: %s", path);

        const utf8 * ext = Path::GetExtension(path);
        if (String::Equals(ext, TITLE_SEQUENCE_EXTENSION))
        {
            IZipArchive * zip = Zip::TryOpen(path, ZIP_ACCESS_READ);
            if (zip == nullptr)
            {
                Console::Error::WriteLine("Unable to open '%s'", path);
                return nullptr;
            }

            script = (utf8 *)zip->GetFileData("script.txt", &scriptLength);
            if (script == nullptr)
            {
                Console::Error::WriteLine("Unable to open script.txt in '%s'", path);
                delete zip;
                return nullptr;
            }

            saves = GetSaves(zip);
            isZip = true;

            delete zip;
        }
        else
        {
            utf8 scriptPath[MAX_PATH];
            String::Set(scriptPath, sizeof(scriptPath), path);
            Path::Append(scriptPath, sizeof(scriptPath), "script.txt");
            script = (utf8 *)ReadScriptFile(scriptPath, &scriptLength);
            if (script == nullptr)
            {
                Console::Error::WriteLine("Unable to open '%s'", scriptPath);
                return nullptr;
            }

            saves = GetSaves(path);
            isZip = false;
        }

        std::vector<TitleCommand> commands = LegacyScriptRead(script, scriptLength, saves);
        Memory::Free(script);

        TitleSequence * seq = CreateTitleSequence();
        seq->Name = Path::GetFileNameWithoutExtension(path);
        seq->Path = String::Duplicate(path);
        seq->NumSaves = saves.size();
        seq->Saves = Collections::ToArray(saves);
        seq->NumCommands = commands.size();
        seq->Commands = Collections::ToArray(commands);
        seq->IsZip = isZip;
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
            Memory::Free(seq);
        }
    }

    TitleSequenceParkHandle * TitleSequenceGetParkHandle(TitleSequence * seq, size_t index)
    {
        TitleSequenceParkHandle * handle = nullptr;
        if (index <= seq->NumSaves)
        {
            const utf8 * filename = seq->Saves[index];
            if (seq->IsZip)
            {
                IZipArchive * zip = Zip::TryOpen(seq->Path, ZIP_ACCESS_READ);
                if (zip != nullptr)
                {
                    handle = Memory::Allocate<TitleSequenceParkHandle>();
                    handle->Stream = zip->GetFileStream(filename);
                    handle->HintPath = String::Duplicate(filename);
                    delete zip;
                } else {
                    Console::Error::WriteLine("Failed to open zipped path '%s' from zip '%s'", filename, seq->Path);
                }
            }
            else
            {
                utf8 absolutePath[MAX_PATH];
                String::Set(absolutePath, sizeof(absolutePath), seq->Path);
                Path::Append(absolutePath, sizeof(absolutePath), filename);

                handle = Memory::Allocate<TitleSequenceParkHandle>();
                handle->Stream = new FileStream(absolutePath, FILE_MODE_OPEN);
                handle->HintPath = String::Duplicate(filename);
            }
        }
        return handle;
    }

    void TitleSequenceCloseParkHandle(TitleSequenceParkHandle * handle)
    {
        if (handle != nullptr)
        {
            Memory::Free(handle->HintPath);
            delete ((IStream *)handle->Stream);
            Memory::Free(handle);
        }
    }

    bool TileSequenceSave(TitleSequence * seq)
    {
        bool success = false;
        utf8 * script = LegacyScriptWrite(seq);
        if (seq->IsZip)
        {
            IZipArchive * zip = Zip::TryOpen(seq->Path, ZIP_ACCESS_WRITE);
            zip->SetFileData("script.txt", script, String::SizeOf(script));
            delete zip;
            success = true;
        }
        else
        {
            utf8 scriptPath[MAX_PATH];
            String::Set(scriptPath, sizeof(scriptPath), seq->Path);
            Path::Append(scriptPath, sizeof(scriptPath), "script.txt");

            try
            {
                auto fs = FileStream(scriptPath, FILE_MODE_WRITE);
                fs.Write(script, String::SizeOf(script));
                success = true;
            }
            catch (Exception)
            {
            }
        }

        Memory::Free(script);
        return success;
    }

    bool TileSequenceAddPark(TitleSequence * seq, const utf8 * path, const utf8 * name)
    {
        // Get new save index
        size_t index = SIZE_MAX;
        for (size_t i = 0; i < seq->NumSaves; i++)
        {
            if (String::Equals(seq->Saves[i], path, true))
            {
                index = i;
                break;
            }
        }
        if (index == SIZE_MAX)
        {
            seq->Saves = Memory::ReallocateArray(seq->Saves, seq->NumSaves + 1);
            Guard::Assert(seq->Saves != nullptr, GUARD_LINE);
            index = seq->NumSaves;
            seq->NumSaves++;
        }
        seq->Saves[index] = String::Duplicate(name);

        if (seq->IsZip)
        {
            try
            {
                size_t fsize;
                void * fdata = File::ReadAllBytes(path, &fsize);

                IZipArchive * zip = Zip::TryOpen(seq->Path, ZIP_ACCESS_WRITE);
                if (zip == nullptr)
                {
                    Console::Error::WriteLine("Unable to open '%s'", seq->Path);
                    return false;
                }
                zip->SetFileData(name, fdata, fsize);
                delete zip;
                Memory::Free(fdata);
            }
            catch (const Exception &ex)
            {
                Console::Error::WriteLine(ex.GetMessage());
            }
        }
        else
        {
            // Determine destination path
            utf8 dstPath[MAX_PATH];
            String::Set(dstPath, sizeof(dstPath), seq->Path);
            Path::Append(dstPath, sizeof(dstPath), name);
            if (!File::Copy(path, dstPath, true))
            {
                Console::Error::WriteLine("Unable to copy '%s' to '%s'", path, dstPath);
                return false;
            }
        }
        return true;
    }

    bool TileSequenceRenamePark(TitleSequence * seq, size_t index, const utf8 * name)
    {
        Guard::Assert(index < seq->NumSaves, GUARD_LINE);

        utf8 * oldRelativePath = seq->Saves[index];
        if (seq->IsZip)
        {
            IZipArchive * zip = Zip::TryOpen(seq->Path, ZIP_ACCESS_WRITE);
            if (zip == nullptr)
            {
                Console::Error::WriteLine("Unable to open '%s'", seq->Path);
                return false;
            }
            zip->RenameFile(oldRelativePath, name);
            delete zip;
        }
        else
        {
            utf8 srcPath[MAX_PATH];
            utf8 dstPath[MAX_PATH];
            String::Set(srcPath, sizeof(srcPath), seq->Path);
            Path::Append(srcPath, sizeof(srcPath), oldRelativePath);
            String::Set(dstPath, sizeof(dstPath), seq->Path);
            Path::Append(dstPath, sizeof(dstPath), name);
            if (!File::Move(srcPath, dstPath))
            {
                Console::Error::WriteLine("Unable to move '%s' to '%s'", srcPath, dstPath);
                return false;
            }
        }

        Memory::Free(seq->Saves[index]);
        seq->Saves[index] = String::Duplicate(name);
        return true;
    }

    bool TitleSequenceRemovePark(TitleSequence * seq, size_t index)
    {
        Guard::Assert(index < seq->NumSaves, GUARD_LINE);

        // Delete park file
        utf8 * relativePath = seq->Saves[index];
        if (seq->IsZip)
        {
            IZipArchive * zip = Zip::TryOpen(seq->Path, ZIP_ACCESS_WRITE);
            if (zip == nullptr)
            {
                Console::Error::WriteLine("Unable to open '%s'", seq->Path);
                return false;
            }
            zip->DeleteFile(relativePath);
            delete zip;
        }
        else
        {
            utf8 absolutePath[MAX_PATH];
            String::Set(absolutePath, sizeof(absolutePath), seq->Path);
            Path::Append(absolutePath, sizeof(absolutePath), relativePath);
            if (!File::Delete(absolutePath))
            {
                Console::Error::WriteLine("Unable to delete '%s'", absolutePath);
                return false;
            }
        }

        // Remove from sequence
        Memory::Free(relativePath);
        for (size_t i = index; i < seq->NumSaves - 1; i++)
        {
            seq->Saves[i] = seq->Saves[i + 1];
        }
        seq->NumSaves--;

        // Update load commands
        for (size_t i = 0; i < seq->NumCommands; i++)
        {
            TitleCommand * command = &seq->Commands[i];
            if (command->Type == TITLE_SCRIPT_LOAD)
            {
                if (command->SaveIndex == index)
                {
                    // Park no longer exists, so reset load command to invalid
                    command->SaveIndex = SAVE_INDEX_INVALID;
                }
                else if (command->SaveIndex > index)
                {
                    // Park index will have shifted by -1
                    command->SaveIndex--;
                }
            }
        }

        return true;
    }
}

static std::vector<utf8 *> GetSaves(const utf8 * directory)
{
    std::vector<utf8 *> saves;

    utf8 pattern[MAX_PATH];
    String::Set(pattern, sizeof(pattern), directory);
    Path::Append(pattern, sizeof(pattern), "*.sc6;*.sv6");

    IFileScanner * scanner = Path::ScanDirectory(pattern, true);
    while (scanner->Next())
    {
        const utf8 * path = scanner->GetPathRelative();
        saves.push_back(String::Duplicate(path));
    }
    return saves;
}

static std::vector<utf8 *> GetSaves(IZipArchive * zip)
{
    std::vector<utf8 *> saves;
    size_t numFiles = zip->GetNumFiles();
    for (size_t i = 0; i < numFiles; i++)
    {
        const utf8 * name = zip->GetFileName(i);
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
    auto fs = MemoryStream(script, scriptLength);
    do
    {
        char parts[3 * 128], *token, *part1, *part2;
        LegacyScriptGetLine(&fs, parts);

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
                command.SaveIndex = SAVE_INDEX_INVALID;
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
    }
    while (fs.GetPosition() < scriptLength);
    return commands;
}

static void LegacyScriptGetLine(IStream * stream, char * parts)
{
    for (sint32 i = 0; i < 3; i++)
    {
        parts[i * 128] = 0;
    }
    sint32 part = 0;
    sint32 cindex = 0;
    sint32 whitespace = 1;
    sint32 comment = 0;
    sint32 load = 0;
    for (; part < 3;)
    {
        sint32 c = 0;
        if (stream->TryRead(&c, 1) != 1)
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

static void * ReadScriptFile(const utf8 * path, size_t * outSize)
{
    void * buffer = nullptr;
    size_t size = 0;
    try
    {
        auto fs = FileStream(path, FILE_MODE_OPEN);
        size = (size_t)fs.GetLength();
        buffer = Memory::Allocate<void>(size);
        fs.Read(buffer, size);
    }
    catch (Exception)
    {
        Memory::Free(buffer);
        buffer = nullptr;
        size = 0;
    }

    *outSize = size;
    return buffer;
}

static utf8 * LegacyScriptWrite(TitleSequence * seq)
{
    utf8 buffer[128];
    auto sb = StringBuilder(128);

    sb.Append("# SCRIPT FOR ");
    sb.Append(seq->Name);
    sb.Append("\n");
    for (size_t i = 0; i < seq->NumCommands; i++)
    {
        const TitleCommand * command = &seq->Commands[i];
        switch (command->Type) {
        case TITLE_SCRIPT_LOAD:
            if (command->SaveIndex == 0xFF)
            {
                sb.Append("LOAD <No save file>");
            } else
            {
                sb.Append("LOAD ");
                sb.Append(seq->Saves[command->SaveIndex]);
            }
            break;
        case TITLE_SCRIPT_LOCATION:
            String::Format(buffer, sizeof(buffer), "LOCATION %u %u", command->X, command->Y);
            sb.Append(buffer);
            break;
        case TITLE_SCRIPT_ROTATE:
            String::Format(buffer, sizeof(buffer), "ROTATE %u", command->Rotations);
            sb.Append(buffer);
            break;
        case TITLE_SCRIPT_ZOOM:
            String::Format(buffer, sizeof(buffer), "ZOOM %u", command->Zoom);
            sb.Append(buffer);
            break;
        case TITLE_SCRIPT_SPEED:
            String::Format(buffer, sizeof(buffer), "SPEED %u", command->Speed);
            sb.Append(buffer);
            break;
        case TITLE_SCRIPT_WAIT:
            String::Format(buffer, sizeof(buffer), "WAIT %u", command->Seconds);
            sb.Append(buffer);
            break;
        case TITLE_SCRIPT_RESTART:
            sb.Append("RESTART");
            break;
        case TITLE_SCRIPT_END:
            sb.Append("END");
        }
        sb.Append("\n");
    }

    utf8 * result = sb.StealString();
    return result;
}

bool TitleSequenceIsLoadCommand(const TitleCommand * command)
{
    switch (command->Type) {
    case TITLE_SCRIPT_LOADMM:
    case TITLE_SCRIPT_LOAD:
    case TITLE_SCRIPT_LOADRCT1:
        return true;
    default:
        return false;
    }
}
