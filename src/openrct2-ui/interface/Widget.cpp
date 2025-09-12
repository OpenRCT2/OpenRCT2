/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "Widget.h"

#include <algorithm>
#include <cmath>
#include <openrct2-ui/UiStringIds.h>
#include <openrct2-ui/windows/Windows.h>
#include <openrct2/Diagnostic.h>
#include <openrct2/Game.h>
#include <openrct2/Input.h>
#include <openrct2/SpriteIds.h>
#include <openrct2/config/Config.h>
#include <openrct2/drawing/Text.h>
#include <openrct2/localisation/Formatter.h>
#include <openrct2/localisation/Formatting.h>
#include <openrct2/localisation/StringIds.h>

using namespace OpenRCT2;

namespace OpenRCT2::Ui
{
    static void WidgetFrameDraw(RenderTarget& rt, WindowBase& w, WidgetIndex widgetIndex);
    static void WidgetResizeDraw(RenderTarget& rt, WindowBase& w, WidgetIndex widgetIndex);
    static void WidgetButtonDraw(RenderTarget& rt, WindowBase& w, WidgetIndex widgetIndex);
    static void WidgetTabDraw(RenderTarget& rt, WindowBase& w, WidgetIndex widgetIndex);
    static void WidgetFlatButtonDraw(RenderTarget& rt, WindowBase& w, WidgetIndex widgetIndex);
    static void WidgetTextButton(RenderTarget& rt, WindowBase& w, WidgetIndex widgetIndex);
    static void WidgetTextCentred(RenderTarget& rt, WindowBase& w, WidgetIndex widgetIndex);
    static void WidgetText(RenderTarget& rt, WindowBase& w, WidgetIndex widgetIndex);
    static void WidgetTextInset(RenderTarget& rt, WindowBase& w, WidgetIndex widgetIndex);
    static void WidgetTextBoxDraw(RenderTarget& rt, WindowBase& w, WidgetIndex widgetIndex);
    static void WidgetProgressBarDraw(RenderTarget& rt, WindowBase& w, WidgetIndex widgetIndex);
    static void WidgetHorizontalSeparatorDraw(RenderTarget& rt, WindowBase& w, const Widget& widget);
    static void WidgetGroupboxDraw(RenderTarget& rt, WindowBase& w, WidgetIndex widgetIndex);
    static void WidgetCaptionDraw(RenderTarget& rt, WindowBase& w, WidgetIndex widgetIndex);
    static void WidgetCheckboxDraw(RenderTarget& rt, WindowBase& w, WidgetIndex widgetIndex);
    static void WidgetCloseboxDraw(RenderTarget& rt, WindowBase& w, WidgetIndex widgetIndex);
    static void WidgetScrollDraw(RenderTarget& rt, WindowBase& w, WidgetIndex widgetIndex);
    static void WidgetHScrollbarDraw(
        RenderTarget& rt, const ScrollArea& scroll, int32_t l, int32_t t, int32_t r, int32_t b, ColourWithFlags colour);
    static void WidgetVScrollbarDraw(
        RenderTarget& rt, const ScrollArea& scroll, int32_t l, int32_t t, int32_t r, int32_t b, ColourWithFlags colour);
    static void WidgetDrawImage(RenderTarget& rt, WindowBase& w, WidgetIndex widgetIndex);

    /**
     *
     *  rct2: 0x006EB2A8
     */
    void widgetDraw(RenderTarget& rt, WindowBase& w, WidgetIndex widgetIndex)
    {
        const auto* widget = getWidgetByIndex(w, widgetIndex);
        if (widget == nullptr)
        {
            LOG_ERROR("Tried drawing an out-of-bounds widget index!");
            return;
        }

        switch (widget->type)
        {
            case WidgetType::frame:
                WidgetFrameDraw(rt, w, widgetIndex);
                break;
            case WidgetType::resize:
                WidgetResizeDraw(rt, w, widgetIndex);
                break;
            case WidgetType::imgBtn:
                WidgetButtonDraw(rt, w, widgetIndex);
                break;
            case WidgetType::colourBtn:
            case WidgetType::trnBtn:
            case WidgetType::tab:
                WidgetTabDraw(rt, w, widgetIndex);
                break;
            case WidgetType::flatBtn:
                WidgetFlatButtonDraw(rt, w, widgetIndex);
                break;
            case WidgetType::button:
            case WidgetType::tableHeader:
                WidgetTextButton(rt, w, widgetIndex);
                break;
            case WidgetType::labelCentred:
                WidgetTextCentred(rt, w, widgetIndex);
                break;
            case WidgetType::label:
                WidgetText(rt, w, widgetIndex);
                break;
            case WidgetType::spinner:
            case WidgetType::dropdownMenu:
            case WidgetType::viewport:
                WidgetTextInset(rt, w, widgetIndex);
                break;
            case WidgetType::groupbox:
                WidgetGroupboxDraw(rt, w, widgetIndex);
                break;
            case WidgetType::caption:
                WidgetCaptionDraw(rt, w, widgetIndex);
                break;
            case WidgetType::closeBox:
                WidgetCloseboxDraw(rt, w, widgetIndex);
                break;
            case WidgetType::scroll:
                WidgetScrollDraw(rt, w, widgetIndex);
                break;
            case WidgetType::checkbox:
                WidgetCheckboxDraw(rt, w, widgetIndex);
                break;
            case WidgetType::textBox:
                WidgetTextBoxDraw(rt, w, widgetIndex);
                break;
            case WidgetType::progressBar:
                WidgetProgressBarDraw(rt, w, widgetIndex);
                break;
            case WidgetType::horizontalSeparator:
                WidgetHorizontalSeparatorDraw(rt, w, *widget);
                break;
            default:
                break;
        }
    }

    /**
     *
     *  rct2: 0x006EB6CE
     */
    static void WidgetFrameDraw(RenderTarget& rt, WindowBase& w, WidgetIndex widgetIndex)
    {
        // Get the widget
        const auto& widget = w.widgets[widgetIndex];

        // Resolve the absolute ltrb
        auto leftTop = w.windowPos + ScreenCoordsXY{ widget.left, widget.top };
        int32_t r = w.windowPos.x + widget.right;
        int32_t b = w.windowPos.y + widget.bottom;

        //
        uint8_t press = ((w.flags & WF_10) ? INSET_RECT_FLAG_FILL_MID_LIGHT : 0);

        auto colour = w.colours[widget.colour];

        // Draw the frame
        GfxFillRectInset(rt, { leftTop, { r, b } }, colour, press);

        if (!w.canBeResized())
            return;

        // Draw the resize sprite at the bottom right corner
        leftTop = w.windowPos + ScreenCoordsXY{ widget.right - 18, widget.bottom - 18 };
        GfxDrawSprite(rt, ImageId(SPR_RESIZE, colour.colour), leftTop);
    }

    /**
     *
     *  rct2: 0x006EB765
     */
    static void WidgetResizeDraw(RenderTarget& rt, WindowBase& w, WidgetIndex widgetIndex)
    {
        // Get the widget
        const auto& widget = w.widgets[widgetIndex];

        // Resolve the absolute ltrb
        auto leftTop = w.windowPos + ScreenCoordsXY{ widget.left, widget.top };
        int32_t r = w.windowPos.x + widget.right;
        int32_t b = w.windowPos.y + widget.bottom;

        auto colour = w.colours[widget.colour];

        // Draw the panel
        GfxFillRectInset(rt, { leftTop, { r, b } }, colour, 0);

        if (!w.canBeResized())
            return;

        // Draw the resize sprite at the bottom right corner
        leftTop = w.windowPos + ScreenCoordsXY{ widget.right - 18, widget.bottom - 18 };
        GfxDrawSprite(rt, ImageId(SPR_RESIZE, colour.colour), leftTop);
    }

    /**
     *
     *  rct2: 0x006EB8E5
     */
    static void WidgetButtonDraw(RenderTarget& rt, WindowBase& w, WidgetIndex widgetIndex)
    {
        // Get the widget
        const auto& widget = w.widgets[widgetIndex];

        // Resolve the absolute ltrb
        ScreenRect rect{ w.windowPos + ScreenCoordsXY{ widget.left, widget.top },
                         w.windowPos + ScreenCoordsXY{ widget.right, widget.bottom } };

        // Check if the button is pressed down
        uint8_t press = widgetIsPressed(w, widgetIndex) || isToolActive(w, widgetIndex) ? INSET_RECT_FLAG_BORDER_INSET : 0;

        auto colour = w.colours[widget.colour];

        // Dead code?
        if (static_cast<int32_t>(widget.image.GetIndex()) == -2)
        {
            // Draw border with no fill
            GfxFillRectInset(rt, rect, colour, press | INSET_RECT_FLAG_FILL_NONE);
            return;
        }

        // Draw the border with fill
        GfxFillRectInset(rt, rect, colour, press);

        WidgetDrawImage(rt, w, widgetIndex);
    }

    /**
     *
     *  rct2: 0x006EB806
     */
    static void WidgetTabDraw(RenderTarget& rt, WindowBase& w, WidgetIndex widgetIndex)
    {
        // Get the widget
        auto& widget = w.widgets[widgetIndex];

        if (widget.type != WidgetType::tab && widget.image.GetIndex() == kImageIndexUndefined)
            return;

        if (widget.type == WidgetType::tab)
        {
            if (widgetIsDisabled(w, widgetIndex))
                return;

            if (widget.image.GetIndex() == kImageIndexUndefined)
            {
                // Set standard tab sprite to use.
                widget.image = ImageId(SPR_TAB, FilterPaletteID::PaletteNull);
            }
        }

        // Draw widgets that aren't explicitly disabled.
        if (!widgetIsDisabled(w, widgetIndex))
        {
            WidgetDrawImage(rt, w, widgetIndex);
            return;
        }

        if (widget.type != WidgetType::trnBtn)
        {
            WidgetDrawImage(rt, w, widgetIndex);
            return;
        }

        // Resolve the absolute ltrb
        auto leftTop = w.windowPos + ScreenCoordsXY{ widget.left, widget.top };

        // Get the colour and disabled image
        auto colour = w.colours[widget.colour].colour;
        const auto newIndex = widget.image.GetIndex() + 2;
        auto image = widget.image.WithIndex(newIndex).WithPrimary(colour);

        // Draw disabled image
        GfxDrawSprite(rt, image, leftTop);
    }

    /**
     *
     *  rct2: 0x006EB861
     */
    static void WidgetFlatButtonDraw(RenderTarget& rt, WindowBase& w, WidgetIndex widgetIndex)
    {
        if (!widgetIsDisabled(w, widgetIndex) && widgetIsHighlighted(w, widgetIndex))
        {
            WidgetButtonDraw(rt, w, widgetIndex);
            return;
        }

        // Get the widget
        const auto& widget = w.widgets[widgetIndex];

        // Resolve the absolute ltrb
        ScreenRect rect{ w.windowPos + ScreenCoordsXY{ widget.left, widget.top },
                         w.windowPos + ScreenCoordsXY{ widget.right, widget.bottom } };

        auto colour = w.colours[widget.colour];

        // Check if the button is pressed down
        if (widgetIsPressed(w, widgetIndex) || isToolActive(w, widgetIndex))
        {
            // Dead code?
            if (static_cast<int32_t>(widget.image.GetIndex()) == -2)
            {
                // Draw border with no fill
                GfxFillRectInset(rt, rect, colour, INSET_RECT_FLAG_BORDER_INSET | INSET_RECT_FLAG_FILL_NONE);
                return;
            }

            // Draw the border with fill
            GfxFillRectInset(rt, rect, colour, INSET_RECT_FLAG_BORDER_INSET);
        }

        // Draw image
        WidgetDrawImage(rt, w, widgetIndex);
    }

    /**
     *
     *  rct2: 0x006EBBEB
     */
    static void WidgetTextButton(RenderTarget& rt, WindowBase& w, WidgetIndex widgetIndex)
    {
        // Get the widget
        const auto& widget = w.widgets[widgetIndex];

        // Resolve the absolute ltrb
        ScreenRect rect{ w.windowPos + ScreenCoordsXY{ widget.left, widget.top },
                         w.windowPos + ScreenCoordsXY{ widget.right, widget.bottom } };

        auto colour = w.colours[widget.colour];

        // Border
        uint8_t press = widgetIsPressed(w, widgetIndex) || isToolActive(w, widgetIndex) ? INSET_RECT_FLAG_BORDER_INSET : 0;
        GfxFillRectInset(rt, rect, colour, press);

        // Button caption
        if (widget.type != WidgetType::tableHeader)
        {
            WidgetTextCentred(rt, w, widgetIndex);
        }
        else
        {
            WidgetText(rt, w, widgetIndex);
        }
    }

    /**
     *
     *  rct2: 0x006EBC41
     */
    static void WidgetTextCentred(RenderTarget& rt, WindowBase& w, WidgetIndex widgetIndex)
    {
        // Get the widget
        const auto& widget = w.widgets[widgetIndex];

        if (widget.text == kStringIdNone)
            return;

        auto colour = w.colours[widget.colour];
        colour.setFlag(ColourFlag::translucent, false);
        if (widgetIsDisabled(w, widgetIndex))
            colour.setFlag(ColourFlag::inset, true);

        // Resolve the absolute ltrb
        auto topLeft = w.windowPos + ScreenCoordsXY{ widget.left, 0 };
        int32_t r = w.windowPos.x + widget.right;

        if (widget.type == WidgetType::button || widget.type == WidgetType::tableHeader)
            topLeft.y += widget.textTop();
        else
            topLeft.y += widget.top;

        auto stringId = widget.text;
        auto ft = Formatter::Common();
        if (widget.flags.has(WidgetFlag::textIsString))
        {
            stringId = STR_STRING;
            ft.Add<utf8*>(widget.string);
        }

        ScreenCoordsXY coords = { (topLeft.x + r + 1) / 2 - 1, topLeft.y };
        if (widget.type == WidgetType::labelCentred)
        {
            DrawTextWrapped(rt, coords, widget.width() - 2, stringId, ft, { colour, TextAlignment::CENTRE });
        }
        else
        {
            DrawTextEllipsised(rt, coords, widget.width() - 2, stringId, ft, { colour, TextAlignment::CENTRE });
        }
    }

    /**
     *
     *  rct2: 0x006EBD52
     */
    static void WidgetText(RenderTarget& rt, WindowBase& w, WidgetIndex widgetIndex)
    {
        // Get the widget
        const auto& widget = w.widgets[widgetIndex];

        if (widget.text == kStringIdNone || widget.content == kWidgetContentEmpty)
            return;

        auto colour = w.colours[widget.colour];
        if (widgetIsDisabled(w, widgetIndex))
            colour.setFlag(ColourFlag::inset, true);

        // Resolve the absolute ltrb
        int32_t l = w.windowPos.x + widget.left;
        int32_t r = w.windowPos.x + widget.right;
        int32_t t;

        if (widget.type == WidgetType::button || widget.type == WidgetType::dropdownMenu || widget.type == WidgetType::spinner
            || widget.type == WidgetType::tableHeader)
        {
            t = w.windowPos.y + widget.textTop();
        }
        else
            t = w.windowPos.y + widget.top;

        auto stringId = widget.text;
        auto ft = Formatter::Common();
        if (widget.flags.has(WidgetFlag::textIsString))
        {
            stringId = STR_STRING;
            ft.Add<utf8*>(widget.string);
        }

        ScreenCoordsXY coords = { l + 1, t };
        if (widget.type == WidgetType::labelCentred)
        {
            DrawTextWrapped(rt, coords, r - l, stringId, ft, { colour, TextAlignment::CENTRE });
        }
        else
        {
            DrawTextEllipsised(rt, coords, r - l, stringId, ft, colour);
        }
    }

    /**
     *
     *  rct2: 0x006EBD1F
     */
    static void WidgetTextInset(RenderTarget& rt, WindowBase& w, WidgetIndex widgetIndex)
    {
        // Get the widget
        const auto& widget = w.widgets[widgetIndex];

        // Resolve the absolute ltrb
        ScreenRect rect{ w.windowPos + ScreenCoordsXY{ widget.left, widget.top },
                         w.windowPos + ScreenCoordsXY{ widget.right, widget.bottom } };

        auto colour = w.colours[widget.colour];

        GfxFillRectInset(rt, rect, colour, INSET_RECT_F_60);
        WidgetText(rt, w, widgetIndex);
    }

    /**
     *
     *  rct2: 0x006EB535
     */
    static void WidgetGroupboxDraw(RenderTarget& rt, WindowBase& w, WidgetIndex widgetIndex)
    {
        // Get the widget
        const auto& widget = w.widgets[widgetIndex];

        // Resolve the absolute ltrb
        auto l = w.windowPos.x + widget.left + 5;
        auto t = w.windowPos.y + widget.top;
        auto textRight = l;

        // Text
        auto stringId = widget.text;
        auto rawFt = Formatter::Common();
        if (widget.flags.has(WidgetFlag::textIsString))
        {
            if (widget.string != nullptr && widget.string[0] != '\0')
            {
                stringId = STR_STRING;
                rawFt.Add<utf8*>(widget.string);
            }
            else
            {
                stringId = kStringIdNone;
            }
        }

        if (stringId != kStringIdNone)
        {
            auto colour = w.colours[widget.colour].withFlag(ColourFlag::translucent, false);
            if (widgetIsDisabled(w, widgetIndex))
                colour.setFlag(ColourFlag::inset, true);

            utf8 buffer[512] = { 0 };
            OpenRCT2::FormatStringLegacy(buffer, sizeof(buffer), stringId, rawFt.Data());

            auto ft = Formatter();
            ft.Add<utf8*>(buffer);
            DrawTextBasic(rt, { l, t }, STR_STRING, ft, { colour });
            textRight = l + GfxGetStringWidth(buffer, FontStyle::Medium) + 1;
        }

        // Border
        // Resolve the absolute ltrb
        l = w.windowPos.x + widget.left;
        t = w.windowPos.y + widget.top + 4;
        const auto r = w.windowPos.x + widget.right;
        const auto b = w.windowPos.y + widget.bottom;

        uint8_t colour = w.colours[widget.colour].colour;

        // Border left of text
        GfxFillRect(rt, { { l, t }, { l + 4, t } }, ColourMapA[colour].mid_dark);
        GfxFillRect(rt, { { l + 1, t + 1 }, { l + 4, t + 1 } }, ColourMapA[colour].lighter);

        // Border right of text
        GfxFillRect(rt, { { textRight, t }, { r - 1, t } }, ColourMapA[colour].mid_dark);
        GfxFillRect(rt, { { textRight, t + 1 }, { r - 2, t + 1 } }, ColourMapA[colour].lighter);

        // Border right
        GfxFillRect(rt, { { r - 1, t + 1 }, { r - 1, b - 1 } }, ColourMapA[colour].mid_dark);
        GfxFillRect(rt, { { r, t }, { r, b } }, ColourMapA[colour].lighter);

        // Border bottom
        GfxFillRect(rt, { { l, b - 1 }, { r - 2, b - 1 } }, ColourMapA[colour].mid_dark);
        GfxFillRect(rt, { { l, b }, { r - 1, b } }, ColourMapA[colour].lighter);

        // Border left
        GfxFillRect(rt, { { l, t + 1 }, { l, b - 2 } }, ColourMapA[colour].mid_dark);
        GfxFillRect(rt, { { l + 1, t + 2 }, { l + 1, b - 2 } }, ColourMapA[colour].lighter);
    }

    /**
     *
     *  rct2: 0x006EB2F9
     */
    static void WidgetCaptionDraw(RenderTarget& rt, WindowBase& w, WidgetIndex widgetIndex)
    {
        // Get the widget
        const auto* widget = &w.widgets[widgetIndex];

        // Resolve the absolute ltrb
        auto topLeft = w.windowPos + ScreenCoordsXY{ widget->left, widget->top };
        auto bottomRight = w.windowPos + ScreenCoordsXY{ widget->right, widget->bottom };

        auto colour = w.colours[widget->colour];

        uint8_t press = INSET_RECT_F_60;
        if (w.flags & WF_10)
            press |= INSET_RECT_FLAG_FILL_MID_LIGHT;

        GfxFillRectInset(rt, { topLeft, bottomRight }, colour, press);

        // Black caption bars look slightly green, this fixes that
        if (colour.colour == COLOUR_BLACK)
            GfxFillRect(
                rt, { { topLeft + ScreenCoordsXY{ 1, 1 } }, { bottomRight - ScreenCoordsXY{ 1, 1 } } },
                ColourMapA[colour.colour].dark);
        else
            GfxFilterRect(
                rt, { { topLeft + ScreenCoordsXY{ 1, 1 } }, { bottomRight - ScreenCoordsXY{ 1, 1 } } },
                FilterPaletteID::PaletteDarken3);

        // Draw text
        if (widget->text == kStringIdNone)
            return;

        topLeft = w.windowPos + ScreenCoordsXY{ widget->left + 2, widget->top + 1 };
        int32_t width = widget->width() - 4;

        if (static_cast<size_t>(widgetIndex + 1) < w.widgets.size()
            && (w.widgets[widgetIndex + 1]).type == WidgetType::closeBox)
        {
            width -= kCloseButtonSize;
            if (static_cast<size_t>(widgetIndex + 2) < w.widgets.size()
                && (w.widgets[widgetIndex + 2]).type == WidgetType::closeBox)
                width -= kCloseButtonSize;
        }
        topLeft.x += width / 2;
        if (Config::Get().interface.WindowButtonsOnTheLeft)
            topLeft.x += kCloseButtonSize;
        if (Config::Get().interface.EnlargedUi)
            topLeft.y += kTitleHeightLarge / 4;

        DrawTextEllipsised(
            rt, topLeft, width, widget->text, Formatter::Common(),
            { ColourWithFlags{ COLOUR_WHITE }.withFlag(ColourFlag::withOutline, true), TextAlignment::CENTRE });
    }

    /**
     *
     *  rct2: 0x006EBB85
     */
    static void WidgetCloseboxDraw(RenderTarget& rt, WindowBase& w, WidgetIndex widgetIndex)
    {
        // Get the widget
        const auto& widget = w.widgets[widgetIndex];

        // Resolve the absolute ltrb
        auto topLeft = w.windowPos + ScreenCoordsXY{ widget.left, widget.top };
        auto bottomRight = w.windowPos + ScreenCoordsXY{ widget.right, widget.bottom };

        // Check if the button is pressed down
        uint8_t press = 0;
        if (w.flags & WF_10)
            press |= INSET_RECT_FLAG_FILL_MID_LIGHT;
        if (widgetIsPressed(w, widgetIndex) || isToolActive(w, widgetIndex))
            press |= INSET_RECT_FLAG_BORDER_INSET;

        auto colour = w.colours[widget.colour];

        // Draw the button
        GfxFillRectInset(rt, { topLeft, bottomRight }, colour, press);

        if (widget.string == nullptr)
            return;

        topLeft = w.windowPos + ScreenCoordsXY{ widget.midX() - 1, std::max<int32_t>(widget.top, widget.midY() - 5) };

        if (widgetIsDisabled(w, widgetIndex))
            colour.setFlag(ColourFlag::inset, true);

        DrawText(rt, topLeft, { colour, TextAlignment::CENTRE }, widget.string);
    }

    /**
     *
     *  rct2: 0x006EBAD9
     */
    static void WidgetCheckboxDraw(RenderTarget& rt, WindowBase& w, WidgetIndex widgetIndex)
    {
        // Get the widget
        const auto& widget = w.widgets[widgetIndex];

        // Resolve the absolute ltb
        ScreenCoordsXY topLeft = w.windowPos + ScreenCoordsXY{ widget.left, widget.top };
        ScreenCoordsXY bottomRight = w.windowPos + ScreenCoordsXY{ widget.right, widget.bottom };
        ScreenCoordsXY midLeft = { topLeft.x, (topLeft.y + bottomRight.y) / 2 };

        auto colour = w.colours[widget.colour];

        // checkbox
        GfxFillRectInset(rt, { midLeft - ScreenCoordsXY{ 0, 5 }, midLeft + ScreenCoordsXY{ 9, 4 } }, colour, INSET_RECT_F_60);

        if (widgetIsDisabled(w, widgetIndex))
        {
            colour.setFlag(ColourFlag::inset, true);
        }

        // fill it when checkbox is pressed
        if (widgetIsPressed(w, widgetIndex))
        {
            DrawText(
                rt, { midLeft - ScreenCoordsXY{ 0, 5 } }, { colour.withFlag(ColourFlag::translucent, false) },
                kCheckMarkString);
        }

        // draw the text
        if (widget.text == kStringIdNone)
            return;

        auto stringId = widget.text;
        auto ft = Formatter::Common();
        if (widget.flags.has(WidgetFlag::textIsString))
        {
            stringId = STR_STRING;
            ft.Add<utf8*>(widget.string);
        }

        DrawTextEllipsised(
            rt, w.windowPos + ScreenCoordsXY{ widget.left + 14, widget.textTop() }, widget.width() - 14, stringId, ft, colour);
    }

    /**
     *
     *  rct2: 0x006EBD96
     */
    static void WidgetScrollDraw(RenderTarget& rt, WindowBase& w, WidgetIndex widgetIndex)
    {
        // Get the widget
        int32_t scrollIndex = WindowGetScrollDataIndex(w, widgetIndex);
        const auto& widget = w.widgets[widgetIndex];
        const auto& scroll = w.scrolls[scrollIndex];

        // Resolve the absolute ltrb
        ScreenCoordsXY topLeft = w.windowPos + ScreenCoordsXY{ widget.left, widget.top };
        ScreenCoordsXY bottomRight = w.windowPos + ScreenCoordsXY{ widget.right, widget.bottom };

        auto colour = w.colours[widget.colour];

        // Draw the border
        GfxFillRectInset(rt, { topLeft, bottomRight }, colour, INSET_RECT_F_60);

        // Inflate by -1
        topLeft.x++;
        topLeft.y++;
        bottomRight.x--;
        bottomRight.y--;

        bool hScrollNeeded = scroll.contentWidth > widget.width() && (scroll.flags & HSCROLLBAR_VISIBLE);
        bool vScrollNeeded = scroll.contentHeight > widget.height() && (scroll.flags & VSCROLLBAR_VISIBLE);

        // Horizontal scrollbar
        if (hScrollNeeded)
        {
            WidgetHScrollbarDraw(
                rt, scroll, topLeft.x, bottomRight.y - kScrollBarWidth,
                ((scroll.flags & VSCROLLBAR_VISIBLE) ? bottomRight.x - (kScrollBarWidth + 1) : bottomRight.x), bottomRight.y,
                colour);
        }

        // Vertical scrollbar
        if (vScrollNeeded)
        {
            WidgetVScrollbarDraw(
                rt, scroll, bottomRight.x - kScrollBarWidth, topLeft.y, bottomRight.x,
                ((scroll.flags & HSCROLLBAR_VISIBLE) ? bottomRight.y - (kScrollBarWidth + 1) : bottomRight.y), colour);
        }

        // Contents
        if (hScrollNeeded)
            bottomRight.y -= (kScrollBarWidth + 1);
        if (vScrollNeeded)
            bottomRight.x -= (kScrollBarWidth + 1);

        bottomRight.y++;
        bottomRight.x++;

        // Create a new inner scroll render target
        RenderTarget scrollRT = rt;

        // Clip the scroll dpi against the outer dpi
        int32_t cl = std::max<int32_t>(rt.x, topLeft.x);
        int32_t ct = std::max<int32_t>(rt.y, topLeft.y);
        int32_t cr = std::min<int32_t>(rt.x + rt.width, bottomRight.x);
        int32_t cb = std::min<int32_t>(rt.y + rt.height, bottomRight.y);

        // Set the respective render target attributes
        scrollRT.x = cl - topLeft.x + scroll.contentOffsetX;
        scrollRT.y = ct - topLeft.y + scroll.contentOffsetY;
        scrollRT.width = cr - cl;
        scrollRT.height = cb - ct;
        scrollRT.bits += cl - rt.x;
        scrollRT.bits += (ct - rt.y) * rt.LineStride();
        scrollRT.pitch = rt.LineStride() - scrollRT.width;

        // Draw the scroll contents
        if (scrollRT.width > 0 && scrollRT.height > 0)
            w.OnScrollDraw(scrollIndex, scrollRT);
    }

    static void WidgetHScrollbarDraw(
        RenderTarget& rt, const ScrollArea& scroll, int32_t l, int32_t t, int32_t r, int32_t b, ColourWithFlags colour)
    {
        colour.setFlag(ColourFlag::translucent, false);

        // Trough
        GfxFillRect(rt, { { l + kScrollBarWidth, t }, { r - kScrollBarWidth, b } }, ColourMapA[colour.colour].lighter);
        GfxFillRect(
            rt, { { l + kScrollBarWidth, t }, { r - kScrollBarWidth, b } }, 0x1000000 | ColourMapA[colour.colour].mid_dark);
        GfxFillRect(rt, { { l + kScrollBarWidth, t + 2 }, { r - kScrollBarWidth, t + 2 } }, ColourMapA[colour.colour].mid_dark);
        GfxFillRect(rt, { { l + kScrollBarWidth, t + 3 }, { r - kScrollBarWidth, t + 3 } }, ColourMapA[colour.colour].lighter);
        GfxFillRect(rt, { { l + kScrollBarWidth, t + 7 }, { r - kScrollBarWidth, t + 7 } }, ColourMapA[colour.colour].mid_dark);
        GfxFillRect(rt, { { l + kScrollBarWidth, t + 8 }, { r - kScrollBarWidth, t + 8 } }, ColourMapA[colour.colour].lighter);

        // Left button
        {
            uint8_t flags = (scroll.flags & HSCROLLBAR_LEFT_PRESSED) ? INSET_RECT_FLAG_BORDER_INSET : 0;

            GfxFillRectInset(rt, { { l, t }, { l + (kScrollBarWidth - 1), b } }, colour, flags);
            DrawText(rt, { l + 1, t }, {}, kBlackLeftArrowString);
        }

        // Thumb
        {
            int16_t left = std::max(l + kScrollBarWidth, l + scroll.hThumbLeft - 1);
            int16_t right = std::min(r - kScrollBarWidth, l + scroll.hThumbRight - 1);
            uint8_t flags = (scroll.flags & HSCROLLBAR_THUMB_PRESSED) ? INSET_RECT_FLAG_BORDER_INSET : 0;

            GfxFillRectInset(rt, { { left, t }, { right, b } }, colour, flags);
        }

        // Right button
        {
            uint8_t flags = (scroll.flags & HSCROLLBAR_RIGHT_PRESSED) ? INSET_RECT_FLAG_BORDER_INSET : 0;

            GfxFillRectInset(rt, { { r - (kScrollBarWidth - 1), t }, { r, b } }, colour, flags);
            DrawText(rt, { r - 6, t }, {}, kBlackRightArrowString);
        }
    }

    static void WidgetVScrollbarDraw(
        RenderTarget& rt, const ScrollArea& scroll, int32_t l, int32_t t, int32_t r, int32_t b, ColourWithFlags colour)
    {
        colour.setFlag(ColourFlag::translucent, false);

        // Trough
        GfxFillRect(rt, { { l, t + kScrollBarWidth }, { r, b - kScrollBarWidth } }, ColourMapA[colour.colour].lighter);
        GfxFillRect(
            rt, { { l, t + kScrollBarWidth }, { r, b - kScrollBarWidth } }, 0x1000000 | ColourMapA[colour.colour].mid_dark);
        GfxFillRect(rt, { { l + 2, t + kScrollBarWidth }, { l + 2, b - kScrollBarWidth } }, ColourMapA[colour.colour].mid_dark);
        GfxFillRect(rt, { { l + 3, t + kScrollBarWidth }, { l + 3, b - kScrollBarWidth } }, ColourMapA[colour.colour].lighter);
        GfxFillRect(rt, { { l + 7, t + kScrollBarWidth }, { l + 7, b - kScrollBarWidth } }, ColourMapA[colour.colour].mid_dark);
        GfxFillRect(rt, { { l + 8, t + kScrollBarWidth }, { l + 8, b - kScrollBarWidth } }, ColourMapA[colour.colour].lighter);

        // Up button
        GfxFillRectInset(
            rt, { { l, t }, { r, t + (kScrollBarWidth - 1) } }, colour,
            ((scroll.flags & VSCROLLBAR_UP_PRESSED) ? INSET_RECT_FLAG_BORDER_INSET : 0));
        DrawText(rt, { l + 1, t - 1 }, {}, kBlackUpArrowString);

        // Thumb
        GfxFillRectInset(
            rt,
            { { l, std::max(t + kScrollBarWidth, t + scroll.vThumbTop - 1) },
              { r, std::min(b - kScrollBarWidth, t + scroll.vThumbBottom - 1) } },
            { colour }, ((scroll.flags & VSCROLLBAR_THUMB_PRESSED) ? INSET_RECT_FLAG_BORDER_INSET : 0));

        // Down button
        GfxFillRectInset(
            rt, { { l, b - (kScrollBarWidth - 1) }, { r, b } }, colour,
            ((scroll.flags & VSCROLLBAR_DOWN_PRESSED) ? INSET_RECT_FLAG_BORDER_INSET : 0));
        DrawText(rt, { l + 1, b - (kScrollBarWidth - 1) }, {}, kBlackDownArrowString);
    }

    /**
     *
     *  rct2: 0x006EB951
     */
    static void WidgetDrawImage(RenderTarget& rt, WindowBase& w, WidgetIndex widgetIndex)
    {
        // Get the widget
        const auto& widget = w.widgets[widgetIndex];

        // Get the image
        if (widget.image.GetIndex() == kSpriteIdNull)
            return;
        auto image = widget.image;

        // Resolve the absolute ltrb
        auto screenCoords = w.windowPos + ScreenCoordsXY{ widget.left, widget.top };

        if (widget.type == WidgetType::colourBtn || widget.type == WidgetType::trnBtn || widget.type == WidgetType::tab)
            if (widgetIsPressed(w, widgetIndex) || isToolActive(w, widgetIndex))
                image = image.WithIndexOffset(1);

        const auto colour = w.colours[widget.colour].colour;
        if (widgetIsDisabled(w, widgetIndex))
        {
            // Draw greyed out (light border bottom right shadow)
            auto mappedColour = ColourMapA[colour].lighter;
            GfxDrawSpriteSolid(rt, image, screenCoords + ScreenCoordsXY{ 1, 1 }, mappedColour);

            // Draw greyed out (dark)
            mappedColour = ColourMapA[colour].mid_light;
            GfxDrawSpriteSolid(rt, image, screenCoords, mappedColour);
        }
        else
        {
            if (image.HasSecondary())
            {
                // ?
            }

            if (image.IsBlended())
                image = image.WithBlended(false);
            else
                image = image.WithPrimary(colour);

            GfxDrawSprite(rt, image, screenCoords);
        }
    }

    bool widgetIsDisabled(const WindowBase& w, WidgetIndex widgetIndex)
    {
        if (w.classification == WindowClass::Custom)
            return w.widgets[widgetIndex].flags.has(WidgetFlag::isDisabled);
        return (w.disabled_widgets & (1LL << widgetIndex)) != 0;
    }

    bool widgetIsHoldable(const WindowBase& w, WidgetIndex widgetIndex)
    {
        if (w.classification == WindowClass::Custom)
            return w.widgets[widgetIndex].flags.has(WidgetFlag::isHoldable);
        return (w.hold_down_widgets & (1LL << widgetIndex)) != 0;
    }

    bool widgetIsVisible(const WindowBase& w, WidgetIndex widgetIndex)
    {
        return w.widgets[widgetIndex].IsVisible();
    }

    bool widgetIsPressed(const WindowBase& w, WidgetIndex widgetIndex)
    {
        if (w.classification == WindowClass::Custom)
        {
            if (w.widgets[widgetIndex].flags.has(WidgetFlag::isPressed))
            {
                return true;
            }
        }
        else
        {
            if (w.pressed_widgets & (1LL << widgetIndex))
            {
                return true;
            }
        }

        if (InputGetState() == InputState::WidgetPressed || InputGetState() == InputState::DropdownActive)
        {
            if (!gInputFlags.has(InputFlag::widgetPressed))
                return false;
            if (gPressedWidget.window_classification != w.classification)
                return false;
            if (gPressedWidget.window_number != w.number)
                return false;
            if (gPressedWidget.widget_index != widgetIndex)
                return false;
            return true;
        }
        return false;
    }

    bool widgetIsHighlighted(const WindowBase& w, WidgetIndex widgetIndex)
    {
        if (gHoverWidget.window_classification != w.classification)
            return false;
        if (gHoverWidget.window_number != w.number)
            return false;
        if (gHoverWidget.widget_index != widgetIndex)
            return false;
        return true;
    }

    /**
     *
     *  rct2: 0x006E9F92
     *  eax: x / output_x
     *  ebx: y / output_y
     *  ecx: output_scroll_area
     *  edx: scroll_id
     *  esi: w
     *  edi: widget
     */
    void widgetScrollGetPart(
        WindowBase& w, const Widget* widget, const ScreenCoordsXY& screenCoords, ScreenCoordsXY& retScreenCoords,
        int32_t* output_scroll_area, int32_t* scroll_id)
    {
        *scroll_id = 0;
        auto itLast = std::find_if(
            w.widgets.begin(), w.widgets.end(), [&](auto& otherWidget) { return &otherWidget == widget; });
        for (auto it = w.widgets.begin(); it != itLast; it++)
        {
            if (it->type == WidgetType::scroll)
            {
                *scroll_id += 1;
            }
        }

        const auto& scroll = w.scrolls[*scroll_id];
        if ((scroll.flags & HSCROLLBAR_VISIBLE) && scroll.contentWidth > widget->width()
            && screenCoords.y >= (w.windowPos.y + widget->bottom - (kScrollBarWidth + 1)))
        {
            // horizontal scrollbar
            int32_t rightOffset = 0;
            int32_t iteratorLeft = widget->left + w.windowPos.x + kScrollBarWidth;
            int32_t iteratorRight = widget->right + w.windowPos.x - kScrollBarWidth;
            if (!(scroll.flags & VSCROLLBAR_VISIBLE))
            {
                rightOffset = kScrollBarWidth + 1;
            }

            if (screenCoords.x <= iteratorLeft)
            {
                *output_scroll_area = SCROLL_PART_HSCROLLBAR_LEFT;
            }
            else if (screenCoords.x >= iteratorRight + rightOffset)
            {
                *output_scroll_area = SCROLL_PART_NONE;
            }
            else if (screenCoords.x >= iteratorRight + rightOffset - kScrollBarWidth)
            {
                *output_scroll_area = SCROLL_PART_HSCROLLBAR_RIGHT;
            }
            else if (screenCoords.x < (widget->left + w.windowPos.x + scroll.hThumbLeft))
            {
                *output_scroll_area = SCROLL_PART_HSCROLLBAR_LEFT_TROUGH;
            }
            else if (screenCoords.x > (widget->left + w.windowPos.x + scroll.hThumbRight))
            {
                *output_scroll_area = SCROLL_PART_HSCROLLBAR_RIGHT_TROUGH;
            }
            else
            {
                *output_scroll_area = SCROLL_PART_HSCROLLBAR_THUMB;
            }
        }
        else if (
            (scroll.flags & VSCROLLBAR_VISIBLE) && scroll.contentHeight > widget->height()
            && (screenCoords.x >= w.windowPos.x + widget->right - (kScrollBarWidth + 1)))
        {
            // vertical scrollbar
            int32_t bottomOffset = 0;
            int32_t iteratorTop = widget->top + w.windowPos.y + kScrollBarWidth;
            int32_t iteratorBottom = widget->bottom + w.windowPos.y;
            if (scroll.flags & HSCROLLBAR_VISIBLE)
            {
                bottomOffset = (kScrollBarWidth + 1);
            }

            if (screenCoords.y <= iteratorTop)
            {
                *output_scroll_area = SCROLL_PART_VSCROLLBAR_TOP;
            }
            else if (screenCoords.y >= (iteratorBottom - bottomOffset))
            {
                *output_scroll_area = SCROLL_PART_NONE;
            }
            else if (screenCoords.y >= (iteratorBottom - bottomOffset - kScrollBarWidth))
            {
                *output_scroll_area = SCROLL_PART_VSCROLLBAR_BOTTOM;
            }
            else if (screenCoords.y < (widget->top + w.windowPos.y + scroll.vThumbTop))
            {
                *output_scroll_area = SCROLL_PART_VSCROLLBAR_TOP_TROUGH;
            }
            else if (screenCoords.y > (widget->top + w.windowPos.y + scroll.vThumbBottom))
            {
                *output_scroll_area = SCROLL_PART_VSCROLLBAR_BOTTOM_TROUGH;
            }
            else
            {
                *output_scroll_area = SCROLL_PART_VSCROLLBAR_THUMB;
            }
        }
        else
        {
            // view
            *output_scroll_area = SCROLL_PART_VIEW;
            retScreenCoords.x = screenCoords.x - widget->left;
            retScreenCoords.y = screenCoords.y - widget->top;
            retScreenCoords -= w.windowPos;
            if (retScreenCoords.x <= 0 || retScreenCoords.y <= 0)
            {
                *output_scroll_area = SCROLL_PART_NONE;
            }
            else
            {
                retScreenCoords.x += scroll.contentOffsetX - 1;
                retScreenCoords.y += scroll.contentOffsetY - 1;
            }
        }
    }

    Widget* getWidgetByIndex(WindowBase& w, WidgetIndex widgetIndex)
    {
        if (widgetIndex >= w.widgets.size())
        {
            LOG_ERROR("Widget index %i out of bounds for window class %u", widgetIndex, w.classification);
        }

        return &w.widgets[widgetIndex];
    }

    static void SafeSetWidgetFlag(WindowBase& w, WidgetIndex widgetIndex, WidgetFlag flag, bool value)
    {
        Widget* widget = getWidgetByIndex(w, widgetIndex);
        if (widget == nullptr)
        {
            return;
        }

        widget->flags.set(flag, value);
    }

    void widgetSetEnabled(WindowBase& w, WidgetIndex widgetIndex, bool enabled)
    {
        widgetSetDisabled(w, widgetIndex, !enabled);
    }

    void widgetSetDisabled(WindowBase& w, WidgetIndex widgetIndex, bool value)
    {
        SafeSetWidgetFlag(w, widgetIndex, WidgetFlag::isDisabled, value);
        if (value)
        {
            w.disabled_widgets |= (1uLL << widgetIndex);
        }
        else
        {
            w.disabled_widgets &= ~(1uLL << widgetIndex);
        }
    }

    void widgetSetHoldable(WindowBase& w, WidgetIndex widgetIndex, bool value)
    {
        SafeSetWidgetFlag(w, widgetIndex, WidgetFlag::isHoldable, value);
        if (value)
        {
            w.hold_down_widgets |= (1uLL << widgetIndex);
        }
        else
        {
            w.hold_down_widgets &= ~(1uLL << widgetIndex);
        }
    }

    void widgetSetVisible(WindowBase& w, WidgetIndex widgetIndex, bool value)
    {
        SafeSetWidgetFlag(w, widgetIndex, WidgetFlag::isHidden, !value);
    }

    void widgetSetPressed(WindowBase& w, WidgetIndex widgetIndex, bool value)
    {
        SafeSetWidgetFlag(w, widgetIndex, WidgetFlag::isPressed, value);
        if (value)
            w.pressed_widgets |= (1uLL << widgetIndex);
        else
            w.pressed_widgets &= ~(1uLL << widgetIndex);
    }

    void widgetSetCheckboxValue(WindowBase& w, WidgetIndex widgetIndex, bool value)
    {
        widgetSetPressed(w, widgetIndex, value);
    }

    static void WidgetTextBoxDraw(RenderTarget& rt, WindowBase& w, WidgetIndex widgetIndex)
    {
        // Get the widget
        const auto& widget = w.widgets[widgetIndex];

        // Resolve the absolute ltrb
        ScreenCoordsXY topLeft{ w.windowPos + ScreenCoordsXY{ widget.left, widget.top } };
        ScreenCoordsXY bottomRight{ w.windowPos + ScreenCoordsXY{ widget.right, widget.bottom } };

        auto& tbIdent = OpenRCT2::Ui::Windows::GetCurrentTextBox();
        bool active = w.classification == tbIdent.window.classification && w.number == tbIdent.window.number
            && widgetIndex == tbIdent.widget_index;

        // GfxFillRectInset(rt, l, t, r, b, colour, 0x20 | (!active ? 0x40 : 0x00));
        GfxFillRectInset(rt, { topLeft, bottomRight }, w.colours[widget.colour], INSET_RECT_F_60);

        // Figure out where the text should be positioned vertically.
        topLeft.y = w.windowPos.y + widget.textTop();

        auto* textInput = OpenRCT2::Ui::Windows::GetTextboxSession();
        if (!active || textInput == nullptr)
        {
            if (widget.text != 0)
            {
                u8string wrappedString;
                GfxWrapString(widget.string, bottomRight.x - topLeft.x - 5, FontStyle::Medium, &wrappedString, nullptr);
                DrawText(rt, { topLeft.x + 2, topLeft.y }, { w.colours[1] }, wrappedString.c_str(), true);
            }
            return;
        }

        // String length needs to add 12 either side of box
        // +13 for cursor when max length.
        u8string wrappedString;
        GfxWrapString(*textInput->Buffer, bottomRight.x - topLeft.x - 5 - 6, FontStyle::Medium, &wrappedString, nullptr);

        DrawText(rt, { topLeft.x + 2, topLeft.y }, { w.colours[1] }, wrappedString.c_str(), true);

        // Make a trimmed view of the string for measuring the width.
        int32_t curX = topLeft.x
            + GfxGetStringWidthNoFormatting(
                           u8string_view{ wrappedString.c_str(), std::min(wrappedString.length(), textInput->SelectionStart) },
                           FontStyle::Medium)
            + 3;

        int32_t width = 6;
        if (static_cast<uint32_t>(textInput->SelectionStart) < textInput->Buffer->size())
        {
            // Make a new 1 character wide string for measuring the width
            // of the character that the cursor is under. (NOTE: this is broken for multi byte utf8 codepoints)
            width = std::max(
                GfxGetStringWidthNoFormatting(u8string{ (*textInput->Buffer)[textInput->SelectionStart] }, FontStyle::Medium)
                    - 2,
                4);
        }

        if (OpenRCT2::Ui::Windows::TextBoxCaretIsFlashed())
        {
            auto colour = ColourMapA[w.colours[1].colour].mid_light;
            auto y = topLeft.y + 1 + widget.height() - 4;
            GfxFillRect(rt, { { curX, y }, { curX + width, y } }, colour + 5);
        }
    }

    static void WidgetProgressBarDraw(RenderTarget& rt, WindowBase& w, WidgetIndex widgetIndex)
    {
        const auto& widget = w.widgets[widgetIndex];

        ScreenCoordsXY topLeft{ w.windowPos + ScreenCoordsXY{ widget.left, widget.top } };
        ScreenCoordsXY bottomRight{ w.windowPos + ScreenCoordsXY{ widget.right, widget.bottom } };

        auto percentage = widget.content & 0xFF;
        auto lowerBlinkBounds = (widget.content >> 8) & 0xFF;
        auto upperBlinkBounds = (widget.content >> 16) & 0xFF;

        auto isBlinking = (lowerBlinkBounds != upperBlinkBounds) && (percentage >= lowerBlinkBounds)
            && (percentage <= upperBlinkBounds);

        GfxFillRectInset(rt, { topLeft, bottomRight }, w.colours[1], INSET_RECT_F_30);
        if (isBlinking)
        {
            if (GameIsNotPaused() && (gCurrentRealTimeTicks & 8))
                return;
        }

        const auto barWidth = widget.width() - 2;
        const int32_t fillSize = (barWidth * percentage) / 100;
        if (fillSize > 0)
        {
            GfxFillRectInset(
                rt, { topLeft + ScreenCoordsXY{ 1, 1 }, topLeft + ScreenCoordsXY{ fillSize + 1, widget.height() - 1 } },
                { widget.colour }, 0);
        }
    }

    static void WidgetHorizontalSeparatorDraw(RenderTarget& rt, WindowBase& w, const Widget& widget)
    {
        ScreenCoordsXY topLeft{ w.windowPos + ScreenCoordsXY{ widget.left, widget.top } };
        ScreenCoordsXY bottomRight{ w.windowPos + ScreenCoordsXY{ widget.right, widget.bottom } };

        GfxFillRectInset(rt, { topLeft, bottomRight }, w.colours[1], INSET_RECT_FLAG_BORDER_INSET);
    }

    ImageId getColourButtonImage(colour_t colour)
    {
        if (colour == COLOUR_INVISIBLE)
        {
            return ImageId(SPR_G2_ICON_PALETTE_INVISIBLE, colour).WithBlended(false);
        }
        else
        {
            return ImageId(SPR_PALETTE_BTN, colour).WithBlended(true);
        }
    }

    void widgetProgressBarSetNewPercentage(Widget& widget, uint8_t newPercentage)
    {
        widget.content &= ~0xFF;
        widget.content |= newPercentage;
    }

    /**
     *
     *  rct2: 0x006EAF26
     */
    void widgetScrollUpdateThumbs(WindowBase& w, WidgetIndex widget_index)
    {
        const auto& widget = w.widgets[widget_index];
        auto& scroll = w.scrolls[WindowGetScrollDataIndex(w, widget_index)];

        if (scroll.flags & HSCROLLBAR_VISIBLE)
        {
            int32_t view_size = widget.width() - 21;
            if (scroll.flags & VSCROLLBAR_VISIBLE)
                view_size -= 11;
            int32_t x = scroll.contentOffsetX * view_size;
            if (scroll.contentWidth != 0)
                x /= scroll.contentWidth;
            scroll.hThumbLeft = x + 11;

            x = widget.width() - 2;
            if (scroll.flags & VSCROLLBAR_VISIBLE)
                x -= 11;
            x += scroll.contentOffsetX;
            if (scroll.contentWidth != 0)
                x = (x * view_size) / scroll.contentWidth;
            x += 11;
            view_size += 10;
            scroll.hThumbRight = std::min(x, view_size);

            if (scroll.hThumbRight - scroll.hThumbLeft < 20)
            {
                double barPosition = (scroll.hThumbRight * 1.0) / view_size;

                scroll.hThumbLeft = static_cast<int32_t>(std::lround(scroll.hThumbLeft - (20 * barPosition)));
                scroll.hThumbRight = static_cast<int32_t>(std::lround(scroll.hThumbRight + (20 * (1 - barPosition))));
            }
        }

        if (scroll.flags & VSCROLLBAR_VISIBLE)
        {
            int32_t view_size = widget.height() - 21;
            if (scroll.flags & HSCROLLBAR_VISIBLE)
                view_size -= 11;
            int32_t y = scroll.contentOffsetY * view_size;
            if (scroll.contentHeight != 0)
                y /= scroll.contentHeight;
            scroll.vThumbTop = y + 11;

            y = widget.height() - 2;
            if (scroll.flags & HSCROLLBAR_VISIBLE)
                y -= 11;
            y += scroll.contentOffsetY;
            if (scroll.contentHeight != 0)
                y = (y * view_size) / scroll.contentHeight;
            y += 11;
            view_size += 10;
            scroll.vThumbBottom = std::min(y, view_size);

            if (scroll.vThumbBottom - scroll.vThumbTop < 20)
            {
                double barPosition = (scroll.vThumbBottom * 1.0) / view_size;

                scroll.vThumbTop = static_cast<int32_t>(std::lround(scroll.vThumbTop - (20 * barPosition)));
                scroll.vThumbBottom = static_cast<int32_t>(std::lround(scroll.vThumbBottom + (20 * (1 - barPosition))));
            }
        }
    }
} // namespace OpenRCT2::Ui
