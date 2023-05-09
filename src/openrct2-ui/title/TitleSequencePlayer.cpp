/*****************************************************************************
 * Copyright (c) 2014-2023 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "TitleSequencePlayer.h"

#include "../interface/Window.h"

#include <algorithm>
#include <memory>
#include <openrct2/Context.h>
#include <openrct2/Game.h>
#include <openrct2/GameState.h>
#include <openrct2/OpenRCT2.h>
#include <openrct2/ParkImporter.h>
#include <openrct2/common.h>
#include <openrct2/core/Console.hpp>
#include <openrct2/core/Guard.hpp>
#include <openrct2/core/Path.hpp>
#include <openrct2/core/String.hpp>
#include <openrct2/entity/EntityRegistry.h>
#include <openrct2/interface/Viewport.h>
#include <openrct2/interface/Window.h>
#include <openrct2/management/NewsItem.h>
#include <openrct2/object/ObjectManager.h>
#include <openrct2/scenario/ScenarioRepository.h>
#include <openrct2/scenario/ScenarioSources.h>
#include <openrct2/title/TitleScreen.h>
#include <openrct2/title/TitleSequence.h>
#include <openrct2/title/TitleSequenceManager.h>
#include <openrct2/title/TitleSequencePlayer.h>
#include <openrct2/ui/UiContext.h>
#include <openrct2/ui/WindowManager.h>
#include <openrct2/windows/Intent.h>
#include <openrct2/world/Map.h>
#include <openrct2/world/Scenery.h>
#include <stdexcept>

namespace OpenRCT2::Title
{
    class TitleSequencePlayer final : public ITitleSequencePlayer
    {
    private:
        GameState& _gameState;

        std::unique_ptr<TitleSequence> _sequence;
        int32_t _position = 0;
        int32_t _waitCounter = 0;

        int32_t _previousWindowWidth = 0;
        int32_t _previousWindowHeight = 0;
        ScreenCoordsXY _previousViewPosition = {};

    public:
        explicit TitleSequencePlayer(GameState& gameState)
            : _gameState(gameState)
        {
        }

        ~TitleSequencePlayer() override
        {
            Eject();
        }

        int32_t GetCurrentPosition() const override
        {
            return _position;
        }

        void Eject() override
        {
            _sequence = nullptr;
        }

        bool Begin(size_t titleSequenceId) override
        {
            StoreCurrentViewLocation();

            size_t numSequences = TitleSequenceManager::GetCount();
            if (titleSequenceId >= numSequences)
            {
                return false;
            }

            auto seqItem = TitleSequenceManager::GetItem(titleSequenceId);
            auto sequence = LoadTitleSequence(seqItem->Path);
            if (sequence == nullptr)
            {
                return false;
            }

            Eject();
            _sequence = std::move(sequence);

            Reset();
            return true;
        }

        bool Update() override
        {
            RestoreViewLocationIfResized();

            if (_sequence == nullptr)
            {
                return false;
            }

            if (_sequence->Commands.empty())
            {
                return false;
            }

            // Run commands in order, until we reach one that is not instantly done
            int32_t entryPosition = _position;
            while (true)
            {
                auto& currentCommand = _sequence->Commands[_position];
                try
                {
                    int framesToWait = std::visit([&](auto& command) { return command(_waitCounter); }, currentCommand);
                    if (framesToWait > _waitCounter)
                    {
                        _waitCounter++;
                        break;
                    }

                    // TODO: Make the loading interface simpler so these blocks can be moved to their respective command classes
                    if (std::holds_alternative<LoadParkCommand>(currentCommand))
                    {
                        bool loadSuccess = false;
                        const auto saveIndex = std::get<LoadParkCommand>(currentCommand).SaveIndex;
                        auto parkHandle = TitleSequenceGetParkHandle(*_sequence, saveIndex);
                        if (parkHandle != nullptr)
                        {
                            GameNotifyMapChange();
                            loadSuccess = LoadParkFromStream(parkHandle->Stream.get(), parkHandle->HintPath);
                        }
                        if (!loadSuccess)
                        {
                            if (_sequence->Saves.size() > saveIndex)
                            {
                                const auto& path = _sequence->Saves[saveIndex];
                                throw std::domain_error("Failed to load: \"" + path + "\" for the title sequence.");
                            }

                            throw std::out_of_range("Failed to load park; index out of range.");
                        }

                        GameNotifyMapChanged();
                    }
                    else if (std::holds_alternative<LoadScenarioCommand>(currentCommand))
                    {
                        auto& scenarioName = std::get<LoadScenarioCommand>(currentCommand).Scenario;
                        bool loadSuccess = false;
                        auto scenario = GetScenarioRepository()->GetByInternalName(scenarioName);
                        if (scenario != nullptr)
                        {
                            GameNotifyMapChange();
                            loadSuccess = LoadParkFromFile(scenario->Path);
                        }

                        if (!loadSuccess)
                        {
                            auto message = std::string("Failed to load: \"") + scenarioName + "\" for the title sequence.";
                            throw std::domain_error(message);
                        }

                        GameNotifyMapChanged();
                    }

                    IncrementPosition();
                }
                catch (std::exception& e)
                {
                    const char* commandName = std::visit(
                        [](auto&& command) { return std::decay_t<decltype(command)>::Name; }, currentCommand);
                    Console::Error::WriteLine("%s (command %i) failed with error: %s", commandName, _position, e.what());

                    if (TitleSequenceIsLoadCommand(currentCommand))
                    {
                        Console::Error::WriteLine("  Skipping to the next load command.");
                        SkipToNextLoadCommand();
                    }
                    else
                    {
                        Console::Error::WriteLine("  Skipping to the next command.");
                        IncrementPosition();
                    }
                }

                if (_position == entryPosition)
                {
                    Console::Error::WriteLine("Infinite loop detected in title sequence.");
                    Console::Error::WriteLine("  A wait command may be missing.");
                    return false;
                }
            }

            // Store current window size and screen position in case the window resizes and the main focus changes
            StoreCurrentViewLocation();

            return true;
        }

        void Reset() override
        {
            _position = 0;
            _waitCounter = 0;
        }

        void Seek(int32_t targetPosition) override
        {
            if (targetPosition < 0 || targetPosition >= static_cast<int32_t>(_sequence->Commands.size()))
            {
                throw std::runtime_error("Invalid position.");
            }
            if (_position >= targetPosition)
            {
                Reset();
            }

            if (std::holds_alternative<RestartCommand>(_sequence->Commands[targetPosition]))
            {
                targetPosition = 0;
            }

            // Set position to the last LOAD command before target position
            for (int32_t i = targetPosition; i >= 0; i--)
            {
                const TitleCommand& command = _sequence->Commands[i];
                if ((_position == i && _position != targetPosition) || TitleSequenceIsLoadCommand(command))
                {
                    // Break if we have a new load command or if we're already in the range of the correct load command
                    _position = i;
                    break;
                }
            }

            // Keep updating until we reach target position
            gInUpdateCode = true;

            while (_position < targetPosition)
            {
                if (Update())
                {
                    _gameState.UpdateLogic();
                }
                else
                {
                    break;
                }
            }

            gInUpdateCode = false;

            _waitCounter = 0;
        }

    private:
        void IncrementPosition()
        {
            _position++;
            if (_position >= static_cast<int32_t>(_sequence->Commands.size()))
            {
                _position = 0;
                printf(" Reached the end of script, closing ");
                ContextQuit();
            }
            _waitCounter = 0;
        }

        bool SkipToNextLoadCommand()
        {
            int32_t entryPosition = _position;
            const TitleCommand* command;
            do
            {
                IncrementPosition();
                command = &_sequence->Commands[_position];
            } while (!TitleSequenceIsLoadCommand(*command) && _position != entryPosition);
            return _position != entryPosition;
        }

        bool LoadParkFromFile(const u8string& path)
        {
            LOG_VERBOSE("TitleSequencePlayer::LoadParkFromFile(%s)", path.c_str());
            bool success = false;
            try
            {
                if (gPreviewingTitleSequenceInGame)
                {
                    gLoadKeepWindowsOpen = true;
                    CloseParkSpecificWindows();
                    GetContext()->LoadParkFromFile(path);
                }
                else
                {
                    auto parkImporter = ParkImporter::Create(path);
                    auto result = parkImporter->Load(path);

                    auto& objectManager = GetContext()->GetObjectManager();
                    objectManager.LoadObjects(result.RequiredObjects);

                    parkImporter->Import();
                }
                PrepareParkForPlayback();
                success = true;
            }
            catch (const std::exception&)
            {
                Console::Error::WriteLine("Unable to load park: %s", path.c_str());
            }
            gLoadKeepWindowsOpen = false;
            return success;
        }

        /**
         * @param stream The stream to read the park data from.
         * @param hintPath Hint path, the extension is grabbed to determine what importer to use.
         */
        bool LoadParkFromStream(OpenRCT2::IStream* stream, const std::string& hintPath)
        {
            LOG_VERBOSE("TitleSequencePlayer::LoadParkFromStream(%s)", hintPath.c_str());
            bool success = false;
            try
            {
                if (gPreviewingTitleSequenceInGame)
                {
                    gLoadKeepWindowsOpen = true;
                    CloseParkSpecificWindows();
                    ContextLoadParkFromStream(stream);
                }
                else
                {
                    bool isScenario = ParkImporter::ExtensionIsScenario(hintPath);
                    auto parkImporter = ParkImporter::Create(hintPath);
                    auto result = parkImporter->LoadFromStream(stream, isScenario);

                    auto& objectManager = GetContext()->GetObjectManager();
                    objectManager.LoadObjects(result.RequiredObjects);

                    parkImporter->Import();
                }
                PrepareParkForPlayback();
                success = true;
            }
            catch (const std::exception&)
            {
                Console::Error::WriteLine("Unable to load park: %s", hintPath.c_str());
            }
            gLoadKeepWindowsOpen = false;
            return success;
        }

        void CloseParkSpecificWindows()
        {
            WindowCloseByClass(WindowClass::ConstructRide);
            WindowCloseByClass(WindowClass::DemolishRidePrompt);
            WindowCloseByClass(WindowClass::EditorInventionListDrag);
            WindowCloseByClass(WindowClass::EditorInventionList);
            WindowCloseByClass(WindowClass::EditorObjectSelection);
            WindowCloseByClass(WindowClass::EditorObjectiveOptions);
            WindowCloseByClass(WindowClass::EditorScenarioOptions);
            WindowCloseByClass(WindowClass::Finances);
            WindowCloseByClass(WindowClass::FirePrompt);
            WindowCloseByClass(WindowClass::GuestList);
            WindowCloseByClass(WindowClass::InstallTrack);
            WindowCloseByClass(WindowClass::Peep);
            WindowCloseByClass(WindowClass::Ride);
            WindowCloseByClass(WindowClass::RideConstruction);
            WindowCloseByClass(WindowClass::RideList);
            WindowCloseByClass(WindowClass::Scenery);
            WindowCloseByClass(WindowClass::Staff);
            WindowCloseByClass(WindowClass::TrackDeletePrompt);
            WindowCloseByClass(WindowClass::TrackDesignList);
            WindowCloseByClass(WindowClass::TrackDesignPlace);
        }

        void PrepareParkForPlayback()
        {
            auto windowManager = GetContext()->GetUiContext()->GetWindowManager();
            windowManager->SetMainView(gSavedView, gSavedViewZoom, gSavedViewRotation);
            ResetEntitySpatialIndices();
            ResetAllSpriteQuadrantPlacements();
            auto intent = Intent(INTENT_ACTION_REFRESH_NEW_RIDES);
            ContextBroadcastIntent(&intent);
            ScenerySetDefaultPlacementConfiguration();
            News::InitQueue();
            LoadPalette();
            gScreenAge = 0;
            gGamePaused = false;
            gGameSpeed = 1;
        }

        void StoreCurrentViewLocation()
        {
            WindowBase* w = WindowGetMain();
            if (w != nullptr && w->viewport_smart_follow_sprite.IsNull())
            {
                _previousWindowWidth = w->width;
                _previousWindowHeight = w->height;
                _previousViewPosition = w->savedViewPos;
            }
        }

        /**
         * Fixes the view location for when the game window has changed size.
         */
        void RestoreViewLocationIfResized()
        {
            WindowBase* w = WindowGetMain();
            if (w != nullptr && w->viewport_smart_follow_sprite.IsNull())
            {
                if (w->width != _previousWindowWidth || w->height != _previousWindowHeight)
                {
                    w->savedViewPos.x += (_previousWindowWidth - w->width) / 2;
                    w->savedViewPos.y += (_previousWindowHeight - w->height) / 2;
                }
            }
        }
    };

    std::unique_ptr<ITitleSequencePlayer> CreateTitleSequencePlayer(GameState& gameState)
    {
        return std::make_unique<TitleSequencePlayer>(gameState);
    }
} // namespace OpenRCT2::Title
