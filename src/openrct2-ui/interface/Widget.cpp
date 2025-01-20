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
#include <openrct2/Context.h>
#include <openrct2/Diagnostic.h>
#include <openrct2/Game.h>
#include <openrct2/Input.h>
#include <openrct2/config/Config.h>
#include <openrct2/drawing/Text.h>
#include <openrct2/localisation/Formatter.h>
#include <openrct2/localisation/Formatting.h>
#include <openrct2/localisation/StringIds.h>
#include <openrct2/sprites.h>

using namespace OpenRCT2;

namespace OpenRCT2::Ui
{
    static void WidgetFrameDraw(DrawPixelInfo& dpi, WindowBase& w, WidgetIndex widgetIndex);
    static void WidgetResizeDraw(DrawPixelInfo& dpi, WindowBase& w, WidgetIndex widgetIndex);
    static void WidgetButtonDraw(DrawPixelInfo& dpi, WindowBase& w, WidgetIndex widgetIndex);
    static void WidgetTabDraw(DrawPixelInfo& dpi, WindowBase& w, WidgetIndex widgetIndex);
    static void WidgetFlatButtonDraw(DrawPixelInfo& dpi, WindowBase& w, WidgetIndex widgetIndex);
    static void WidgetTextButton(DrawPixelInfo& dpi, WindowBase& w, WidgetIndex widgetIndex);
    static void WidgetTextCentred(DrawPixelInfo& dpi, WindowBase& w, WidgetIndex widgetIndex);
    static void WidgetText(DrawPixelInfo& dpi, WindowBase& w, WidgetIndex widgetIndex);
    static void WidgetTextInset(DrawPixelInfo& dpi, WindowBase& w, WidgetIndex widgetIndex);
    static void WidgetTextBoxDraw(DrawPixelInfo& dpi, WindowBase& w, WidgetIndex widgetIndex);
    static void WidgetProgressBarDraw(DrawPixelInfo& dpi, WindowBase& w, WidgetIndex widgetIndex);
    static void WidgetGroupboxDraw(DrawPixelInfo& dpi, WindowBase& w, WidgetIndex widgetIndex);
    static void WidgetCaptionDraw(DrawPixelInfo& dpi, WindowBase& w, WidgetIndex widgetIndex);
    static void WidgetCheckboxDraw(DrawPixelInfo& dpi, WindowBase& w, WidgetIndex widgetIndex);
    static void WidgetCloseboxDraw(DrawPixelInfo& dpi, WindowBase& w, WidgetIndex widgetIndex);
    static void WidgetScrollDraw(DrawPixelInfo& dpi, WindowBase& w, WidgetIndex widgetIndex);
    static void WidgetHScrollbarDraw(
        DrawPixelInfo& dpi, const ScrollArea& scroll, int32_t l, int32_t t, int32_t r, int32_t b, ColourWithFlags colour);
    static void WidgetVScrollbarDraw(
        DrawPixelInfo& dpi, const ScrollArea& scroll, int32_t l, int32_t t, int32_t r, int32_t b, ColourWithFlags colour);
    static void WidgetDrawImage(DrawPixelInfo& dpi, WindowBase& w, WidgetIndex widgetIndex);

    /**
     *
     *  rct2: 0x006EB2A8
     */
    void WidgetDraw(DrawPixelInfo& dpi, WindowBase& w, WidgetIndex widgetIndex)
    {
        const auto* widget = GetWidgetByIndex(w, widgetIndex);
        if (widget == nullptr)
        {
            LOG_ERROR("Tried drawing an out-of-bounds widget index!");
            return;
        }

        switch (widget->type)
        {
            case WindowWidgetType::Frame:
                WidgetFrameDraw(dpi, w, widgetIndex);
                break;
            case WindowWidgetType::Resize:
                WidgetResizeDraw(dpi, w, widgetIndex);
                break;
            case WindowWidgetType::ImgBtn:
                WidgetButtonDraw(dpi, w, widgetIndex);
                break;
            case WindowWidgetType::ColourBtn:
            case WindowWidgetType::TrnBtn:
            case WindowWidgetType::Tab:
                WidgetTabDraw(dpi, w, widgetIndex);
                break;
            case WindowWidgetType::FlatBtn:
                WidgetFlatButtonDraw(dpi, w, widgetIndex);
                break;
            case WindowWidgetType::Button:
            case WindowWidgetType::TableHeader:
                WidgetTextButton(dpi, w, widgetIndex);
                break;
            case WindowWidgetType::LabelCentred:
                WidgetTextCentred(dpi, w, widgetIndex);
                break;
            case WindowWidgetType::Label:
                WidgetText(dpi, w, widgetIndex);
                break;
            case WindowWidgetType::Spinner:
            case WindowWidgetType::DropdownMenu:
            case WindowWidgetType::Viewport:
                WidgetTextInset(dpi, w, widgetIndex);
                break;
            case WindowWidgetType::Groupbox:
                WidgetGroupboxDraw(dpi, w, widgetIndex);
                break;
            case WindowWidgetType::Caption:
                WidgetCaptionDraw(dpi, w, widgetIndex);
                break;
            case WindowWidgetType::CloseBox:
                WidgetCloseboxDraw(dpi, w, widgetIndex);
                break;
            case WindowWidgetType::Scroll:
                WidgetScrollDraw(dpi, w, widgetIndex);
                break;
            case WindowWidgetType::Checkbox:
                WidgetCheckboxDraw(dpi, w, widgetIndex);
                break;
            case WindowWidgetType::TextBox:
                WidgetTextBoxDraw(dpi, w, widgetIndex);
                break;
            case WindowWidgetType::ProgressBar:
                WidgetProgressBarDraw(dpi, w, widgetIndex);
                break;
            default:
                break;
        }
    }

    /**
     *
     *  rct2: 0x006EB6CE
     */
    static void WidgetFrameDraw(DrawPixelInfo& dpi, WindowBase& w, WidgetIndex widgetIndex)
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
        GfxFillRectInset(dpi, { leftTop, { r, b } }, colour, press);

        // Check if the window can be resized
        if (!(w.flags & WF_RESIZABLE))
            return;
        if (w.min_width == w.max_width && w.min_height == w.max_height)
            return;

        // Draw the resize sprite at the bottom right corner
        leftTop = w.windowPos + ScreenCoordsXY{ widget.right - 18, widget.bottom - 18 };
        GfxDrawSprite(dpi, ImageId(SPR_RESIZE, colour.colour), leftTop);
    }

    /**
     *
     *  rct2: 0x006EB765
     */
    static void WidgetResizeDraw(DrawPixelInfo& dpi, WindowBase& w, WidgetIndex widgetIndex)
    {
        // Get the widget
        const auto& widget = w.widgets[widgetIndex];

        // Resolve the absolute ltrb
        auto leftTop = w.windowPos + ScreenCoordsXY{ widget.left, widget.top };
        int32_t r = w.windowPos.x + widget.right;
        int32_t b = w.windowPos.y + widget.bottom;

        auto colour = w.colours[widget.colour];

        // Draw the panel
        GfxFillRectInset(dpi, { leftTop, { r, b } }, colour, 0);

        // Check if the window can be resized
        if (!(w.flags & WF_RESIZABLE))
            return;
        if (w.min_width == w.max_width && w.min_height == w.max_height)
            return;

        // Draw the resize sprite at the bottom right corner
        leftTop = w.windowPos + ScreenCoordsXY{ widget.right - 18, widget.bottom - 18 };
        GfxDrawSprite(dpi, ImageId(SPR_RESIZE, colour.colour), leftTop);
    }

    /**
     *
     *  rct2: 0x006EB8E5
     */
    static void WidgetButtonDraw(DrawPixelInfo& dpi, WindowBase& w, WidgetIndex widgetIndex)
    {
        // Get the widget
        const auto& widget = w.widgets[widgetIndex];

        // Resolve the absolute ltrb
        ScreenRect rect{ w.windowPos + ScreenCoordsXY{ widget.left, widget.top },
                         w.windowPos + ScreenCoordsXY{ widget.right, widget.bottom } };

        // Check if the button is pressed down
        uint8_t press = WidgetIsPressed(w, widgetIndex) || isToolActive(w, widgetIndex) ? INSET_RECT_FLAG_BORDER_INSET : 0;

        auto colour = w.colours[widget.colour];

        // Dead code?
        if (static_cast<int32_t>(widget.image.GetIndex()) == -2)
        {
            // Draw border with no fill
            GfxFillRectInset(dpi, rect, colour, press | INSET_RECT_FLAG_FILL_NONE);
            return;
        }

        // Draw the border with fill
        GfxFillRectInset(dpi, rect, colour, press);

        WidgetDrawImage(dpi, w, widgetIndex);
    }

    /**
     *
     *  rct2: 0x006EB806
     */
    static void WidgetTabDraw(DrawPixelInfo& dpi, WindowBase& w, WidgetIndex widgetIndex)
    {
        // Get the widget
        auto& widget = w.widgets[widgetIndex];

        if (widget.type != WindowWidgetType::Tab && widget.image.GetIndex() == kImageIndexUndefined)
            return;

        if (widget.type == WindowWidgetType::Tab)
        {
            if (WidgetIsDisabled(w, widgetIndex))
                return;

            if (widget.image.GetIndex() == kImageIndexUndefined)
            {
                // Set standard tab sprite to use.
                widget.image = ImageId(SPR_TAB, FilterPaletteID::PaletteNull);
            }
        }

        // Draw widgets that aren't explicitly disabled.
        if (!WidgetIsDisabled(w, widgetIndex))
        {
            WidgetDrawImage(dpi, w, widgetIndex);
            return;
        }

        if (widget.type != WindowWidgetType::TrnBtn)
        {
            WidgetDrawImage(dpi, w, widgetIndex);
            return;
        }

        // Resolve the absolute ltrb
        auto leftTop = w.windowPos + ScreenCoordsXY{ widget.left, widget.top };

        // Get the colour and disabled image
        auto colour = w.colours[widget.colour].colour;
        const auto newIndex = widget.image.GetIndex() + 2;
        auto image = widget.image.WithIndex(newIndex).WithPrimary(colour);

        // Draw disabled image
        GfxDrawSprite(dpi, image, leftTop);
    }

    /**
     *
     *  rct2: 0x006EB861
     */
    static void WidgetFlatButtonDraw(DrawPixelInfo& dpi, WindowBase& w, WidgetIndex widgetIndex)
    {
        if (!WidgetIsDisabled(w, widgetIndex) && WidgetIsHighlighted(w, widgetIndex))
        {
            WidgetButtonDraw(dpi, w, widgetIndex);
            return;
        }

        // Get the widget
        const auto& widget = w.widgets[widgetIndex];

        // Resolve the absolute ltrb
        ScreenRect rect{ w.windowPos + ScreenCoordsXY{ widget.left, widget.top },
                         w.windowPos + ScreenCoordsXY{ widget.right, widget.bottom } };

        auto colour = w.colours[widget.colour];

        // Check if the button is pressed down
        if (WidgetIsPressed(w, widgetIndex) || isToolActive(w, widgetIndex))
        {
            // Dead code?
            if (static_cast<int32_t>(widget.image.GetIndex()) == -2)
            {
                // Draw border with no fill
                GfxFillRectInset(dpi, rect, colour, INSET_RECT_FLAG_BORDER_INSET | INSET_RECT_FLAG_FILL_NONE);
                return;
            }

            // Draw the border with fill
            GfxFillRectInset(dpi, rect, colour, INSET_RECT_FLAG_BORDER_INSET);
        }

        // Draw image
        WidgetDrawImage(dpi, w, widgetIndex);
    }

    /**
     *
     *  rct2: 0x006EBBEB
     */
    static void WidgetTextButton(DrawPixelInfo& dpi, WindowBase& w, WidgetIndex widgetIndex)
    {
        // Get the widget
        const auto& widget = w.widgets[widgetIndex];

        // Resolve the absolute ltrb
        ScreenRect rect{ w.windowPos + ScreenCoordsXY{ widget.left, widget.top },
                         w.windowPos + ScreenCoordsXY{ widget.right, widget.bottom } };

        auto colour = w.colours[widget.colour];

        // Border
        uint8_t press = WidgetIsPressed(w, widgetIndex) || isToolActive(w, widgetIndex) ? INSET_RECT_FLAG_BORDER_INSET : 0;
        GfxFillRectInset(dpi, rect, colour, press);

        // Button caption
        if (widget.type != WindowWidgetType::TableHeader)
        {
            WidgetTextCentred(dpi, w, widgetIndex);
        }
        else
        {
            WidgetText(dpi, w, widgetIndex);
        }
    }

    /**
     *
     *  rct2: 0x006EBC41
     */
    static void WidgetTextCentred(DrawPixelInfo& dpi, WindowBase& w, WidgetIndex widgetIndex)
    {
        // Get the widget
        const auto& widget = w.widgets[widgetIndex];

        if (widget.text == kStringIdNone)
            return;

        auto colour = w.colours[widget.colour];
        colour.setFlag(ColourFlag::translucent, false);
        if (WidgetIsDisabled(w, widgetIndex))
            colour.setFlag(ColourFlag::inset, true);

        // Resolve the absolute ltrb
        auto topLeft = w.windowPos + ScreenCoordsXY{ widget.left, 0 };
        int32_t r = w.windowPos.x + widget.right;

        if (widget.type == WindowWidgetType::Button || widget.type == WindowWidgetType::TableHeader)
            topLeft.y += widget.textTop();
        else
            topLeft.y += widget.top;

        auto stringId = widget.text;
        auto ft = Formatter::Common();
        if (widget.flags & WIDGET_FLAGS::TEXT_IS_STRING)
        {
            stringId = STR_STRING;
            ft.Add<utf8*>(widget.string);
        }

        ScreenCoordsXY coords = { (topLeft.x + r + 1) / 2 - 1, topLeft.y };
        if (widget.type == WindowWidgetType::LabelCentred)
        {
            DrawTextWrapped(dpi, coords, widget.width() - 2, stringId, ft, { colour, TextAlignment::CENTRE });
        }
        else
        {
            DrawTextEllipsised(dpi, coords, widget.width() - 2, stringId, ft, { colour, TextAlignment::CENTRE });
        }
    }

    /**
     *
     *  rct2: 0x006EBD52
     */
    static void WidgetText(DrawPixelInfo& dpi, WindowBase& w, WidgetIndex widgetIndex)
    {
        // Get the widget
        const auto& widget = w.widgets[widgetIndex];

        if (widget.text == kStringIdNone || widget.content == kWidgetContentEmpty)
            return;

        auto colour = w.colours[widget.colour];
        if (WidgetIsDisabled(w, widgetIndex))
            colour.setFlag(ColourFlag::inset, true);

        // Resolve the absolute ltrb
        int32_t l = w.windowPos.x + widget.left;
        int32_t r = w.windowPos.x + widget.right;
        int32_t t;

        if (widget.type == WindowWidgetType::Button || widget.type == WindowWidgetType::DropdownMenu
            || widget.type == WindowWidgetType::Spinner || widget.type == WindowWidgetType::TableHeader)
        {
            t = w.windowPos.y + widget.textTop();
        }
        else
            t = w.windowPos.y + widget.top;

        auto stringId = widget.text;
        auto ft = Formatter::Common();
        if (widget.flags & WIDGET_FLAGS::TEXT_IS_STRING)
        {
            stringId = STR_STRING;
            ft.Add<utf8*>(widget.string);
        }

        ScreenCoordsXY coords = { l + 1, t };
        if (widget.type == WindowWidgetType::LabelCentred)
        {
            DrawTextWrapped(dpi, coords, r - l, stringId, ft, { colour, TextAlignment::CENTRE });
        }
        else
        {
            DrawTextEllipsised(dpi, coords, r - l, stringId, ft, colour);
        }
    }

    /**
     *
     *  rct2: 0x006EBD1F
     */
    static void WidgetTextInset(DrawPixelInfo& dpi, WindowBase& w, WidgetIndex widgetIndex)
    {
        // Get the widget
        const auto& widget = w.widgets[widgetIndex];

        // Resolve the absolute ltrb
        ScreenRect rect{ w.windowPos + ScreenCoordsXY{ widget.left, widget.top },
                         w.windowPos + ScreenCoordsXY{ widget.right, widget.bottom } };

        auto colour = w.colours[widget.colour];

        GfxFillRectInset(dpi, rect, colour, INSET_RECT_F_60);
        WidgetText(dpi, w, widgetIndex);
    }

    static std::pair<StringId, void*> WidgetGetStringidAndArgs(const Widget& widget)
    {
        auto stringId = widget.text;
        void* formatArgs = gCommonFormatArgs;
        if (widget.flags & WIDGET_FLAGS::TEXT_IS_STRING)
        {
            if (widget.string == nullptr || widget.string[0] == '\0')
            {
                stringId = kStringIdNone;
                formatArgs = nullptr;
            }
            else
            {
                stringId = STR_STRING;
                formatArgs = const_cast<void*>(reinterpret_cast<const void*>(&widget.string));
            }
        }
        return std::make_pair(stringId, formatArgs);
    }

    /**
     *
     *  rct2: 0x006EB535
     */
    static void WidgetGroupboxDraw(DrawPixelInfo& dpi, WindowBase& w, WidgetIndex widgetIndex)
    {
        // Get the widget
        const auto& widget = w.widgets[widgetIndex];

        // Resolve the absolute ltrb
        auto l = w.windowPos.x + widget.left + 5;
        auto t = w.windowPos.y + widget.top;
        auto textRight = l;

        // Text
        auto [stringId, formatArgs] = WidgetGetStringidAndArgs(widget);
        if (stringId != kStringIdNone)
        {
            auto colour = w.colours[widget.colour].withFlag(ColourFlag::translucent, false);
            if (WidgetIsDisabled(w, widgetIndex))
                colour.setFlag(ColourFlag::inset, true);

            utf8 buffer[512] = { 0 };
            OpenRCT2::FormatStringLegacy(buffer, sizeof(buffer), stringId, formatArgs);
            auto ft = Formatter();
            ft.Add<utf8*>(buffer);
            DrawTextBasic(dpi, { l, t }, STR_STRING, ft, { colour });
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
        GfxFillRect(dpi, { { l, t }, { l + 4, t } }, ColourMapA[colour].mid_dark);
        GfxFillRect(dpi, { { l + 1, t + 1 }, { l + 4, t + 1 } }, ColourMapA[colour].lighter);

        // Border right of text
        GfxFillRect(dpi, { { textRight, t }, { r - 1, t } }, ColourMapA[colour].mid_dark);
        GfxFillRect(dpi, { { textRight, t + 1 }, { r - 2, t + 1 } }, ColourMapA[colour].lighter);

        // Border right
        GfxFillRect(dpi, { { r - 1, t + 1 }, { r - 1, b - 1 } }, ColourMapA[colour].mid_dark);
        GfxFillRect(dpi, { { r, t }, { r, b } }, ColourMapA[colour].lighter);

        // Border bottom
        GfxFillRect(dpi, { { l, b - 1 }, { r - 2, b - 1 } }, ColourMapA[colour].mid_dark);
        GfxFillRect(dpi, { { l, b }, { r - 1, b } }, ColourMapA[colour].lighter);

        // Border left
        GfxFillRect(dpi, { { l, t + 1 }, { l, b - 2 } }, ColourMapA[colour].mid_dark);
        GfxFillRect(dpi, { { l + 1, t + 2 }, { l + 1, b - 2 } }, ColourMapA[colour].lighter);
    }

    /**
     *
     *  rct2: 0x006EB2F9
     */
    static void WidgetCaptionDraw(DrawPixelInfo& dpi, WindowBase& w, WidgetIndex widgetIndex)
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

        GfxFillRectInset(dpi, { topLeft, bottomRight }, colour, press);

        // Black caption bars look slightly green, this fixes that
        if (colour.colour == COLOUR_BLACK)
            GfxFillRect(
                dpi, { { topLeft + ScreenCoordsXY{ 1, 1 } }, { bottomRight - ScreenCoordsXY{ 1, 1 } } },
                ColourMapA[colour.colour].dark);
        else
            GfxFilterRect(
                dpi, { { topLeft + ScreenCoordsXY{ 1, 1 } }, { bottomRight - ScreenCoordsXY{ 1, 1 } } },
                FilterPaletteID::PaletteDarken3);

        // Draw text
        if (widget->text == kStringIdNone)
            return;

        topLeft = w.windowPos + ScreenCoordsXY{ widget->left + 2, widget->top + 1 };
        int32_t width = widget->width() - 4;
        if ((widget + 1)->type == WindowWidgetType::CloseBox)
        {
            width -= kCloseButtonWidth;
            if ((widget + 2)->type == WindowWidgetType::CloseBox)
                width -= kCloseButtonWidth;
        }
        topLeft.x += width / 2;
        if (Config::Get().interface.WindowButtonsOnTheLeft)
            topLeft.x += kCloseButtonWidth;

        DrawTextEllipsised(
            dpi, topLeft, width, widget->text, Formatter::Common(),
            { ColourWithFlags{ COLOUR_WHITE }.withFlag(ColourFlag::withOutline, true), TextAlignment::CENTRE });
    }

    /**
     *
     *  rct2: 0x006EBB85
     */
    static void WidgetCloseboxDraw(DrawPixelInfo& dpi, WindowBase& w, WidgetIndex widgetIndex)
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
        if (WidgetIsPressed(w, widgetIndex) || isToolActive(w, widgetIndex))
            press |= INSET_RECT_FLAG_BORDER_INSET;

        auto colour = w.colours[widget.colour];

        // Draw the button
        GfxFillRectInset(dpi, { topLeft, bottomRight }, colour, press);

        if (widget.text == kStringIdNone)
            return;

        topLeft = w.windowPos + ScreenCoordsXY{ widget.midX() - 1, std::max<int32_t>(widget.top, widget.midY() - 5) };

        if (WidgetIsDisabled(w, widgetIndex))
            colour.setFlag(ColourFlag::inset, true);
        ;

        DrawTextEllipsised(
            dpi, topLeft, widget.width() - 2, widget.text, Formatter::Common(), { colour, TextAlignment::CENTRE });
    }

    /**
     *
     *  rct2: 0x006EBAD9
     */
    static void WidgetCheckboxDraw(DrawPixelInfo& dpi, WindowBase& w, WidgetIndex widgetIndex)
    {
        // Get the widget
        const auto& widget = w.widgets[widgetIndex];

        // Resolve the absolute ltb
        ScreenCoordsXY topLeft = w.windowPos + ScreenCoordsXY{ widget.left, widget.top };
        ScreenCoordsXY bottomRight = w.windowPos + ScreenCoordsXY{ widget.right, widget.bottom };
        ScreenCoordsXY midLeft = { topLeft.x, (topLeft.y + bottomRight.y) / 2 };

        auto colour = w.colours[widget.colour];

        // checkbox
        GfxFillRectInset(dpi, { midLeft - ScreenCoordsXY{ 0, 5 }, midLeft + ScreenCoordsXY{ 9, 4 } }, colour, INSET_RECT_F_60);

        if (WidgetIsDisabled(w, widgetIndex))
        {
            colour.setFlag(ColourFlag::inset, true);
        }

        // fill it when checkbox is pressed
        if (WidgetIsPressed(w, widgetIndex))
        {
            DrawText(
                dpi, { midLeft - ScreenCoordsXY{ 0, 5 } }, { colour.withFlag(ColourFlag::translucent, false) },
                kCheckMarkString);
        }

        // draw the text
        if (widget.text == kStringIdNone)
            return;

        auto [stringId, formatArgs] = WidgetGetStringidAndArgs(widget);
        GfxDrawStringLeftCentred(dpi, stringId, formatArgs, colour, { midLeft + ScreenCoordsXY{ 14, 0 } });
    }

    /**
     *
     *  rct2: 0x006EBD96
     */
    static void WidgetScrollDraw(DrawPixelInfo& dpi, WindowBase& w, WidgetIndex widgetIndex)
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
        GfxFillRectInset(dpi, { topLeft, bottomRight }, colour, INSET_RECT_F_60);

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
                dpi, scroll, topLeft.x, bottomRight.y - kScrollBarWidth,
                ((scroll.flags & VSCROLLBAR_VISIBLE) ? bottomRight.x - (kScrollBarWidth + 1) : bottomRight.x), bottomRight.y,
                colour);
        }

        // Vertical scrollbar
        if (vScrollNeeded)
        {
            WidgetVScrollbarDraw(
                dpi, scroll, bottomRight.x - kScrollBarWidth, topLeft.y, bottomRight.x,
                ((scroll.flags & HSCROLLBAR_VISIBLE) ? bottomRight.y - (kScrollBarWidth + 1) : bottomRight.y), colour);
        }

        // Contents
        if (hScrollNeeded)
            bottomRight.y -= (kScrollBarWidth + 1);
        if (vScrollNeeded)
            bottomRight.x -= (kScrollBarWidth + 1);

        bottomRight.y++;
        bottomRight.x++;

        // Create a new inner scroll dpi
        DrawPixelInfo scroll_dpi = dpi;

        // Clip the scroll dpi against the outer dpi
        int32_t cl = std::max<int32_t>(dpi.x, topLeft.x);
        int32_t ct = std::max<int32_t>(dpi.y, topLeft.y);
        int32_t cr = std::min<int32_t>(dpi.x + dpi.width, bottomRight.x);
        int32_t cb = std::min<int32_t>(dpi.y + dpi.height, bottomRight.y);

        // Set the respective dpi attributes
        scroll_dpi.x = cl - topLeft.x + scroll.contentOffsetX;
        scroll_dpi.y = ct - topLeft.y + scroll.contentOffsetY;
        scroll_dpi.width = cr - cl;
        scroll_dpi.height = cb - ct;
        scroll_dpi.bits += cl - dpi.x;
        scroll_dpi.bits += (ct - dpi.y) * dpi.LineStride();
        scroll_dpi.pitch = dpi.LineStride() - scroll_dpi.width;

        // Draw the scroll contents
        if (scroll_dpi.width > 0 && scroll_dpi.height > 0)
            w.OnScrollDraw(scrollIndex, scroll_dpi);
    }

    static void WidgetHScrollbarDraw(
        DrawPixelInfo& dpi, const ScrollArea& scroll, int32_t l, int32_t t, int32_t r, int32_t b, ColourWithFlags colour)
    {
        colour.setFlag(ColourFlag::translucent, false);

        // Trough
        GfxFillRect(dpi, { { l + kScrollBarWidth, t }, { r - kScrollBarWidth, b } }, ColourMapA[colour.colour].lighter);
        GfxFillRect(
            dpi, { { l + kScrollBarWidth, t }, { r - kScrollBarWidth, b } }, 0x1000000 | ColourMapA[colour.colour].mid_dark);
        GfxFillRect(
            dpi, { { l + kScrollBarWidth, t + 2 }, { r - kScrollBarWidth, t + 2 } }, ColourMapA[colour.colour].mid_dark);
        GfxFillRect(dpi, { { l + kScrollBarWidth, t + 3 }, { r - kScrollBarWidth, t + 3 } }, ColourMapA[colour.colour].lighter);
        GfxFillRect(
            dpi, { { l + kScrollBarWidth, t + 7 }, { r - kScrollBarWidth, t + 7 } }, ColourMapA[colour.colour].mid_dark);
        GfxFillRect(dpi, { { l + kScrollBarWidth, t + 8 }, { r - kScrollBarWidth, t + 8 } }, ColourMapA[colour.colour].lighter);

        // Left button
        {
            uint8_t flags = (scroll.flags & HSCROLLBAR_LEFT_PRESSED) ? INSET_RECT_FLAG_BORDER_INSET : 0;

            GfxFillRectInset(dpi, { { l, t }, { l + (kScrollBarWidth - 1), b } }, colour, flags);
            DrawText(dpi, { l + 1, t }, {}, kBlackLeftArrowString);
        }

        // Thumb
        {
            int16_t left = std::max(l + kScrollBarWidth, l + scroll.hThumbLeft - 1);
            int16_t right = std::min(r - kScrollBarWidth, l + scroll.hThumbRight - 1);
            uint8_t flags = (scroll.flags & HSCROLLBAR_THUMB_PRESSED) ? INSET_RECT_FLAG_BORDER_INSET : 0;

            GfxFillRectInset(dpi, { { left, t }, { right, b } }, colour, flags);
        }

        // Right button
        {
            uint8_t flags = (scroll.flags & HSCROLLBAR_RIGHT_PRESSED) ? INSET_RECT_FLAG_BORDER_INSET : 0;

            GfxFillRectInset(dpi, { { r - (kScrollBarWidth - 1), t }, { r, b } }, colour, flags);
            DrawText(dpi, { r - 6, t }, {}, kBlackRightArrowString);
        }
    }

    static void WidgetVScrollbarDraw(
        DrawPixelInfo& dpi, const ScrollArea& scroll, int32_t l, int32_t t, int32_t r, int32_t b, ColourWithFlags colour)
    {
        colour.setFlag(ColourFlag::translucent, false);

        // Trough
        GfxFillRect(dpi, { { l, t + kScrollBarWidth }, { r, b - kScrollBarWidth } }, ColourMapA[colour.colour].lighter);
        GfxFillRect(
            dpi, { { l, t + kScrollBarWidth }, { r, b - kScrollBarWidth } }, 0x1000000 | ColourMapA[colour.colour].mid_dark);
        GfxFillRect(
            dpi, { { l + 2, t + kScrollBarWidth }, { l + 2, b - kScrollBarWidth } }, ColourMapA[colour.colour].mid_dark);
        GfxFillRect(dpi, { { l + 3, t + kScrollBarWidth }, { l + 3, b - kScrollBarWidth } }, ColourMapA[colour.colour].lighter);
        GfxFillRect(
            dpi, { { l + 7, t + kScrollBarWidth }, { l + 7, b - kScrollBarWidth } }, ColourMapA[colour.colour].mid_dark);
        GfxFillRect(dpi, { { l + 8, t + kScrollBarWidth }, { l + 8, b - kScrollBarWidth } }, ColourMapA[colour.colour].lighter);

        // Up button
        GfxFillRectInset(
            dpi, { { l, t }, { r, t + (kScrollBarWidth - 1) } }, colour,
            ((scroll.flags & VSCROLLBAR_UP_PRESSED) ? INSET_RECT_FLAG_BORDER_INSET : 0));
        DrawText(dpi, { l + 1, t - 1 }, {}, kBlackUpArrowString);

        // Thumb
        GfxFillRectInset(
            dpi,
            { { l, std::max(t + kScrollBarWidth, t + scroll.vThumbTop - 1) },
              { r, std::min(b - kScrollBarWidth, t + scroll.vThumbBottom - 1) } },
            { colour }, ((scroll.flags & VSCROLLBAR_THUMB_PRESSED) ? INSET_RECT_FLAG_BORDER_INSET : 0));

        // Down button
        GfxFillRectInset(
            dpi, { { l, b - (kScrollBarWidth - 1) }, { r, b } }, colour,
            ((scroll.flags & VSCROLLBAR_DOWN_PRESSED) ? INSET_RECT_FLAG_BORDER_INSET : 0));
        DrawText(dpi, { l + 1, b - (kScrollBarWidth - 1) }, {}, kBlackDownArrowString);
    }

    /**
     *
     *  rct2: 0x006EB951
     */
    static void WidgetDrawImage(DrawPixelInfo& dpi, WindowBase& w, WidgetIndex widgetIndex)
    {
        // Get the widget
        const auto& widget = w.widgets[widgetIndex];

        // Get the image
        if (widget.image.GetIndex() == kSpriteIdNull)
            return;
        auto image = widget.image;

        // Resolve the absolute ltrb
        auto screenCoords = w.windowPos + ScreenCoordsXY{ widget.left, widget.top };

        if (widget.type == WindowWidgetType::ColourBtn || widget.type == WindowWidgetType::TrnBtn
            || widget.type == WindowWidgetType::Tab)
            if (WidgetIsPressed(w, widgetIndex) || isToolActive(w, widgetIndex))
                image = image.WithIndexOffset(1);

        const auto colour = w.colours[widget.colour].colour;
        if (WidgetIsDisabled(w, widgetIndex))
        {
            // Draw greyed out (light border bottom right shadow)
            auto mappedColour = ColourMapA[colour].lighter;
            GfxDrawSpriteSolid(dpi, image, screenCoords + ScreenCoordsXY{ 1, 1 }, mappedColour);

            // Draw greyed out (dark)
            mappedColour = ColourMapA[colour].mid_light;
            GfxDrawSpriteSolid(dpi, image, screenCoords, mappedColour);
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

            GfxDrawSprite(dpi, image, screenCoords);
        }
    }

    bool WidgetIsDisabled(const WindowBase& w, WidgetIndex widgetIndex)
    {
        if (w.classification == WindowClass::Custom)
            return w.widgets[widgetIndex].flags & WIDGET_FLAGS::IS_DISABLED;
        return (w.disabled_widgets & (1LL << widgetIndex)) != 0;
    }

    bool WidgetIsHoldable(const WindowBase& w, WidgetIndex widgetIndex)
    {
        if (w.classification == WindowClass::Custom)
            return w.widgets[widgetIndex].flags & WIDGET_FLAGS::IS_HOLDABLE;
        return (w.hold_down_widgets & (1LL << widgetIndex)) != 0;
    }

    bool WidgetIsVisible(const WindowBase& w, WidgetIndex widgetIndex)
    {
        return w.widgets[widgetIndex].IsVisible();
    }

    bool WidgetIsPressed(const WindowBase& w, WidgetIndex widgetIndex)
    {
        if (w.classification == WindowClass::Custom)
        {
            if (w.widgets[widgetIndex].flags & WIDGET_FLAGS::IS_PRESSED)
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
            if (!(InputTestFlag(INPUT_FLAG_WIDGET_PRESSED)))
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

    bool WidgetIsHighlighted(const WindowBase& w, WidgetIndex widgetIndex)
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
    void WidgetScrollGetPart(
        WindowBase& w, const Widget* widget, const ScreenCoordsXY& screenCoords, ScreenCoordsXY& retScreenCoords,
        int32_t* output_scroll_area, int32_t* scroll_id)
    {
        *scroll_id = 0;
        auto itLast = std::find_if(
            w.widgets.begin(), w.widgets.end(), [&](auto& otherWidget) { return &otherWidget == widget; });
        for (auto it = w.widgets.begin(); it != itLast; it++)
        {
            if (it->type == WindowWidgetType::Scroll)
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

    Widget* GetWidgetByIndex(const WindowBase& w, WidgetIndex widgetIndex)
    {
        if (widgetIndex >= w.widgets.size())
        {
            LOG_ERROR("Widget index %i out of bounds for window class %u", widgetIndex, w.classification);
        }

        // FIXME: This const_cast is bad.
        return const_cast<Widget*>(w.widgets.data() + widgetIndex);
    }

    static void SafeSetWidgetFlag(WindowBase& w, WidgetIndex widgetIndex, WidgetFlags mask, bool value)
    {
        Widget* widget = GetWidgetByIndex(w, widgetIndex);
        if (widget == nullptr)
        {
            return;
        }

        if (value)
            widget->flags |= mask;
        else
            widget->flags &= ~mask;
    }

    void WidgetSetEnabled(WindowBase& w, WidgetIndex widgetIndex, bool enabled)
    {
        WidgetSetDisabled(w, widgetIndex, !enabled);
    }

    void WidgetSetDisabled(WindowBase& w, WidgetIndex widgetIndex, bool value)
    {
        SafeSetWidgetFlag(w, widgetIndex, WIDGET_FLAGS::IS_DISABLED, value);
        if (value)
        {
            w.disabled_widgets |= (1uLL << widgetIndex);
        }
        else
        {
            w.disabled_widgets &= ~(1uLL << widgetIndex);
        }
    }

    void WidgetSetHoldable(WindowBase& w, WidgetIndex widgetIndex, bool value)
    {
        SafeSetWidgetFlag(w, widgetIndex, WIDGET_FLAGS::IS_HOLDABLE, value);
        if (value)
        {
            w.hold_down_widgets |= (1uLL << widgetIndex);
        }
        else
        {
            w.hold_down_widgets &= ~(1uLL << widgetIndex);
        }
    }

    void WidgetSetVisible(WindowBase& w, WidgetIndex widgetIndex, bool value)
    {
        SafeSetWidgetFlag(w, widgetIndex, WIDGET_FLAGS::IS_HIDDEN, !value);
    }

    void WidgetSetPressed(WindowBase& w, WidgetIndex widgetIndex, bool value)
    {
        SafeSetWidgetFlag(w, widgetIndex, WIDGET_FLAGS::IS_PRESSED, value);
        if (value)
            w.pressed_widgets |= (1uLL << widgetIndex);
        else
            w.pressed_widgets &= ~(1uLL << widgetIndex);
    }

    void WidgetSetCheckboxValue(WindowBase& w, WidgetIndex widgetIndex, bool value)
    {
        WidgetSetPressed(w, widgetIndex, value);
    }

    static void WidgetTextBoxDraw(DrawPixelInfo& dpi, WindowBase& w, WidgetIndex widgetIndex)
    {
        // Get the widget
        const auto& widget = w.widgets[widgetIndex];

        // Resolve the absolute ltrb
        ScreenCoordsXY topLeft{ w.windowPos + ScreenCoordsXY{ widget.left, widget.top } };
        ScreenCoordsXY bottomRight{ w.windowPos + ScreenCoordsXY{ widget.right, widget.bottom } };

        auto& tbIdent = OpenRCT2::Ui::Windows::GetCurrentTextBox();
        bool active = w.classification == tbIdent.window.classification && w.number == tbIdent.window.number
            && widgetIndex == tbIdent.widget_index;

        // GfxFillRectInset(dpi, l, t, r, b, colour, 0x20 | (!active ? 0x40 : 0x00));
        GfxFillRectInset(dpi, { topLeft, bottomRight }, w.colours[widget.colour], INSET_RECT_F_60);

        // Figure out where the text should be positioned vertically.
        topLeft.y = w.windowPos.y + widget.textTop();

        auto* textInput = OpenRCT2::Ui::Windows::GetTextboxSession();
        if (!active || textInput == nullptr)
        {
            if (widget.text != 0)
            {
                u8string wrappedString;
                GfxWrapString(widget.string, bottomRight.x - topLeft.x - 5, FontStyle::Medium, &wrappedString, nullptr);
                DrawText(dpi, { topLeft.x + 2, topLeft.y }, { w.colours[1] }, wrappedString.c_str(), true);
            }
            return;
        }

        // String length needs to add 12 either side of box
        // +13 for cursor when max length.
        u8string wrappedString;
        GfxWrapString(*textInput->Buffer, bottomRight.x - topLeft.x - 5 - 6, FontStyle::Medium, &wrappedString, nullptr);

        DrawText(dpi, { topLeft.x + 2, topLeft.y }, { w.colours[1] }, wrappedString.c_str(), true);

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
            GfxFillRect(dpi, { { curX, y }, { curX + width, y } }, colour + 5);
        }
    }

    static void WidgetProgressBarDraw(DrawPixelInfo& dpi, WindowBase& w, WidgetIndex widgetIndex)
    {
        const auto& widget = w.widgets[widgetIndex];

        ScreenCoordsXY topLeft{ w.windowPos + ScreenCoordsXY{ widget.left, widget.top } };
        ScreenCoordsXY bottomRight{ w.windowPos + ScreenCoordsXY{ widget.right, widget.bottom } };

        auto percentage = widget.content & 0xFF;
        auto lowerBlinkBounds = (widget.content >> 8) & 0xFF;
        auto upperBlinkBounds = (widget.content >> 16) & 0xFF;

        auto isBlinking = (lowerBlinkBounds != upperBlinkBounds) && (percentage >= lowerBlinkBounds)
            && (percentage <= upperBlinkBounds);

        GfxFillRectInset(dpi, { topLeft, bottomRight }, w.colours[1], INSET_RECT_F_30);
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
                dpi, { topLeft + ScreenCoordsXY{ 1, 1 }, topLeft + ScreenCoordsXY{ fillSize + 1, widget.height() - 1 } },
                { widget.colour }, 0);
        }
    }

    ImageId GetColourButtonImage(colour_t colour)
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

    void WidgetProgressBarSetNewPercentage(Widget& widget, uint8_t newPercentage)
    {
        widget.content &= ~0xFF;
        widget.content |= newPercentage;
    }

    /**
     *
     *  rct2: 0x006EAF26
     */
    void WidgetScrollUpdateThumbs(WindowBase& w, WidgetIndex widget_index)
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
