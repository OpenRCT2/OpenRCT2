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

#include <openrct2/ui/WindowManager.h>
#include "input/KeyboardShortcuts.h"
#include "WindowManager.h"

using namespace OpenRCT2::Ui;

class WindowManager final : public IWindowManager
{
public:
    rct_window * OpenWindow(rct_windowclass wc) override
    {
        switch (wc) {
        case WC_KEYBOARD_SHORTCUT_LIST:
            return window_shortcut_keys_open();
        default:
            return nullptr;
        }
    }

    void HandleKeyboardShortcut(sint32 key) override
    {
        rct_window * w = window_find_by_class(WC_CHANGE_KEYBOARD_SHORTCUT);
        if (w != NULL) {
            keyboard_shortcuts_set(key);
            window_close_by_class(WC_CHANGE_KEYBOARD_SHORTCUT);
            window_invalidate_by_class(WC_KEYBOARD_SHORTCUT_LIST);
        } else {
            keyboard_shortcut_handle(key);
        }
    }

    void GetKeyboardMapScroll(const uint8 * keysState, sint32 * x, sint32 * y) override
    {
        get_keyboard_map_scroll(keysState, x, y);
    }

    std::string GetKeyboardShortcutString(sint32 shortcut) override
    {
        utf8 buffer[256];
        keyboard_shortcuts_format_string(buffer, sizeof(buffer), shortcut);
        return std::string(buffer);
    }
};

IWindowManager * OpenRCT2::Ui::CreateWindowManager()
{
    return new WindowManager();
}
