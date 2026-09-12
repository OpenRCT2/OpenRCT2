/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include <openrct2-ui/interface/Window.h>

namespace OpenRCT2::Ui::Widgets
{
    struct Caption : Widget
    {
        static constexpr auto kWidgetType = WidgetType::caption;

        constexpr Caption(
            ScreenCoordsXY origin, ScreenSize size, WindowColour colour_, StringId content_ = kStringIdNone,
            StringId tooltip_ = kStringIdNone)
            : Widget(origin, size, kWidgetType, colour_, content_, tooltip_)
        {
            events.draw = &draw;
        }

        static void draw(
            Drawing::RenderTarget& RenderTarget, Widget& widget, const WidgetIndex widgetIndex, const WindowBase& window);
    };

} // namespace OpenRCT2::Ui::Widgets
