/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "Input.h"

#include "../UiContext.h"
#include "../interface/InGameConsole.h"
#include "KeyboardShortcuts.h"

#include <SDL.h>
#include <cctype>
#include <openrct2-ui/windows/Window.h>
#include <openrct2/Context.h>
#include <openrct2/Input.h>
#include <openrct2/OpenRCT2.h>
#include <openrct2/common.h>
#include <openrct2/config/Config.h>
#include <openrct2/interface/Chat.h>
#include <openrct2/interface/InteractiveConsole.h>
#include <openrct2/paint/VirtualFloor.h>

using namespace OpenRCT2::Ui;

static void InputHandleConsole(int32_t key)
{
    ConsoleInput input = ConsoleInput::None;
    switch (key)
    {
        case SDL_SCANCODE_ESCAPE:
            input = ConsoleInput::LineClear;
            break;
        case SDL_SCANCODE_RETURN:
        case SDL_SCANCODE_KP_ENTER:
            input = ConsoleInput::LineExecute;
            break;
        case SDL_SCANCODE_UP:
            input = ConsoleInput::HistoryPrevious;
            break;
        case SDL_SCANCODE_DOWN:
            input = ConsoleInput::HistoryNext;
            break;
        case SDL_SCANCODE_PAGEUP:
            input = ConsoleInput::ScrollPrevious;
            break;
        case SDL_SCANCODE_PAGEDOWN:
            input = ConsoleInput::ScrollNext;
            break;
    }
    if (input != ConsoleInput::None)
    {
        auto& console = GetInGameConsole();
        console.Input(input);
    }
}

static void InputHandleChat(int32_t key)
{
    ChatInput input = ChatInput::None;
    switch (key)
    {
        case SDL_SCANCODE_ESCAPE:
            input = ChatInput::Close;
            break;
        case SDL_SCANCODE_RETURN:
        case SDL_SCANCODE_KP_ENTER:
            input = ChatInput::Send;
            break;
    }
    if (input != ChatInput::None)
    {
        chat_input(input);
    }
}

static void GameHandleKeyScroll()
{
    rct_window* mainWindow;

    mainWindow = window_get_main();
    if (mainWindow == nullptr)
        return;
    if ((mainWindow->flags & WF_NO_SCROLLING) || (gScreenFlags & (SCREEN_FLAGS_TRACK_MANAGER | SCREEN_FLAGS_TITLE_DEMO)))
        return;
    if (mainWindow->viewport == nullptr)
        return;

    rct_window* textWindow;

    textWindow = window_find_by_class(WC_TEXTINPUT);
    if (textWindow || gUsingWidgetTextBox)
        return;
    if (gChatOpen)
        return;

    const uint8_t* keysState = context_get_keys_state();
    auto scrollCoords = GetKeyboardMapScroll(keysState);

    if (scrollCoords.x != 0 || scrollCoords.y != 0)
    {
        window_unfollow_sprite(mainWindow);
    }
    InputScrollViewport(scrollCoords);
}

static int32_t InputScancodeToRCTKeycode(int32_t sdl_key)
{
    char keycode = static_cast<char>(SDL_GetKeyFromScancode(static_cast<SDL_Scancode>(sdl_key)));

    // Until we reshuffle the text files to use the new positions
    // this will suffice to move the majority to the correct positions.
    // Note any special buttons PgUp PgDwn are mapped wrong.
    if (keycode >= 'a' && keycode <= 'z')
        keycode = toupper(keycode);

    return keycode;
}

void InputHandleKeyboard(bool isTitle)
{
}
