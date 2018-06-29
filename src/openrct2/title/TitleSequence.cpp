/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include <memory>
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
#include "../scenario/ScenarioRepository.h"
#include "../scenario/ScenarioSources.h"
#include "../util/Util.h"
#include "TitleSequence.h"


static std::vector<utf8 *> GetSaves(const utf8 * path);
static std::vector<utf8 *> GetSaves(IZipArchive * zip);
static std::vector<TitleCommand> LegacyScriptRead(utf8 * script, size_t scriptLength, std::vector<utf8 *> saves);
static void LegacyScriptGetLine(IStream * stream, char * parts);
static std::vector<uint8_t> ReadScriptFile(const utf8 * path);
static std::string LegacyScriptWrite(TitleSequence * seq);

TitleSequence * CreateTitleSequence()
{
    TitleSequence * seq = Memory::Allocate<TitleSequence>();
    *seq = {};
    return seq;
}

TitleSequence * LoadTitleSequence(const utf8 * path)
{
    std::vector<uint8_t> script;
    std::vector<utf8 *> saves;
    bool isZip;

    log_verbose("Loading title sequence: %s", path);

    const utf8 * ext = Path::GetExtension(path);
    if (String::Equals(ext, TITLE_SEQUENCE_EXTENSION))
    {
        auto zip = std::unique_ptr<IZipArchive>(Zip::TryOpen(path, ZIP_ACCESS::READ));
        if (zip == nullptr)
        {
            Console::Error::WriteLine("Unable to open '%s'", path);
            return nullptr;
        }

        script = zip->GetFileData("script.txt");
        if (script.empty())
        {
            Console::Error::WriteLine("Unable to open script.txt in '%s'", path);
            return nullptr;
        }

        saves = GetSaves(zip.get());
        isZip = true;
    }
    else
    {
        utf8 scriptPath[MAX_PATH];
        String::Set(scriptPath, sizeof(scriptPath), path);
        Path::Append(scriptPath, sizeof(scriptPath), "script.txt");
        script = ReadScriptFile(scriptPath);
        if (script.empty())
        {
            Console::Error::WriteLine("Unable to open '%s'", scriptPath);
            return nullptr;
        }

        saves = GetSaves(path);
        isZip = false;
    }

    auto commands = LegacyScriptRead((utf8 *)script.data(), script.size(), saves);

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
            auto zip = std::unique_ptr<IZipArchive>(Zip::TryOpen(seq->Path, ZIP_ACCESS::READ));
            if (zip != nullptr)
            {
                auto data = zip->GetFileData(filename);
                auto dataForMs = Memory::Allocate<uint8_t>(data.size());
                std::copy_n(data.data(), data.size(), dataForMs);
                auto ms = new MemoryStream(dataForMs, data.size(), MEMORY_ACCESS::READ | MEMORY_ACCESS::OWNER);

                handle = Memory::Allocate<TitleSequenceParkHandle>();
                handle->Stream = ms;
                handle->HintPath = String::Duplicate(filename);
            } else {
                Console::Error::WriteLine("Failed to open zipped path '%s' from zip '%s'", filename, seq->Path);
            }
        }
        else
        {
            utf8 absolutePath[MAX_PATH];
            String::Set(absolutePath, sizeof(absolutePath), seq->Path);
            Path::Append(absolutePath, sizeof(absolutePath), filename);

            FileStream* fileStream = nullptr;
            try
            {
                fileStream = new FileStream(absolutePath, FILE_MODE_OPEN);
            }
            catch (const IOException& exception)
            {
                Console::Error::WriteLine(exception.what());
            }

            if (fileStream != nullptr)
            {
                handle = Memory::Allocate<TitleSequenceParkHandle>();
                handle->Stream = fileStream;
                handle->HintPath = String::Duplicate(filename);
            }
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

bool TitleSequenceSave(TitleSequence * seq)
{
    try
    {
        auto script = LegacyScriptWrite(seq);
        if (seq->IsZip)
        {
            auto fdata = std::vector<uint8_t>(script.begin(), script.end());
            auto zip = Zip::Open(seq->Path, ZIP_ACCESS::WRITE);
            zip->SetFileData("script.txt", std::move(fdata));
        }
        else
        {
            auto scriptPath = Path::Combine(seq->Path, "script.txt");
            File::WriteAllBytes(scriptPath, script.data(), script.size());
        }
        return true;
    }
    catch (const std::exception &)
    {
        return false;
    }
}

bool TitleSequenceAddPark(TitleSequence * seq, const utf8 * path, const utf8 * name)
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
            auto fdata = File::ReadAllBytes(path);
            auto zip = Zip::TryOpen(seq->Path, ZIP_ACCESS::WRITE);
            if (zip == nullptr)
            {
                Console::Error::WriteLine("Unable to open '%s'", seq->Path);
                return false;
            }
            zip->SetFileData(name, std::move(fdata));
        }
        catch (const std::exception &ex)
        {
            Console::Error::WriteLine(ex.what());
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

bool TitleSequenceRenamePark(TitleSequence * seq, size_t index, const utf8 * name)
{
    Guard::Assert(index < seq->NumSaves, GUARD_LINE);

    utf8 * oldRelativePath = seq->Saves[index];
    if (seq->IsZip)
    {
        auto zip = Zip::TryOpen(seq->Path, ZIP_ACCESS::WRITE);
        if (zip == nullptr)
        {
            Console::Error::WriteLine("Unable to open '%s'", seq->Path);
            return false;
        }
        zip->RenameFile(oldRelativePath, name);
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
        auto zip = Zip::TryOpen(seq->Path, ZIP_ACCESS::WRITE);
        if (zip == nullptr)
        {
            Console::Error::WriteLine("Unable to open '%s'", seq->Path);
            return false;
        }
        zip->DeleteFile(relativePath);
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
        auto name = zip->GetFileName(i);
        auto ext = Path::GetExtension(name);
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
        TitleCommand command = {};
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
                        command.SaveIndex = (uint8_t)i;
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
                command.Speed = std::max(1, std::min(4, atoi(part1) & 0xFF));
            }
            else if (_stricmp(token, "FOLLOW") == 0)
            {
                command.Type = TITLE_SCRIPT_FOLLOW;
                command.SpriteIndex = atoi(part1) & 0xFFFF;
                safe_strcpy(command.SpriteName, part2, USER_STRING_MAX_LENGTH);
            }
            else if (_stricmp(token, "WAIT") == 0)
            {
                command.Type = TITLE_SCRIPT_WAIT;
                command.Milliseconds = atoi(part1) & 0xFFFF;
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
            else if (_stricmp(token, "LOADSC") == 0)
            {
                command.Type = TITLE_SCRIPT_LOADSC;
                // Confirm the scenario exists
                //source_desc desc;
                //if (ScenarioSources::TryGetByName(part1, &desc))
                //{
                    safe_strcpy(command.Scenario, part1, sizeof(command.Scenario));
                //}
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
    for (int32_t i = 0; i < 3; i++)
    {
        parts[i * 128] = 0;
    }
    int32_t part = 0;
    int32_t cindex = 0;
    int32_t whitespace = 1;
    int32_t comment = 0;
    bool load = false;
    bool sprite = false;
    for (; part < 3;)
    {
        int32_t c = 0;
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
        else if (c == ' ' && !comment && !load && (!sprite || part != 2))
        {
            if (!whitespace)
            {
                if (part == 0 && ((cindex == 4 && _strnicmp(parts, "LOAD", 4) == 0) ||
                                  (cindex == 6 && _strnicmp(parts, "LOADSC", 6) == 0)))
                {
                    load = true;
                }
                else if (part == 0 && cindex == 6 && _strnicmp(parts, "FOLLOW", 6) == 0)
                {
                    sprite = true;
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

static std::vector<uint8_t> ReadScriptFile(const utf8 * path)
{
    std::vector<uint8_t> result;
    try
    {
        auto fs = FileStream(path, FILE_MODE_OPEN);
        auto size = (size_t)fs.GetLength();
        result.resize(size);
        fs.Read(result.data(), size);
    }
    catch (const std::exception &)
    {
        result.clear();
        result.shrink_to_fit();
    }
    return result;
}

static std::string LegacyScriptWrite(TitleSequence * seq)
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
        case TITLE_SCRIPT_LOADMM:
            sb.Append("LOADMM");
            break;
        case TITLE_SCRIPT_LOADRCT1:
            String::Format(buffer, sizeof(buffer), "LOADRCT1 %u", command->SaveIndex);
            sb.Append(buffer);
            break;
        case TITLE_SCRIPT_LOAD:
            if (command->SaveIndex == 0xFF)
            {
                sb.Append("LOAD <No save file>");
            }
            else
            {
                sb.Append("LOAD ");
                sb.Append(seq->Saves[command->SaveIndex]);
            }
            break;
        case TITLE_SCRIPT_LOADSC:
            if (command->Scenario[0] == '\0')
            {
                sb.Append("LOADSC <No scenario name>");
            }
            else
            {
                sb.Append("LOADSC ");
                sb.Append(command->Scenario);
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
        case TITLE_SCRIPT_FOLLOW:
            String::Format(buffer, sizeof(buffer), "FOLLOW %u ", command->SpriteIndex);
            sb.Append(buffer);
            sb.Append(command->SpriteName);
            break;
        case TITLE_SCRIPT_SPEED:
            String::Format(buffer, sizeof(buffer), "SPEED %u", command->Speed);
            sb.Append(buffer);
            break;
        case TITLE_SCRIPT_WAIT:
            String::Format(buffer, sizeof(buffer), "WAIT %u", command->Milliseconds);
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

    return sb.GetBuffer();
}

bool TitleSequenceIsLoadCommand(const TitleCommand * command)
{
    switch (command->Type) {
    case TITLE_SCRIPT_LOADMM:
    case TITLE_SCRIPT_LOAD:
    case TITLE_SCRIPT_LOADRCT1:
    case TITLE_SCRIPT_LOADSC:
        return true;
    default:
        return false;
    }
}
