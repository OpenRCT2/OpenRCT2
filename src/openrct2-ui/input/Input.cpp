/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
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

#include <SDL2/SDL.h>
#include <cctype>
#include <openrct2-ui/windows/Window.h>
#include <openrct2/Context.h>
#include <openrct2/Input.h>
#include <openrct2/OpenRCT2.h>
#include <openrct2/common.h>
#include <openrct2/config/Config.h>
#include <openrct2/interface/Chat.h>
#include <openrct2/interface/Cursors.h>
#include <openrct2/interface/InteractiveConsole.h>
#include <openrct2/paint/VirtualFloor.h>

using namespace OpenRCT2::Ui;

static void HandleConsole(int32_t key)
{
    InGameConsole& console = GetInGameConsole();

    switch (key)
    {
        case SDL_SCANCODE_ESCAPE:
            console.Input(CONSOLE_INPUT_LINE_CLEAR);
            break;
        case SDL_SCANCODE_RETURN:
            console.Input(CONSOLE_INPUT_LINE_EXECUTE);
            break;
        case SDL_SCANCODE_UP:
            console.Input(CONSOLE_INPUT_HISTORY_PREVIOUS);
            break;
        case SDL_SCANCODE_DOWN:
            console.Input(CONSOLE_INPUT_HISTORY_NEXT);
            break;
        case SDL_SCANCODE_PAGEUP:
            console.Input(CONSOLE_INPUT_SCROLL_PREVIOUS);
            break;
        case SDL_SCANCODE_PAGEDOWN:
            console.Input(CONSOLE_INPUT_SCROLL_NEXT);
            break;
        default:
            break;
    }
}

static void HandleChat(int32_t key)
{
    switch (key)
    {
        case SDL_SCANCODE_ESCAPE:
            chat_input(CHAT_INPUT_CLOSE);
            break;
        case SDL_SCANCODE_RETURN:
            chat_input(CHAT_INPUT_SEND);
            break;
        default:
            break;
    }
}

static int32_t InputToRctScancode(int32_t sdl_key)
{
    int32_t keycode = SDL_GetKeyFromScancode((SDL_Scancode)sdl_key);

    // Until we reshuffle the text files to use the new positions
    // this will suffice to move the majority to the correct positions.
    // Note any special buttons PgUp PgDwn are mapped wrong.
    return (keycode >= 'a' && keycode <= 'z') ? toupper(keycode) : keycode;
}

static void HandleKeyboardModifiers()
{
    if (GetInGameConsole().IsOpen())
    {
        return;
    }

    // Handle modifier keys and key scrolling
    gInputPlaceObjectModifier = PLACE_OBJECT_MODIFIER_NONE;

    const uint8_t* keysState = context_get_keys_state();
    if (keysState[SDL_SCANCODE_LSHIFT] || keysState[SDL_SCANCODE_RSHIFT])
    {
        gInputPlaceObjectModifier |= PLACE_OBJECT_MODIFIER_SHIFT_Z;
    }

    if (keysState[SDL_SCANCODE_LCTRL] || keysState[SDL_SCANCODE_RCTRL])
    {
        gInputPlaceObjectModifier |= PLACE_OBJECT_MODIFIER_COPY_Z;
    }

    if (keysState[SDL_SCANCODE_LALT] || keysState[SDL_SCANCODE_RALT])
    {
        gInputPlaceObjectModifier |= 4;
    }

#ifdef __MACOSX__
    if (keysState[SDL_SCANCODE_LGUI] || keysState[SDL_SCANCODE_RGUI])
    {
        gInputPlaceObjectModifier |= 8;
    }
#endif
}

static void HandleScrolling()
{
    // Handle mouse scrolling
    if (input_get_state() == INPUT_STATE_NORMAL && gConfigGeneral.edge_scrolling
        && !(gInputPlaceObjectModifier & (PLACE_OBJECT_MODIFIER_SHIFT_Z | PLACE_OBJECT_MODIFIER_COPY_Z)))
    {
        input_handle_edge_scroll();
    }

    // Handle key scrolling
    rct_window* mainWindow = window_get_main();
    if (mainWindow == nullptr || mainWindow->viewport == nullptr || (mainWindow->flags & WF_NO_SCROLLING)
        || (gScreenFlags & (SCREEN_FLAGS_TRACK_MANAGER | SCREEN_FLAGS_TITLE_DEMO)))
    {
        return;
    }

    rct_window* textWindow;

    textWindow = window_find_by_class(WC_TEXTINPUT);
    if (textWindow || gUsingWidgetTextBox || gChatOpen)
    {
        return;
    }

    int32_t scrollX = 0;
    int32_t scrollY = 0;
    const uint8_t* keysState = context_get_keys_state();
    get_keyboard_map_scroll(keysState, &scrollX, &scrollY);

    if (scrollX != 0 || scrollY != 0)
    {
        window_unfollow_sprite(mainWindow);
    }

    input_scroll_viewport(scrollX, scrollY);
}

static void HandleKeyboardKey(int32_t key, bool isTitle)
{
    if (key == 255)
    {
        return;
    }

    auto& console = GetInGameConsole();

    // Reserve backtick for console
    if (key == SDL_SCANCODE_GRAVE)
    {
        if ((gConfigGeneral.debugging_tools && !context_is_input_active()) || console.IsOpen())
        {
            window_cancel_textbox();
            console.Toggle();
        }

        return;
    }

    if (console.IsOpen())
    {
        HandleConsole(key);
        return;
    }

    if (!isTitle && gChatOpen)
    {
        HandleChat(key);
        return;
    }

    key |= gInputPlaceObjectModifier << 8;

    if (rct_window* w = window_find_by_class(WC_TEXTINPUT); w != nullptr)
    {
        char keychar = static_cast<char>(InputToRctScancode(key & 0xFF));
        window_text_input_key(w, keychar);
        return;
    }

    if (gUsingWidgetTextBox)
    {
        return;
    }

    if (rct_window* w = window_find_by_class(WC_CHANGE_KEYBOARD_SHORTCUT); w != nullptr)
    {
        keyboard_shortcuts_set(key);
        window_close_by_class(WC_CHANGE_KEYBOARD_SHORTCUT);
        window_invalidate_by_class(WC_KEYBOARD_SHORTCUT_LIST);
        return;
    }

    keyboard_shortcut_handle(key);
}

void input_handle_keyboard(bool isTitle)
{
    if (gOpenRCT2Headless)
    {
        return;
    }

    if (!isTitle && !GetInGameConsole().IsOpen())
    {
        HandleScrolling();
    }

    HandleKeyboardModifiers();

    if (gConfigGeneral.virtual_floor_style != VIRTUAL_FLOOR_STYLE_OFF)
    {
        if (gInputPlaceObjectModifier & (PLACE_OBJECT_MODIFIER_COPY_Z | PLACE_OBJECT_MODIFIER_SHIFT_Z))
        {
            virtual_floor_enable();
        }
        else
        {
            virtual_floor_disable();
        }
    }

    // Handle key input
    // TODO: Make this possible without a const_cast
    auto* keysPressed = const_cast<uint8_t*>(context_get_keys_pressed());
    for (int32_t key = 0; key < 221; key++)
    {
        if (keysPressed[key])
        {
            keysPressed[key] = 0;
            HandleKeyboardKey(key, isTitle);
        }
    }
}

/**
 *
 *  rct2: 0x006ED990
 */
void input_set_mouse_cursor(int32_t cursor_id)
{
    if (gInputState == INPUT_STATE_RESIZING)
    {
        cursor_id = CURSOR_DIAGONAL_ARROWS;
    }

    context_setcurrentcursor(cursor_id);
}
