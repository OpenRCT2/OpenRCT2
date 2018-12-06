/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "ReplayManager.h"

#include "Context.h"
#include "Game.h"
#include "OpenRCT2.h"
#include "PlatformEnvironment.h"
#include "actions/GameAction.h"
#include "core/DataSerialiser.h"
#include "core/Path.hpp"
#include "object/ObjectManager.h"
#include "object/ObjectRepository.h"
#include "openrct2/ParkImporter.h"
#include "rct2/S6Exporter.h"

#include <vector>

namespace OpenRCT2
{
    struct ReplayCommand
    {
        ReplayCommand() = default;

        ReplayCommand(uint32_t t, uint32_t* args, uint8_t cb, uint32_t id)
        {
            tick = t;
            eax = args[0];
            ebx = args[1];
            ecx = args[2];
            edx = args[3];
            esi = args[4];
            edi = args[5];
            ebp = args[6];
            callback = cb;
            action = nullptr;
            commandIndex = id;
        }

        ReplayCommand(uint32_t t, std::unique_ptr<GameAction>&& ga, uint32_t id)
        {
            tick = t;
            action = std::move(ga);
            commandIndex = id;
        }

        uint32_t tick = 0;
        uint32_t eax = 0, ebx = 0, ecx = 0, edx = 0, esi = 0, edi = 0, ebp = 0;
        GameAction::Ptr action;
        uint8_t playerid = 0;
        uint8_t callback = 0;
        uint32_t commandIndex = 0;

        bool operator<(const ReplayCommand& comp) const
        {
            // First sort by tick
            if (tick < comp.tick)
                return true;
            if (tick > comp.tick)
                return false;

            // If the ticks are equal sort by commandIndex
            return commandIndex < comp.commandIndex;
        }
    };

    struct ReplayRecordData
    {
        MemoryStream parkData;
        std::string name;      // Name of play
        uint32_t timeRecorded; // Posix Time.
        uint32_t tickStart;    // First tick of replay.
        uint32_t tickEnd;      // Last tick of replay.
        std::multiset<ReplayCommand> commands;
    };

    class ReplayManager : public IReplayManager
    {
        enum class ReplayMode
        {
            NONE = 0,
            RECORDING,
            PLAYING,
        };

    public:
        virtual ~ReplayManager()
        {
        }

        virtual bool IsReplaying() const override
        {
            return _mode == ReplayMode::PLAYING;
        }

        virtual bool IsRecording() const override
        {
            return _mode == ReplayMode::RECORDING;
        }

        virtual void AddGameCommand(
            uint32_t tick, uint32_t eax, uint32_t ebx, uint32_t ecx, uint32_t edx, uint32_t esi, uint32_t edi, uint32_t ebp,
            uint8_t callback) override
        {
            if (_current == nullptr)
                return;

            uint32_t args[7];
            args[0] = eax;
            args[1] = ebx;
            args[2] = ecx;
            args[3] = edx;
            args[4] = esi;
            args[5] = edi;
            args[6] = ebp;

            _current->commands.emplace(gCurrentTicks, args, callback, _commandId++);
        }

        virtual void AddGameAction(uint32_t tick, const GameAction* action) override
        {
            if (_current == nullptr)
                return;

            MemoryStream stream;
            DataSerialiser dsOut(true, stream);
            action->Serialise(dsOut);

            std::unique_ptr<GameAction> ga = GameActions::Create(action->GetType());
            ga->SetCallback(action->GetCallback());

            stream.SetPosition(0);
            DataSerialiser dsIn(false, stream);
            ga->Serialise(dsIn);

            _current->commands.emplace(gCurrentTicks, std::move(ga), _commandId++);
        }

        virtual void Update() override
        {
            if (_mode == ReplayMode::NONE)
                return;

            if (_mode == ReplayMode::RECORDING)
            {
                if (gCurrentTicks >= _current->tickEnd)
                {
                    StopRecording();
                    return;
                }
            }
            else if (_mode == ReplayMode::PLAYING)
            {
                ReplayCommands();
            }
        }

        virtual bool StartRecording(const std::string& name, uint32_t maxTicks /*= 0xFFFFFFFF*/) override
        {
            if (_mode != ReplayMode::NONE)
                return false;

            auto replayData = std::make_unique<ReplayRecordData>();
            replayData->name = name;
            replayData->tickStart = gCurrentTicks;
            if (maxTicks != 0xFFFFFFFF)
                replayData->tickEnd = gCurrentTicks + maxTicks;
            else
                replayData->tickEnd = 0xFFFFFFFF;

            auto context = GetContext();
            auto& objManager = context->GetObjectManager();
            auto objects = objManager.GetPackableObjects();

            auto s6exporter = std::make_unique<S6Exporter>();
            s6exporter->ExportObjectsList = objects;
            s6exporter->Export();
            s6exporter->SaveGame(&replayData->parkData);

            _mode = ReplayMode::RECORDING;
            _current = std::move(replayData);

            return true;
        }

        virtual bool StopRecording() override
        {
            if (_mode != ReplayMode::RECORDING)
                return false;

            DataSerialiser serialiser(true);
            Serialise(serialiser, *_current);

            char replayName[512] = {};
            snprintf(replayName, sizeof(replayName), "replay_%s_%d.sv6r", _current->name.c_str(), 0);

            std::string outPath = GetContext()->GetPlatformEnvironment()->GetDirectoryPath(DIRBASE::USER, DIRID::REPLAY);
            std::string outFile = Path::Combine(outPath, replayName);

            FILE* fp = fopen(outFile.c_str(), "wb");
            if (fp)
            {
                auto& stream = serialiser.GetStream();

                fwrite(stream.GetData(), 1, stream.GetLength(), fp);
                fclose(fp);
            }

            //_current.reset();
            _mode = ReplayMode::NONE;

            return true;
        }

        virtual bool StartPlayback(const std::string& file) override
        {
            auto replayData = std::make_unique<ReplayRecordData>();

            if (!ReadReplayData(file, *replayData))
            {
                return false;
            }

            if (!LoadReplayDataMap(*replayData))
            {
                return false;
            }

            gCurrentTicks = replayData->tickStart;

            _current = std::move(replayData);
            _mode = ReplayMode::PLAYING;

            return true;
        }

        virtual bool StopPlayback() override
        {
            if (_mode != ReplayMode::PLAYING)
                return false;

            _current.reset();
            _mode = ReplayMode::NONE;

            return true;
        }

    private:
        bool LoadReplayDataMap(ReplayRecordData& data)
        {
            try
            {
                data.parkData.SetPosition(0);

                auto context = GetContext();
                auto& objManager = context->GetObjectManager();
                auto importer = ParkImporter::CreateS6(context->GetObjectRepository());

                auto loadResult = importer->LoadFromStream(&data.parkData, false);
                objManager.LoadObjects(loadResult.RequiredObjects.data(), loadResult.RequiredObjects.size());

                importer->Import();

                sprite_position_tween_reset();
                game_load_init();
                fix_invalid_vehicle_sprite_sizes();
            }
            catch (const std::exception&)
            {
                return false;
            }
            return true;
        }

        bool ReadReplayFromFile(const std::string& file, MemoryStream& stream)
        {
            FILE* fp = fopen(file.c_str(), "rb");
            if (!fp)
                return false;

            char buffer[128];
            while (feof(fp) == false)
            {
                size_t read = fread(buffer, 1, 128, fp);
                if (read == 0)
                    break;
                stream.Write(buffer, read);
            }

            fclose(fp);
            return true;
        }

        bool ReadReplayData(const std::string& file, ReplayRecordData& data)
        {
            MemoryStream stream;
            DataSerialiser serialiser(false, stream);

            std::string fileName = file;
            if (fileName.size() < 5 || fileName.substr(fileName.size() - 5) != ".sv6r")
            {
                fileName += ".sv6r";
            }

            std::string outPath = GetContext()->GetPlatformEnvironment()->GetDirectoryPath(DIRBASE::USER, DIRID::REPLAY);
            std::string outFile = Path::Combine(outPath, file);

            bool loaded = false;
            if (ReadReplayFromFile(outFile, stream))
            {
                loaded = true;
            }
            else if (ReadReplayFromFile(file, stream))
            {
                loaded = true;
            }
            if (!loaded)
                return false;

            stream.SetPosition(0);

            if (!Serialise(serialiser, data))
            {
                return false;
            }

            return true;
        }

        bool Serialise(DataSerialiser& serialiser, ReplayRecordData& data)
        {
            serialiser << data.name;
            serialiser << data.parkData;
            serialiser << data.tickStart;
            serialiser << data.tickEnd;

            uint32_t countCommands = (uint32_t)data.commands.size();
            serialiser << countCommands;

            if (serialiser.IsSaving())
            {
                for (auto& command : data.commands)
                {
                    serialiser << command.tick;
                    serialiser << command.commandIndex;
                    if (command.action != nullptr)
                    {
                        bool isGameAction = true;
                        serialiser << isGameAction;

                        uint32_t actionType = command.action->GetType();
                        serialiser << actionType;
                        command.action->Serialise(serialiser);
                    }
                    else
                    {
                        bool isGameAction = false;
                        serialiser << isGameAction;

                        serialiser << command.eax;
                        serialiser << command.ebx;
                        serialiser << command.ecx;
                        serialiser << command.edx;
                        serialiser << command.esi;
                        serialiser << command.edi;
                        serialiser << command.ebp;
                        serialiser << command.callback;
                    }
                }
            }
            else
            {
                for (uint32_t i = 0; i < countCommands; i++)
                {
                    ReplayCommand command = {};
                    serialiser << command.tick;
                    serialiser << command.commandIndex;

                    bool isGameAction = false;
                    serialiser << isGameAction;

                    if (isGameAction)
                    {
                        uint32_t actionType = 0;
                        serialiser << actionType;

                        command.action = GameActions::Create(actionType);
                        Guard::Assert(command.action != nullptr);

                        command.action->Serialise(serialiser);
                    }
                    else
                    {
                        serialiser << command.eax;
                        serialiser << command.ebx;
                        serialiser << command.ecx;
                        serialiser << command.edx;
                        serialiser << command.esi;
                        serialiser << command.edi;
                        serialiser << command.ebp;
                        serialiser << command.callback;
                    }

                    data.commands.emplace(std::move(command));
                }
            }

            return true;
        }

        void ReplayCommands()
        {
            auto& replayQueue = _current->commands;

            while (replayQueue.begin() != replayQueue.end())
            {
                const ReplayCommand& command = (*replayQueue.begin());
                if (command.tick != gCurrentTicks)
                    break;

                if (command.action != nullptr)
                {
                    GameAction* action = command.action.get();
                    action->SetFlags(action->GetFlags());

                    Guard::Assert(action != nullptr);

                    GameActionResult::Ptr result = GameActions::Execute(action);
                }
                else
                {
                    game_do_command(command.eax, command.ebx, command.ecx, command.edx, command.esi, command.edi, command.ebp);
                }

                replayQueue.erase(replayQueue.begin());
            }
        }

    private:
        ReplayMode _mode = ReplayMode::NONE;
        std::unique_ptr<ReplayRecordData> _current;
        uint32_t _commandId = 0;
    };

    std::unique_ptr<IReplayManager> CreateReplayManager()
    {
        return std::make_unique<ReplayManager>();
    }

} // namespace OpenRCT2
