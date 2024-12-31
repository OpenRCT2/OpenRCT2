/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "ReplayManager.h"

#include "Context.h"
#include "Diagnostic.h"
#include "Game.h"
#include "GameState.h"
#include "GameStateSnapshots.h"
#include "OpenRCT2.h"
#include "ParkImporter.h"
#include "PlatformEnvironment.h"
#include "actions/CheatSetAction.h"
#include "actions/FootpathPlaceAction.h"
#include "actions/GameAction.h"
#include "actions/RideEntranceExitPlaceAction.h"
#include "actions/RideSetSettingAction.h"
#include "actions/TileModifyAction.h"
#include "actions/TrackPlaceAction.h"
#include "config/Config.h"
#include "core/DataSerialiser.h"
#include "core/Path.hpp"
#include "entity/EntityRegistry.h"
#include "entity/EntityTweener.h"
#include "interface/Window.h"
#include "management/NewsItem.h"
#include "object/ObjectManager.h"
#include "object/ObjectRepository.h"
#include "park/ParkFile.h"
#include "scenario/Scenario.h"
#include "world/Park.h"
#include "zlib.h"

#include <chrono>
#include <memory>
#include <vector>

namespace OpenRCT2
{
    struct ReplayCommand
    {
        uint32_t tick = 0;
        GameAction::Ptr action;
        uint32_t commandIndex = 0;

        ReplayCommand() = default;

        ReplayCommand(uint32_t t, std::unique_ptr<GameAction>&& ga, uint32_t id)
            : tick(t)
            , action(std::move(ga))
            , commandIndex(id)

        {
        }

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

    struct ReplayRecordFile
    {
        uint32_t magic;
        uint16_t version;
        uint64_t uncompressedSize;
        OpenRCT2::MemoryStream data;
    };

    struct ReplayRecordData
    {
        uint32_t magic;
        uint16_t version;
        std::string networkId;
        OpenRCT2::MemoryStream parkData;
        OpenRCT2::MemoryStream parkParams;
        OpenRCT2::MemoryStream cheatData;
        std::string name;      // Name of play
        std::string filePath;  // File path of replay.
        uint64_t timeRecorded; // Posix Time.
        uint32_t tickStart;    // First tick of replay.
        uint32_t tickEnd;      // Last tick of replay.
        std::multiset<ReplayCommand> commands;
        std::vector<std::pair<uint32_t, EntitiesChecksum>> checksums;
        uint32_t checksumIndex;
        OpenRCT2::MemoryStream gameStateSnapshots;
    };

    class ReplayManager final : public IReplayManager
    {
        static constexpr uint16_t kReplayVersion = 10;
        static constexpr uint32_t kReplayMagic = 0x5243524F; // ORCR.
        static constexpr int kReplayCompressionLevel = 9;
        static constexpr int kNormalRecordingChecksumTicks = 1;
        static constexpr int kSilentRecordingChecksumTicks = 40; // Same as network server

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

        virtual bool ShouldDisplayNotice() const override
        {
            return IsRecording() && _recordType == RecordType::NORMAL;
        }

        virtual void AddGameAction(uint32_t tick, const GameAction* action) override
        {
            if (_currentRecording == nullptr)
                return;

            auto ga = GameActions::Clone(action);

            _currentRecording->commands.emplace(tick, std::move(ga), _commandId++);
        }

        void AddChecksum(uint32_t tick, EntitiesChecksum&& checksum)
        {
            _currentRecording->checksums.emplace_back(std::make_pair(tick, std::move(checksum)));
        }

        // Function runs each Tick.
        virtual void Update() override
        {
            if (_mode == ReplayMode::NONE)
                return;

            const auto currentTicks = GetGameState().CurrentTicks;

            if ((_mode == ReplayMode::RECORDING || _mode == ReplayMode::NORMALISATION) && currentTicks == _nextChecksumTick)
            {
                EntitiesChecksum checksum = GetAllEntitiesChecksum();
                AddChecksum(currentTicks, std::move(checksum));

                _nextChecksumTick = currentTicks + ChecksumTicksDelta();
            }

            if (_mode == ReplayMode::RECORDING)
            {
                if (currentTicks >= _currentRecording->tickEnd)
                {
                    StopRecording();
                    return;
                }
            }
            else if (_mode == ReplayMode::PLAYING)
            {
#ifndef DISABLE_NETWORK
                // If the network is disabled we will only get a dummy hash which will cause
                // false positives during replay.
                if (!gSilentReplays)
                {
                    CheckState();
                }
#endif
                ReplayCommands();

                // Normal playback will always end at the specific tick.
                if (currentTicks >= _currentReplay->tickEnd)
                {
                    StopPlayback();
                    return;
                }
            }
            else if (_mode == ReplayMode::NORMALISATION)
            {
                ReplayCommands();

                // If we run out of commands we can just stop
                if (_currentReplay->commands.empty())
                {
                    StopPlayback();
                    StopRecording();

                    // Reset mode, in normalisation nothing will set it.
                    _mode = ReplayMode::NONE;
                    return;
                }
            }
        }

        void TakeGameStateSnapshot(MemoryStream& snapshotStream)
        {
            IGameStateSnapshots* snapshots = GetContext()->GetGameStateSnapshots();

            auto& snapshot = snapshots->CreateSnapshot();
            snapshots->Capture(snapshot);
            snapshots->LinkSnapshot(snapshot, GetGameState().CurrentTicks, ScenarioRandState().s0);
            DataSerialiser snapShotDs(true, snapshotStream);
            snapshots->SerialiseSnapshot(snapshot, snapShotDs);
        }

        virtual bool StartRecording(
            const std::string& name, uint32_t maxTicks /*= k_MaxReplayTicks*/, RecordType rt /*= RecordType::NORMAL*/) override
        {
            // If using silent recording, discard whatever recording there is going on, even if a new silent recording is to be
            // started.
            if (_mode == ReplayMode::RECORDING && _recordType == RecordType::SILENT)
                StopRecording(true);

            if (_mode != ReplayMode::NONE && _mode != ReplayMode::NORMALISATION)
                return false;

            auto& gameState = GetGameState();
            const auto currentTicks = gameState.CurrentTicks;

            auto replayData = std::make_unique<ReplayRecordData>();
            replayData->magic = kReplayMagic;
            replayData->version = kReplayVersion;
            replayData->networkId = NetworkGetVersion();
            replayData->name = name;
            replayData->tickStart = currentTicks;
            if (maxTicks != k_MaxReplayTicks)
                replayData->tickEnd = currentTicks + maxTicks;
            else
                replayData->tickEnd = k_MaxReplayTicks;

            replayData->filePath = name;

            auto context = GetContext();
            auto& objManager = context->GetObjectManager();
            auto objects = objManager.GetPackableObjects();

            auto exporter = std::make_unique<ParkFileExporter>();
            exporter->ExportObjectsList = objects;
            exporter->Export(gameState, replayData->parkData);

            replayData->timeRecorded = std::chrono::seconds(std::time(nullptr)).count();

            DataSerialiser parkParamsDs(true, replayData->parkParams);
            SerialiseParkParameters(parkParamsDs);

            DataSerialiser cheatDataDs(true, replayData->cheatData);
            SerialiseCheats(cheatDataDs);

            TakeGameStateSnapshot(replayData->gameStateSnapshots);

            if (_mode != ReplayMode::NORMALISATION)
                _mode = ReplayMode::RECORDING;

            _currentRecording = std::move(replayData);
            _recordType = rt;
            _nextChecksumTick = currentTicks + 1;

            return true;
        }

        virtual bool StopRecording(bool discard = false) override
        {
            if (_mode != ReplayMode::RECORDING && _mode != ReplayMode::NORMALISATION)
                return false;

            if (discard)
            {
                _currentRecording.reset();
                _mode = ReplayMode::NONE;
                return true;
            }

            const auto currentTicks = GetGameState().CurrentTicks;

            _currentRecording->tickEnd = currentTicks;

            {
                EntitiesChecksum checksum = GetAllEntitiesChecksum();
                AddChecksum(currentTicks, std::move(checksum));
            }

            TakeGameStateSnapshot(_currentRecording->gameStateSnapshots);

            // Serialise Body.
            DataSerialiser recSerialiser(true);
            Serialise(recSerialiser, *_currentRecording);

            const auto& stream = recSerialiser.GetStream();
            unsigned long streamLength = static_cast<unsigned long>(stream.GetLength());
            unsigned long compressLength = compressBound(streamLength);

            MemoryStream data(compressLength);

            ReplayRecordFile file{ _currentRecording->magic, _currentRecording->version, streamLength, data };

            auto compressBuf = std::make_unique<unsigned char[]>(compressLength);
            compress2(
                compressBuf.get(), &compressLength, static_cast<const unsigned char*>(stream.GetData()), stream.GetLength(),
                kReplayCompressionLevel);
            file.data.Write(compressBuf.get(), compressLength);

            DataSerialiser fileSerialiser(true);
            fileSerialiser << file.magic;
            fileSerialiser << file.version;
            fileSerialiser << file.uncompressedSize;
            fileSerialiser << file.data;

            bool result = false;

            const std::string& outFile = _currentRecording->filePath;

            FILE* fp = fopen(outFile.c_str(), "wb");
            if (fp != nullptr)
            {
                const auto& fileStream = fileSerialiser.GetStream();
                fwrite(fileStream.GetData(), 1, fileStream.GetLength(), fp);
                fclose(fp);

                result = true;
            }
            else
            {
                LOG_ERROR("Unable to write to file '%s'", outFile.c_str());
                result = false;
            }

            // When normalizing the output we don't touch the mode.
            if (_mode != ReplayMode::NORMALISATION)
                _mode = ReplayMode::NONE;

            _currentRecording.reset();

            News::Item* news = News::AddItemToQueue(News::ItemType::Blank, "Replay recording stopped", 0);
            news->SetFlags(News::ItemFlags::HasButton); // Has no subject.

            return result;
        }

        virtual bool GetCurrentReplayInfo(ReplayRecordInfo& info) const override
        {
            ReplayRecordData* data = nullptr;

            if (_mode == ReplayMode::PLAYING)
                data = _currentReplay.get();
            else if (_mode == ReplayMode::RECORDING)
                data = _currentRecording.get();
            else if (_mode == ReplayMode::NORMALISATION)
                data = _currentRecording.get();

            if (data == nullptr)
                return false;

            info.FilePath = data->filePath;
            info.Name = data->name;
            info.Version = data->version;
            info.TimeRecorded = data->timeRecorded;
            if (_mode == ReplayMode::RECORDING)
                info.Ticks = GetGameState().CurrentTicks - data->tickStart;
            else if (_mode == ReplayMode::PLAYING)
                info.Ticks = data->tickEnd - data->tickStart;
            info.NumCommands = static_cast<uint32_t>(data->commands.size());
            info.NumChecksums = static_cast<uint32_t>(data->checksums.size());

            return true;
        }

        void LoadAndCompareSnapshot(MemoryStream& snapshotStream)
        {
            DataSerialiser ds(false, snapshotStream);

            IGameStateSnapshots* snapshots = GetContext()->GetGameStateSnapshots();

            GameStateSnapshot_t& replaySnapshot = snapshots->CreateSnapshot();
            snapshots->SerialiseSnapshot(replaySnapshot, ds);

            const auto currentTicks = GetGameState().CurrentTicks;

            auto& localSnapshot = snapshots->CreateSnapshot();
            snapshots->Capture(localSnapshot);
            snapshots->LinkSnapshot(localSnapshot, currentTicks, ScenarioRandState().s0);
            try
            {
                GameStateCompareData cmpData = snapshots->Compare(replaySnapshot, localSnapshot);

                // Find out if there are any differences between the two states
                auto res = std::find_if(
                    cmpData.spriteChanges.begin(), cmpData.spriteChanges.end(),
                    [](const GameStateSpriteChange& diff) { return diff.changeType != GameStateSpriteChange::EQUAL; });

                // If there are difference write a log to the desyncs folder
                if (res != cmpData.spriteChanges.end())
                {
                    std::string outputPath = GetContext()->GetPlatformEnvironment()->GetDirectoryPath(
                        DIRBASE::USER, DIRID::LOG_DESYNCS);
                    char uniqueFileName[128] = {};
                    snprintf(uniqueFileName, sizeof(uniqueFileName), "replay_desync_%u.txt", currentTicks);

                    std::string outputFile = Path::Combine(outputPath, uniqueFileName);
                    snapshots->LogCompareDataToFile(outputFile, cmpData);
                }
            }
            catch (const std::runtime_error& err)
            {
                LOG_WARNING("Snapshot data failed to be read. Snapshot not compared. %s", err.what());
            }
        }

        virtual bool StartPlayback(const std::string& file) override
        {
            if (_mode != ReplayMode::NONE && _mode != ReplayMode::NORMALISATION)
                return false;

            auto replayData = std::make_unique<ReplayRecordData>();

            if (!ReadReplayData(file, *replayData))
            {
                LOG_ERROR("Unable to read replay data.");
                return false;
            }

            if (!LoadReplayDataMap(*replayData))
            {
                LOG_ERROR("Unable to load map.");
                return false;
            }

            GetGameState().CurrentTicks = replayData->tickStart;

            LoadAndCompareSnapshot(replayData->gameStateSnapshots);

            _currentReplay = std::move(replayData);
            _currentReplay->checksumIndex = 0;
            _faultyChecksumIndex = -1;

            // Make sure game is not paused.
            gGamePaused = 0;

            if (_mode != ReplayMode::NORMALISATION)
                _mode = ReplayMode::PLAYING;

            return true;
        }

        virtual bool IsPlaybackStateMismatching() const override
        {
            return _faultyChecksumIndex != -1;
        }

        virtual bool StopPlayback() override
        {
            if (_mode != ReplayMode::PLAYING && _mode != ReplayMode::NORMALISATION)
                return false;

            LoadAndCompareSnapshot(_currentReplay->gameStateSnapshots);

            // During normal playback we pause the game if stopped.
            if (_mode == ReplayMode::PLAYING)
            {
                News::Item* news = News::AddItemToQueue(News::ItemType::Blank, "Replay playback complete", 0);
                news->SetFlags(News::ItemFlags::HasButton); // Has no subject.
            }

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

            if (!StartPlayback(file))
            {
                return false;
            }

            if (!StartRecording(outFile, k_MaxReplayTicks, RecordType::NORMAL))
            {
                StopPlayback();
                return false;
            }

            _nextReplayTick = GetGameState().CurrentTicks + 1;

            return true;
        }

    private:
        int ChecksumTicksDelta() const
        {
            switch (_recordType)
            {
                default:
                case RecordType::NORMAL:
                    return kNormalRecordingChecksumTicks;
                case RecordType::SILENT:
                    return kSilentRecordingChecksumTicks;
            }
        }

        bool LoadReplayDataMap(ReplayRecordData& data)
        {
            try
            {
                data.parkData.SetPosition(0);

                auto context = GetContext();
                auto& objManager = context->GetObjectManager();
                auto importer = ParkImporter::CreateParkFile(context->GetObjectRepository());

                auto loadResult = importer->LoadFromStream(&data.parkData, false);
                objManager.LoadObjects(loadResult.RequiredObjects);

                // TODO: Have a separate GameState and exchange once loaded.
                auto& gameState = GetGameState();
                importer->Import(gameState);

                EntityTweener::Get().Reset();

                // Load all map global variables.
                DataSerialiser parkParamsDs(false, data.parkParams);
                SerialiseParkParameters(parkParamsDs);

                GameLoadInit();
                FixInvalidVehicleSpriteSizes();
            }
            catch (const std::exception& ex)
            {
                LOG_ERROR("Exception: %s", ex.what());
                return false;
            }
            return true;
        }

        bool ReadReplayFromFile(const std::string& file, MemoryStream& stream)
        {
            FILE* fp = fopen(file.c_str(), "rb");
            if (fp == nullptr)
                return false;

            char buffer[128];
            while (feof(fp) == 0)
            {
                size_t numBytesRead = fread(buffer, 1, 128, fp);
                if (numBytesRead == 0)
                    break;
                stream.Write(buffer, numBytesRead);
            }

            fclose(fp);
            return true;
        }

        /**
         * Returns true if decompression was not needed or succeeded
         * @param stream
         * @return
         */
        bool TryDecompress(MemoryStream& stream)
        {
            ReplayRecordFile recFile;
            stream.SetPosition(0);
            DataSerialiser fileSerializer(false, stream);
            fileSerializer << recFile.magic;
            fileSerializer << recFile.version;

            if (recFile.version >= 2)
            {
                fileSerializer << recFile.uncompressedSize;
                fileSerializer << recFile.data;

                auto buff = std::make_unique<unsigned char[]>(recFile.uncompressedSize);
                unsigned long outSize = recFile.uncompressedSize;
                uncompress(
                    static_cast<unsigned char*>(buff.get()), &outSize,
                    static_cast<const unsigned char*>(recFile.data.GetData()), recFile.data.GetLength());
                if (outSize != recFile.uncompressedSize)
                {
                    return false;
                }
                stream.SetPosition(0);
                stream.Write(buff.get(), outSize);
            }

            return true;
        }

        bool ReadReplayData(const std::string& file, ReplayRecordData& data)
        {
            MemoryStream stream;

            std::string fileName = file;
            if (fileName.size() < 5 || fileName.substr(fileName.size() - 5) != ".parkrep")
            {
                fileName += ".parkrep";
            }

            std::string outPath = GetContext()->GetPlatformEnvironment()->GetDirectoryPath(DIRBASE::USER, DIRID::REPLAY);
            std::string outFile = Path::Combine(outPath, fileName);

            bool loaded = false;
            if (ReadReplayFromFile(outFile, stream))
            {
                data.filePath = outFile;
                loaded = true;
            }
            else if (ReadReplayFromFile(file, stream))
            {
                data.filePath = file;
                loaded = true;
            }
            if (!loaded)
                return false;

            if (!TryDecompress(stream))
                return false;

            stream.SetPosition(0);
            DataSerialiser serialiser(false, stream);
            if (!Serialise(serialiser, data))
            {
                return false;
            }

            // Reset position of all streams.
            data.parkData.SetPosition(0);
            data.parkParams.SetPosition(0);
            data.cheatData.SetPosition(0);
            data.gameStateSnapshots.SetPosition(0);

            return true;
        }

        bool SerialiseCheats(DataSerialiser& serialiser)
        {
            CheatsSerialise(serialiser);

            return true;
        }

        bool SerialiseParkParameters(DataSerialiser& serialiser)
        {
            auto& gameState = GetGameState();

            serialiser << gameState.GuestGenerationProbability;
            serialiser << gameState.SuggestedGuestMaximum;
            serialiser << Config::Get().general.ShowRealNamesOfGuests;

            // To make this a little bit less volatile against updates
            // we reserve some space for future additions.
            uint64_t tempStorage = 0;

            // If another park parameter has to be added simply swap tempStorage.
            // and ensure the length read/write will stay uint64_t
            serialiser << tempStorage;
            serialiser << tempStorage;
            serialiser << tempStorage;
            serialiser << tempStorage;
            serialiser << tempStorage;
            serialiser << tempStorage;
            serialiser << tempStorage;
            serialiser << tempStorage;

            return true;
        }

        bool SerialiseCommand(DataSerialiser& serialiser, ReplayCommand& command)
        {
            serialiser << command.tick;
            serialiser << command.commandIndex;

            uint32_t actionType = 0;
            if (serialiser.IsSaving())
            {
                if (!command.action)
                {
                    return false;
                }
                actionType = EnumValue(command.action->GetType());
            }
            serialiser << actionType;

            if (serialiser.IsLoading())
            {
                command.action = GameActions::Create(static_cast<GameCommand>(actionType));
                Guard::Assert(command.action != nullptr);
            }

            command.action->Serialise(serialiser);

            return true;
        }

        bool Compatible(ReplayRecordData& data)
        {
            return data.version == kReplayVersion;
        }

        bool Serialise(DataSerialiser& serialiser, ReplayRecordData& data)
        {
            serialiser << data.magic;
            if (data.magic != kReplayMagic)
            {
                LOG_ERROR("Magic does not match %08X, expected: %08X", data.magic, kReplayMagic);
                return false;
            }
            serialiser << data.version;
            if (data.version != kReplayVersion && !Compatible(data))
            {
                LOG_ERROR("Invalid version detected %04X, expected: %04X", data.version, kReplayVersion);
                return false;
            }

            serialiser << data.networkId;
#ifndef DISABLE_NETWORK
            // NOTE: This does not mean the replay will not function, only a warning.
            if (data.networkId != NetworkGetVersion())
            {
                LOG_WARNING(
                    "Replay network version mismatch: '%s', expected: '%s'", data.networkId.c_str(),
                    NetworkGetVersion().c_str());
            }
#endif

            serialiser << data.name;
            serialiser << data.timeRecorded;
            serialiser << data.parkData;
            serialiser << data.parkParams;
            serialiser << data.cheatData;
            serialiser << data.tickStart;
            serialiser << data.tickEnd;

            uint32_t countCommands = static_cast<uint32_t>(data.commands.size());
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

            uint32_t countChecksums = static_cast<uint32_t>(data.checksums.size());
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

            serialiser << data.gameStateSnapshots;
            return true;
        }

#ifndef DISABLE_NETWORK
        void CheckState()
        {
            uint32_t checksumIndex = _currentReplay->checksumIndex;

            if (checksumIndex >= _currentReplay->checksums.size())
                return;

            const auto currentTicks = GetGameState().CurrentTicks;

            const auto& savedChecksum = _currentReplay->checksums[checksumIndex];
            if (_currentReplay->checksums[checksumIndex].first == currentTicks)
            {
                _currentReplay->checksumIndex++;

                EntitiesChecksum checksum = GetAllEntitiesChecksum();
                if (savedChecksum.second.raw != checksum.raw)
                {
                    uint32_t replayTick = currentTicks - _currentReplay->tickStart;

                    // Detected different game state.
                    LOG_WARNING(
                        "Different sprite checksum at tick %u (Replay Tick: %u) ; Saved: %s, Current: %s", currentTicks,
                        replayTick, savedChecksum.second.ToString().c_str(), checksum.ToString().c_str());

                    _faultyChecksumIndex = checksumIndex;
                }
                else
                {
                    // Good state.
                    LOG_VERBOSE(
                        "Good state at tick %u ; Saved: %s, Current: %s", currentTicks, savedChecksum.second.ToString().c_str(),
                        checksum.ToString().c_str());
                }
            }
        }
#endif // DISABLE_NETWORK

        void ReplayCommands()
        {
            auto& replayQueue = _currentReplay->commands;

            const auto currentTicks = GetGameState().CurrentTicks;

            while (replayQueue.begin() != replayQueue.end())
            {
                const ReplayCommand& command = (*replayQueue.begin());

                if (_mode == ReplayMode::PLAYING)
                {
                    // If this is a normal playback wait for the correct tick.
                    if (command.tick != currentTicks)
                        break;
                }
                else if (_mode == ReplayMode::NORMALISATION)
                {
                    // Allow one entry per tick.
                    if (currentTicks != _nextReplayTick)
                        break;

                    _nextReplayTick = currentTicks + 1;
                }

                bool isPositionValid = false;

                GameAction* action = command.action.get();
                action->SetFlags(action->GetFlags() | GAME_COMMAND_FLAG_REPLAY);

                GameActions::Result result = GameActions::Execute(action);
                if (result.Error == GameActions::Status::Ok)
                {
                    isPositionValid = true;
                }

                // Focus camera on event.
                if (!gSilentReplays && isPositionValid && !result.Position.IsNull())
                {
                    auto* mainWindow = WindowGetMain();
                    if (mainWindow != nullptr)
                        WindowScrollToLocation(*mainWindow, result.Position);
                }

                replayQueue.erase(replayQueue.begin());
            }
        }

    private:
        ReplayMode _mode = ReplayMode::NONE;
        std::unique_ptr<ReplayRecordData> _currentRecording;
        std::unique_ptr<ReplayRecordData> _currentReplay;
        int32_t _faultyChecksumIndex = -1;
        uint32_t _commandId = 0;
        uint32_t _nextChecksumTick = 0;
        uint32_t _nextReplayTick = 0;
        RecordType _recordType = RecordType::NORMAL;
    };

    std::unique_ptr<IReplayManager> CreateReplayManager()
    {
        return std::make_unique<ReplayManager>();
    }

} // namespace OpenRCT2
