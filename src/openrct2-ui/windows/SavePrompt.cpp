/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include <openrct2/config/Config.h>
#include <openrct2/OpenRCT2.h>
#include <openrct2/core/Util.hpp>
#include <openrct2-ui/windows/Window.h>
#include <openrct2/windows/Intent.h>
#include <openrct2/Context.h>

#include <openrct2/audio/audio.h>
#include <openrct2/Game.h>
#include <openrct2/localisation/Localisation.h>
#include <openrct2-ui/interface/Widget.h>
#include <openrct2/scenario/Scenario.h>

// clang-format off
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
    { WWT_FRAME,            0,  0,      259,    0,  53, STR_NONE,                   STR_NONE },                 // panel / background
    { WWT_CAPTION,          0,  1,      258,    1,  14, 0,                          STR_WINDOW_TITLE_TIP },     // title bar
    { WWT_CLOSEBOX,         0,  247,    257,    2,  13, STR_CLOSE_X_WHITE,          STR_CLOSE_WINDOW_TIP },     // close x button
    { WWT_LABEL_CENTRED,    0,  2,      257,    19, 30, 0,                          STR_NONE },                 // question/label
    { WWT_BUTTON,           0,  8,      85,     35, 48, STR_SAVE_PROMPT_SAVE,       STR_NONE },     // save
    { WWT_BUTTON,           0,  91,     168,    35, 48, STR_SAVE_PROMPT_DONT_SAVE,  STR_NONE },     // don't save
    { WWT_BUTTON,           0,  174,    251,    35, 48, STR_SAVE_PROMPT_CANCEL,     STR_NONE },     // cancel
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
    { WWT_FRAME,            0,  0,      176,    0,  37, STR_NONE,                   STR_NONE },                 // panel / background
    { WWT_CAPTION,          0,  1,      175,    1,  14, STR_QUIT_GAME_PROMPT_TITLE, STR_WINDOW_TITLE_TIP },     // title bar
    { WWT_CLOSEBOX,         0,  164,    174,    2,  13, STR_CLOSE_X_WHITE,          STR_CLOSE_WINDOW_TIP },     // close x button
    { WWT_BUTTON,           0,  8,      85,     19, 32, STR_OK,                     STR_NONE },     // ok
    { WWT_BUTTON,           0,  91,     168,    19, 32, STR_CANCEL,                 STR_NONE },     // cancel
    { WIDGETS_END },
};

static constexpr const rct_string_id window_save_prompt_labels[][2] = {
    { STR_LOAD_GAME_PROMPT_TITLE,   STR_SAVE_BEFORE_LOADING },
    { STR_QUIT_GAME_PROMPT_TITLE,   STR_SAVE_BEFORE_QUITTING },
    { STR_QUIT_GAME_2_PROMPT_TITLE, STR_SAVE_BEFORE_QUITTING_2 },
};


static void window_save_prompt_close(rct_window *w);
static void window_save_prompt_mouseup(rct_window *w, rct_widgetindex widgetIndex);
static void window_save_prompt_paint(rct_window *w, rct_drawpixelinfo *dpi);
static void window_save_prompt_callback(int32_t result, const utf8 * path);

static rct_window_event_list window_save_prompt_events = {
    window_save_prompt_close,
    window_save_prompt_mouseup,
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
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    window_save_prompt_paint,
    nullptr
};
// clang-format on

/**
 *
 *  rct2: 0x0066DCBE
 */
rct_window * window_save_prompt_open()
{
    int32_t width, height;
    rct_string_id stringId;
    rct_window* window;
    uint8_t prompt_mode;
    rct_widget *widgets;
    uint64_t enabled_widgets;

    prompt_mode = gSavePromptMode;
    if (prompt_mode == PM_QUIT)
        prompt_mode = PM_SAVE_BEFORE_QUIT;

    // do not show save prompt if we're in the title demo and click on load game
    if (gScreenFlags & SCREEN_FLAGS_TITLE_DEMO) {
        game_load_or_quit_no_save_prompt();
        return nullptr;
    }

    if (!gConfigGeneral.confirmation_prompt) {
        /* game_load_or_quit_no_save_prompt() will exec requested task and close this window
        * immediately again.
        * TODO restructure these functions when we're sure game_load_or_quit_no_save_prompt()
        * and game_load_or_quit() are not called by the original binary anymore.
        */

        if (gScreenAge < 3840) {
            game_load_or_quit_no_save_prompt();
            return nullptr;
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
        height = 38;
    } else {
        widgets = window_save_prompt_widgets;
        enabled_widgets =
            (1 << WIDX_CLOSE) |
            (1 << WIDX_SAVE) |
            (1 << WIDX_DONT_SAVE) |
            (1 << WIDX_CANCEL);
        width = 260;
        height = 54;
    }

    if (prompt_mode >= Util::CountOf(window_save_prompt_labels)) {
        log_warning("Invalid save prompt mode %u", prompt_mode);
        return nullptr;
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
    audio_stop_all_music_and_sounds();
    window_invalidate_by_class(WC_TOP_TOOLBAR);

    stringId = window_save_prompt_labels[prompt_mode][0];
    if (stringId == STR_LOAD_GAME_PROMPT_TITLE && gScreenFlags & SCREEN_FLAGS_SCENARIO_EDITOR)
        stringId = STR_LOAD_LANDSCAPE_PROMPT_TITLE;
    if (stringId == STR_QUIT_GAME_PROMPT_TITLE && gScreenFlags & SCREEN_FLAGS_SCENARIO_EDITOR)
        stringId = STR_QUIT_SCENARIO_EDITOR;
    window_save_prompt_widgets[WIDX_TITLE].text = stringId;
    window_save_prompt_widgets[WIDX_LABEL].text = window_save_prompt_labels[prompt_mode][1];

    return window;
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
        switch (widgetIndex)
        {
        case WIDX_SAVE:
        {
            Intent * intent;

            if (gScreenFlags & (SCREEN_FLAGS_EDITOR))
            {
                intent = new Intent(WC_LOADSAVE);
                intent->putExtra(INTENT_EXTRA_LOADSAVE_TYPE, LOADSAVETYPE_SAVE | LOADSAVETYPE_LANDSCAPE);
                intent->putExtra(INTENT_EXTRA_PATH, std::string{gS6Info.name});
            }
            else
            {
                intent = (Intent *) create_save_game_as_intent();
            }
            window_close(w);
            intent->putExtra(INTENT_EXTRA_CALLBACK, (void *) window_save_prompt_callback);
            context_open_intent(intent);
            delete intent;
            break;
        }
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

static void window_save_prompt_callback(int32_t result, const utf8 * path)
{
    if (result == MODAL_RESULT_OK) {
        game_load_or_quit_no_save_prompt();
    }
}
