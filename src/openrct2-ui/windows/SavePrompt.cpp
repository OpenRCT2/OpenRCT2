/*****************************************************************************
 * Copyright (c) 2014-2022 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include <iterator>
#include <openrct2-ui/interface/Widget.h>
#include <openrct2-ui/windows/Window.h>
#include <openrct2/Context.h>
#include <openrct2/Game.h>
#include <openrct2/OpenRCT2.h>
#include <openrct2/audio/audio.h>
#include <openrct2/config/Config.h>
#include <openrct2/localisation/Localisation.h>
#include <openrct2/network/network.h>
#include <openrct2/scenario/Scenario.h>
#include <openrct2/windows/Intent.h>

static constexpr const int32_t WH_SAVE = 54;
static constexpr const int32_t WW_SAVE = 260;
static constexpr const int32_t WH_QUIT = 38;
static constexpr const int32_t WW_QUIT = 177;

// clang-format off
enum WindowSavePromptWidgetIdx {
    WIDX_BACKGROUND,
    WIDX_TITLE,
    WIDX_CLOSE,
    WIDX_LABEL,
    WIDX_SAVE,
    WIDX_DONT_SAVE,
    WIDX_CANCEL
};

static rct_widget window_save_prompt_widgets[] = {
    WINDOW_SHIM_WHITE(STR_NONE, WW_SAVE, WH_SAVE),
    MakeWidget({  2, 19}, {256, 12}, WindowWidgetType::LabelCentred, WindowColour::Primary, STR_EMPTY                ), // question/label
    MakeWidget({  8, 35}, { 78, 14}, WindowWidgetType::Button,        WindowColour::Primary, STR_SAVE_PROMPT_SAVE     ), // save
    MakeWidget({ 91, 35}, { 78, 14}, WindowWidgetType::Button,        WindowColour::Primary, STR_SAVE_PROMPT_DONT_SAVE), // don't save
    MakeWidget({174, 35}, { 78, 14}, WindowWidgetType::Button,        WindowColour::Primary, STR_SAVE_PROMPT_CANCEL   ), // cancel
    WIDGETS_END,
};

enum WindowQuitPromptWidgetIdx {
    WQIDX_BACKGROUND,
    WQIDX_TITLE,
    WQIDX_CLOSE,
    WQIDX_OK,
    WQIDX_CANCEL
};

static rct_widget window_quit_prompt_widgets[] = {
    WINDOW_SHIM_WHITE(STR_QUIT_GAME_PROMPT_TITLE, WW_QUIT, WH_QUIT),
    MakeWidget({ 8, 19}, {78, 14}, WindowWidgetType::Button, WindowColour::Primary, STR_OK    ), // ok
    MakeWidget({91, 19}, {78, 14}, WindowWidgetType::Button, WindowColour::Primary, STR_CANCEL), // cancel
    WIDGETS_END,
};

static constexpr const StringId window_save_prompt_labels[][2] = {
    { STR_LOAD_GAME_PROMPT_TITLE,   STR_SAVE_BEFORE_LOADING },
    { STR_QUIT_GAME_PROMPT_TITLE,   STR_SAVE_BEFORE_QUITTING },
    { STR_QUIT_GAME_2_PROMPT_TITLE, STR_SAVE_BEFORE_QUITTING_2 },
};


static void WindowSavePromptClose(rct_window *w);
static void WindowSavePromptMouseup(rct_window *w, WidgetIndex widgetIndex);
static void WindowSavePromptPaint(rct_window *w, rct_drawpixelinfo *dpi);
static void WindowSavePromptCallback(int32_t result, const utf8 * path);

static rct_window_event_list window_save_prompt_events([](auto& events)
{
    events.close = &WindowSavePromptClose;
    events.mouse_up = &WindowSavePromptMouseup;
    events.paint = &WindowSavePromptPaint;
});
// clang-format on

/**
 *
 *  rct2: 0x0066DCBE
 */
rct_window* WindowSavePromptOpen()
{
    int32_t width, height;
    StringId stringId;
    rct_window* window;
    PromptMode prompt_mode;
    rct_widget* widgets;

    prompt_mode = gSavePromptMode;
    if (prompt_mode == PromptMode::Quit)
        prompt_mode = PromptMode::SaveBeforeQuit;

    // do not show save prompt if we're in the title demo and click on load game
    if (gScreenFlags & SCREEN_FLAGS_TITLE_DEMO)
    {
        game_load_or_quit_no_save_prompt();
        return nullptr;
    }

    if (!gConfigGeneral.confirmation_prompt)
    {
        /* game_load_or_quit_no_save_prompt() will exec requested task and close this window
         * immediately again.
         * TODO restructure these functions when we're sure game_load_or_quit_no_save_prompt()
         * and game_load_or_quit() are not called by the original binary anymore.
         */

        if (gScreenAge < 3840 && network_get_mode() == NETWORK_MODE_NONE)
        {
            game_load_or_quit_no_save_prompt();
            return nullptr;
        }
    }

    // Check if window is already open
    window = window_bring_to_front_by_class(WindowClass::SavePrompt);
    if (window != nullptr)
    {
        window_close(*window);
    }

    if (gScreenFlags & (SCREEN_FLAGS_TRACK_DESIGNER | SCREEN_FLAGS_TRACK_MANAGER))
    {
        widgets = window_quit_prompt_widgets;
        width = WW_QUIT;
        height = WH_QUIT;
    }
    else
    {
        widgets = window_save_prompt_widgets;
        width = WW_SAVE;
        height = WH_SAVE;
    }

    if (EnumValue(prompt_mode) >= std::size(window_save_prompt_labels))
    {
        log_warning("Invalid save prompt mode %u", prompt_mode);
        return nullptr;
    }
    window = WindowCreateCentred(
        width, height, &window_save_prompt_events, WindowClass::SavePrompt, WF_TRANSPARENT | WF_STICK_TO_FRONT);

    window->widgets = widgets;
    WindowInitScrollWidgets(*window);

    // Pause the game if not network play.
    if (network_get_mode() == NETWORK_MODE_NONE)
    {
        gGamePaused |= GAME_PAUSED_MODAL;
        OpenRCT2::Audio::StopAll();
    }

    window_invalidate_by_class(WindowClass::TopToolbar);

    stringId = window_save_prompt_labels[EnumValue(prompt_mode)][0];
    if (stringId == STR_LOAD_GAME_PROMPT_TITLE && gScreenFlags & SCREEN_FLAGS_SCENARIO_EDITOR)
        stringId = STR_LOAD_LANDSCAPE_PROMPT_TITLE;
    if (stringId == STR_QUIT_GAME_PROMPT_TITLE && gScreenFlags & SCREEN_FLAGS_SCENARIO_EDITOR)
        stringId = STR_QUIT_SCENARIO_EDITOR;
    window_save_prompt_widgets[WIDX_TITLE].text = stringId;
    window_save_prompt_widgets[WIDX_LABEL].text = window_save_prompt_labels[EnumValue(prompt_mode)][1];

    return window;
}

/**
 *
 *  rct2: 0x0066DF17
 */
static void WindowSavePromptClose(rct_window* w)
{
    // Unpause the game
    if (network_get_mode() == NETWORK_MODE_NONE)
    {
        gGamePaused &= ~GAME_PAUSED_MODAL;
        OpenRCT2::Audio::Resume();
    }

    window_invalidate_by_class(WindowClass::TopToolbar);
}

/**
 *
 *  rct2: 0x0066DDF2
 */
static void WindowSavePromptMouseup(rct_window* w, WidgetIndex widgetIndex)
{
    if (gScreenFlags & (SCREEN_FLAGS_TITLE_DEMO | SCREEN_FLAGS_TRACK_DESIGNER | SCREEN_FLAGS_TRACK_MANAGER))
    {
        switch (widgetIndex)
        {
            case WQIDX_OK:
                game_load_or_quit_no_save_prompt();
                break;
            case WQIDX_CLOSE:
            case WQIDX_CANCEL:
                window_close(*w);
                break;
        }
        return;
    }

    switch (widgetIndex)
    {
        case WIDX_SAVE:
        {
            std::unique_ptr<Intent> intent;

            if (gScreenFlags & (SCREEN_FLAGS_EDITOR))
            {
                intent = std::make_unique<Intent>(WindowClass::Loadsave);
                intent->putExtra(INTENT_EXTRA_LOADSAVE_TYPE, LOADSAVETYPE_SAVE | LOADSAVETYPE_LANDSCAPE);
                intent->putExtra(INTENT_EXTRA_PATH, gScenarioName);
            }
            else
            {
                intent = create_save_game_as_intent();
            }
            window_close(*w);
            intent->putExtra(INTENT_EXTRA_CALLBACK, reinterpret_cast<void*>(WindowSavePromptCallback));
            context_open_intent(intent.get());
            break;
        }
        case WIDX_DONT_SAVE:
            game_load_or_quit_no_save_prompt();
            return;
        case WIDX_CLOSE:
        case WIDX_CANCEL:
            window_close(*w);
            return;
    }
}

static void WindowSavePromptPaint(rct_window* w, rct_drawpixelinfo* dpi)
{
    WindowDrawWidgets(*w, dpi);
}

static void WindowSavePromptCallback(int32_t result, const utf8* path)
{
    if (result == MODAL_RESULT_OK)
    {
        game_load_or_quit_no_save_prompt();
    }
}
