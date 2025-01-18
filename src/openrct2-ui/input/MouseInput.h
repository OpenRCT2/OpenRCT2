/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include <openrct2/interface/Window.h>

namespace OpenRCT2
{
    enum class MouseState : uint32_t
    {
        Released,
        LeftPress,
        LeftRelease,
        RightPress,
        RightRelease
    };

    extern ScreenCoordsXY gInputDragLast;

    void InputWindowPositionBegin(WindowBase& w, WidgetIndex widgetIndex, const ScreenCoordsXY& screenCoords);
    void GameHandleInput();
    void GameHandleEdgeScroll();

    void StoreMouseInput(MouseState state, const ScreenCoordsXY& screenCoords);

    void InputScrollViewport(const ScreenCoordsXY& screenCoords);
} // namespace OpenRCT2
