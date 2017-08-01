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

#include "../config/Config.h"
#include "../OpenRCT2.h"
#include "../core/Util.hpp"

extern "C"
{
#include "../audio/audio.h"
#include "../game.h"
#include "../localisation/localisation.h"
#include "../interface/widget.h"
}

enum WINDOW_SAVE_PROMPT_WIDGET_IDX {
    WIDX_BACKGROUND,
    WIDX_TITLE,
    WIDX_CLOSE,
    WIDX_LABEL,
    WIDX_SAVE,
    WIDX_DONT_SAVE,
    WIDX_CANCEL
};

static rct_widget window_save_prompt_widgets[] = {
    { WWT_FRAME,            0,  0,      259,    0,  49, STR_NONE,                   STR_NONE },                 // panel / background
    { WWT_CAPTION,          0,  1,      258,    1,  14, 0,                          STR_WINDOW_TITLE_TIP },     // title bar
    { WWT_CLOSEBOX,         0,  247,    257,    2,  13, STR_CLOSE_X,                STR_CLOSE_WINDOW_TIP },     // close x button
    { WWT_12,               0,  2,      257,    19, 30, 0,                          STR_NONE },                 // question/label
    { WWT_DROPDOWN_BUTTON,  0,  8,      85,     35, 46, STR_SAVE_PROMPT_SAVE,       STR_NONE },     // save
    { WWT_DROPDOWN_BUTTON,  0,  91,     168,    35, 46, STR_SAVE_PROMPT_DONT_SAVE,  STR_NONE },     // don't save
    { WWT_DROPDOWN_BUTTON,  0,  174,    251,    35, 46, STR_SAVE_PROMPT_CANCEL,     STR_NONE },     // cancel
    { WIDGETS_END },
};

enum WINDOW_QUIT_PROMPT_WIDGET_IDX {
    WQIDX_BACKGROUND,
    WQIDX_TITLE,
    WQIDX_CLOSE,
    WQIDX_OK,
    WQIDX_CANCEL
};

static rct_widget window_quit_prompt_widgets[] = {
    { WWT_FRAME,            0,  0,      176,    0,  33, STR_NONE,                   STR_NONE },                 // panel / background
    { WWT_CAPTION,          0,  1,      175,    1,  14, STR_QUIT_GAME_PROMPT_TITLE, STR_WINDOW_TITLE_TIP },     // title bar
    { WWT_CLOSEBOX,         0,  164,    174,    2,  13, STR_CLOSE_X,                STR_CLOSE_WINDOW_TIP },     // close x button
    { WWT_DROPDOWN_BUTTON,  0,  8,      85,     19, 30, STR_OK,                     STR_NONE },     // ok
    { WWT_DROPDOWN_BUTTON,  0,  91,     168,    19, 30, STR_CANCEL,                 STR_NONE },     // cancel
    { WIDGETS_END },
};

static const rct_string_id window_save_prompt_labels[][2] = {
    { STR_LOAD_GAME_PROMPT_TITLE,   STR_SAVE_BEFORE_LOADING },
    { STR_QUIT_GAME_PROMPT_TITLE,   STR_SAVE_BEFORE_QUITTING },
    { STR_QUIT_GAME_2_PROMPT_TITLE, STR_SAVE_BEFORE_QUITTING_2 },
};


static void window_save_prompt_close(rct_window *w);
static void window_save_prompt_mouseup(rct_window *w, rct_widgetindex widgetIndex);
static void window_save_prompt_paint(rct_window *w, rct_drawpixelinfo *dpi);
static void window_save_prompt_callback(sint32 result, const utf8 * path);

static rct_window_event_list window_save_prompt_events = {
    window_save_prompt_close,
    window_save_prompt_mouseup,
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
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    window_save_prompt_paint,
    NULL
};

/**
 *
 *  rct2: 0x0066DCBE
 */
void window_save_prompt_open()
{
    sint32 width, height;
    rct_string_id stringId;
    rct_window* window;
    uint8 prompt_mode;
    rct_widget *widgets;
    uint64 enabled_widgets;

    prompt_mode = gSavePromptMode;
    if (prompt_mode == PM_QUIT)
        prompt_mode = PM_SAVE_BEFORE_QUIT;

    // do not show save prompt if we're in the title demo and click on load game
    if (gScreenFlags & SCREEN_FLAGS_TITLE_DEMO) {
        game_load_or_quit_no_save_prompt();
        return;
    }

    if (!gConfigGeneral.confirmation_prompt) {
        /* game_load_or_quit_no_save_prompt() will exec requested task and close this window
        * immediately again.
        * TODO restructure these functions when we're sure game_load_or_quit_no_save_prompt()
        * and game_load_or_quit() are not called by the original binary anymore.
        */

        if (gScreenAge < 3840) {
            game_load_or_quit_no_save_prompt();
            return;
        }
    }

    // Check if window is already open
    window = window_bring_to_front_by_class(WC_SAVE_PROMPT);
    if (window){
        window_close(window);
    }

    if (gScreenFlags & (SCREEN_FLAGS_TRACK_DESIGNER | SCREEN_FLAGS_TRACK_MANAGER)) {
        widgets = window_quit_prompt_widgets;
        enabled_widgets =
            (1 << WQIDX_CLOSE) |
            (1 << WQIDX_OK) |
            (1 << WQIDX_CANCEL);
        width = 177;
        height = 34;
    } else {
        widgets = window_save_prompt_widgets;
        enabled_widgets =
            (1 << WIDX_CLOSE) |
            (1 << WIDX_SAVE) |
            (1 << WIDX_DONT_SAVE) |
            (1 << WIDX_CANCEL);
        width = 260;
        height = 50;
    }

    if (prompt_mode >= Util::CountOf(window_save_prompt_labels)) {
        log_warning("Invalid save prompt mode %u", prompt_mode);
        return;
    }
    window = window_create_centred(
        width,
        height,
        &window_save_prompt_events,
        WC_SAVE_PROMPT,
        WF_TRANSPARENT | WF_STICK_TO_FRONT
    );

    window->widgets = widgets;
    window->enabled_widgets = enabled_widgets;
    window_init_scroll_widgets(window);

    // Pause the game
    gGamePaused |= GAME_PAUSED_MODAL;
    audio_pause_sounds();
    window_invalidate_by_class(WC_TOP_TOOLBAR);

    stringId = window_save_prompt_labels[prompt_mode][0];
    if (stringId == STR_LOAD_GAME_PROMPT_TITLE && gScreenFlags & SCREEN_FLAGS_SCENARIO_EDITOR)
        stringId = STR_LOAD_LANDSCAPE_PROMPT_TITLE;
    if (stringId == STR_QUIT_GAME_PROMPT_TITLE && gScreenFlags & SCREEN_FLAGS_SCENARIO_EDITOR)
        stringId = STR_QUIT_SCENARIO_EDITOR;
    window_save_prompt_widgets[WIDX_TITLE].text = stringId;
    window_save_prompt_widgets[WIDX_LABEL].text = window_save_prompt_labels[prompt_mode][1];
}

/**
 *
 *  rct2: 0x0066DF17
 */
static void window_save_prompt_close(rct_window *w)
{
    // Unpause the game
    gGamePaused &= ~GAME_PAUSED_MODAL;
    audio_unpause_sounds();
    window_invalidate_by_class(WC_TOP_TOOLBAR);
}

/**
 *
 *  rct2: 0x0066DDF2
 */
static void window_save_prompt_mouseup(rct_window *w, rct_widgetindex widgetIndex)
{
    if (gScreenFlags & (SCREEN_FLAGS_TITLE_DEMO | SCREEN_FLAGS_TRACK_DESIGNER | SCREEN_FLAGS_TRACK_MANAGER)) {
        switch (widgetIndex) {
        case WQIDX_OK:
            game_load_or_quit_no_save_prompt();
            break;
        case WQIDX_CLOSE:
        case WQIDX_CANCEL:
            window_close(w);
            break;
        }
        return;
    } else {
        switch (widgetIndex) {
        case WIDX_SAVE:
            if (gScreenFlags & (SCREEN_FLAGS_EDITOR)) {
                window_loadsave_open(LOADSAVETYPE_SAVE | LOADSAVETYPE_LANDSCAPE, gS6Info.name);
            } else {
                save_game_as();
            }
            window_close(w);
            window_loadsave_set_loadsave_callback(window_save_prompt_callback);
            break;
        case WIDX_DONT_SAVE:
            game_load_or_quit_no_save_prompt();
            return;
        case WIDX_CLOSE:
        case WIDX_CANCEL:
            window_close(w);
            return;
        }
    }
}

static void window_save_prompt_paint(rct_window *w, rct_drawpixelinfo *dpi)
{
    window_draw_widgets(w, dpi);
}

static void window_save_prompt_callback(sint32 result, const utf8 * path)
{
    if (result == MODAL_RESULT_OK) {
        game_load_or_quit_no_save_prompt();
    }
}
