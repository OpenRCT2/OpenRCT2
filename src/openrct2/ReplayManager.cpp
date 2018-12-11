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
#include "ParkImporter.h"
#include "PlatformEnvironment.h"
#include "World/Park.h"
#include "actions/GameAction.h"
#include "config/Config.h"
#include "core/DataSerialiser.h"
#include "core/Path.hpp"
#include "object/ObjectManager.h"
#include "object/ObjectRepository.h"
#include "rct2/S6Exporter.h"

#include <vector>

namespace OpenRCT2
{
    // NOTE: This is currently very close to what the network version uses.
    //       Should be refactored once the old game commands are gone.
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
        MemoryStream spriteSpatialData;
        MemoryStream parkParams;
        std::string name;      // Name of play
        uint64_t timeRecorded; // Posix Time.
        uint32_t tickStart;    // First tick of replay.
        uint32_t tickEnd;      // Last tick of replay.
        std::multiset<ReplayCommand> commands;
        std::vector<std::pair<uint32_t, rct_sprite_checksum>> checksums;
        int32_t checksumIndex;
    };

    class ReplayManager final : public IReplayManager
    {
        enum class ReplayMode
        {
            NONE = 0,
            RECORDING,
            PLAYING,
            NORMALISATION,
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

        virtual bool IsNormalising() const override
        {
            return _mode == ReplayMode::NORMALISATION;
        }

        virtual void AddGameCommand(
            uint32_t tick, uint32_t eax, uint32_t ebx, uint32_t ecx, uint32_t edx, uint32_t esi, uint32_t edi, uint32_t ebp,
            uint8_t callback) override
        {
            if (_currentRecording == nullptr)
                return;

            uint32_t args[7];
            args[0] = eax;
            args[1] = ebx;
            args[2] = ecx;
            args[3] = edx;
            args[4] = esi;
            args[5] = edi;
            args[6] = ebp;

            _currentRecording->commands.emplace(gCurrentTicks, args, callback, _commandId++);

            // Force a checksum record the next tick.
            _nextChecksumTick = tick + 1;
        }

        virtual void AddGameAction(uint32_t tick, const GameAction* action) override
        {
            if (_currentRecording == nullptr)
                return;

            MemoryStream stream;
            DataSerialiser dsOut(true, stream);
            action->Serialise(dsOut);

            std::unique_ptr<GameAction> ga = GameActions::Create(action->GetType());
            ga->SetCallback(action->GetCallback());

            stream.SetPosition(0);
            DataSerialiser dsIn(false, stream);
            ga->Serialise(dsIn);

            _currentRecording->commands.emplace(gCurrentTicks, std::move(ga), _commandId++);

            // Force a checksum record the next tick.
            _nextChecksumTick = tick + 1;
        }

        void AddChecksum(uint32_t tick, rct_sprite_checksum&& checksum)
        {
            _currentRecording->checksums.emplace_back(std::make_pair(tick, checksum));
        }

        // Function runs each Tick.
        virtual void Update() override
        {
            if (_mode == ReplayMode::NONE)
                return;

            if ((_mode == ReplayMode::RECORDING || _mode == ReplayMode::NORMALISATION) && gCurrentTicks == _nextChecksumTick)
            {
                rct_sprite_checksum checksum = sprite_checksum();
                AddChecksum(gCurrentTicks, std::move(checksum));

                // Reset.
                _nextChecksumTick = 0;
            }

            if (_mode == ReplayMode::RECORDING)
            {
                if (gCurrentTicks >= _currentRecording->tickEnd)
                {
                    StopRecording();
                    return;
                }
            }
            else if (_mode == ReplayMode::PLAYING)
            {
                CheckState();
                ReplayCommands();

                // If we run out of commands we can stop the replay and checked all checksums we can stop.
                if (_currentReplay->commands.empty() && _currentReplay->checksumIndex >= _currentReplay->checksums.size())
                {
                    StopPlayback();
                    return;
                }
            }
            else if (_mode == ReplayMode::NORMALISATION)
            {
                ReplayCommands();

                // If we run out of commands we can just stop
                if (_currentReplay->commands.empty() && _nextChecksumTick == 0)
                {
                    StopPlayback();
                    StopRecording();

                    // Reset mode, in normalisation nothing will set it.
                    _mode = ReplayMode::NONE;
                    return;
                }
            }
        }

        virtual bool StartRecording(const std::string& name, uint32_t maxTicks /*= 0xFFFFFFFF*/) override
        {
            if (_mode != ReplayMode::NONE && _mode != ReplayMode::NORMALISATION)
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

            replayData->spriteSpatialData.Write(gSpriteSpatialIndex, sizeof(gSpriteSpatialIndex));

            DataSerialiser parkParams(true, replayData->parkParams);
            parkParams << _guestGenerationProbability;
            parkParams << _suggestedGuestMaximum;
            parkParams << gCheatsSandboxMode;
            parkParams << gCheatsDisableClearanceChecks;
            parkParams << gCheatsDisableSupportLimits;
            parkParams << gCheatsDisableTrainLengthLimit;
            parkParams << gCheatsEnableChainLiftOnAllTrack;
            parkParams << gCheatsShowAllOperatingModes;
            parkParams << gCheatsShowVehiclesFromOtherTrackTypes;
            parkParams << gCheatsFastLiftHill;
            parkParams << gCheatsDisableBrakesFailure;
            parkParams << gCheatsDisableAllBreakdowns;
            parkParams << gCheatsBuildInPauseMode;
            parkParams << gCheatsIgnoreRideIntensity;
            parkParams << gCheatsDisableVandalism;
            parkParams << gCheatsDisableLittering;
            parkParams << gCheatsNeverendingMarketing;
            parkParams << gCheatsFreezeWeather;
            parkParams << gCheatsDisablePlantAging;
            parkParams << gCheatsAllowArbitraryRideTypeChanges;
            parkParams << gCheatsDisableRideValueAging;
            parkParams << gConfigGeneral.show_real_names_of_guests;
            parkParams << gCheatsIgnoreResearchStatus;

            if (_mode != ReplayMode::NORMALISATION)
                _mode = ReplayMode::RECORDING;

            _currentRecording = std::move(replayData);

            return true;
        }

        virtual bool StopRecording() override
        {
            if (_mode != ReplayMode::RECORDING && _mode != ReplayMode::NORMALISATION)
                return false;

            _currentRecording->tickEnd = gCurrentTicks;

            DataSerialiser serialiser(true);
            Serialise(serialiser, *_currentRecording);

            char replayName[512] = {};
            snprintf(replayName, sizeof(replayName), "%s.sv6r", _currentRecording->name.c_str());

            std::string outPath = GetContext()->GetPlatformEnvironment()->GetDirectoryPath(DIRBASE::USER, DIRID::REPLAY);
            std::string outFile = Path::Combine(outPath, replayName);

            FILE* fp = fopen(outFile.c_str(), "wb");
            if (fp)
            {
                auto& stream = serialiser.GetStream();

                fwrite(stream.GetData(), 1, stream.GetLength(), fp);
                fclose(fp);
            }

            // When normalizing the output we don't touch the mode.
            if (_mode != ReplayMode::NORMALISATION)
                _mode = ReplayMode::NONE;

            _currentRecording.reset();

            return true;
        }

        virtual bool StartPlayback(const std::string& file) override
        {
            if (_mode != ReplayMode::NONE && _mode != ReplayMode::NORMALISATION)
                return false;

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

            _currentReplay = std::move(replayData);
            _currentReplay->checksumIndex = 0;

            if (_mode != ReplayMode::NORMALISATION)
                _mode = ReplayMode::PLAYING;

            return true;
        }

        virtual bool StopPlayback() override
        {
            if (_mode != ReplayMode::PLAYING && _mode != ReplayMode::NORMALISATION)
                return false;

            // When normalizing the output we don't touch the mode.
            if (_mode != ReplayMode::NORMALISATION)
            {
                _mode = ReplayMode::NONE;
            }

            _currentReplay.reset();

            return true;
        }

        virtual bool NormaliseReplay(const std::string& file, const std::string& outFile) override
        {
            _mode = ReplayMode::NORMALISATION;

            if (StartPlayback(file) == false)
            {
                return false;
            }

            if (StartRecording(outFile, 0xFFFFFFFF) == false)
            {
                StopPlayback();
                return false;
            }

            _nextReplayTick = gCurrentTicks + 1;

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

                std::memcpy(gSpriteSpatialIndex, data.spriteSpatialData.GetData(), data.spriteSpatialData.GetLength());

                DataSerialiser parkParams(false, data.parkParams);
                parkParams << _guestGenerationProbability;
                parkParams << _suggestedGuestMaximum;
                parkParams << gCheatsSandboxMode;
                parkParams << gCheatsDisableClearanceChecks;
                parkParams << gCheatsDisableSupportLimits;
                parkParams << gCheatsDisableTrainLengthLimit;
                parkParams << gCheatsEnableChainLiftOnAllTrack;
                parkParams << gCheatsShowAllOperatingModes;
                parkParams << gCheatsShowVehiclesFromOtherTrackTypes;
                parkParams << gCheatsFastLiftHill;
                parkParams << gCheatsDisableBrakesFailure;
                parkParams << gCheatsDisableAllBreakdowns;
                parkParams << gCheatsBuildInPauseMode;
                parkParams << gCheatsIgnoreRideIntensity;
                parkParams << gCheatsDisableVandalism;
                parkParams << gCheatsDisableLittering;
                parkParams << gCheatsNeverendingMarketing;
                parkParams << gCheatsFreezeWeather;
                parkParams << gCheatsDisablePlantAging;
                parkParams << gCheatsAllowArbitraryRideTypeChanges;
                parkParams << gCheatsDisableRideValueAging;
                parkParams << gConfigGeneral.show_real_names_of_guests;
                parkParams << gCheatsIgnoreResearchStatus;

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
            std::string outFile = Path::Combine(outPath, fileName);

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

            // Reset position of all streams.
            data.parkData.SetPosition(0);
            data.parkParams.SetPosition(0);
            data.spriteSpatialData.SetPosition(0);

            return true;
        }

        bool SerialiseCommand(DataSerialiser& serialiser, ReplayCommand& command)
        {
            serialiser << command.tick;
            serialiser << command.commandIndex;

            bool isGameAction = false;
            if (serialiser.IsSaving())
            {
                isGameAction = command.action != nullptr;
            }
            serialiser << isGameAction;

            if (isGameAction)
            {
                uint32_t actionType = 0;
                if (serialiser.IsSaving())
                {
                    actionType = command.action->GetType();
                }
                serialiser << actionType;

                if (serialiser.IsLoading())
                {
                    command.action = GameActions::Create(actionType);
                    Guard::Assert(command.action != nullptr);
                }

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
            return true;
        }

        bool Serialise(DataSerialiser& serialiser, ReplayRecordData& data)
        {
            serialiser << data.name;
            serialiser << data.parkData;
            serialiser << data.parkParams;
            serialiser << data.spriteSpatialData;
            serialiser << data.tickStart;
            serialiser << data.tickEnd;

            uint32_t countCommands = (uint32_t)data.commands.size();
            serialiser << countCommands;

            if (serialiser.IsSaving())
            {
                for (auto& command : data.commands)
                {
                    SerialiseCommand(serialiser, const_cast<ReplayCommand&>(command));
                }
            }
            else
            {
                for (uint32_t i = 0; i < countCommands; i++)
                {
                    ReplayCommand command = {};
                    SerialiseCommand(serialiser, command);

                    data.commands.emplace(std::move(command));
                }
            }

            uint32_t countChecksums = (uint32_t)data.checksums.size();
            serialiser << countChecksums;

            if (serialiser.IsLoading())
            {
                data.checksums.resize(countChecksums);
            }

            for (uint32_t i = 0; i < countChecksums; i++)
            {
                serialiser << data.checksums[i].first;
                serialiser << data.checksums[i].second.raw;
            }

            return true;
        }

        void CheckState()
        {
            int32_t checksumIndex = _currentReplay->checksumIndex;

            if (checksumIndex >= _currentReplay->checksums.size())
                return;

            const auto& savedChecksum = _currentReplay->checksums[checksumIndex];
            if (_currentReplay->checksums[checksumIndex].first == gCurrentTicks)
            {
                rct_sprite_checksum checksum = sprite_checksum();
                if (savedChecksum.second.ToString() != checksum.ToString())
                {
                    // Detected different game state.
                    log_info(
                        "Different sprite checksum at tick %u ; Saved: %s, Current: %s", gCurrentTicks,
                        savedChecksum.second.ToString().c_str(), checksum.ToString().c_str());
                }
                else
                {
                    // Good state.
                    log_info(
                        "Good state at tick %u ; Saved: %s, Current: %s", gCurrentTicks,
                        savedChecksum.second.ToString().c_str(), checksum.ToString().c_str());
                }
                _currentReplay->checksumIndex++;
            }
        }

        void ReplayCommands()
        {
            auto& replayQueue = _currentReplay->commands;

            while (replayQueue.begin() != replayQueue.end())
            {
                const ReplayCommand& command = (*replayQueue.begin());

                if (_mode == ReplayMode::PLAYING)
                {
                    // If this is a normal playback wait for the correct tick.
                    if (command.tick != gCurrentTicks)
                        break;
                }
                else if (_mode == ReplayMode::NORMALISATION)
                {
                    if (gCurrentTicks != _nextReplayTick)
                        break;
                    _nextReplayTick = gCurrentTicks + 1;
                }

                if (command.action != nullptr)
                {
                    GameAction* action = command.action.get();
                    action->SetFlags(action->GetFlags() | GAME_COMMAND_FLAG_REPLAY);

                    Guard::Assert(action != nullptr);

                    GameActionResult::Ptr result = GameActions::Execute(action);
                }
                else
                {
                    uint32_t flags = command.ebx | GAME_COMMAND_FLAG_REPLAY;
                    game_do_command(command.eax, flags, command.ecx, command.edx, command.esi, command.edi, command.ebp);
                }

                replayQueue.erase(replayQueue.begin());
            }
        }

    private:
        ReplayMode _mode = ReplayMode::NONE;
        std::unique_ptr<ReplayRecordData> _currentRecording;
        std::unique_ptr<ReplayRecordData> _currentReplay;
        uint32_t _commandId = 0;
        uint32_t _nextChecksumTick = 0;
        uint32_t _nextReplayTick = 0;
    };

    std::unique_ptr<IReplayManager> CreateReplayManager()
    {
        return std::make_unique<ReplayManager>();
    }

} // namespace OpenRCT2
