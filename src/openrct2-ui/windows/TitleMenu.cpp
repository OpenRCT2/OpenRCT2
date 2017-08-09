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

#include <openrct2/config/Config.h>
#include <openrct2/Context.h>
#include <openrct2/ParkImporter.h>
#include <openrct2-ui/windows/Window.h>

extern "C"
{
    #include <openrct2/editor.h>
    #include <openrct2/game.h>
    #include <openrct2/input.h>
    #include <openrct2/interface/widget.h>
    #include <openrct2/localisation/localisation.h>
    #include <openrct2/sprites.h>
    #include <openrct2/windows/dropdown.h>
}

enum {
    WIDX_START_NEW_GAME,
    WIDX_CONTINUE_SAVED_GAME,
    WIDX_MULTIPLAYER,
    WIDX_SHOW_TUTORIAL,
    WIDX_GAME_TOOLS
};

static rct_widget window_title_menu_widgets[] = {
    { WWT_IMGBTN, 2, 0, 0, 0, 81, SPR_MENU_NEW_GAME,        STR_START_NEW_GAME_TIP          },
    { WWT_IMGBTN, 2, 0, 0, 0, 81, SPR_MENU_LOAD_GAME,       STR_CONTINUE_SAVED_GAME_TIP     },
    { WWT_IMGBTN, 2, 0, 0, 0, 81, SPR_G2_MENU_MULTIPLAYER,  STR_SHOW_MULTIPLAYER_TIP        },
    { WWT_IMGBTN, 2, 0, 0, 0, 81, SPR_MENU_TUTORIAL,        STR_SHOW_TUTORIAL_TIP           },
    { WWT_IMGBTN, 2, 0, 0, 0, 81, SPR_MENU_TOOLBOX,         STR_GAME_TOOLS_TIP              },
    { WIDGETS_END },
};

static void window_title_menu_mouseup(rct_window *w, rct_widgetindex widgetIndex);
static void window_title_menu_mousedown(rct_window *w, rct_widgetindex widgetIndex, rct_widget* widget);
static void window_title_menu_dropdown(rct_window *w, rct_widgetindex widgetIndex, sint32 dropdownIndex);
static void window_title_menu_cursor(rct_window *w, rct_widgetindex widgetIndex, sint32 x, sint32 y, sint32 *cursorId);
static void window_title_menu_paint(rct_window *w, rct_drawpixelinfo *dpi);

static rct_window_event_list window_title_menu_events = {
    NULL,
    window_title_menu_mouseup,
    NULL,
    window_title_menu_mousedown,
    window_title_menu_dropdown,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    window_title_menu_cursor,
    NULL,
    NULL,
    window_title_menu_paint,
    NULL
};

/**
 * Creates the window containing the menu buttons on the title screen.
 *  rct2: 0x0066B5C0 (part of 0x0066B3E8)
 */
rct_window * window_title_menu_open()
{
    rct_window* window;

    window = window_create(
        0, context_get_height() - 142,
        0, 100,
        &window_title_menu_events,
        WC_TITLE_MENU,
        WF_STICK_TO_BACK | WF_TRANSPARENT | WF_NO_BACKGROUND
    );
    window->widgets = window_title_menu_widgets;
    window->enabled_widgets = (
        (1 << WIDX_START_NEW_GAME) |
        (1 << WIDX_CONTINUE_SAVED_GAME) |
#ifndef DISABLE_NETWORK
        (1 << WIDX_MULTIPLAYER) |
#endif
        // Disable tutorial
        // (1 << WIDX_SHOW_TUTORIAL) |
        (1 << WIDX_GAME_TOOLS)
    );

    rct_widgetindex i = 0;
    sint32 x = 0;
    for (rct_widget *widget = window->widgets; widget->type != WWT_LAST; widget++) {
        if (widget_is_enabled(window, i)) {
            widget->left = x;
            widget->right = x + 81;

            x += 82;
        } else {
            widget->type = WWT_EMPTY;
        }
        i++;
    }
    window->width = x;
    window->x = (context_get_width() - window->width) / 2;

    window_init_scroll_widgets(window);

    return window;
}

static void window_title_menu_scenarioselect_callback(const utf8 *path)
{
    ParkLoadResult * result = scenario_load_and_play_from_path(path);
    handle_park_load_failure(result, path);
    ParkLoadResult_Delete(result);
}

static void window_title_menu_mouseup(rct_window *w, rct_widgetindex widgetIndex)
{
    rct_window *windowToOpen = NULL;

    switch (widgetIndex) {
    case WIDX_START_NEW_GAME:
        windowToOpen = window_find_by_class(WC_SCENARIO_SELECT);
        if (windowToOpen != NULL) {
            window_bring_to_front(windowToOpen);
        }
        else {
            window_close_by_class(WC_LOADSAVE);
            window_close_by_class(WC_SERVER_LIST);
            window_scenarioselect_open(window_title_menu_scenarioselect_callback);
        }
        break;
    case WIDX_CONTINUE_SAVED_GAME:
        windowToOpen = window_find_by_class(WC_LOADSAVE);
        if (windowToOpen != NULL) {
            window_bring_to_front(windowToOpen);
        }
        else {
            window_close_by_class(WC_SCENARIO_SELECT);
            window_close_by_class(WC_SERVER_LIST);
            game_do_command(0, 1, 0, 0, GAME_COMMAND_LOAD_OR_QUIT, 0, 0);
        }
        break;
    case WIDX_MULTIPLAYER:
        windowToOpen = window_find_by_class(WC_SERVER_LIST);
        if (windowToOpen != NULL) {
            window_bring_to_front(windowToOpen);
        }
        else {
            window_close_by_class(WC_SCENARIO_SELECT);
            window_close_by_class(WC_LOADSAVE);
            context_open_window(WC_SERVER_LIST);
        }
        break;
    }
}

static void window_title_menu_mousedown(rct_window *w, rct_widgetindex widgetIndex, rct_widget* widget)
{
    if (widgetIndex == WIDX_GAME_TOOLS) {
        gDropdownItemsFormat[0] = STR_SCENARIO_EDITOR;
        gDropdownItemsFormat[1] = STR_CONVERT_SAVED_GAME_TO_SCENARIO;
        gDropdownItemsFormat[2] = STR_ROLLER_COASTER_DESIGNER;
        gDropdownItemsFormat[3] = STR_TRACK_DESIGNS_MANAGER;
        window_dropdown_show_text(
            w->x + widget->left,
            w->y + widget->top,
            widget->bottom - widget->top + 1,
            TRANSLUCENT(w->colours[0]),
            DROPDOWN_FLAG_STAY_OPEN,
            4
        );
    }
}

static void window_title_menu_dropdown(rct_window *w, rct_widgetindex widgetIndex, sint32 dropdownIndex)
{
    if (widgetIndex == WIDX_GAME_TOOLS) {
        switch (dropdownIndex) {
        case 0:
            editor_load();
            break;
        case 1:
            editor_convert_save_to_scenario();
            break;
        case 2:
            trackdesigner_load();
            break;
        case 3:
            trackmanager_load();
            break;
        }
    }
}

static void window_title_menu_cursor(rct_window *w, rct_widgetindex widgetIndex, sint32 x, sint32 y, sint32 *cursorId)
{
    gTooltipTimeout = 2000;
}

static void window_title_menu_paint(rct_window *w, rct_drawpixelinfo *dpi)
{
    gfx_filter_rect(dpi, w->x, w->y, w->x + w->width - 1, w->y + 82 - 1, PALETTE_51);
    window_draw_widgets(w, dpi);
}
