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
#include "input/input.h"
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

    void HandleKeyboard(bool isTitle) override
    {
        input_handle_keyboard(isTitle);
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
