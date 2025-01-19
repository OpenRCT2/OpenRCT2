/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "interface/Window.h"

namespace OpenRCT2
{
    enum INPUT_FLAGS
    {
        INPUT_FLAG_WIDGET_PRESSED = (1 << 0),

        // The dropdown can stay open if the mouse is released, set on flag Dropdown::Flag::StayOpen
        INPUT_FLAG_DROPDOWN_STAY_OPEN = (1 << 1),

        // The mouse has been released and the dropdown is still open
        // INPUT_FLAG_DROPDOWN_STAY_OPEN is already set if this happens
        INPUT_FLAG_DROPDOWN_MOUSE_UP = (1 << 2),

        INPUT_FLAG_TOOL_ACTIVE = (1 << 3),

        // Left click on a viewport
        INPUT_FLAG_4 = (1 << 4),

        INPUT_FLAG_5 = (1 << 5),

        // Some of the map tools (clear, footpath, scenery)
        // never read as far as I know.
        INPUT_FLAG_6 = (1 << 6),

        INPUT_FLAG_VIEWPORT_SCROLLING = (1 << 7)
    };

    enum class InputState
    {
        Reset,
        Normal,
        WidgetPressed,
        PositioningWindow,
        ViewportRight,
        DropdownActive,
        ViewportLeft,
        ScrollLeft,
        Resizing,
        ScrollRight
    };

    extern WidgetRef gHoverWidget;
    extern WidgetRef gPressedWidget;

    extern uint32_t gTooltipCloseTimeout;
    extern WidgetRef gTooltipWidget;
    extern ScreenCoordsXY gTooltipCursor;

    // TODO: Move to openrct2-ui and make static again
    extern InputState _inputState;
    extern uint8_t _inputFlags;
    extern uint32_t _tooltipNotShownTimeout;

    void TitleHandleKeyboardInput();
    void GameHandleKeyboardInput();

    void InputSetFlag(INPUT_FLAGS flag, bool on);
    bool InputTestFlag(INPUT_FLAGS flag);
    void InputResetFlags();

    void InputSetState(InputState state);
    InputState InputGetState();

    void ResetTooltipNotShown();
} // namespace OpenRCT2
