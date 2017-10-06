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
#include <openrct2/core/Console.hpp>
#include "input/input.h"
#include "input/KeyboardShortcuts.h"
#include "WindowManager.h"

using namespace OpenRCT2::Ui;

class WindowManager final : public IWindowManager
{
public:
    void Init() override
    {
        window_guest_list_init_vars();
    }

    rct_window * OpenWindow(rct_windowclass wc) override
    {
        switch (wc) {
        case WC_ABOUT:
            return window_about_open();
        case WC_CHANGELOG:
            return window_changelog_open();
        case WC_CHEATS:
            return window_cheats_open();
        case WC_CLEAR_SCENERY:
            return window_clear_scenery_open();
        case WC_CUSTOM_CURRENCY_CONFIG:
            return custom_currency_window_open();
        case WC_DEBUG_PAINT:
            return window_debug_paint_open();
        case WC_EDITOR_INVENTION_LIST:
            return window_editor_inventions_list_open();
        case WC_EDTIOR_OBJECTIVE_OPTIONS:
            return window_editor_objective_options_open();
        case WC_EDITOR_SCENARIO_OPTIONS:
            return window_editor_scenario_options_open();
        case WC_FINANCES:
            return window_finances_open();
        case WC_FOOTPATH:
            return window_footpath_open();
        case WC_GUEST_LIST:
            return window_guest_list_open();
        case WC_LAND:
            return window_land_open();
        case WC_LAND_RIGHTS:
            return window_land_rights_open();
        case WC_MAIN_WINDOW:
            return window_main_open();
        case WC_MAP:
            return window_map_open();
        case WC_MAPGEN:
            return window_mapgen_open();
        case WC_MULTIPLAYER:
            return window_multiplayer_open();
        case WC_MUSIC_CREDITS:
            return window_music_credits_open();
        case WC_PARK_INFORMATION:
            return window_park_entrance_open();
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
        case WC_STAFF_LIST:
            return window_staff_list_open();
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
        case WC_VIEW_CLIPPING:
            return window_view_clipping_open();
        case WC_VIEWPORT:
            return window_viewport_open();
        case WC_WATER:
            return window_water_open();
        default:
            Console::Error::WriteLine("Unhandled window class (%d)", wc);
            return nullptr;
        }
    }

    rct_window * OpenView(uint8 view) override
    {
        switch (view) {
        case WV_PARK_AWARDS:
            return window_park_awards_open();
        case WV_PARK_RATING:
            return window_park_rating_open();
        case WV_PARK_OBJECTIVE:
            return window_park_objective_open();
        case WV_PARK_GUESTS:
            return window_park_guests_open();
        case WV_FINANCES_RESEARCH:
            return window_finances_research_open();
        default:
            return nullptr;
        }
    }

    rct_window * OpenDetails(uint8 type, sint32 id) override
    {
        switch (type)
        {
        case WD_BANNER:
            return window_banner_open(id);
        case WD_DEMOLISH_RIDE:
            return window_ride_demolish_prompt_open(id);
        case WD_NEW_CAMPAIGN:
            return window_new_campaign_open(id);
        case WD_SIGN:
            return window_sign_open(id);
        case WD_SIGN_SMALL:
            return window_sign_small_open(id);

        case WD_PLAYER:
            return window_player_open(id);

        default:
            return nullptr;
        }
    }

    rct_window * ShowError(rct_string_id title, rct_string_id message) override
    {
        return window_error_open(title, message);
    }

    rct_window * OpenIntent(Intent * intent) override
    {
        switch(intent->GetWindowClass()) {
        case WC_PEEP:
            return window_guest_open((rct_peep*)intent->GetPointerExtra(INTENT_EXTRA_PEEP));
        case WC_FIRE_PROMPT:
            return window_staff_fire_prompt_open((rct_peep*)intent->GetPointerExtra(INTENT_EXTRA_PEEP));
        case WC_INSTALL_TRACK:
            return window_install_track_open(intent->GetStringExtra(INTENT_EXTRA_PATH).c_str());
        case WC_GUEST_LIST:
            return window_guest_list_open_with_filter(intent->GetSIntExtra(INTENT_EXTRA_GUEST_LIST_FILTER), intent->GetSIntExtra(INTENT_EXTRA_RIDE_ID));
        case WC_LOADSAVE:
        {
            uint32 type = intent->GetUIntExtra(INTENT_EXTRA_LOADSAVE_TYPE);
            std::string defaultName = intent->GetStringExtra(INTENT_EXTRA_PATH);
            loadsave_callback callback = (loadsave_callback) intent->GetPointerExtra(INTENT_EXTRA_CALLBACK);
            rct_window *w = window_loadsave_open(type, defaultName.c_str());
            window_loadsave_set_loadsave_callback(callback);

            return w;
        }
        case WC_MANAGE_TRACK_DESIGN:
            return window_track_manage_open((track_design_file_ref *) intent->GetPointerExtra(INTENT_EXTRA_TRACK_DESIGN));
        case WC_TRACK_DESIGN_PLACE:
            return window_track_place_open((track_design_file_ref *) intent->GetPointerExtra(INTENT_EXTRA_TRACK_DESIGN));
        case WC_SCENARIO_SELECT:
            return window_scenarioselect_open((scenarioselect_callback) intent->GetPointerExtra(INTENT_EXTRA_CALLBACK));
        default:
            Console::Error::WriteLine("Unhandled window class for intent (%d)", intent->GetWindowClass());
            return nullptr;
        }
    }

    void BroadcastIntent(Intent * intent) override
    {
        switch (intent->GetWindowClass()) {
        case INTENT_ACTION_MAP:
            window_map_reset();
            break;
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
