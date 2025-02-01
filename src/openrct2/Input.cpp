/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "Input.h"

#include "Context.h"
#include "Game.h"

namespace OpenRCT2
{
    InputState _inputState;
    uint8_t _inputFlags;

    WidgetRef gHoverWidget;
    WidgetRef gPressedWidget;

    uint32_t _tooltipNotShownTimeout;

    /**
     *
     *  rct2: 0x006E3B43
     */
    void TitleHandleKeyboardInput()
    {
        ContextInputHandleKeyboard(true);
    }

    /**
     *
     *  rct2: 0x006E3B43
     */
    void GameHandleKeyboardInput()
    {
        ContextInputHandleKeyboard(false);
    }

    void InputSetFlag(INPUT_FLAGS flag, bool on)
    {
        if (on)
        {
            _inputFlags |= flag;
        }
        else
        {
            _inputFlags &= ~flag;
        }
    }

    bool InputTestFlag(INPUT_FLAGS flag)
    {
        return _inputFlags & flag;
    }

    void InputResetFlags()
    {
        _inputFlags = 0;
    }

    void InputSetState(InputState state)
    {
        _inputState = state;
    }

    InputState InputGetState()
    {
        return _inputState;
    }

    void ResetTooltipNotShown()
    {
        _tooltipNotShownTimeout = gCurrentRealTimeTicks + 50;
    }
} // namespace OpenRCT2
