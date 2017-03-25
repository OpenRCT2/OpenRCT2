#pragma region Copyright (c) 2014-2016 OpenRCT2 Developers
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

#include <exception>
#include "config/Config.h"
#include "Context.h"
#include "OpenRCT2.h"
#include "ui/UiContext.h"

using namespace OpenRCT2;
using namespace OpenRCT2::Ui;

namespace OpenRCT2
{
    class Context : public IContext
    {
    private:
        IUiContext * const _uiContext;

    public:
        // Singleton of Context.
        // Remove this when GetContext() is no longer called so that
        // multiple instances can be created in parallel
        static Context * Instance;

    public:
        Context(IUiContext * uiContext)
            : _uiContext(uiContext)
        {
            Instance = this;
        }

        ~Context() override
        {
            Instance = nullptr;
        }

        IUiContext * GetUiContext() override
        {
            return _uiContext;
        }

        sint32 RunOpenRCT2(int argc, char * * argv) override
        {
            core_init();
            int runGame = cmdline_run((const char * *)argv, argc);
            if (runGame == 1)
            {
                openrct2_launch();
            }
            return gExitCode;
        }
    };

    Context * Context::Instance = nullptr;

    IContext * CreateContext()
    {
        return new Context(nullptr);
    }

    IContext * CreateContext(IUiContext * uiContext)
    {
        return new Context(uiContext);
    }

    IContext * GetContext()
    {
        return Context::Instance;
    }
}

extern "C"
{
    void context_setcurrentcursor(sint32 cursor)
    {
        GetContext()->GetUiContext()->SetCursor((CURSOR_ID)cursor);
    }

    void context_hide_cursor()
    {
        GetContext()->GetUiContext()->SetCursorVisible(false);
    }

    void context_show_cursor()
    {
        GetContext()->GetUiContext()->SetCursorVisible(true);
    }

    void context_get_cursor_position(sint32 * x, sint32 * y)
    {
        GetContext()->GetUiContext()->GetCursorPosition(x, y);
    }

    void context_get_cursor_position_scaled(sint32 * x, sint32 * y)
    {
        context_get_cursor_position(x, y);

        // Compensate for window scaling.
        *x = (sint32)ceilf(*x / gConfigGeneral.window_scale);
        *y = (sint32)ceilf(*y / gConfigGeneral.window_scale);
    }

    void context_set_cursor_position(sint32 x, sint32 y)
    {
        GetContext()->GetUiContext()->SetCursorPosition(x, y);
    }

    const CursorState * context_get_cursor_state()
    {
        return GetContext()->GetUiContext()->GetCursorState();
    }

    const uint8 * context_get_keys_state()
    {
        return GetContext()->GetUiContext()->GetKeysState();
    }

    const uint8 * context_get_keys_pressed()
    {
        return GetContext()->GetUiContext()->GetKeysPressed();
    }

    void context_start_text_input(utf8 * buffer, size_t maxLength)
    {
        GetContext()->GetUiContext()->StartTextInput(buffer, maxLength);
    }

    void context_stop_text_input()
    {
        GetContext()->GetUiContext()->StopTextInput();
    }

    bool context_is_input_active()
    {
        return GetContext()->GetUiContext()->IsTextInputActive();
    }
}
