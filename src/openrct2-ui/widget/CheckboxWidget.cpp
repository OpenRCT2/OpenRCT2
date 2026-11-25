/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "CheckboxWidget.h"

#include <openrct2-ui/interface/Widget.h>
#include <openrct2/config/Config.h>
#include <openrct2/drawing/ColourMap.h>
#include <openrct2/drawing/Rectangle.h>
#include <openrct2/drawing/Text.h>
#include <openrct2/localisation/StringIds.h>

using namespace OpenRCT2::Drawing;

namespace OpenRCT2::Ui::Widgets
{
    void Checkbox::draw(Drawing::RenderTarget& rt, Widget& widget, const WidgetIndex widgetIndex, const WindowBase& w)
    {
        // Resolve the absolute ltb
        ScreenCoordsXY topLeft = w.windowPos + ScreenCoordsXY{ widget.left, widget.top };
        ScreenCoordsXY bottomRight = w.windowPos + ScreenCoordsXY{ widget.right, widget.bottom };
        ScreenCoordsXY midLeft = { topLeft.x, (topLeft.y + bottomRight.y) / 2 };

        auto colour = w.colours[widget.colour];

        // checkbox
        Rectangle::fillInset(
            rt, { midLeft - ScreenCoordsXY{ 0, 5 }, midLeft + ScreenCoordsXY{ 9, 4 } }, colour, Rectangle::BorderStyle::inset,
            Rectangle::FillBrightness::light, Rectangle::FillMode::dontLightenWhenInset);

        if (widgetIsDisabled(w, widgetIndex))
        {
            colour.flags.set(ColourFlag::inset, true);
        }

        // fill it when checkbox is pressed
        if (widgetIsPressed(w, widgetIndex))
        {
            drawText(
                rt, { midLeft - ScreenCoordsXY{ 0, 5 } }, kCheckMarkString,
                { colour.withFlag(ColourFlag::translucent, false) });
        }

        // draw the text
        if (widget.text == kStringIdNone)
            return;

        auto stringId = widget.text;
        auto ft = Formatter();
        if (widget.flags.has(WidgetFlag::textIsString))
        {
            stringId = STR_STRING;
            ft.Add<utf8*>(widget.string);
        }

        drawTextEllipsised(
            rt, w.windowPos + ScreenCoordsXY{ widget.left + 14, widget.textTop() }, widget.width() - 15, stringId, ft, colour);
    }
} // namespace OpenRCT2::Ui::Widgets
