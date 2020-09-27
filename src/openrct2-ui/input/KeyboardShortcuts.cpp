/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "KeyboardShortcuts.h"

#include <SDL.h>
#include <algorithm>
#include <map>
#include <openrct2/PlatformEnvironment.h>
#include <openrct2/common.h>
#include <openrct2/core/Console.hpp>
#include <openrct2/core/File.h>
#include <openrct2/core/FileStream.hpp>
#include <openrct2/core/Path.hpp>
#include <openrct2/core/String.hpp>
#include <openrct2/localisation/Localisation.h>
#include <openrct2/world/Location.hpp>

using namespace OpenRCT2;
using namespace OpenRCT2::Input;

// Remove when the C calls are removed
static KeyboardShortcuts* _instance;

static const std::map<const SDL_Scancode, const rct_string_id> specialCharNames = {
    { SDL_SCANCODE_BACKSPACE, STR_SHORTCUT_BACKSPACE },
    { SDL_SCANCODE_TAB, STR_SHORTCUT_TAB },
    { SDL_SCANCODE_CLEAR, STR_SHORTCUT_CLEAR },
    { SDL_SCANCODE_RETURN, STR_SHORTCUT_RETURN },
    { SDL_SCANCODE_LALT, STR_SHORTCUT_ALT },
    { SDL_SCANCODE_PAUSE, STR_SHORTCUT_PAUSE },
    { SDL_SCANCODE_CAPSLOCK, STR_SHORTCUT_CAPS },
    { SDL_SCANCODE_ESCAPE, STR_SHORTCUT_ESCAPE },
    { SDL_SCANCODE_SPACE, STR_SHORTCUT_SPACEBAR },
    { SDL_SCANCODE_PAGEUP, STR_SHORTCUT_PGUP },
    { SDL_SCANCODE_PAGEDOWN, STR_SHORTCUT_PGDN },
    { SDL_SCANCODE_END, STR_SHORTCUT_END },
    { SDL_SCANCODE_HOME, STR_SHORTCUT_HOME },
    { SDL_SCANCODE_LEFT, STR_SHORTCUT_LEFT },
    { SDL_SCANCODE_UP, STR_SHORTCUT_UP },
    { SDL_SCANCODE_RIGHT, STR_SHORTCUT_RIGHT },
    { SDL_SCANCODE_DOWN, STR_SHORTCUT_DOWN },
    { SDL_SCANCODE_SELECT, STR_SHORTCUT_SELECT },
    { SDL_SCANCODE_PRINTSCREEN, STR_SHORTCUT_PRINT },
    { SDL_SCANCODE_EXECUTE, STR_SHORTCUT_EXECUTE },
    { SDL_SCANCODE_SYSREQ, STR_SHORTCUT_SNAPSHOT },
    { SDL_SCANCODE_INSERT, STR_SHORTCUT_INSERT },
    { SDL_SCANCODE_DELETE, STR_SHORTCUT_DELETE },
    { SDL_SCANCODE_HELP, STR_SHORTCUT_HELP },
    { SDL_SCANCODE_APPLICATION, STR_SHORTCUT_MENU },
    { SDL_SCANCODE_KP_0, STR_SHORTCUT_NUMPAD_0 },
    { SDL_SCANCODE_KP_1, STR_SHORTCUT_NUMPAD_1 },
    { SDL_SCANCODE_KP_2, STR_SHORTCUT_NUMPAD_2 },
    { SDL_SCANCODE_KP_3, STR_SHORTCUT_NUMPAD_3 },
    { SDL_SCANCODE_KP_4, STR_SHORTCUT_NUMPAD_4 },
    { SDL_SCANCODE_KP_5, STR_SHORTCUT_NUMPAD_5 },
    { SDL_SCANCODE_KP_6, STR_SHORTCUT_NUMPAD_6 },
    { SDL_SCANCODE_KP_7, STR_SHORTCUT_NUMPAD_7 },
    { SDL_SCANCODE_KP_8, STR_SHORTCUT_NUMPAD_8 },
    { SDL_SCANCODE_KP_9, STR_SHORTCUT_NUMPAD_9 },
    { SDL_SCANCODE_KP_MULTIPLY, STR_SHORTCUT_NUMPAD_MULTIPLY },
    { SDL_SCANCODE_KP_PLUS, STR_SHORTCUT_NUMPAD_PLUS },
    { SDL_SCANCODE_KP_MINUS, STR_SHORTCUT_NUMPAD_MINUS },
    { SDL_SCANCODE_KP_PERIOD, STR_SHORTCUT_NUMPAD_PERIOD },
    { SDL_SCANCODE_KP_DIVIDE, STR_SHORTCUT_NUMPAD_DIVIDE },
    { SDL_SCANCODE_NUMLOCKCLEAR, STR_SHORTCUT_NUMLOCK },
    { SDL_SCANCODE_SCROLLLOCK, STR_SHORTCUT_SCROLL },
};

KeyboardShortcuts::KeyboardShortcuts(const std::shared_ptr<IPlatformEnvironment>& env)
    : _env(env)
{
    _instance = this;
}

KeyboardShortcuts::~KeyboardShortcuts()
{
    _instance = nullptr;
}

void KeyboardShortcuts::Reset()
{
    for (size_t i = 0; i < ShortcutsCount; i++)
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
            uint16_t version = fs.ReadValue<uint16_t>();
            if (version == KeyboardShortcuts::CURRENT_FILE_VERSION)
            {
                int32_t numShortcutsInFile = (fs.GetLength() - sizeof(uint16_t)) / sizeof(uint16_t);
                auto numShortcutsToRead = std::min<size_t>(ShortcutsCount, numShortcutsInFile);
                for (size_t i = 0; i < numShortcutsToRead; i++)
                {
                    _keys[i] = fs.ReadValue<uint16_t>();
                }
                result = true;
            }
        }
    }
    catch (const std::exception& ex)
    {
        Console::WriteLine("Error reading shortcut keys: %s", ex.what());
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
        fs.WriteValue<uint16_t>(KeyboardShortcuts::CURRENT_FILE_VERSION);
        for (size_t i = 0; i < ShortcutsCount; i++)
        {
            fs.WriteValue<uint16_t>(_keys[i]);
        }
        result = true;
    }
    catch (const std::exception& ex)
    {
        Console::WriteLine("Error writing shortcut keys: %s", ex.what());
    }
    return result;
}

void KeyboardShortcuts::Set(int32_t key)
{
    // Unmap shortcut that already uses this key
    auto shortcut = GetFromKey(key);
    if (shortcut != Input::Shortcut::Undefined)
    {
        _keys[static_cast<size_t>(shortcut)] = static_cast<uint16_t>(Input::ScanCodeUndefined);
    }

    // Map shortcut to this key
    _keys[static_cast<size_t>(gKeyboardShortcutChangeId)] = key;
    Save();
}

Shortcut KeyboardShortcuts::GetFromKey(int32_t key)
{
    for (size_t i = 0; i < ShortcutsCount; i++)
    {
        if (key == _keys[i])
        {
            return static_cast<Shortcut>(i);
        }
    }
    return Input::Shortcut::Undefined;
}

std::string KeyboardShortcuts::GetShortcutString(int32_t shortcut) const
{
    utf8 buffer[256] = { 0 };
    utf8 formatBuffer[256] = { 0 };
    uint16_t shortcutKey = _keys[shortcut];
    if (shortcutKey == Input::ScanCodeUndefined)
        return std::string();
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

    SDL_Scancode scanCode = static_cast<SDL_Scancode>(shortcutKey & 0xFF);
    auto keyPair = specialCharNames.find(scanCode);
    if (keyPair != specialCharNames.end())
    {
        format_string(formatBuffer, sizeof(formatBuffer), keyPair->second, nullptr);
        String::Append(buffer, sizeof(buffer), formatBuffer);
    }
    else
    {
        String::Append(buffer, sizeof(buffer), SDL_GetKeyName(SDL_GetKeyFromScancode(scanCode)));
    }

    return std::string(buffer);
}

ScreenCoordsXY KeyboardShortcuts::GetKeyboardMapScroll(const uint8_t* keysState) const
{
    ScreenCoordsXY screenCoords;
    for (int32_t shortcutId = static_cast<int32_t>(Input::Shortcut::ScrollMapUp);
         shortcutId <= static_cast<int32_t>(Input::Shortcut::ScrollMapRight); shortcutId++)
    {
        uint16_t shortcutKey = _keys[shortcutId];
        uint8_t scancode = shortcutKey & 0xFF;

        if (shortcutKey == Input::ScanCodeUndefined)
            continue;
        if (!keysState[scancode])
            continue;

        // Check if SHIFT is either set in the shortcut key and currently pressed,
        // or not set in the shortcut key and not currently pressed (in other words: check if they match).
        if (static_cast<bool>(shortcutKey & SHIFT) != (keysState[SDL_SCANCODE_LSHIFT] || keysState[SDL_SCANCODE_RSHIFT]))
            continue;
        if (static_cast<bool>(shortcutKey & CTRL) != (keysState[SDL_SCANCODE_LCTRL] || keysState[SDL_SCANCODE_RCTRL]))
            continue;
        if (static_cast<bool>(shortcutKey & ALT) != (keysState[SDL_SCANCODE_LALT] || keysState[SDL_SCANCODE_RALT]))
            continue;
#ifdef __MACOSX__
        if (static_cast<bool>(shortcutKey & CMD) != (keysState[SDL_SCANCODE_LGUI] || keysState[SDL_SCANCODE_RGUI]))
            continue;
#endif
        auto convertedShortcut = static_cast<Input::Shortcut>(shortcutId);
        switch (convertedShortcut)
        {
            case Input::Shortcut::ScrollMapUp:
                screenCoords.y = -1;
                break;
            case Input::Shortcut::ScrollMapLeft:
                screenCoords.x = -1;
                break;
            case Input::Shortcut::ScrollMapDown:
                screenCoords.y = 1;
                break;
            case Input::Shortcut::ScrollMapRight:
                screenCoords.x = 1;
                break;
            default:
                break;
        }
    }
    return screenCoords;
}

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

void keyboard_shortcuts_set(int32_t key)
{
    return _instance->Set(key);
}

Shortcut keyboard_shortcuts_get_from_key(int32_t key)
{
    return _instance->GetFromKey(key);
}

void keyboard_shortcuts_format_string(char* buffer, size_t bufferSize, int32_t shortcut)
{
    auto str = _instance->GetShortcutString(shortcut);
    String::Set(buffer, bufferSize, str.c_str());
}

ScreenCoordsXY get_keyboard_map_scroll(const uint8_t* keysState)
{
    return _instance->GetKeyboardMapScroll(keysState);
}

// Default keyboard shortcuts
const uint16_t KeyboardShortcuts::DefaultKeys[Input::ShortcutsCount] = {
    SDL_SCANCODE_BACKSPACE,                   // SHORTCUT_CLOSE_TOP_MOST_WINDOW
    SHIFT | SDL_SCANCODE_BACKSPACE,           // SHORTCUT_CLOSE_ALL_FLOATING_WINDOWS
    SDL_SCANCODE_ESCAPE,                      // SHORTCUT_CANCEL_CONSTRUCTION_MODE
    SDL_SCANCODE_PAUSE,                       // SHORTCUT_PAUSE_GAME
    SDL_SCANCODE_PAGEUP,                      // SHORTCUT_ZOOM_VIEW_OUT
    SDL_SCANCODE_PAGEDOWN,                    // SHORTCUT_ZOOM_VIEW_IN
    SDL_SCANCODE_RETURN,                      // SHORTCUT_ROTATE_VIEW_CLOCKWISE
    SHIFT | SDL_SCANCODE_RETURN,              // SHORTCUT_ROTATE_VIEW_ANTICLOCKWISE
    SDL_SCANCODE_Z,                           // SHORTCUT_ROTATE_CONSTRUCTION_OBJECT
    SDL_SCANCODE_1,                           // SHORTCUT_UNDERGROUND_VIEW_TOGGLE
    SDL_SCANCODE_H,                           // SHORTCUT_REMOVE_BASE_LAND_TOGGLE
    SDL_SCANCODE_V,                           // SHORTCUT_REMOVE_VERTICAL_LAND_TOGGLE
    SDL_SCANCODE_3,                           // SHORTCUT_SEE_THROUGH_RIDES_TOGGLE
    SDL_SCANCODE_4,                           // SHORTCUT_SEE_THROUGH_SCENERY_TOGGLE
    SDL_SCANCODE_5,                           // SHORTCUT_INVISIBLE_SUPPORTS_TOGGLE
    SDL_SCANCODE_6,                           // SHORTCUT_INVISIBLE_PEOPLE_TOGGLE
    SDL_SCANCODE_8,                           // SHORTCUT_HEIGHT_MARKS_ON_LAND_TOGGLE
    SDL_SCANCODE_9,                           // SHORTCUT_HEIGHT_MARKS_ON_RIDE_TRACKS_TOGGLE
    SDL_SCANCODE_0,                           // SHORTCUT_HEIGHT_MARKS_ON_PATHS_TOGGLE
    SDL_SCANCODE_F1,                          // SHORTCUT_ADJUST_LAND
    SDL_SCANCODE_F2,                          // SHORTCUT_ADJUST_WATER
    SDL_SCANCODE_F3,                          // SHORTCUT_BUILD_SCENERY
    SDL_SCANCODE_F4,                          // SHORTCUT_BUILD_PATHS
    SDL_SCANCODE_F5,                          // SHORTCUT_BUILD_NEW_RIDE
    SDL_SCANCODE_F,                           // SHORTCUT_SHOW_FINANCIAL_INFORMATION
    SDL_SCANCODE_D,                           // SHORTCUT_SHOW_RESEARCH_INFORMATION
    SDL_SCANCODE_R,                           // SHORTCUT_SHOW_RIDES_LIST
    SDL_SCANCODE_P,                           // SHORTCUT_SHOW_PARK_INFORMATION
    SDL_SCANCODE_G,                           // SHORTCUT_SHOW_GUEST_LIST
    SDL_SCANCODE_S,                           // SHORTCUT_SHOW_STAFF_LIST
    SDL_SCANCODE_M,                           // SHORTCUT_SHOW_RECENT_MESSAGES
    SDL_SCANCODE_TAB,                         // SHORTCUT_SHOW_MAP
    PLATFORM_MODIFIER | SDL_SCANCODE_S,       // SHORTCUT_SCREENSHOT
    SDL_SCANCODE_MINUS,                       // SHORTCUT_REDUCE_GAME_SPEED,
    SDL_SCANCODE_EQUALS,                      // SHORTCUT_INCREASE_GAME_SPEED,
    PLATFORM_MODIFIER | ALT | SDL_SCANCODE_C, // SHORTCUT_OPEN_CHEAT_WINDOW,
    SDL_SCANCODE_T,                           // SHORTCUT_REMOVE_TOP_BOTTOM_TOOLBAR_TOGGLE,
    SDL_SCANCODE_UP,                          // SHORTCUT_SCROLL_MAP_UP
    SDL_SCANCODE_LEFT,                        // SHORTCUT_SCROLL_MAP_LEFT
    SDL_SCANCODE_DOWN,                        // SHORTCUT_SCROLL_MAP_DOWN
    SDL_SCANCODE_RIGHT,                       // SHORTCUT_SCROLL_MAP_RIGHT
    SDL_SCANCODE_C,                           // SHORTCUT_OPEN_CHAT_WINDOW
    PLATFORM_MODIFIER | SDL_SCANCODE_F10,     // SHORTCUT_QUICK_SAVE_GAME
    Input::ScanCodeUndefined,                 // SHORTCUT_SHOW_OPTIONS
    Input::ScanCodeUndefined,                 // SHORTCUT_MUTE_SOUND
    ALT | SDL_SCANCODE_RETURN,                // SHORTCUT_WINDOWED_MODE_TOGGLE
    Input::ScanCodeUndefined,                 // SHORTCUT_SHOW_MULTIPLAYER
    Input::ScanCodeUndefined,                 // SHORTCUT_PAINT_ORIGINAL_TOGGLE
    Input::ScanCodeUndefined,                 // SHORTCUT_DEBUG_PAINT_TOGGLE
    Input::ScanCodeUndefined,                 // SHORTCUT_SEE_THROUGH_PATHS_TOGGLE
    SDL_SCANCODE_KP_4,                        // SHORTCUT_RIDE_CONSTRUCTION_TURN_LEFT
    SDL_SCANCODE_KP_6,                        // SHORTCUT_RIDE_CONSTRUCTION_TURN_RIGHT
    SDL_SCANCODE_KP_5,                        // SHORTCUT_RIDE_CONSTRUCTION_USE_TRACK_DEFAULT
    SDL_SCANCODE_KP_2,                        // SHORTCUT_RIDE_CONSTRUCTION_SLOPE_DOWN
    SDL_SCANCODE_KP_8,                        // SHORTCUT_RIDE_CONSTRUCTION_SLOPE_UP
    SDL_SCANCODE_KP_PLUS,                     // SHORTCUT_RIDE_CONSTRUCTION_CHAIN_LIFT_TOGGLE
    SDL_SCANCODE_KP_1,                        // SHORTCUT_RIDE_CONSTRUCTION_BANK_LEFT
    SDL_SCANCODE_KP_3,                        // SHORTCUT_RIDE_CONSTRUCTION_BANK_RIGHT
    SDL_SCANCODE_KP_7,                        // SHORTCUT_RIDE_CONSTRUCTION_PREVIOUS_TRACK
    SDL_SCANCODE_KP_9,                        // SHORTCUT_RIDE_CONSTRUCTION_NEXT_TRACK
    SDL_SCANCODE_KP_0,                        // SHORTCUT_RIDE_CONSTRUCTION_BUILD_CURRENT
    SDL_SCANCODE_KP_MINUS,                    // SHORTCUT_RIDE_CONSTRUCTION_DEMOLISH_CURRENT
    PLATFORM_MODIFIER | SDL_SCANCODE_L,       // SHORTCUT_LOAD_GAME
    SDL_SCANCODE_B,                           // SHORTCUT_CLEAR_SCENERY
    SDL_SCANCODE_7,                           // SHORTCUT_GRIDLINES_DISPLAY_TOGGLE
    Input::ScanCodeUndefined,                 // SHORTCUT_VIEW_CLIPPING
    SDL_SCANCODE_I,                           // SHORTCUT_HIGHLIGHT_PATH_ISSUES_TOGGLE
    Input::ScanCodeUndefined,                 // SHORTCUT_TILE_INSPECTOR
    Input::ScanCodeUndefined,                 // SHORTCUT_ADVANCE_TO_NEXT_TICK
    Input::ScanCodeUndefined,                 // SHORTCUT_SCENERY_PICKER
    Input::ScanCodeUndefined,                 // SHORTCUT_SCALE_UP
    Input::ScanCodeUndefined,                 // SHORTCUT_SCALE_DOWN
    Input::ScanCodeUndefined,                 // SHORTCUT_TOGGLE_CLEARANCE_CHECKS
};
