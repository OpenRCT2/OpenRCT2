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

#include "WindowManager.h"

namespace OpenRCT2 { namespace Ui
{
    class DummyWindowManager final : public IWindowManager
    {
        rct_window * OpenWindow(rct_windowclass wc) override { return nullptr; }
        rct_window * OpenView(uint8 view) override { return nullptr; }
        rct_window * OpenDetails(uint8 type, sint32 id) override { return nullptr; }
        rct_window * ShowError(rct_string_id title, rct_string_id message) override { return nullptr; }
        void HandleKeyboard(bool isTitle) override { }
        std::string GetKeyboardShortcutString(sint32 shortcut) override { return std::string(); }
    };

    IWindowManager * CreateDummyWindowManager()
    {
        return new DummyWindowManager();
    }
} }
