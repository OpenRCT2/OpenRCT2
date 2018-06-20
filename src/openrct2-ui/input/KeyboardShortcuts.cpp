/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include <openrct2/common.h>
#include <SDL2/SDL.h>
#include <openrct2/core/Console.hpp>
#include <openrct2/core/File.h>
#include <openrct2/core/FileStream.hpp>
#include <openrct2/core/Path.hpp>
#include <openrct2/core/String.hpp>
#include <openrct2/PlatformEnvironment.h>
#include "KeyboardShortcuts.h"

#include <openrct2/localisation/Localisation.h>

using namespace OpenRCT2;
using namespace OpenRCT2::Input;

// Remove when the C calls are removed
static KeyboardShortcuts * _instance;

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
            uint16_t version = fs.ReadValue<uint16_t>();
            if (version == KeyboardShortcuts::CURRENT_FILE_VERSION)
            {
                int32_t numShortcutsInFile = (fs.GetLength() - sizeof(uint16_t)) / sizeof(uint16_t);
                int32_t numShortcutsToRead = std::min<int32_t>(SHORTCUT_COUNT, numShortcutsInFile);
                for (int32_t i = 0; i < numShortcutsToRead; i++)
                {
                    _keys[i] = fs.ReadValue<uint16_t>();
                }
                result = true;
            }
        }
    }
    catch (const std::exception &ex)
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
        for (int32_t i = 0; i < SHORTCUT_COUNT; i++)
        {
            fs.WriteValue<uint16_t>(_keys[i]);
        }
        result = true;
    }
    catch (const std::exception &ex)
    {
        Console::WriteLine("Error writing shortcut keys: %s", ex.what());
    }
    return result;
}

void KeyboardShortcuts::Set(int32_t key)
{
    // Unmap shortcut that already uses this key
    int32_t shortcut = GetFromKey(key);
    if (shortcut != SHORTCUT_UNDEFINED)
    {
        _keys[shortcut] = SHORTCUT_UNDEFINED;
    }

    // Map shortcut to this key
    _keys[gKeyboardShortcutChangeId] = key;
    Save();
}

int32_t KeyboardShortcuts::GetFromKey(int32_t key)
{
    for (int32_t i = 0; i < SHORTCUT_COUNT; i++)
    {
        if (key == _keys[i])
        {
            return i;
        }
    }
    return SHORTCUT_UNDEFINED;
}

std::string KeyboardShortcuts::GetShortcutString(int32_t shortcut) const
{
    utf8 buffer[256] = { 0 };
    utf8 formatBuffer[256] = { 0 };
    uint16_t shortcutKey = _keys[shortcut];
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

void KeyboardShortcuts::GetKeyboardMapScroll(const uint8_t * keysState, int32_t * x, int32_t * y) const
{
    for (int32_t shortcutId = SHORTCUT_SCROLL_MAP_UP; shortcutId <= SHORTCUT_SCROLL_MAP_RIGHT; shortcutId++)
    {
        uint16_t shortcutKey = _keys[shortcutId];
        uint8_t scancode = shortcutKey & 0xFF;

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

int32_t keyboard_shortcuts_get_from_key(int32_t key)
{
    return _instance->GetFromKey(key);
}

void keyboard_shortcuts_format_string(char * buffer, size_t bufferSize, int32_t shortcut)
{
    auto str = _instance->GetShortcutString(shortcut);
    String::Set(buffer, bufferSize, str.c_str());
}

void get_keyboard_map_scroll(const uint8_t * keysState, int32_t * x, int32_t * y)
{
    _instance->GetKeyboardMapScroll(keysState, x, y);
}


// Default keyboard shortcuts
const uint16_t KeyboardShortcuts::DefaultKeys[SHORTCUT_COUNT] =
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
    SDL_SCANCODE_7,                             // SHORTCUT_GRIDLINES_DISPLAY_TOGGLE
    SHORTCUT_UNDEFINED,                         // SHORTCUT_VIEW_CLIPPING
    SDL_SCANCODE_I,                             // SHORTCUT_HIGHLIGHT_PATH_ISSUES_TOGGLE
};
