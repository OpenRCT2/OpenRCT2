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

#include <openrct2/common.h>
#include <SDL.h>
#include <openrct2/core/Console.hpp>
#include <openrct2/core/File.h>
#include <openrct2/core/FileStream.hpp>
#include <openrct2/core/Memory.hpp>
#include <openrct2/core/Path.hpp>
#include <openrct2/core/String.hpp>
#include <openrct2/PlatformEnvironment.h>
#include "KeyboardShortcuts.h"

#include <openrct2/localisation/localisation.h>

using namespace OpenRCT2;
using namespace OpenRCT2::Input;

// Remove when the C calls are removed
static KeyboardShortcuts * _instance;

KeyboardShortcuts::KeyboardShortcuts(IPlatformEnvironment * env)
    : _env(env)
{
    _instance = this;
}

void KeyboardShortcuts::Reset()
{
    for (size_t i = 0; i < SHORTCUT_COUNT; i++)
    {
        _keys[i] = DefaultKeys[i];
    }
}

bool KeyboardShortcuts::Load()
{
    bool result = false;
    Reset();
    try
    {
        std::string path = _env->GetFilePath(PATHID::CONFIG_KEYBOARD);
        if (File::Exists(path))
        {
            auto fs = FileStream(path, FILE_MODE_OPEN);
            uint16 version = fs.ReadValue<uint16>();
            if (version == KeyboardShortcuts::CURRENT_FILE_VERSION)
            {
                sint32 numShortcutsInFile = (fs.GetLength() - sizeof(uint16)) / sizeof(uint16);
                sint32 numShortcutsToRead = std::min<sint32>(SHORTCUT_COUNT, numShortcutsInFile);
                for (sint32 i = 0; i < numShortcutsToRead; i++)
                {
                    _keys[i] = fs.ReadValue<uint16>();
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

bool KeyboardShortcuts::Save()
{
    bool result = false;
    try
    {
        std::string path = _env->GetFilePath(PATHID::CONFIG_KEYBOARD);
        auto fs = FileStream(path, FILE_MODE_WRITE);
        fs.WriteValue<uint16>(KeyboardShortcuts::CURRENT_FILE_VERSION);
        for (sint32 i = 0; i < SHORTCUT_COUNT; i++)
        {
            fs.WriteValue<uint16>(_keys[i]);
        }
        result = true;
    }
    catch (const Exception &ex)
    {
        Console::WriteLine("Error writing shortcut keys: %s", ex.GetMessage());
    }
    return result;
}

void KeyboardShortcuts::Set(sint32 key)
{
    // Unmap shortcut that already uses this key
    sint32 shortcut = GetFromKey(key);
    if (shortcut != SHORTCUT_UNDEFINED)
    {
        _keys[shortcut] = SHORTCUT_UNDEFINED;
    }

    // Map shortcut to this key
    _keys[gKeyboardShortcutChangeId] = key;
    Save();
}

sint32 KeyboardShortcuts::GetFromKey(sint32 key)
{
    for (sint32 i = 0; i < SHORTCUT_COUNT; i++)
    {
        if (key == _keys[i])
        {
            return i;
        }
    }
    return SHORTCUT_UNDEFINED;
}

std::string KeyboardShortcuts::GetShortcutString(sint32 shortcut) const
{
    utf8 buffer[256] = { 0 };
    utf8 formatBuffer[256] = { 0 };
    uint16 shortcutKey = _keys[shortcut];
    if (shortcutKey == SHORTCUT_UNDEFINED) return std::string();
    if (shortcutKey & SHIFT)
    {
        format_string(formatBuffer, sizeof(formatBuffer), STR_SHIFT_PLUS, nullptr);
        String::Append(buffer, sizeof(buffer), formatBuffer);
    }
    if (shortcutKey & CTRL)
    {
        format_string(formatBuffer, sizeof(formatBuffer), STR_CTRL_PLUS, nullptr);
        String::Append(buffer, sizeof(buffer), formatBuffer);
    }
    if (shortcutKey & ALT)
    {
#ifdef __MACOSX__
        format_string(formatBuffer, sizeof(formatBuffer), STR_OPTION_PLUS, nullptr);
#else
        format_string(formatBuffer, sizeof(formatBuffer), STR_ALT_PLUS, nullptr);
#endif
        String::Append(buffer, sizeof(buffer), formatBuffer);
    }
    if (shortcutKey & CMD)
    {
        format_string(formatBuffer, sizeof(formatBuffer), STR_CMD_PLUS, nullptr);
        String::Append(buffer, sizeof(buffer), formatBuffer);
    }
    String::Append(buffer, sizeof(buffer), SDL_GetScancodeName((SDL_Scancode)(shortcutKey & 0xFF)));
    return std::string(buffer);
}

void KeyboardShortcuts::GetKeyboardMapScroll(const uint8 * keysState, sint32 * x, sint32 * y) const
{
    for (sint32 shortcutId = SHORTCUT_SCROLL_MAP_UP; shortcutId <= SHORTCUT_SCROLL_MAP_RIGHT; shortcutId++)
    {
        uint16 shortcutKey = _keys[shortcutId];
        uint8 scancode = shortcutKey & 0xFF;

        if (shortcutKey == 0xFFFF) continue;
        if (!keysState[scancode]) continue;

        if (shortcutKey & SHIFT) {
            if (!keysState[SDL_SCANCODE_LSHIFT] && !keysState[SDL_SCANCODE_RSHIFT]) continue;
        }
        if (shortcutKey & CTRL) {
            if (!keysState[SDL_SCANCODE_LCTRL] && !keysState[SDL_SCANCODE_RCTRL]) continue;
        }
        if (shortcutKey & ALT) {
            if (!keysState[SDL_SCANCODE_LALT] && !keysState[SDL_SCANCODE_RALT]) continue;
        }
#ifdef __MACOSX__
        if (shortcutKey & CMD) {
            if (!keysState[SDL_SCANCODE_LGUI] && !keysState[SDL_SCANCODE_RGUI]) continue;
        }
#endif
        switch (shortcutId) {
        case SHORTCUT_SCROLL_MAP_UP:
            *y = -1;
            break;
        case SHORTCUT_SCROLL_MAP_LEFT:
            *x = -1;
            break;
        case SHORTCUT_SCROLL_MAP_DOWN:
            *y = 1;
            break;
        case SHORTCUT_SCROLL_MAP_RIGHT:
            *x = 1;
            break;
        default:
            break;
        }
    }
}

extern "C"
{
    void keyboard_shortcuts_reset()
    {
        _instance->Reset();
    }

    bool keyboard_shortcuts_load()
    {
        return _instance->Load();
    }

    bool keyboard_shortcuts_save()
    {
        return _instance->Save();
    }

    void keyboard_shortcuts_set(sint32 key)
    {
        return _instance->Set(key);
    }

    sint32 keyboard_shortcuts_get_from_key(sint32 key)
    {
        return _instance->GetFromKey(key);
    }

    void keyboard_shortcuts_format_string(char * buffer, size_t bufferSize, sint32 shortcut)
    {
        auto str = _instance->GetShortcutString(shortcut);
        String::Set(buffer, bufferSize, str.c_str());
    }

    void get_keyboard_map_scroll(const uint8 * keysState, sint32 * x, sint32 * y)
    {
        _instance->GetKeyboardMapScroll(keysState, x, y);
    }
}

// Default keyboard shortcuts
const uint16 KeyboardShortcuts::DefaultKeys[SHORTCUT_COUNT] =
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
    PLATFORM_MODIFIER | SDL_SCANCODE_L,         // SHORTCUT_LOAD_GAME
    SDL_SCANCODE_B,                             // SHORTCUT_CLEAR_SCENERY
};
