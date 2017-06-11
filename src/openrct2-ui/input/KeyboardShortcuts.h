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

#ifdef __cplusplus
extern "C"
{
#endif
    #include "../input/keyboard_shortcut.h"
#ifdef __cplusplus
}
#endif

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
        };
    }
}

#endif

#ifdef __cplusplus
extern "C"
{
#endif
    typedef struct rct_window rct_window;

    void keyboard_shortcuts_reset();
    bool keyboard_shortcuts_load();
    bool keyboard_shortcuts_save();
    void keyboard_shortcuts_set(sint32 key);
    sint32 keyboard_shortcuts_get_from_key(sint32 key);
    void keyboard_shortcuts_format_string(char * buffer, size_t bufferSize, sint32 shortcut);

    rct_window * window_shortcut_keys_open();
    rct_window * window_shortcut_change_open(sint32 selected_key);
#ifdef __cplusplus
}
#endif
