/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include <openrct2/config/Config.h>
#include <openrct2/Context.h>
#include <openrct2/ParkImporter.h>
#include <openrct2-ui/windows/Window.h>

#include <openrct2/Editor.h>
#include <openrct2/Game.h>
#include <openrct2/Input.h>
#include <openrct2-ui/interface/Widget.h>
#include <openrct2/localisation/Localisation.h>
#include <openrct2/sprites.h>
#include <openrct2-ui/interface/Dropdown.h>

// clang-format off
enum {
    WIDX_START_NEW_GAME,
    WIDX_CONTINUE_SAVED_GAME,
    WIDX_MULTIPLAYER,
    WIDX_GAME_TOOLS
};

static rct_widget window_title_menu_widgets[] = {
    { WWT_IMGBTN, 2, 0, 0, 0, 81, SPR_MENU_NEW_GAME,        STR_START_NEW_GAME_TIP          },
    { WWT_IMGBTN, 2, 0, 0, 0, 81, SPR_MENU_LOAD_GAME,       STR_CONTINUE_SAVED_GAME_TIP     },
    { WWT_IMGBTN, 2, 0, 0, 0, 81, SPR_G2_MENU_MULTIPLAYER,  STR_SHOW_MULTIPLAYER_TIP        },
    { WWT_IMGBTN, 2, 0, 0, 0, 81, SPR_MENU_TOOLBOX,         STR_GAME_TOOLS_TIP              },
    { WIDGETS_END },
};

static void window_title_menu_mouseup(rct_window *w, rct_widgetindex widgetIndex);
static void window_title_menu_mousedown(rct_window *w, rct_widgetindex widgetIndex, rct_widget* widget);
static void window_title_menu_dropdown(rct_window *w, rct_widgetindex widgetIndex, int32_t dropdownIndex);
static void window_title_menu_cursor(rct_window *w, rct_widgetindex widgetIndex, int32_t x, int32_t y, int32_t *cursorId);
static void window_title_menu_paint(rct_window *w, rct_drawpixelinfo *dpi);

static rct_window_event_list window_title_menu_events = {
    nullptr,
    window_title_menu_mouseup,
    nullptr,
    window_title_menu_mousedown,
    window_title_menu_dropdown,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    window_title_menu_cursor,
    nullptr,
    nullptr,
    window_title_menu_paint,
    nullptr
};
// clang-format on

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
        (1 << WIDX_GAME_TOOLS)
    );

    rct_widgetindex i = 0;
    int32_t x = 0;
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
    context_load_park_from_file(path);
}

static void window_title_menu_mouseup(rct_window *w, rct_widgetindex widgetIndex)
{
    rct_window *windowToOpen = nullptr;

    switch (widgetIndex) {
    case WIDX_START_NEW_GAME:
        windowToOpen = window_find_by_class(WC_SCENARIO_SELECT);
        if (windowToOpen != nullptr) {
            window_bring_to_front(windowToOpen);
        }
        else {
            window_close_by_class(WC_LOADSAVE);
            window_close_by_class(WC_SERVER_LIST);
            window_scenarioselect_open(window_title_menu_scenarioselect_callback, false);
        }
        break;
    case WIDX_CONTINUE_SAVED_GAME:
        windowToOpen = window_find_by_class(WC_LOADSAVE);
        if (windowToOpen != nullptr) {
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
        if (windowToOpen != nullptr) {
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

static void window_title_menu_dropdown(rct_window *w, rct_widgetindex widgetIndex, int32_t dropdownIndex)
{
    if (widgetIndex == WIDX_GAME_TOOLS) {
        switch (dropdownIndex) {
        case 0:
            Editor::Load();
            break;
        case 1:
            Editor::ConvertSaveToScenario();
            break;
        case 2:
            Editor::LoadTrackDesigner();
            break;
        case 3:
            Editor::LoadTrackManager();
            break;
        }
    }
}

static void window_title_menu_cursor(rct_window *w, rct_widgetindex widgetIndex, int32_t x, int32_t y, int32_t *cursorId)
{
    gTooltipTimeout = 2000;
}

static void window_title_menu_paint(rct_window *w, rct_drawpixelinfo *dpi)
{
    gfx_filter_rect(dpi, w->x, w->y, w->x + w->width - 1, w->y + 82 - 1, PALETTE_51);
    window_draw_widgets(w, dpi);
}
