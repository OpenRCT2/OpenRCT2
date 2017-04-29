#pragma region Copyright (c) 2014-2016 OpenRCT2 Developers
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
keypress gShortcutKeys[SHORTCUT_COUNT];

namespace KeyboardShortcuts
{
    // Default keyboard shortcuts
    static const keypress _defaultShortcutKeys[SHORTCUT_COUNT] = {
        { SDLK_BACKSPACE,   KMOD_NONE },                        // SHORTCUT_CLOSE_TOP_MOST_WINDOW
        { SDLK_BACKSPACE,   KMOD_SHIFT },                       // SHORTCUT_CLOSE_ALL_FLOATING_WINDOWS
        { SDLK_ESCAPE,      KMOD_NONE },                        // SHORTCUT_CANCEL_CONSTRUCTION_MODE
        { SDLK_PAUSE,       KMOD_NONE },                        // SHORTCUT_PAUSE_GAME
        { SDLK_PAGEUP,      KMOD_NONE },                        // SHORTCUT_ZOOM_VIEW_OUT
        { SDLK_PAGEDOWN,    KMOD_NONE },                        // SHORTCUT_ZOOM_VIEW_IN
        { SDLK_RETURN,      KMOD_NONE },                        // SHORTCUT_ROTATE_VIEW_CLOCKWISE
        { SDLK_RETURN,      KMOD_SHIFT },                       // SHORTCUT_ROTATE_VIEW_ANTICLOCKWISE
        { SDLK_z,           KMOD_NONE },                        // SHORTCUT_ROTATE_CONSTRUCTION_OBJECT
        { SDLK_1,           KMOD_NONE },                        // SHORTCUT_UNDERGROUND_VIEW_TOGGLE
        { SDLK_h,           KMOD_NONE },                        // SHORTCUT_REMOVE_BASE_LAND_TOGGLE
        { SDLK_v,           KMOD_NONE },                        // SHORTCUT_REMOVE_VERTICAL_LAND_TOGGLE
        { SDLK_3,           KMOD_NONE },                        // SHORTCUT_SEE_THROUGH_RIDES_TOGGLE
        { SDLK_4,           KMOD_NONE },                        // SHORTCUT_SEE_THROUGH_SCENERY_TOGGLE
        { SDLK_5,           KMOD_NONE },                        // SHORTCUT_INVISIBLE_SUPPORTS_TOGGLE
        { SDLK_6,           KMOD_NONE },                        // SHORTCUT_INVISIBLE_PEOPLE_TOGGLE
        { SDLK_8,           KMOD_NONE },                        // SHORTCUT_HEIGHT_MARKS_ON_LAND_TOGGLE
        { SDLK_9,           KMOD_NONE },                        // SHORTCUT_HEIGHT_MARKS_ON_RIDE_TRACKS_TOGGLE
        { SDLK_0,           KMOD_NONE },                        // SHORTCUT_HEIGHT_MARKS_ON_PATHS_TOGGLE
        { SDLK_F1,          KMOD_NONE },                        // SHORTCUT_ADJUST_LAND
        { SDLK_F2,          KMOD_NONE },                        // SHORTCUT_ADJUST_WATER
        { SDLK_F3,          KMOD_NONE },                        // SHORTCUT_BUILD_SCENERY
        { SDLK_F4,          KMOD_NONE },                        // SHORTCUT_BUILD_PATHS
        { SDLK_F5,          KMOD_NONE },                        // SHORTCUT_BUILD_NEW_RIDE
        { SDLK_f,           KMOD_NONE },                        // SHORTCUT_SHOW_FINANCIAL_INFORMATION
        { SDLK_d,           KMOD_NONE },                        // SHORTCUT_SHOW_RESEARCH_INFORMATION
        { SDLK_r,           KMOD_NONE },                        // SHORTCUT_SHOW_RIDES_LIST
        { SDLK_p,           KMOD_NONE },                        // SHORTCUT_SHOW_PARK_INFORMATION
        { SDLK_g,           KMOD_NONE },                        // SHORTCUT_SHOW_GUEST_LIST
        { SDLK_s,           KMOD_NONE },                        // SHORTCUT_SHOW_STAFF_LIST
        { SDLK_m,           KMOD_NONE },                        // SHORTCUT_SHOW_RECENT_MESSAGES
        { SDLK_TAB,         KMOD_NONE },                        // SHORTCUT_SHOW_MAP
        { SDLK_s,           PLATFORM_MODIFIER },                // SHORTCUT_SCREENSHOT
        { SDLK_MINUS,       KMOD_NONE },                        // SHORTCUT_REDUCE_GAME_SPEED,
        { SDLK_EQUALS,      KMOD_NONE },                        // SHORTCUT_INCREASE_GAME_SPEED,
        { SDLK_c,           PLATFORM_MODIFIER | KMOD_ALT },     // SHORTCUT_OPEN_CHEAT_WINDOW,
        { SDLK_t,           KMOD_NONE },                        // SHORTCUT_REMOVE_TOP_BOTTOM_TOOLBAR_TOGGLE,
        { SDLK_UP,          KMOD_NONE },                        // SHORTCUT_SCROLL_MAP_UP
        { SDLK_LEFT,        KMOD_NONE },                        // SHORTCUT_SCROLL_MAP_LEFT
        { SDLK_DOWN,        KMOD_NONE },                        // SHORTCUT_SCROLL_MAP_DOWN
        { SDLK_RIGHT,       KMOD_NONE },                        // SHORTCUT_SCROLL_MAP_RIGHT
        { SDLK_c,           KMOD_NONE },                        // SHORTCUT_OPEN_CHAT_WINDOW
        { SDLK_F10,         PLATFORM_MODIFIER },                // SHORTCUT_QUICK_SAVE_GAME
        SHORTCUT_UNDEFINED,                                     // SHORTCUT_SHOW_OPTIONS
        SHORTCUT_UNDEFINED,                                     // SHORTCUT_MUTE_SOUND
        { SDLK_RETURN,      KMOD_ALT },                         // SHORTCUT_WINDOWED_MODE_TOGGLE
        SHORTCUT_UNDEFINED,                                     // SHORTCUT_SHOW_MULTIPLAYER
        SHORTCUT_UNDEFINED,                                     // SHORTCUT_PAINT_ORIGINAL_TOGGLE
        SHORTCUT_UNDEFINED,                                     // SHORTCUT_DEBUG_PAINT_TOGGLE
        SHORTCUT_UNDEFINED,                                     // SHORTCUT_SEE_THROUGH_PATHS_TOGGLE
        { SDLK_KP_4,        KMOD_NONE },                        // SHORTCUT_RIDE_CONSTRUCTION_TURN_LEFT
        { SDLK_KP_6,        KMOD_NONE },                        // SHORTCUT_RIDE_CONSTRUCTION_TURN_RIGHT
        { SDLK_KP_5,        KMOD_NONE },                        // SHORTCUT_RIDE_CONSTRUCTION_USE_TRACK_DEFAULT
        { SDLK_KP_2,        KMOD_NONE },                        // SHORTCUT_RIDE_CONSTRUCTION_SLOPE_DOWN
        { SDLK_KP_8,        KMOD_NONE },                        // SHORTCUT_RIDE_CONSTRUCTION_SLOPE_UP
        { SDLK_KP_PLUS,     KMOD_NONE },                        // SHORTCUT_RIDE_CONSTRUCTION_CHAIN_LIFT_TOGGLE
        { SDLK_KP_1,        KMOD_NONE },                        // SHORTCUT_RIDE_CONSTRUCTION_BANK_LEFT
        { SDLK_KP_3,        KMOD_NONE },                        // SHORTCUT_RIDE_CONSTRUCTION_BANK_RIGHT
        { SDLK_KP_7,        KMOD_NONE },                        // SHORTCUT_RIDE_CONSTRUCTION_PREVIOUS_TRACK
        { SDLK_KP_9,        KMOD_NONE },                        // SHORTCUT_RIDE_CONSTRUCTION_NEXT_TRACK
        { SDLK_KP_0,        KMOD_NONE },                        // SHORTCUT_RIDE_CONSTRUCTION_BUILD_CURRENT
        { SDLK_KP_MINUS,    KMOD_NONE },                        // SHORTCUT_RIDE_CONSTRUCTION_DEMOLISH_CURRENT
    };

    constexpr uint16 CURRENT_FILE_VERSION = 2;

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
            auto fs = FileStream(path, FILE_MODE_OPEN);

            uint16 version = fs.ReadValue<uint16>();
            if (version == KeyboardShortcuts::CURRENT_FILE_VERSION)
            {
                for (sint32 i = 0; i < SHORTCUT_COUNT; i++)
                {
                    gShortcutKeys[i] = fs.ReadValue<keypress>();
                }
                result = true;
            }
            else
            {
                result = false;
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
                fs.WriteValue<keypress>(gShortcutKeys[i]);
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
