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

#pragma once

#include <openrct2/common.h>

#define SHIFT   0x100
#define CTRL    0x200
#define ALT     0x400
#define CMD     0x800
#ifdef __MACOSX__
    #define PLATFORM_MODIFIER CMD
#else
    #define PLATFORM_MODIFIER CTRL
#endif

enum
{
    SHORTCUT_CLOSE_TOP_MOST_WINDOW,
    SHORTCUT_CLOSE_ALL_FLOATING_WINDOWS,
    SHORTCUT_CANCEL_CONSTRUCTION_MODE,
    SHORTCUT_PAUSE_GAME,
    SHORTCUT_ZOOM_VIEW_OUT,
    SHORTCUT_ZOOM_VIEW_IN,
    SHORTCUT_ROTATE_VIEW_CLOCKWISE,
    SHORTCUT_ROTATE_VIEW_ANTICLOCKWISE,
    SHORTCUT_ROTATE_CONSTRUCTION_OBJECT,
    SHORTCUT_UNDERGROUND_VIEW_TOGGLE,
    SHORTCUT_REMOVE_BASE_LAND_TOGGLE,
    SHORTCUT_REMOVE_VERTICAL_LAND_TOGGLE,
    SHORTCUT_SEE_THROUGH_RIDES_TOGGLE,
    SHORTCUT_SEE_THROUGH_SCENERY_TOGGLE,
    SHORTCUT_INVISIBLE_SUPPORTS_TOGGLE,
    SHORTCUT_INVISIBLE_PEOPLE_TOGGLE,
    SHORTCUT_HEIGHT_MARKS_ON_LAND_TOGGLE,
    SHORTCUT_HEIGHT_MARKS_ON_RIDE_TRACKS_TOGGLE,
    SHORTCUT_HEIGHT_MARKS_ON_PATHS_TOGGLE,
    SHORTCUT_ADJUST_LAND,
    SHORTCUT_ADJUST_WATER,
    SHORTCUT_BUILD_SCENERY,
    SHORTCUT_BUILD_PATHS,
    SHORTCUT_BUILD_NEW_RIDE,
    SHORTCUT_SHOW_FINANCIAL_INFORMATION,
    SHORTCUT_SHOW_RESEARCH_INFORMATION,
    SHORTCUT_SHOW_RIDES_LIST,
    SHORTCUT_SHOW_PARK_INFORMATION,
    SHORTCUT_SHOW_GUEST_LIST,
    SHORTCUT_SHOW_STAFF_LIST,
    SHORTCUT_SHOW_RECENT_MESSAGES,
    SHORTCUT_SHOW_MAP,
    SHORTCUT_SCREENSHOT,

    // New
    SHORTCUT_REDUCE_GAME_SPEED,
    SHORTCUT_INCREASE_GAME_SPEED,
    SHORTCUT_OPEN_CHEAT_WINDOW,
    SHORTCUT_REMOVE_TOP_BOTTOM_TOOLBAR_TOGGLE,
    SHORTCUT_SCROLL_MAP_UP,
    SHORTCUT_SCROLL_MAP_LEFT,
    SHORTCUT_SCROLL_MAP_DOWN,
    SHORTCUT_SCROLL_MAP_RIGHT,
    SHORTCUT_OPEN_CHAT_WINDOW,
    SHORTCUT_QUICK_SAVE_GAME,
    SHORTCUT_SHOW_OPTIONS,
    SHORTCUT_MUTE_SOUND,
    SHORTCUT_WINDOWED_MODE_TOGGLE,
    SHORTCUT_SHOW_MULTIPLAYER,
    SHORTCUT_PAINT_ORIGINAL_TOGGLE,
    SHORTCUT_DEBUG_PAINT_TOGGLE,
    SHORTCUT_SEE_THROUGH_PATHS_TOGGLE,
    SHORTCUT_RIDE_CONSTRUCTION_TURN_LEFT,
    SHORTCUT_RIDE_CONSTRUCTION_TURN_RIGHT,
    SHORTCUT_RIDE_CONSTRUCTION_USE_TRACK_DEFAULT,
    SHORTCUT_RIDE_CONSTRUCTION_SLOPE_DOWN,
    SHORTCUT_RIDE_CONSTRUCTION_SLOPE_UP,
    SHORTCUT_RIDE_CONSTRUCTION_CHAIN_LIFT_TOGGLE,
    SHORTCUT_RIDE_CONSTRUCTION_BANK_LEFT,
    SHORTCUT_RIDE_CONSTRUCTION_BANK_RIGHT,
    SHORTCUT_RIDE_CONSTRUCTION_PREVIOUS_TRACK,
    SHORTCUT_RIDE_CONSTRUCTION_NEXT_TRACK,
    SHORTCUT_RIDE_CONSTRUCTION_BUILD_CURRENT,
    SHORTCUT_RIDE_CONSTRUCTION_DEMOLISH_CURRENT,
    SHORTCUT_LOAD_GAME,
    SHORTCUT_CLEAR_SCENERY,

    SHORTCUT_COUNT,

    SHORTCUT_UNDEFINED = 0xFFFF,
};

#ifdef __cplusplus

#include <string>

namespace OpenRCT2
{
    interface IPlatformEnvironment;

    namespace Input
    {
        class KeyboardShortcuts
        {
        private:
            constexpr static sint32 CURRENT_FILE_VERSION = 1;
            static const uint16 DefaultKeys[SHORTCUT_COUNT];

            IPlatformEnvironment * const _env;
            uint16 _keys[SHORTCUT_COUNT];

        public:
            KeyboardShortcuts(IPlatformEnvironment * env);

            void Reset();
            bool Load();
            bool Save();

            std::string GetShortcutString(sint32 shortcut) const;

            void Set(sint32 key);
            sint32 GetFromKey(sint32 key);
            void GetKeyboardMapScroll(const uint8 * keysState, sint32 * x, sint32 * y) const;
        };
    }
}

#endif

#ifdef __cplusplus
extern "C"
{
#endif
    typedef struct rct_window rct_window;

    /** The current shortcut being changed. */
    extern uint8 gKeyboardShortcutChangeId;
    extern const rct_string_id ShortcutStringIds[SHORTCUT_COUNT];

    void keyboard_shortcuts_reset();
    bool keyboard_shortcuts_load();
    bool keyboard_shortcuts_save();
    void keyboard_shortcuts_set(sint32 key);
    sint32 keyboard_shortcuts_get_from_key(sint32 key);
    void keyboard_shortcuts_format_string(char * buffer, size_t bufferSize, sint32 shortcut);

    void keyboard_shortcut_handle(sint32 key);
    void keyboard_shortcut_handle_command(sint32 shortcutIndex);
    void keyboard_shortcut_format_string(char *buffer, size_t size, uint16 shortcutKey);

    void get_keyboard_map_scroll(const uint8 * keysState, sint32 * x, sint32 * y);
#ifdef __cplusplus
}
#endif
