/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "core/FlagHolder.hpp"
#include "core/StringTypes.h"
#include "interface/Window.h"

namespace OpenRCT2
{
    enum class InputFlag : uint8_t
    {
        widgetPressed,
        // The dropdown can stay open if the mouse is released, set on flag Dropdown::Flag::StayOpen.
        dropdownStayOpen,
        // The mouse has been released and the dropdown is still open. dropdownStayOpen is already set if this happens.
        dropdownMouseUp,
        toolActive,
        // Left click on a viewport
        leftMousePressed,
        rightMousePressed,
        allowRightMouseRemoval,
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

    struct CursorState
    {
        ScreenCoordsXY position;
        uint8_t left, middle, right, any;
        int32_t wheel;
        int32_t old;
        bool touch, touchIsDouble;
        uint32_t touchDownTimestamp;
    };

    struct TextInputSession
    {
        u8string* Buffer;      // UTF-8 string buffer, non-owning.
        size_t Length;         // Number of codepoints
        size_t MaxLength;      // Maximum length of text, Length can't be larger than this.
        size_t SelectionStart; // Selection start, in bytes
        size_t SelectionSize;  // Selection length in bytes

        const utf8* ImeBuffer; // IME UTF-8 stream
    };

    enum
    {
        CURSOR_UP = 0,
        CURSOR_DOWN = 1,
        CURSOR_CHANGED = 2,
        CURSOR_RELEASED = CURSOR_UP | CURSOR_CHANGED,
        CURSOR_PRESSED = CURSOR_DOWN | CURSOR_CHANGED,
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
