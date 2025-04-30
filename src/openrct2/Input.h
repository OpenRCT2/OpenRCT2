/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "core/FlagHolder.hpp"
#include "interface/Window.h"

namespace OpenRCT2
{
    enum InputFlag
    {
        widgetPressed,

        // The dropdown can stay open if the mouse is released, set on flag Dropdown::Flag::StayOpen.
        dropdownStayOpen,

        // The mouse has been released and the dropdown is still open.
        // InputFlag::dropdownStayOpen is already set if this happens.
        dropdownMouseUp,

        toolActive,

        // Left click on a viewport
        unk4,

        unk5,

        // Some of the map tools (clear, footpath, scenery)
        // never read as far as I know.
        unk6,

        viewportScrolling,
    };
    using InputFlags = FlagHolder<uint8_t, InputFlag>;

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
    extern InputFlags gInputFlags;
    extern uint32_t _tooltipNotShownTimeout;

    void TitleHandleKeyboardInput();
    void GameHandleKeyboardInput();

    void InputSetState(InputState state);
    InputState InputGetState();

    void ResetTooltipNotShown();
} // namespace OpenRCT2
