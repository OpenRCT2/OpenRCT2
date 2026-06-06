/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "CaptionWidget.h"

#include <openrct2/config/Config.h>
#include <openrct2/drawing/ColourMap.h>
#include <openrct2/drawing/Rectangle.h>
#include <openrct2/drawing/Text.h>
#include <openrct2/localisation/StringIds.h>

using namespace OpenRCT2::Drawing;

namespace OpenRCT2::Ui::Widgets
{
    void Caption::draw(Drawing::RenderTarget& rt, const Widget& widget, const WidgetIndex widgetIndex, const WindowBase& w)
    {
        // Resolve the absolute ltrb
        auto topLeft = w.windowPos + ScreenCoordsXY{ widget.left, widget.top };
        auto bottomRight = w.windowPos + ScreenCoordsXY{ widget.right, widget.bottom };

        auto colour = w.colours[widget.colour];

        auto brightness = Drawing::Rectangle::FillBrightness::light;
        if (w.flags.has(WindowFlag::higherContrastOnPress))
            brightness = Rectangle::FillBrightness::dark;

        Rectangle::fillInset(
            rt, { topLeft, bottomRight }, colour, Rectangle::BorderStyle::inset, brightness,
            Rectangle::FillMode::dontLightenWhenInset);

        // Black caption bars look slightly green, this fixes that
        if (colour.colour == Drawing::Colour::black)
            Rectangle::fill(
                rt, { { topLeft + ScreenCoordsXY{ 1, 1 } }, { bottomRight - ScreenCoordsXY{ 1, 1 } } },
                getColourMap(colour.colour).dark);
        else
            Rectangle::filter(
                rt, { { topLeft + ScreenCoordsXY{ 1, 1 } }, { bottomRight - ScreenCoordsXY{ 1, 1 } } },
                FilterPaletteID::paletteDarken3);

        // Draw text
        if (!widget.flags.has(WidgetFlag::textIsString) && widget.text == kStringIdNone)
            return;

        topLeft = w.windowPos + ScreenCoordsXY{ widget.left + 2, widget.top + 1 };
        int32_t width = widget.width() - 5;

        if (static_cast<size_t>(widgetIndex + 1) < w.widgets.size()
            && (w.widgets[widgetIndex + 1]).type == WidgetType::closeBox)
        {
            width -= kCloseButtonSize;
            if (static_cast<size_t>(widgetIndex + 2) < w.widgets.size()
                && (w.widgets[widgetIndex + 2]).type == WidgetType::closeBox)
                width -= kCloseButtonSize;
        }
        topLeft.x += width / 2;
        if (Config::Get().interface.windowButtonsOnTheLeft)
            topLeft.x += kCloseButtonSize;
        if (Config::Get().interface.enlargedUi)
            topLeft.y += kTitleHeightLarge / 4;

        Formatter ft{};
        bool hasStringPtr = widget.flags.has(WidgetFlag::textIsString);
        auto formatString = widget.text;
        if (hasStringPtr)
        {
            formatString = STR_STRING;
            ft.Add<const utf8*>(widget.string);
        }

        drawTextEllipsised(
            rt, topLeft, width, formatString, ft,
            { ColourWithFlags{ Drawing::Colour::white }.withFlag(ColourFlag::withOutline, true), TextAlignment::centre });
    }
} // namespace OpenRCT2::Ui::Widgets
