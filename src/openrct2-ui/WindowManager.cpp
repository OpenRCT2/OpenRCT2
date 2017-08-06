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
#include <openrct2-ui/windows/Window.h>
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
        case WC_ABOUT:
            return window_about_open();
        case WC_CHANGELOG:
            return window_changelog_open();
        case WC_CHEATS:
            return window_cheats_open();
        case WC_CUSTOM_CURRENCY_CONFIG:
            return custom_currency_window_open();
        case WC_DEBUG_PAINT:
            return window_debug_paint_open();
        case WC_LAND_RIGHTS:
            return window_land_rights_open();
        case WC_MAPGEN:
            return window_mapgen_open();
        case WC_MULTIPLAYER:
            return window_multiplayer_open();
        case WC_MUSIC_CREDITS:
            return window_music_credits_open();
        case WC_RECENT_NEWS:
            return window_news_open();
        case WC_NOTIFICATION_OPTIONS:
            return window_news_options_open();
        case WC_OPTIONS:
            return window_options_open();
        case WC_SAVE_PROMPT:
            return window_save_prompt_open();
        case WC_SERVER_LIST:
            return window_server_list_open();
        case WC_SERVER_START:
            return window_server_start_open();
        case WC_KEYBOARD_SHORTCUT_LIST:
            return window_shortcut_keys_open();
        case WC_THEMES:
            return window_themes_open();
        case WC_TITLE_EXIT:
            return window_title_exit_open();
        case WC_TITLE_LOGO:
            return window_title_logo_open();
        case WC_TITLE_MENU:
            return window_title_menu_open();
        case WC_TITLE_OPTIONS:
            return window_title_options_open();
        case WC_VIEWPORT:
            return window_viewport_open();
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
