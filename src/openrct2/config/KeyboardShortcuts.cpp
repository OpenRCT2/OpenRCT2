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

#include "../common.h"
#include "../core/Console.hpp"
#include "../core/File.h"
#include "../core/FileStream.hpp"
#include "../core/Memory.hpp"
#include "../core/Path.hpp"
#include "../core/String.hpp"

extern "C"
{
    #include "../interface/keyboard_shortcut.h"
    #include "../platform/platform.h"
}

// Current keyboard shortcuts
uint16 gShortcutKeys[SHORTCUT_COUNT];

namespace KeyboardShortcuts
{
    // Default keyboard shortcuts
    static const uint16 _defaultShortcutKeys[SHORTCUT_COUNT] =
    {
        SDL_SCANCODE_BACKSPACE,                     // SHORTCUT_CLOSE_TOP_MOST_WINDOW
        SHIFT | SDL_SCANCODE_BACKSPACE,             // SHORTCUT_CLOSE_ALL_FLOATING_WINDOWS
        SDL_SCANCODE_ESCAPE,                        // SHORTCUT_CANCEL_CONSTRUCTION_MODE
        SDL_SCANCODE_PAUSE,                         // SHORTCUT_PAUSE_GAME
        SDL_SCANCODE_PAGEUP,                        // SHORTCUT_ZOOM_VIEW_OUT
        SDL_SCANCODE_PAGEDOWN,                      // SHORTCUT_ZOOM_VIEW_IN
        SDL_SCANCODE_RETURN,                        // SHORTCUT_ROTATE_VIEW_CLOCKWISE
        SHIFT | SDL_SCANCODE_RETURN,                // SHORTCUT_ROTATE_VIEW_ANTICLOCKWISE
        SDL_SCANCODE_Z,                             // SHORTCUT_ROTATE_CONSTRUCTION_OBJECT
        SDL_SCANCODE_1,                             // SHORTCUT_UNDERGROUND_VIEW_TOGGLE
        SDL_SCANCODE_H,                             // SHORTCUT_REMOVE_BASE_LAND_TOGGLE
        SDL_SCANCODE_V,                             // SHORTCUT_REMOVE_VERTICAL_LAND_TOGGLE
        SDL_SCANCODE_3,                             // SHORTCUT_SEE_THROUGH_RIDES_TOGGLE
        SDL_SCANCODE_4,                             // SHORTCUT_SEE_THROUGH_SCENERY_TOGGLE
        SDL_SCANCODE_5,                             // SHORTCUT_INVISIBLE_SUPPORTS_TOGGLE
        SDL_SCANCODE_6,                             // SHORTCUT_INVISIBLE_PEOPLE_TOGGLE
        SDL_SCANCODE_8,                             // SHORTCUT_HEIGHT_MARKS_ON_LAND_TOGGLE
        SDL_SCANCODE_9,                             // SHORTCUT_HEIGHT_MARKS_ON_RIDE_TRACKS_TOGGLE
        SDL_SCANCODE_0,                             // SHORTCUT_HEIGHT_MARKS_ON_PATHS_TOGGLE
        SDL_SCANCODE_F1,                            // SHORTCUT_ADJUST_LAND
        SDL_SCANCODE_F2,                            // SHORTCUT_ADJUST_WATER
        SDL_SCANCODE_F3,                            // SHORTCUT_BUILD_SCENERY
        SDL_SCANCODE_F4,                            // SHORTCUT_BUILD_PATHS
        SDL_SCANCODE_F5,                            // SHORTCUT_BUILD_NEW_RIDE
        SDL_SCANCODE_F,                             // SHORTCUT_SHOW_FINANCIAL_INFORMATION
        SDL_SCANCODE_D,                             // SHORTCUT_SHOW_RESEARCH_INFORMATION
        SDL_SCANCODE_R,                             // SHORTCUT_SHOW_RIDES_LIST
        SDL_SCANCODE_P,                             // SHORTCUT_SHOW_PARK_INFORMATION
        SDL_SCANCODE_G,                             // SHORTCUT_SHOW_GUEST_LIST
        SDL_SCANCODE_S,                             // SHORTCUT_SHOW_STAFF_LIST
        SDL_SCANCODE_M,                             // SHORTCUT_SHOW_RECENT_MESSAGES
        SDL_SCANCODE_TAB,                           // SHORTCUT_SHOW_MAP
        PLATFORM_MODIFIER | SDL_SCANCODE_S,         // SHORTCUT_SCREENSHOT
        SDL_SCANCODE_MINUS,                         // SHORTCUT_REDUCE_GAME_SPEED,
        SDL_SCANCODE_EQUALS,                        // SHORTCUT_INCREASE_GAME_SPEED,
        PLATFORM_MODIFIER | ALT | SDL_SCANCODE_C,   // SHORTCUT_OPEN_CHEAT_WINDOW,
        SDL_SCANCODE_T,                             // SHORTCUT_REMOVE_TOP_BOTTOM_TOOLBAR_TOGGLE,
        SDL_SCANCODE_UP,                            // SHORTCUT_SCROLL_MAP_UP
        SDL_SCANCODE_LEFT,                          // SHORTCUT_SCROLL_MAP_LEFT
        SDL_SCANCODE_DOWN,                          // SHORTCUT_SCROLL_MAP_DOWN
        SDL_SCANCODE_RIGHT,                         // SHORTCUT_SCROLL_MAP_RIGHT
        SDL_SCANCODE_C,                             // SHORTCUT_OPEN_CHAT_WINDOW
        PLATFORM_MODIFIER | SDL_SCANCODE_F10,       // SHORTCUT_QUICK_SAVE_GAME
        SHORTCUT_UNDEFINED,                         // SHORTCUT_SHOW_OPTIONS
        SHORTCUT_UNDEFINED,                         // SHORTCUT_MUTE_SOUND
        ALT | SDL_SCANCODE_RETURN,                  // SHORTCUT_WINDOWED_MODE_TOGGLE
        SHORTCUT_UNDEFINED,                         // SHORTCUT_SHOW_MULTIPLAYER
        SHORTCUT_UNDEFINED,                         // SHORTCUT_PAINT_ORIGINAL_TOGGLE
        SHORTCUT_UNDEFINED,                         // SHORTCUT_DEBUG_PAINT_TOGGLE
        SHORTCUT_UNDEFINED,                         // SHORTCUT_SEE_THROUGH_PATHS_TOGGLE
        SDL_SCANCODE_KP_4,                          // SHORTCUT_RIDE_CONSTRUCTION_TURN_LEFT
        SDL_SCANCODE_KP_6,                          // SHORTCUT_RIDE_CONSTRUCTION_TURN_RIGHT
        SDL_SCANCODE_KP_5,                          // SHORTCUT_RIDE_CONSTRUCTION_USE_TRACK_DEFAULT
        SDL_SCANCODE_KP_2,                          // SHORTCUT_RIDE_CONSTRUCTION_SLOPE_DOWN
        SDL_SCANCODE_KP_8,                          // SHORTCUT_RIDE_CONSTRUCTION_SLOPE_UP
        SDL_SCANCODE_KP_PLUS,                       // SHORTCUT_RIDE_CONSTRUCTION_CHAIN_LIFT_TOGGLE
        SDL_SCANCODE_KP_1,                          // SHORTCUT_RIDE_CONSTRUCTION_BANK_LEFT
        SDL_SCANCODE_KP_3,                          // SHORTCUT_RIDE_CONSTRUCTION_BANK_RIGHT
        SDL_SCANCODE_KP_7,                          // SHORTCUT_RIDE_CONSTRUCTION_PREVIOUS_TRACK
        SDL_SCANCODE_KP_9,                          // SHORTCUT_RIDE_CONSTRUCTION_NEXT_TRACK
        SDL_SCANCODE_KP_0,                          // SHORTCUT_RIDE_CONSTRUCTION_BUILD_CURRENT
        SDL_SCANCODE_KP_MINUS,                      // SHORTCUT_RIDE_CONSTRUCTION_DEMOLISH_CURRENT
    };

    constexpr sint32 CURRENT_FILE_VERSION = 1;

    static void Reset()
    {
        Memory::Copy(gShortcutKeys, _defaultShortcutKeys, sizeof(gShortcutKeys));
    }

    static std::string GetPath()
    {
        utf8 path[MAX_PATH];
        platform_get_user_directory(path, nullptr, sizeof(path));
        Path::Append(path, sizeof(path), "hotkeys.cfg");
        return path;
    }
}

extern "C"
{
    void config_reset_shortcut_keys()
    {
        KeyboardShortcuts::Reset();
    }

    bool config_shortcut_keys_load()
    {
        bool result = false;
        try
        {
            std::string path = KeyboardShortcuts::GetPath();
            if (File::Exists(path))
            {
                auto fs = FileStream(path, FILE_MODE_OPEN);
                uint16 version = fs.ReadValue<uint16>();
                if (version == KeyboardShortcuts::CURRENT_FILE_VERSION)
                {
                    for (sint32 i = 0; i < SHORTCUT_COUNT; i++)
                    {
                        gShortcutKeys[i] = fs.ReadValue<uint16>();
                    }
                    result = true;
                }
            }
        }
        catch (const Exception &ex)
        {
            Console::WriteLine("Error reading shortcut keys: %s", ex.GetMessage());
        }
        return result;
    }

    bool config_shortcut_keys_save()
    {
        bool result = false;
        try
        {
            std::string path = KeyboardShortcuts::GetPath();
            auto fs = FileStream(path, FILE_MODE_WRITE);
            fs.WriteValue<uint16>(KeyboardShortcuts::CURRENT_FILE_VERSION);
            for (sint32 i = 0; i < SHORTCUT_COUNT; i++)
            {
                fs.WriteValue<uint16>(gShortcutKeys[i]);
            }
            result = true;
        }
        catch (const Exception &ex)
        {
            Console::WriteLine("Error writing shortcut keys: %s", ex.GetMessage());
        }
        return result;
    }
}
