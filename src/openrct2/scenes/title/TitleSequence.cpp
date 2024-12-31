/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "TitleSequence.h"

#include "../../Diagnostic.h"
#include "../../core/Collections.hpp"
#include "../../core/Console.hpp"
#include "../../core/File.h"
#include "../../core/FileScanner.h"
#include "../../core/FileStream.h"
#include "../../core/Guard.hpp"
#include "../../core/Memory.hpp"
#include "../../core/MemoryStream.h"
#include "../../core/Path.hpp"
#include "../../core/String.hpp"
#include "../../core/StringBuilder.h"
#include "../../core/Zip.h"
#include "../../scenario/ScenarioRepository.h"
#include "../../scenario/ScenarioSources.h"

#include <array>
#include <memory>
#include <optional>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>

namespace OpenRCT2::Title
{
    static std::vector<std::string> GetSaves(const std::string& path);
    static std::vector<std::string> GetSaves(IZipArchive* zip);
    static std::vector<TitleCommand> LegacyScriptRead(const std::vector<uint8_t>& script, std::vector<std::string> saves);
    static void LegacyScriptGetLine(OpenRCT2::IStream* stream, std::vector<std::array<char, 128>>& parts);
    static std::vector<uint8_t> ReadScriptFile(const std::string& path);
    static std::string LegacyScriptWrite(const TitleSequence& seq);

    std::unique_ptr<TitleSequence> CreateTitleSequence()
    {
        return std::make_unique<TitleSequence>();
    }

    std::unique_ptr<TitleSequence> LoadTitleSequence(const std::string& path)
    {
        std::vector<uint8_t> script;
        std::vector<std::string> saves;
        bool isZip;

        LOG_VERBOSE("Loading title sequence: %s", path.c_str());

        auto ext = Path::GetExtension(path);
        if (String::equals(ext, TITLE_SEQUENCE_EXTENSION))
        {
            auto zip = Zip::TryOpen(path, ZIP_ACCESS::READ);
            if (zip == nullptr)
            {
                Console::Error::WriteLine("Unable to open '%s'", path.c_str());
                return nullptr;
            }

            script = zip->GetFileData("script.txt");
            if (script.empty())
            {
                Console::Error::WriteLine("Unable to open script.txt in '%s'", path.c_str());
                return nullptr;
            }

            saves = GetSaves(zip.get());
            isZip = true;
        }
        else
        {
            auto scriptPath = Path::Combine(path, u8"script.txt");
            script = ReadScriptFile(scriptPath);
            if (script.empty())
            {
                Console::Error::WriteLine("Unable to open '%s'", scriptPath.c_str());
                return nullptr;
            }

            saves = GetSaves(path);
            isZip = false;
        }

        auto commands = LegacyScriptRead(script, saves);

        auto seq = OpenRCT2::Title::CreateTitleSequence();
        seq->Name = Path::GetFileNameWithoutExtension(path);
        seq->Path = path;
        seq->Saves = saves;
        seq->Commands = commands;
        seq->IsZip = isZip;
        return seq;
    }

    std::unique_ptr<TitleSequenceParkHandle> TitleSequenceGetParkHandle(const TitleSequence& seq, size_t index)
    {
        std::unique_ptr<TitleSequenceParkHandle> handle;
        if (index < seq.Saves.size())
        {
            const auto& filename = seq.Saves[index];
            if (seq.IsZip)
            {
                auto zip = Zip::TryOpen(seq.Path, ZIP_ACCESS::READ);
                if (zip != nullptr)
                {
                    auto data = zip->GetFileData(filename);
                    auto ms = std::make_unique<OpenRCT2::MemoryStream>();
                    ms->Write(data.data(), data.size());
                    ms->SetPosition(0);

                    handle = std::make_unique<TitleSequenceParkHandle>();
                    handle->Stream = std::move(ms);
                    handle->HintPath = filename;
                }
                else
                {
                    Console::Error::WriteLine(
                        "Failed to open zipped path '%s' from zip '%s'", filename.c_str(), seq.Path.c_str());
                }
            }
            else
            {
                auto absolutePath = Path::Combine(seq.Path, filename);
                std::unique_ptr<OpenRCT2::IStream> fileStream = nullptr;
                try
                {
                    fileStream = std::make_unique<OpenRCT2::FileStream>(absolutePath, OpenRCT2::FILE_MODE_OPEN);
                }
                catch (const IOException& exception)
                {
                    Console::Error::WriteLine(exception.what());
                }

                if (fileStream != nullptr)
                {
                    handle = std::make_unique<TitleSequenceParkHandle>();
                    handle->Stream = std::move(fileStream);
                    handle->HintPath = filename;
                }
            }
        }
        return handle;
    }

    bool TitleSequenceSave(const TitleSequence& seq)
    {
        try
        {
            auto script = LegacyScriptWrite(seq);
            if (seq.IsZip)
            {
                auto fdata = std::vector<uint8_t>(script.begin(), script.end());
                auto zip = Zip::Open(seq.Path, ZIP_ACCESS::WRITE);
                zip->SetFileData("script.txt", std::move(fdata));
            }
            else
            {
                auto scriptPath = Path::Combine(seq.Path, u8"script.txt");
                File::WriteAllBytes(scriptPath, script.data(), script.size());
            }
            return true;
        }
        catch (const std::exception&)
        {
            return false;
        }
    }

    bool TitleSequenceAddPark(TitleSequence& seq, const utf8* path, const utf8* name)
    {
        // Get new save index
        auto it = std::find(seq.Saves.begin(), seq.Saves.end(), path);
        if (it == seq.Saves.end())
        {
            seq.Saves.push_back(name);
        }

        if (seq.IsZip)
        {
            try
            {
                auto fdata = File::ReadAllBytes(path);
                auto zip = Zip::TryOpen(seq.Path, ZIP_ACCESS::WRITE);
                if (zip == nullptr)
                {
                    Console::Error::WriteLine("Unable to open '%s'", seq.Path.c_str());
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
            auto dstPath = Path::Combine(seq.Path, name);
            if (!File::Copy(path, dstPath, true))
            {
                Console::Error::WriteLine("Unable to copy '%s' to '%s'", path, dstPath.c_str());
                return false;
            }
        }
        return true;
    }

    bool TitleSequenceRenamePark(TitleSequence& seq, size_t index, const utf8* name)
    {
        Guard::Assert(index < seq.Saves.size(), GUARD_LINE);

        auto& oldRelativePath = seq.Saves[index];
        if (seq.IsZip)
        {
            auto zip = Zip::TryOpen(seq.Path, ZIP_ACCESS::WRITE);
            if (zip == nullptr)
            {
                Console::Error::WriteLine("Unable to open '%s'", seq.Path.c_str());
                return false;
            }
            zip->RenameFile(oldRelativePath, name);
        }
        else
        {
            auto srcPath = Path::Combine(seq.Path, oldRelativePath);
            auto dstPath = Path::Combine(seq.Path, name);
            if (!File::Move(srcPath, dstPath))
            {
                Console::Error::WriteLine("Unable to move '%s' to '%s'", srcPath.c_str(), dstPath.c_str());
                return false;
            }
        }
        seq.Saves[index] = name;
        return true;
    }

    bool TitleSequenceRemovePark(TitleSequence& seq, size_t index)
    {
        Guard::Assert(index < seq.Saves.size(), GUARD_LINE);

        // Delete park file
        auto& relativePath = seq.Saves[index];
        if (seq.IsZip)
        {
            auto zip = Zip::TryOpen(seq.Path, ZIP_ACCESS::WRITE);
            if (zip == nullptr)
            {
                Console::Error::WriteLine("Unable to open '%s'", seq.Path.c_str());
                return false;
            }
            zip->DeleteFile(relativePath);
        }
        else
        {
            auto absolutePath = Path::Combine(seq.Path, relativePath);
            if (!File::Delete(absolutePath))
            {
                Console::Error::WriteLine("Unable to delete '%s'", absolutePath.c_str());
                return false;
            }
        }

        // Remove from sequence
        seq.Saves.erase(seq.Saves.begin() + index);

        // Update load commands
        for (auto& seqCommand : seq.Commands)
        {
            std::visit(
                [index](auto&& command) {
                    if constexpr (std::is_same_v<std::decay_t<decltype(command)>, LoadParkCommand>)
                    {
                        if (command.SaveIndex == index)
                        {
                            // Park no longer exists, so reset load command to invalid
                            command.SaveIndex = SAVE_INDEX_INVALID;
                        }
                        else if (command.SaveIndex > index)
                        {
                            // Park index will have shifted by -1
                            command.SaveIndex--;
                        }
                    }
                },
                seqCommand);
        }

        return true;
    }

    static std::vector<std::string> GetSaves(const std::string& directory)
    {
        std::vector<std::string> saves;

        auto pattern = Path::Combine(directory, u8"*.sc6;*.sv6;*.park;*.sv4;*.sc4");
        auto scanner = Path::ScanDirectory(pattern, true);
        while (scanner->Next())
        {
            saves.push_back(scanner->GetPathRelative());
        }
        return saves;
    }

    static std::vector<std::string> GetSaves(IZipArchive* zip)
    {
        std::vector<std::string> saves;
        size_t numFiles = zip->GetNumFiles();
        for (size_t i = 0; i < numFiles; i++)
        {
            auto name = zip->GetFileName(i);
            auto ext = Path::GetExtension(name);
            if (String::iequals(ext, ".sv6") || String::iequals(ext, ".sc6") || String::iequals(ext, ".park"))
            {
                saves.push_back(std::move(name));
            }
        }
        return saves;
    }

    static std::vector<TitleCommand> LegacyScriptRead(const std::vector<uint8_t>& script, std::vector<std::string> saves)
    {
        std::vector<TitleCommand> commands;
        auto fs = OpenRCT2::MemoryStream(script.data(), script.size());
        do
        {
            std::vector<std::array<char, 128>> parts;
            LegacyScriptGetLine(&fs, parts);

            const char* token = parts[0].data();
            std::optional<TitleCommand> command = std::nullopt;

            if (token[0] != 0)
            {
                if (String::iequals(token, "LOAD"))
                {
                    auto saveIndex = SAVE_INDEX_INVALID;
                    const std::string relativePath = parts[1].data();
                    for (size_t i = 0; i < saves.size(); i++)
                    {
                        if (String::iequals(relativePath, saves[i]))
                        {
                            saveIndex = static_cast<uint8_t>(i);
                            break;
                        }
                    }
                    command = LoadParkCommand{ saveIndex };
                }
                else if (String::iequals(token, "LOCATION"))
                {
                    uint8_t locationX = atoi(parts[1].data()) & 0xFF;
                    uint8_t locationY = atoi(parts[2].data()) & 0xFF;
                    command = SetLocationCommand{ locationX, locationY };
                }
                else if (String::iequals(token, "ROTATE"))
                {
                    uint8_t rotations = atoi(parts[1].data()) & 0xFF;
                    command = RotateViewCommand{ rotations };
                }
                else if (String::iequals(token, "ZOOM"))
                {
                    uint8_t zoom = atoi(parts[1].data()) & 0xFF;
                    command = SetZoomCommand{ zoom };
                }
                else if (String::iequals(token, "SPEED"))
                {
                    uint8_t speed = std::max(1, std::min(4, atoi(parts[1].data()) & 0xFF));
                    command = SetSpeedCommand{ speed };
                }
                else if (String::iequals(token, "FOLLOW"))
                {
                    auto entityID = EntityId::FromUnderlying(atoi(parts[1].data()) & 0xFFFF);
                    auto followCommand = FollowEntityCommand{ entityID };
                    String::safeUtf8Copy(followCommand.Follow.SpriteName, parts[2].data(), kUserStringMaxLength);
                    command = followCommand;
                }
                else if (String::iequals(token, "WAIT"))
                {
                    uint16_t milliseconds = atoi(parts[1].data()) & 0xFFFF;
                    command = WaitCommand{ milliseconds };
                }
                else if (String::iequals(token, "RESTART"))
                {
                    command = RestartCommand{};
                }
                else if (String::iequals(token, "END"))
                {
                    command = EndCommand{};
                }
                else if (String::iequals(token, "LOADSC"))
                {
                    auto loadScenarioCommand = LoadScenarioCommand{};
                    String::safeUtf8Copy(loadScenarioCommand.Scenario, parts[1].data(), sizeof(loadScenarioCommand.Scenario));
                    command = loadScenarioCommand;
                }
            }

            if (command.has_value())
            {
                commands.push_back(std::move(*command));
            }
        } while (fs.GetPosition() < fs.GetLength());
        return commands;
    }

    static void LegacyScriptGetLine(OpenRCT2::IStream* stream, std::vector<std::array<char, 128>>& parts)
    {
        int32_t part = 0;
        int32_t cindex = 0;
        int32_t whitespace = 1;
        int32_t comment = 0;
        bool load = false;
        bool sprite = false;

        parts.resize(1);

        while (true)
        {
            int32_t c = 0;
            if (stream->TryRead(&c, 1) != 1)
            {
                c = EOF;
            }
            if (c == '\n' || c == '\r' || c == EOF)
            {
                parts[part][cindex] = 0;
                return;
            }
            if (c == '#')
            {
                parts[part][cindex] = 0;
                comment = 1;
            }
            else if (c == ' ' && !comment && !load && (!sprite || part != 2))
            {
                if (!whitespace)
                {
                    if (part == 0
                        && ((cindex == 4 && String::startsWith(parts[0].data(), "LOAD", true))
                            || (cindex == 6 && String::startsWith(parts[0].data(), "LOADSC", true))))
                    {
                        load = true;
                    }
                    else if (part == 0 && cindex == 6 && String::startsWith(parts[0].data(), "FOLLOW", true))
                    {
                        sprite = true;
                    }
                    parts[part][cindex] = 0;
                    part++;
                    parts.resize(part + 1);
                    cindex = 0;
                }
            }
            else if (!comment)
            {
                whitespace = 0;
                if (cindex < 127)
                {
                    parts[part][cindex] = c;
                    cindex++;
                }
                else
                {
                    parts[part][cindex] = 0;
                    part++;
                    parts.resize(part + 1);
                    cindex = 0;
                }
            }
        }
    }

    static std::vector<uint8_t> ReadScriptFile(const std::string& path)
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

    static std::string LegacyScriptWrite(const TitleSequence& seq)
    {
        auto sb = StringBuilder(128);
        sb.Append("# SCRIPT FOR ");
        sb.Append(seq.Name.c_str());
        sb.Append("\n");
        for (const auto& seqCommand : seq.Commands)
        {
            std::visit(
                [&seq, &sb](auto&& command) {
                    using T = std::decay_t<decltype(command)>;
                    if constexpr (std::is_same_v<T, LoadParkCommand>)
                    {
                        if (command.SaveIndex < seq.Saves.size())
                        {
                            sb.Append("LOAD ");
                            sb.Append(seq.Saves[command.SaveIndex].c_str());
                        }
                        else
                        {
                            sb.Append("LOAD <No save file>");
                        }
                    }
                    else if constexpr (std::is_same_v<T, LoadScenarioCommand>)
                    {
                        if (command.Scenario[0] == '\0')
                        {
                            sb.Append("LOADSC <No scenario name>");
                        }
                        else
                        {
                            sb.Append("LOADSC ");
                            sb.Append(command.Scenario);
                        }
                    }
                    else if constexpr (std::is_same_v<T, SetLocationCommand>)
                    {
                        sb.Append(String::stdFormat("LOCATION %u %u", command.Location.X, command.Location.Y));
                    }
                    else if constexpr (std::is_same_v<T, RotateViewCommand>)
                    {
                        sb.Append(String::stdFormat("ROTATE %u", command.Rotations));
                    }
                    else if constexpr (std::is_same_v<T, SetZoomCommand>)
                    {
                        sb.Append(String::stdFormat("ZOOM %u", command.Zoom));
                    }
                    else if constexpr (std::is_same_v<T, FollowEntityCommand>)
                    {
                        sb.Append(String::stdFormat("FOLLOW %u ", command.Follow.SpriteIndex));
                        sb.Append(command.Follow.SpriteName);
                    }
                    else if constexpr (std::is_same_v<T, SetSpeedCommand>)
                    {
                        sb.Append(String::stdFormat("SPEED %u", command.Speed));
                    }
                    else if constexpr (std::is_same_v<T, WaitCommand>)
                    {
                        sb.Append(String::stdFormat("WAIT %u", command.Milliseconds));
                    }
                    else if constexpr (std::is_same_v<T, RestartCommand>)
                    {
                        sb.Append("RESTART");
                    }
                    else if constexpr (std::is_same_v<T, EndCommand>)
                    {
                        sb.Append("END");
                    }
                },
                seqCommand);
            sb.Append("\n");
        }

        return sb.GetBuffer();
    }

    bool TitleSequenceIsLoadCommand(const TitleCommand& command)
    {
        return std::holds_alternative<LoadParkCommand>(command) || std::holds_alternative<LoadScenarioCommand>(command);
    }
} // namespace OpenRCT2::Title
