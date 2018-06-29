/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include <memory>
#include <openrct2/common.h>
#include <openrct2/Context.h>
#include <openrct2/core/Console.hpp>
#include <openrct2/core/Guard.hpp>
#include <openrct2/core/Math.hpp>
#include <openrct2/core/Path.hpp>
#include <openrct2/core/String.hpp>
#include <openrct2/object/ObjectManager.h>
#include <openrct2/OpenRCT2.h>
#include <openrct2/GameState.h>
#include <openrct2/ParkImporter.h>
#include <openrct2/scenario/ScenarioRepository.h>
#include <openrct2/scenario/ScenarioSources.h>
#include <openrct2/title/TitleScreen.h>
#include <openrct2/title/TitleSequence.h>
#include <openrct2/title/TitleSequenceManager.h>
#include <openrct2/title/TitleSequencePlayer.h>
#include <openrct2/Game.h>
#include <openrct2/interface/Viewport.h>
#include <openrct2/interface/Window.h>
#include <openrct2/ui/UiContext.h>
#include <openrct2/ui/WindowManager.h>
#include <openrct2/management/NewsItem.h>
#include <openrct2/windows/Intent.h>
#include <openrct2/world/Map.h>
#include <openrct2/world/Scenery.h>
#include <openrct2/world/Sprite.h>
#include "TitleSequencePlayer.h"
#include "../interface/Window.h"

using namespace OpenRCT2;

class TitleSequencePlayer final : public ITitleSequencePlayer
{
private:
    static constexpr const char * SFMM_FILENAME = "Six Flags Magic Mountain.SC6";

    IScenarioRepository&    _scenarioRepository;
    GameState&              _gameState;

    size_t          _sequenceId = 0;
    TitleSequence * _sequence = nullptr;
    int32_t          _position = 0;
    int32_t          _waitCounter = 0;

    int32_t          _lastScreenWidth = 0;
    int32_t          _lastScreenHeight = 0;
    CoordsXY        _viewCentreLocation = {};

public:
    explicit TitleSequencePlayer(IScenarioRepository& scenarioRepository, GameState& gameState)
        : _scenarioRepository(scenarioRepository),
          _gameState(gameState)
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
        FreeTitleSequence(_sequence);
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
        _sequence = sequence;
        _sequenceId = titleSequenceId;

        Reset();
        return true;
    }

    bool Update() override
    {
        int32_t entryPosition = _position;
        FixViewLocation();

        if (_sequence == nullptr)
        {
            SetViewLocation(75 * 32, 75 * 32);
            return false;
        }

        // Check that position is valid
        if (_position >= (int32_t)_sequence->NumCommands)
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
                const TitleCommand * command = &_sequence->Commands[_position];
                if (command->Type == TITLE_SCRIPT_WAIT)
                {
                    IncrementPosition();
                }
            }
        }
        else
        {
            while (true)
            {
                const TitleCommand * command = &_sequence->Commands[_position];
                if (ExecuteCommand(command))
                {
                    if (command->Type == TITLE_SCRIPT_WAIT)
                    {
                        break;
                    }
                    if (command->Type != TITLE_SCRIPT_RESTART)
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
                        Console::Error::WriteLine("Unable to load any parks from %s.", _sequence->Name);
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
        if (targetPosition < 0 || targetPosition >= (int32_t)_sequence->NumCommands)
        {
            throw std::runtime_error("Invalid position.");
        }
        if (_position >= targetPosition)
        {
            Reset();
        }

        if (_sequence->Commands[targetPosition].Type == TITLE_SCRIPT_RESTART)
        {
            targetPosition = 0;
        }
        // Set position to the last LOAD command before target position
        for (int32_t i = targetPosition; i >= 0; i--)
        {
            const TitleCommand * command = &_sequence->Commands[i];
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
        if (_position >= (int32_t)_sequence->NumCommands)
        {
            _position = 0;
        }
    }

    bool SkipToNextLoadCommand()
    {
        int32_t entryPosition = _position;
        const TitleCommand * command;
        do
        {
            IncrementPosition();
            command = &_sequence->Commands[_position];
        }
        while (!TitleSequenceIsLoadCommand(command) && _position != entryPosition);
        return _position != entryPosition;
    }

    bool ExecuteCommand(const TitleCommand * command)
    {
        switch (command->Type) {
        case TITLE_SCRIPT_END:
            _waitCounter = 1;
            break;
        case TITLE_SCRIPT_WAIT:
            // The waitCounter is measured in 25-ms game ticks. Previously it was seconds * 40 ticks/second, now it is ms / 25 ms/tick
            _waitCounter = std::max<int32_t>(1, command->Milliseconds / (uint32_t)GAME_UPDATE_TIME_MS);
            break;
        case TITLE_SCRIPT_LOADMM:
        {
            const scenario_index_entry * entry = _scenarioRepository.GetByFilename(SFMM_FILENAME);
            if (entry == nullptr)
            {
                Console::Error::WriteLine("%s not found.", SFMM_FILENAME);
                return false;
            }

            const utf8 * path = entry->path;
            if (!LoadParkFromFile(path))
            {
                Console::Error::WriteLine("Failed to load: \"%s\" for the title sequence.", path);
                return false;
            }
            break;
        }
        case TITLE_SCRIPT_LOCATION:
        {
            int32_t x = command->X * 32 + 16;
            int32_t y = command->Y * 32 + 16;
            SetViewLocation(x, y);
            break;
        }
        case TITLE_SCRIPT_ROTATE:
            RotateView(command->Rotations);
            break;
        case TITLE_SCRIPT_ZOOM:
            SetViewZoom(command->Zoom);
            break;
        case TITLE_SCRIPT_SPEED:
            gGameSpeed = Math::Clamp<uint8_t>(1, command->Speed, 4);
            break;
        case TITLE_SCRIPT_FOLLOW:
            FollowSprite(command->SpriteIndex);
            break;
        case TITLE_SCRIPT_RESTART:
            Reset();
            break;
        case TITLE_SCRIPT_LOAD:
        {
            bool loadSuccess = false;
            uint8_t saveIndex = command->SaveIndex;
            TitleSequenceParkHandle * parkHandle = TitleSequenceGetParkHandle(_sequence, saveIndex);
            if (parkHandle != nullptr)
            {
                loadSuccess = LoadParkFromStream((IStream *)parkHandle->Stream, parkHandle->HintPath);
                TitleSequenceCloseParkHandle(parkHandle);
            }
            if (!loadSuccess)
            {
                if (_sequence->NumSaves > saveIndex)
                {
                    const utf8 * path = _sequence->Saves[saveIndex];
                    Console::Error::WriteLine("Failed to load: \"%s\" for the title sequence.", path);
                }
                return false;
            }
            break;
        }
        case TITLE_SCRIPT_LOADRCT1:
        {
            source_desc sourceDesc;
            if (!ScenarioSources::TryGetById(command->SaveIndex, &sourceDesc) || sourceDesc.index == -1)
            {
                Console::Error::WriteLine("Invalid scenario id.");
                return false;
            }

            const utf8 * path = nullptr;
            size_t numScenarios =  _scenarioRepository.GetCount();
            for (size_t i = 0; i < numScenarios; i++)
            {
                const scenario_index_entry * scenario = _scenarioRepository.GetByIndex(i);
                if (scenario && scenario->source_index == sourceDesc.index)
                {
                    path = scenario->path;
                    break;
                }
            }
            if (path == nullptr || !LoadParkFromFile(path))
            {
                return false;
            }
            break;
        }
        case TITLE_SCRIPT_LOADSC:
        {
            bool loadSuccess = false;
            auto scenario = GetScenarioRepository()->GetByInternalName(command->Scenario);
            if (scenario != nullptr)
            {
                loadSuccess = LoadParkFromFile(scenario->path);
            }
            if (!loadSuccess)
            {
                Console::Error::WriteLine("Failed to load: \"%s\" for the title sequence.", command->Scenario);
                return false;
            }
            break;
        }
        }
        return true;
    }

    void SetViewZoom(const uint32_t &zoom)
    {
        rct_window * w = window_get_main();
        if (w != nullptr && w->viewport != nullptr)
        {
            window_zoom_set(w, zoom, false);
        }
    }

    void RotateView(uint32_t count)
    {
        rct_window * w = window_get_main();
        if (w != nullptr)
        {
            for (uint32_t i = 0; i < count; i++)
            {
                window_rotate_camera(w, 1);
            }
        }
    }

    void FollowSprite(uint16_t spriteIndex)
    {
        rct_window * w = window_get_main();
        if (w != nullptr)
        {
            window_follow_sprite(w, spriteIndex);
        }
    }

    void UnfollowSprite()
    {
        rct_window * w = window_get_main();
        if (w != nullptr)
        {
            window_unfollow_sprite(w);
        }
    }

    bool LoadParkFromFile(const utf8 * path)
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

                auto objectManager = GetContext()->GetObjectManager();
                objectManager->LoadObjects(result.RequiredObjects.data(), result.RequiredObjects.size());

                parkImporter->Import();
            }
            PrepareParkForPlayback();
            success = true;
        }
        catch (const std::exception &)
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
    bool LoadParkFromStream(IStream * stream, const std::string &hintPath)
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

                auto objectManager = GetContext()->GetObjectManager();
                objectManager->LoadObjects(result.RequiredObjects.data(), result.RequiredObjects.size());

                parkImporter->Import();
            }
            PrepareParkForPlayback();
            success = true;
        }
        catch (const std::exception &)
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
        window_close_by_class(WC_EDTIOR_OBJECTIVE_OPTIONS);
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
        windowManager->SetMainView(gSavedViewX, gSavedViewY, gSavedViewZoom, gSavedViewRotation);
        reset_sprite_spatial_index();
        reset_all_sprite_quadrant_placements();
        auto intent = Intent(INTENT_ACTION_REFRESH_NEW_RIDES);
        context_broadcast_intent(&intent);
        scenery_set_default_placement_configuration();
        news_item_init_queue();
        load_palette();
        gScreenAge = 0;
        gGameSpeed = 1;
    }

    /**
     * Sets the map location to the given tile coordinates. Z is automatic.
     * @param x X position in map tiles.
     * @param y Y position in map tiles.
     */
    void SetViewLocation(int32_t x, int32_t y)
    {
        // Update viewport
        rct_window * w = window_get_main();
        if (w != nullptr)
        {
            int32_t z = tile_element_height(x, y);

            // Prevent scroll adjustment due to window placement when in-game
            auto oldScreenFlags = gScreenFlags;
            gScreenFlags = SCREEN_FLAGS_TITLE_DEMO;
            window_set_location(w, x, y, z);
            gScreenFlags = oldScreenFlags;

            viewport_update_position(w);

            // Save known tile position in case of window resize
            _lastScreenWidth = w->width;
            _lastScreenHeight = w->height;
            _viewCentreLocation.x = x;
            _viewCentreLocation.y = y;
        }
    }

    /**
     * Fixes the view location for when the game window has changed size.
     */
    void FixViewLocation()
    {
        rct_window * w = window_get_main();
        if (w != nullptr && w->viewport_smart_follow_sprite == SPRITE_INDEX_NULL)
        {
            if (w->width != _lastScreenWidth ||
                w->height != _lastScreenHeight)
            {
                SetViewLocation(_viewCentreLocation.x, _viewCentreLocation.y);
            }
        }
    }
};

std::unique_ptr<ITitleSequencePlayer> CreateTitleSequencePlayer(IScenarioRepository& scenarioRepository, GameState& gameState)
{
    return std::make_unique<TitleSequencePlayer>(scenarioRepository, gameState);
}
