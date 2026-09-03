/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include <cstdint>

struct ScreenCoordsXY;

namespace OpenRCT2
{
    enum class MouseState : uint32_t
    {
        released,
        leftPress,
        leftRelease,
        rightPress,
        rightRelease,
    };

    using WidgetIndex = uint16_t;

    struct WindowBase;

    extern ScreenCoordsXY gInputDragLast;

    void InputWindowPositionBegin(WindowBase& w, WidgetIndex widgetIndex, const ScreenCoordsXY& screenCoords);
    void GameHandleInput();
    void GameHandleEdgeScroll();

    void StoreMouseInput(MouseState state, const ScreenCoordsXY& screenCoords);

    bool InputIsTouchToolHeld();
    void InputCommitHeldTouchTool();
    void InputReleaseHeldTouchTool();
    void InputSetTouchGestureActive(bool active);

    void InputScrollViewport(const ScreenCoordsXY& screenCoords);
    void InputScrollViewportSmooth(const ScreenCoordsXY& screenCoords);
    void InputScrollViewportSmooth(const ScreenCoordsXY& screenCoords, WindowBase* targetWindow);
} // namespace OpenRCT2
