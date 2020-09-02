/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "TitleSequence.h"

#include "../common.h"
#include "../core/Collections.hpp"
#include "../core/Console.hpp"
#include "../core/File.h"
#include "../core/FileScanner.h"
#include "../core/FileStream.hpp"
#include "../core/Guard.hpp"
#include "../core/Memory.hpp"
#include "../core/MemoryStream.h"
#include "../core/Path.hpp"
#include "../core/String.hpp"
#include "../core/StringBuilder.hpp"
#include "../core/Zip.h"
#include "../scenario/ScenarioRepository.h"
#include "../scenario/ScenarioSources.h"
#include "../util/Util.h"

#include <algorithm>
#include <memory>
#include <sstream>
#include <vector>

namespace
{
    std::vector<std::string> GetSaves(const utf8* directory);
    std::vector<std::string> GetSaves(IZipArchive* zip);
    std::vector<uint8_t> ReadScriptFile(const utf8* path);
    void LegacyScriptGetLine(OpenRCT2::IStream* stream, char* parts);
    std::vector<TitleCommand> LegacyScriptRead(utf8* script, size_t scriptLength, const std::vector<std::string>& saves);

    using ScriptType = std::vector<uint8_t>;
    using SaveType = std::vector<std::string>;
    using ScriptResult = std::pair<ScriptType, SaveType>;

    ScriptResult GetZipScript(const utf8* path);
    ScriptResult GetFileScript(const utf8* path);
} // namespace

TitleSequence::TitleSequence(const utf8* path)
    : Path(path)
    , Name(Path::GetFileNameWithoutExtension(Path))
    , IsZip(String::Equals(Path::GetExtension(path), Extension))
{
    log_verbose("Loading title sequence: %s", path);
    std::vector<uint8_t> script;
    std::tie(script, Saves) = IsZip ? GetZipScript(path) : GetFileScript(path);
    Commands = LegacyScriptRead(reinterpret_cast<utf8*>(script.data()), script.size(), Saves);
}

bool TitleSequence::Save()
{
    try
    {
        auto script = LegacyScriptWrite();
        if (IsZip)
        {
            auto fdata = std::vector<uint8_t>(script.begin(), script.end());
            auto zip = Zip::Open(Path, ZIP_ACCESS::WRITE);
            zip->SetFileData("script.txt", std::move(fdata));
        }
        else
        {
            auto scriptPath = Path::Combine(Path, "script.txt");
            File::WriteAllBytes(scriptPath, script.data(), script.size());
        }
        return true;
    }
    catch (const std::exception&)
    {
        return false;
    }
}

bool TitleSequence::AddPark(const utf8* path, const utf8* name)
{
    // Get new save index
    size_t index = SIZE_MAX;
    for (size_t i = 0; i < NumSaves(); i++)
    {
        if (String::Equals(Saves[i].c_str(), path, true))
        {
            index = i;
            break;
        }
    }
    if (index == SIZE_MAX)
    {
        index = NumSaves();
    }

    Saves.insert(Saves.begin() + index, std::string(name));

    if (IsZip)
    {
        try
        {
            auto fdata = File::ReadAllBytes(path);
            auto zip = Zip::TryOpen(Path, ZIP_ACCESS::WRITE);
            if (zip == nullptr)
            {
                Console::Error::WriteLine("Unable to open '%s'", Path.c_str());
                return false;
            }
            zip->SetFileData(name, std::move(fdata));
        }
        catch (const std::exception& ex)
        {
            Console::Error::WriteLine(ex.what());
        }
    }
    else
    {
        // Determine destination path
        utf8 dstPath[MAX_PATH];
        String::Set(dstPath, sizeof(dstPath), Path.c_str());
        Path::Append(dstPath, sizeof(dstPath), name);
        if (!File::Copy(path, dstPath, true))
        {
            Console::Error::WriteLine("Unable to copy '%s' to '%s'", path, dstPath);
            return false;
        }
    }
    return true;
}

bool TitleSequence::RenamePark(size_t index, const utf8* name)
{
    Guard::Assert(index < NumSaves(), GUARD_LINE);

    const utf8* oldRelativePath = Saves[index].c_str();
    if (IsZip)
    {
        auto zip = Zip::TryOpen(Path, ZIP_ACCESS::WRITE);
        if (zip == nullptr)
        {
            Console::Error::WriteLine("Unable to open '%s'", Path.c_str());
            return false;
        }
        zip->RenameFile(oldRelativePath, name);
    }
    else
    {
        utf8 srcPath[MAX_PATH];
        utf8 dstPath[MAX_PATH];
        String::Set(srcPath, sizeof(srcPath), Path.c_str());
        Path::Append(srcPath, sizeof(srcPath), oldRelativePath);
        String::Set(dstPath, sizeof(dstPath), Path.c_str());
        Path::Append(dstPath, sizeof(dstPath), name);
        if (!File::Move(srcPath, dstPath))
        {
            Console::Error::WriteLine("Unable to move '%s' to '%s'", srcPath, dstPath);
            return false;
        }
    }

    Saves[index] = name;
    return true;
}

bool TitleSequence::RemovePark(size_t index)
{
    Guard::Assert(index < NumSaves(), GUARD_LINE);

    // Delete park file
    std::string& relativePath = Saves[index];
    if (IsZip)
    {
        auto zip = Zip::TryOpen(Path, ZIP_ACCESS::WRITE);
        if (zip == nullptr)
        {
            Console::Error::WriteLine("Unable to open '%s'", Path.c_str());
            return false;
        }
        zip->DeleteFile(relativePath);
    }
    else
    {
        utf8 absolutePath[MAX_PATH];
        String::Set(absolutePath, sizeof(absolutePath), Path.c_str());
        Path::Append(absolutePath, sizeof(absolutePath), relativePath.c_str());
        if (!File::Delete(absolutePath))
        {
            Console::Error::WriteLine("Unable to delete '%s'", absolutePath);
            return false;
        }
    }

    // Remove from sequence
    Saves.erase(Saves.begin() + index);

    // Update load commands
    for (size_t i = 0; i < NumCommands(); i++)
    {
        TitleCommand* command = &Commands[i];
        if (command->Type == TITLE_SCRIPT_LOAD)
        {
            if (command->SaveIndex == index)
            {
                // Park no longer exists, so reset load command to invalid
                command->SaveIndex = TitleSequence::SaveIndexInvalid;
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

std::string TitleSequence::LegacyScriptWrite()
{
    utf8 buffer[128];
    auto sb = StringBuilder(128);

    sb.Append("# SCRIPT FOR ");
    sb.Append(Name.c_str());
    sb.Append("\n");
    for (size_t i = 0; i < NumCommands(); i++)
    {
        const TitleCommand* command = &Commands[i];
        switch (command->Type)
        {
            case TITLE_SCRIPT_LOAD:
                if (command->SaveIndex == 0xFF)
                {
                    sb.Append("LOAD <No save file>");
                }
                else
                {
                    sb.Append("LOAD ");
                    sb.Append(Saves[command->SaveIndex].c_str());
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

void TitleSequence::SwapConsecutiveCommands(size_t index)
{
    TitleCommand* first = &Commands[index];
    TitleCommand* second = &Commands[index + 1];
    std::swap(first, second);
}

std::unique_ptr<TitleSequenceParkHandle> TitleSequence::GetParkHandle(size_t index)
{
    std::unique_ptr<TitleSequenceParkHandle> handle;
    if (index <= NumSaves())
    {
        const utf8* filename = Saves[index].c_str();
        if (IsZip)
        {
            auto zip = std::unique_ptr<IZipArchive>(Zip::TryOpen(Path, ZIP_ACCESS::READ));
            if (zip != nullptr)
            {
                try
                {
                    auto data = zip->GetFileData(filename);
                    auto dataForMs = Memory::Allocate<uint8_t>(data.size());
                    std::copy_n(data.data(), data.size(), dataForMs);
                    handle = std::make_unique<TitleSequenceParkHandle>();
                    handle->Stream = std::make_unique<OpenRCT2::MemoryStream>(
                        dataForMs, data.size(), OpenRCT2::MEMORY_ACCESS::READ | OpenRCT2::MEMORY_ACCESS::OWNER);
                    handle->HintPath = filename;
                }
                catch (const std::exception& e)
                {
                    Console::Error::WriteLine(e.what());
                    throw;
                }
            }
            else
            {
                Console::Error::WriteLine("Failed to open zipped path '%s' from zip '%s'", filename, Path.c_str());
            }
        }
        else
        {
            utf8 absolutePath[MAX_PATH];
            String::Set(absolutePath, sizeof(absolutePath), Path.c_str());
            Path::Append(absolutePath, sizeof(absolutePath), filename);

            try
            {
                handle = std::make_unique<TitleSequenceParkHandle>();
                handle->Stream = std::make_unique<OpenRCT2::FileStream>(absolutePath, OpenRCT2::FILE_MODE_OPEN);
                handle->HintPath = filename;
            }
            catch (const std::exception& exception)
            {
                Console::Error::WriteLine(exception.what());
                throw;
            }
        }
    }
    return handle;
}

bool TitleSequence::IsLoadCommand(const TitleCommand* command)
{
    switch (command->Type)
    {
        case TITLE_SCRIPT_LOAD:
        case TITLE_SCRIPT_LOADSC:
            return true;
        default:
            return false;
    }
}

namespace
{
    std::vector<std::string> GetSaves(const utf8* directory)
    {
        std::vector<std::string> saves;

        utf8 pattern[MAX_PATH];
        String::Set(pattern, sizeof(pattern), directory);
        Path::Append(pattern, sizeof(pattern), "*.sc6;*.sv6");

        IFileScanner* scanner = Path::ScanDirectory(pattern, true);
        while (scanner->Next())
        {
            const utf8* path = scanner->GetPathRelative();
            saves.emplace_back(path);
        }
        return saves;
    }

    std::vector<std::string> GetSaves(IZipArchive* zip)
    {
        std::vector<std::string> saves;
        size_t numFiles = zip->GetNumFiles();
        for (size_t i = 0; i < numFiles; i++)
        {
            auto name = zip->GetFileName(i);
            auto ext = Path::GetExtension(name);
            if (String::Equals(ext, ".sv6", true) || String::Equals(ext, ".sc6", true))
            {
                saves.emplace_back(name);
            }
        }
        return saves;
    }

    std::vector<uint8_t> ReadScriptFile(const utf8* path)
    {
        std::vector<uint8_t> result;
        try
        {
            auto fs = OpenRCT2::FileStream(path, OpenRCT2::FILE_MODE_OPEN);
            auto size = static_cast<size_t>(fs.GetLength());
            result.resize(size);
            fs.Read(result.data(), size);
        }
        catch (const std::exception&)
        {
            result.clear();
            result.shrink_to_fit();
        }
        return result;
    }

    using ScriptType = std::vector<uint8_t>;
    using SaveType = std::vector<std::string>;
    using ScriptResult = std::pair<ScriptType, SaveType>;

    ScriptResult GetZipScript(const utf8* path)
    {
        auto zip = std::unique_ptr<IZipArchive>(Zip::TryOpen(path, ZIP_ACCESS::READ));
        if (zip == nullptr)
        {
            Console::Error::WriteLine("Unable to open '%s'", path);
            throw std::exception();
        }

        auto script = zip->GetFileData("script.txt");
        if (script.empty())
        {
            Console::Error::WriteLine("Unable to open script.txt in '%s'", path);
            throw std::exception();
        }

        auto saves = GetSaves(zip.get());
        return { script, saves };
    }

    ScriptResult GetFileScript(const utf8* path)
    {
        std::stringstream scriptPath;
        scriptPath << path << "script.txt";
        auto script = ReadScriptFile(scriptPath.str().c_str());
        if (script.empty())
        {
            Console::Error::WriteLine("Unable to open '%s'", scriptPath.str().c_str());
            throw std::exception();
        }

        auto saves = GetSaves(path);
        return { script, saves };
    }

    void LegacyScriptGetLine(OpenRCT2::IStream* stream, char* parts)
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
                    if (part == 0
                        && ((cindex == 4 && _strnicmp(parts, "LOAD", 4) == 0)
                            || (cindex == 6 && _strnicmp(parts, "LOADSC", 6) == 0)))
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

    std::vector<TitleCommand> LegacyScriptRead(utf8* script, size_t scriptLength, const std::vector<std::string>& saves)
    {
        std::vector<TitleCommand> commands;
        auto fs = OpenRCT2::MemoryStream(script, scriptLength);
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
                    command.SaveIndex = TitleSequence::SaveIndexInvalid;
                    for (size_t i = 0; i < saves.size(); i++)
                    {
                        if (String::Equals(part1, saves[i].c_str(), true))
                        {
                            command.SaveIndex = static_cast<uint8_t>(i);
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
                else if (_stricmp(token, "LOADSC") == 0)
                {
                    command.Type = TITLE_SCRIPT_LOADSC;
                    safe_strcpy(command.Scenario, part1, sizeof(command.Scenario));
                }
            }
            if (command.Type != TITLE_SCRIPT_UNDEFINED)
            {
                commands.push_back(command);
            }
        } while (fs.GetPosition() < scriptLength);
        return commands;
    }
} // namespace
