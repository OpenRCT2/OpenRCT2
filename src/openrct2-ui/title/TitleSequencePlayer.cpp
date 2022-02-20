/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
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

using namespace OpenRCT2;

class TitleSequencePlayer final : public ITitleSequencePlayer
{
private:
    GameState& _gameState;

    std::unique_ptr<TitleSequence> _sequence;
    int32_t _position = 0;
    int32_t _waitCounter = 0;

    int32_t _lastScreenWidth = 0;
    int32_t _lastScreenHeight = 0;
    CoordsXY _viewCentreLocation = {};

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
        size_t numSequences = TitleSequenceManager::GetCount();
        if (titleSequenceId >= numSequences)
        {
            return false;
        }

        auto seqItem = TitleSequenceManager::GetItem(titleSequenceId);
        auto sequence = LoadTitleSequence(seqItem->Path.c_str());
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
        int32_t entryPosition = _position;
        FixViewLocation();

        if (_sequence == nullptr)
        {
            SetViewLocation(TileCoordsXY(75, 75).ToCoordsXY());
            return false;
        }

        // Check that position is valid
        if (_position >= static_cast<int32_t>(_sequence->Commands.size()))
        {
            _position = 0;
            return false;
        }

        // Don't execute next command until we are done with the current wait command
        if (_waitCounter != 0)
        {
            _waitCounter--;
            if (_waitCounter == 0)
            {
                const auto& command = _sequence->Commands[_position];
                if (command.Type == TitleScript::Wait)
                {
                    IncrementPosition();
                }
            }
        }
        else
        {
            while (true)
            {
                const auto& command = _sequence->Commands[_position];
                if (ExecuteCommand(command))
                {
                    if (command.Type == TitleScript::Wait)
                    {
                        break;
                    }
                    if (command.Type != TitleScript::Restart)
                    {
                        IncrementPosition();
                    }
                    if (_position == entryPosition)
                    {
                        Console::Error::WriteLine("Infinite loop detected in title sequence.");
                        Console::Error::WriteLine("  A wait command may be missing.");
                        return false;
                    }
                }
                else
                {
                    if (!SkipToNextLoadCommand() || _position == entryPosition)
                    {
                        Console::Error::WriteLine("Unable to load any parks from %s.", _sequence->Name.c_str());
                        return false;
                    }
                }
            }
        }
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

        if (_sequence->Commands[targetPosition].Type == TitleScript::Restart)
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
        }
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

    bool ExecuteCommand(const TitleCommand& command)
    {
        switch (command.Type)
        {
            case TitleScript::End:
                _waitCounter = 1;
                break;
            case TitleScript::Wait:
                // The waitCounter is measured in 25-ms game ticks. Previously it was seconds * 40 ticks/second, now it is ms /
                // 25 ms/tick
                _waitCounter = std::max<int32_t>(
                    1, command.Milliseconds / static_cast<uint32_t>(GAME_UPDATE_TIME_MS * 1000.0f));
                break;
            case TitleScript::Location:
            {
                auto loc = TileCoordsXY(command.Location.X, command.Location.Y).ToCoordsXY().ToTileCentre();
                SetViewLocation(loc);
                break;
            }
            case TitleScript::Undefined:
                break;
            case TitleScript::Loop:
                break;
            case TitleScript::EndLoop:
                break;
            case TitleScript::Rotate:
                RotateView(command.Rotations);
                break;
            case TitleScript::Zoom:
                SetViewZoom(ZoomLevel{ static_cast<int8_t>(command.Zoom) });
                break;
            case TitleScript::Speed:
                gGameSpeed = std::clamp<uint8_t>(command.Speed, 1, 4);
                break;
            case TitleScript::Follow:
                FollowSprite(command.Follow.SpriteIndex);
                break;
            case TitleScript::Restart:
                Reset();
                break;
            case TitleScript::Load:
            {
                bool loadSuccess = false;
                uint8_t saveIndex = command.SaveIndex;
                auto parkHandle = TitleSequenceGetParkHandle(*_sequence, saveIndex);
                if (parkHandle != nullptr)
                {
                    loadSuccess = LoadParkFromStream(parkHandle->Stream.get(), parkHandle->HintPath);
                }
                if (loadSuccess)
                {
                    game_notify_map_changed();
                }
                else
                {
                    if (_sequence->Saves.size() > saveIndex)
                    {
                        const auto& path = _sequence->Saves[saveIndex];
                        Console::Error::WriteLine("Failed to load: \"%s\" for the title sequence.", path.c_str());
                    }
                    return false;
                }
                break;
            }
            case TitleScript::LoadSc:
            {
                bool loadSuccess = false;
                auto scenario = GetScenarioRepository()->GetByInternalName(command.Scenario);
                if (scenario != nullptr)
                {
                    loadSuccess = LoadParkFromFile(scenario->path);
                }
                if (loadSuccess)
                {
                    game_notify_map_changed();
                }
                else
                {
                    Console::Error::WriteLine("Failed to load: \"%s\" for the title sequence.", command.Scenario);
                    return false;
                }
                break;
            }
        }
        return true;
    }

    void SetViewZoom(ZoomLevel zoom)
    {
        rct_window* w = window_get_main();
        if (w != nullptr && w->viewport != nullptr)
        {
            window_zoom_set(w, zoom, false);
        }
    }

    void RotateView(uint32_t count)
    {
        rct_window* w = window_get_main();
        if (w != nullptr)
        {
            for (uint32_t i = 0; i < count; i++)
            {
                window_rotate_camera(w, 1);
            }
        }
    }

    void FollowSprite(EntityId spriteIndex)
    {
        rct_window* w = window_get_main();
        if (w != nullptr)
        {
            window_follow_sprite(w, spriteIndex);
        }
    }

    void UnfollowSprite()
    {
        rct_window* w = window_get_main();
        if (w != nullptr)
        {
            window_unfollow_sprite(w);
        }
    }

    bool LoadParkFromFile(const utf8* path)
    {
        log_verbose("TitleSequencePlayer::LoadParkFromFile(%s)", path);
        bool success = false;
        try
        {
            if (gPreviewingTitleSequenceInGame)
            {
                gLoadKeepWindowsOpen = true;
                CloseParkSpecificWindows();
                context_load_park_from_file(path);
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
            Console::Error::WriteLine("Unable to load park: %s", path);
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
        log_verbose("TitleSequencePlayer::LoadParkFromStream(%s)", hintPath.c_str());
        bool success = false;
        try
        {
            if (gPreviewingTitleSequenceInGame)
            {
                gLoadKeepWindowsOpen = true;
                CloseParkSpecificWindows();
                context_load_park_from_stream(stream);
            }
            else
            {
                std::string extension = Path::GetExtension(hintPath);
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
        window_close_by_class(WC_CONSTRUCT_RIDE);
        window_close_by_class(WC_DEMOLISH_RIDE_PROMPT);
        window_close_by_class(WC_EDITOR_INVENTION_LIST_DRAG);
        window_close_by_class(WC_EDITOR_INVENTION_LIST);
        window_close_by_class(WC_EDITOR_OBJECT_SELECTION);
        window_close_by_class(WC_EDITOR_OBJECTIVE_OPTIONS);
        window_close_by_class(WC_EDITOR_SCENARIO_OPTIONS);
        window_close_by_class(WC_FINANCES);
        window_close_by_class(WC_FIRE_PROMPT);
        window_close_by_class(WC_GUEST_LIST);
        window_close_by_class(WC_INSTALL_TRACK);
        window_close_by_class(WC_PEEP);
        window_close_by_class(WC_RIDE);
        window_close_by_class(WC_RIDE_CONSTRUCTION);
        window_close_by_class(WC_RIDE_LIST);
        window_close_by_class(WC_SCENERY);
        window_close_by_class(WC_STAFF);
        window_close_by_class(WC_TRACK_DELETE_PROMPT);
        window_close_by_class(WC_TRACK_DESIGN_LIST);
        window_close_by_class(WC_TRACK_DESIGN_PLACE);
    }

    void PrepareParkForPlayback()
    {
        auto windowManager = GetContext()->GetUiContext()->GetWindowManager();
        windowManager->SetMainView(gSavedView, gSavedViewZoom, gSavedViewRotation);
        ResetEntitySpatialIndices();
        reset_all_sprite_quadrant_placements();
        auto intent = Intent(INTENT_ACTION_REFRESH_NEW_RIDES);
        context_broadcast_intent(&intent);
        scenery_set_default_placement_configuration();
        News::InitQueue();
        load_palette();
        gScreenAge = 0;
        gGamePaused = false;
        gGameSpeed = 1;
    }

    /**
     * Sets the map location to the given (big) coordinates. Z is automatic.
     * @param loc X and Y position in big coordinates.
     */
    void SetViewLocation(const CoordsXY& loc)
    {
        // Update viewport
        rct_window* w = window_get_main();
        if (w != nullptr)
        {
            int32_t z = tile_element_height(loc);

            // Prevent scroll adjustment due to window placement when in-game
            auto oldScreenFlags = gScreenFlags;
            gScreenFlags = SCREEN_FLAGS_TITLE_DEMO;
            w->SetLocation({ loc, z });
            gScreenFlags = oldScreenFlags;

            viewport_update_position(w);

            // Save known tile position in case of window resize
            _lastScreenWidth = w->width;
            _lastScreenHeight = w->height;
            _viewCentreLocation = loc;
        }
    }

    /**
     * Fixes the view location for when the game window has changed size.
     */
    void FixViewLocation()
    {
        rct_window* w = window_get_main();
        if (w != nullptr && w->viewport_smart_follow_sprite.IsNull())
        {
            if (w->width != _lastScreenWidth || w->height != _lastScreenHeight)
            {
                SetViewLocation(_viewCentreLocation);
            }
        }
    }
};

std::unique_ptr<ITitleSequencePlayer> CreateTitleSequencePlayer(GameState& gameState)
{
    return std::make_unique<TitleSequencePlayer>(gameState);
}
