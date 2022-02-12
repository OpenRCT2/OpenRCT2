/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "Widget.h"

#include "Window.h"

#include <algorithm>
#include <cmath>
#include <openrct2/Context.h>
#include <openrct2/Input.h>
#include <openrct2/drawing/Drawing.h>
#include <openrct2/localisation/Formatter.h>
#include <openrct2/localisation/Localisation.h>
#include <openrct2/sprites.h>
#include <openrct2/util/Util.h>

static void WidgetFrameDraw(rct_drawpixelinfo* dpi, rct_window* w, rct_widgetindex widgetIndex);
static void WidgetResizeDraw(rct_drawpixelinfo* dpi, rct_window* w, rct_widgetindex widgetIndex);
static void WidgetButtonDraw(rct_drawpixelinfo* dpi, rct_window* w, rct_widgetindex widgetIndex);
static void WidgetTabDraw(rct_drawpixelinfo* dpi, rct_window* w, rct_widgetindex widgetIndex);
static void WidgetFlatButtonDraw(rct_drawpixelinfo* dpi, rct_window* w, rct_widgetindex widgetIndex);
static void WidgetTextButton(rct_drawpixelinfo* dpi, rct_window* w, rct_widgetindex widgetIndex);
static void WidgetTextCentred(rct_drawpixelinfo* dpi, rct_window* w, rct_widgetindex widgetIndex);
static void WidgetText(rct_drawpixelinfo* dpi, rct_window* w, rct_widgetindex widgetIndex);
static void WidgetTextInset(rct_drawpixelinfo* dpi, rct_window* w, rct_widgetindex widgetIndex);
static void WidgetTextBoxDraw(rct_drawpixelinfo* dpi, rct_window* w, rct_widgetindex widgetIndex);
static void WidgetGroupboxDraw(rct_drawpixelinfo* dpi, rct_window* w, rct_widgetindex widgetIndex);
static void WidgetCaptionDraw(rct_drawpixelinfo* dpi, rct_window* w, rct_widgetindex widgetIndex);
static void WidgetCheckboxDraw(rct_drawpixelinfo* dpi, rct_window* w, rct_widgetindex widgetIndex);
static void WidgetCloseboxDraw(rct_drawpixelinfo* dpi, rct_window* w, rct_widgetindex widgetIndex);
static void WidgetScrollDraw(rct_drawpixelinfo* dpi, rct_window* w, rct_widgetindex widgetIndex);
static void WidgetHScrollbarDraw(
    rct_drawpixelinfo* dpi, const rct_scroll& scroll, int32_t l, int32_t t, int32_t r, int32_t b, int32_t colour);
static void WidgetVScrollbarDraw(
    rct_drawpixelinfo* dpi, const rct_scroll& scroll, int32_t l, int32_t t, int32_t r, int32_t b, int32_t colour);
static void WidgetDrawImage(rct_drawpixelinfo* dpi, rct_window* w, rct_widgetindex widgetIndex);

/**
 *
 *  rct2: 0x006EB2A8
 */
void WidgetDraw(rct_drawpixelinfo* dpi, rct_window* w, rct_widgetindex widgetIndex)
{
    const auto& widget = w->widgets[widgetIndex];
    switch (widget.type)
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
        default:
            break;
    }
}

/**
 *
 *  rct2: 0x006EB6CE
 */
static void WidgetFrameDraw(rct_drawpixelinfo* dpi, rct_window* w, rct_widgetindex widgetIndex)
{
    // Get the widget
    const auto& widget = w->widgets[widgetIndex];

    // Resolve the absolute ltrb
    auto leftTop = w->windowPos + ScreenCoordsXY{ widget.left, widget.top };
    int32_t r = w->windowPos.x + widget.right;
    int32_t b = w->windowPos.y + widget.bottom;

    //
    uint8_t press = ((w->flags & WF_10) ? INSET_RECT_FLAG_FILL_MID_LIGHT : 0);

    // Get the colour
    uint8_t colour = w->colours[widget.colour];

    // Draw the frame
    gfx_fill_rect_inset(dpi, { leftTop, { r, b } }, colour, press);

    // Check if the window can be resized
    if (!(w->flags & WF_RESIZABLE))
        return;
    if (w->min_width == w->max_width && w->min_height == w->max_height)
        return;

    // Draw the resize sprite at the bottom right corner
    leftTop = w->windowPos + ScreenCoordsXY{ widget.right - 18, widget.bottom - 18 };
    gfx_draw_sprite(dpi, ImageId(SPR_RESIZE, colour & 0x7F), leftTop);
}

/**
 *
 *  rct2: 0x006EB765
 */
static void WidgetResizeDraw(rct_drawpixelinfo* dpi, rct_window* w, rct_widgetindex widgetIndex)
{
    // Get the widget
    const auto& widget = w->widgets[widgetIndex];

    // Resolve the absolute ltrb
    auto leftTop = w->windowPos + ScreenCoordsXY{ widget.left, widget.top };
    int32_t r = w->windowPos.x + widget.right;
    int32_t b = w->windowPos.y + widget.bottom;

    // Get the colour
    uint8_t colour = w->colours[widget.colour];

    // Draw the panel
    gfx_fill_rect_inset(dpi, { leftTop, { r, b } }, colour, 0);

    // Check if the window can be resized
    if (!(w->flags & WF_RESIZABLE))
        return;
    if (w->min_width == w->max_width && w->min_height == w->max_height)
        return;

    // Draw the resize sprite at the bottom right corner
    leftTop = w->windowPos + ScreenCoordsXY{ widget.right - 18, widget.bottom - 18 };
    gfx_draw_sprite(dpi, ImageId(SPR_RESIZE, colour & 0x7F), leftTop);
}

/**
 *
 *  rct2: 0x006EB8E5
 */
static void WidgetButtonDraw(rct_drawpixelinfo* dpi, rct_window* w, rct_widgetindex widgetIndex)
{
    // Get the widget
    const auto& widget = w->widgets[widgetIndex];

    // Resolve the absolute ltrb
    ScreenRect rect{ w->windowPos + ScreenCoordsXY{ widget.left, widget.top },
                     w->windowPos + ScreenCoordsXY{ widget.right, widget.bottom } };

    // Check if the button is pressed down
    uint8_t press = WidgetIsPressed(w, widgetIndex) || WidgetIsActiveTool(w, widgetIndex) ? INSET_RECT_FLAG_BORDER_INSET : 0;

    // Get the colour
    uint8_t colour = w->colours[widget.colour];

    if (static_cast<int32_t>(widget.image) == -2)
    {
        // Draw border with no fill
        gfx_fill_rect_inset(dpi, rect, colour, press | INSET_RECT_FLAG_FILL_NONE);
        return;
    }

    // Draw the border with fill
    gfx_fill_rect_inset(dpi, rect, colour, press);

    WidgetDrawImage(dpi, w, widgetIndex);
}

/**
 *
 *  rct2: 0x006EB806
 */
static void WidgetTabDraw(rct_drawpixelinfo* dpi, rct_window* w, rct_widgetindex widgetIndex)
{
    // Get the widget
    auto& widget = w->widgets[widgetIndex];

    if (widget.type != WindowWidgetType::Tab && static_cast<int32_t>(widget.image) == -1)
        return;

    if (widget.type == WindowWidgetType::Tab)
    {
        if (WidgetIsDisabled(w, widgetIndex))
            return;

        if (widget.image == static_cast<uint32_t>(SPR_NONE))
        {
            // Set standard tab sprite to use.
            widget.image = IMAGE_TYPE_REMAP | SPR_TAB;
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
    auto leftTop = w->windowPos + ScreenCoordsXY{ widget.left, widget.top };

    // Get the colour and disabled image
    auto colour = w->colours[widget.colour] & 0x7F;
    auto image = ImageId::FromUInt32(widget.image + 2).WithPrimary(colour);

    // Draw disabled image
    gfx_draw_sprite(dpi, image, leftTop);
}

/**
 *
 *  rct2: 0x006EB861
 */
static void WidgetFlatButtonDraw(rct_drawpixelinfo* dpi, rct_window* w, rct_widgetindex widgetIndex)
{
    if (!WidgetIsDisabled(w, widgetIndex) && WidgetIsHighlighted(w, widgetIndex))
    {
        WidgetButtonDraw(dpi, w, widgetIndex);
        return;
    }

    // Get the widget
    const auto& widget = w->widgets[widgetIndex];

    // Resolve the absolute ltrb
    ScreenRect rect{ w->windowPos + ScreenCoordsXY{ widget.left, widget.top },
                     w->windowPos + ScreenCoordsXY{ widget.right, widget.bottom } };

    // Get the colour
    uint8_t colour = w->colours[widget.colour];

    // Check if the button is pressed down
    if (WidgetIsPressed(w, widgetIndex) || WidgetIsActiveTool(w, widgetIndex))
    {
        if (static_cast<int32_t>(widget.image) == -2)
        {
            // Draw border with no fill
            gfx_fill_rect_inset(dpi, rect, colour, INSET_RECT_FLAG_BORDER_INSET | INSET_RECT_FLAG_FILL_NONE);
            return;
        }

        // Draw the border with fill
        gfx_fill_rect_inset(dpi, rect, colour, INSET_RECT_FLAG_BORDER_INSET);
    }

    // Draw image
    WidgetDrawImage(dpi, w, widgetIndex);
}

/**
 *
 *  rct2: 0x006EBBEB
 */
static void WidgetTextButton(rct_drawpixelinfo* dpi, rct_window* w, rct_widgetindex widgetIndex)
{
    // Get the widget
    const auto& widget = w->widgets[widgetIndex];

    // Resolve the absolute ltrb
    ScreenRect rect{ w->windowPos + ScreenCoordsXY{ widget.left, widget.top },
                     w->windowPos + ScreenCoordsXY{ widget.right, widget.bottom } };

    // Get the colour
    uint8_t colour = w->colours[widget.colour];

    // Border
    uint8_t press = WidgetIsPressed(w, widgetIndex) || WidgetIsActiveTool(w, widgetIndex) ? INSET_RECT_FLAG_BORDER_INSET : 0;
    gfx_fill_rect_inset(dpi, rect, colour, press);

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
static void WidgetTextCentred(rct_drawpixelinfo* dpi, rct_window* w, rct_widgetindex widgetIndex)
{
    // Get the widget
    const auto& widget = w->widgets[widgetIndex];

    if (widget.text == STR_NONE)
        return;

    // Get the colour
    colour_t colour = w->colours[widget.colour];
    colour &= ~(COLOUR_FLAG_TRANSLUCENT);
    if (WidgetIsDisabled(w, widgetIndex))
        colour |= COLOUR_FLAG_INSET;

    // Resolve the absolute ltrb
    auto topLeft = w->windowPos + ScreenCoordsXY{ widget.left, 0 };
    int32_t r = w->windowPos.x + widget.right;

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
static void WidgetText(rct_drawpixelinfo* dpi, rct_window* w, rct_widgetindex widgetIndex)
{
    // Get the widget
    const auto& widget = w->widgets[widgetIndex];

    if (widget.text == STR_NONE || widget.text == STR_VIEWPORT)
        return;

    // Get the colour
    uint8_t colour = w->colours[widget.colour];
    if (WidgetIsDisabled(w, widgetIndex))
        colour |= COLOUR_FLAG_INSET;

    // Resolve the absolute ltrb
    int32_t l = w->windowPos.x + widget.left;
    int32_t r = w->windowPos.x + widget.right;
    int32_t t;

    if (widget.type == WindowWidgetType::Button || widget.type == WindowWidgetType::DropdownMenu
        || widget.type == WindowWidgetType::Spinner || widget.type == WindowWidgetType::TableHeader)
    {
        t = w->windowPos.y + widget.textTop();
    }
    else
        t = w->windowPos.y + widget.top;

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
static void WidgetTextInset(rct_drawpixelinfo* dpi, rct_window* w, rct_widgetindex widgetIndex)
{
    // Get the widget
    const auto& widget = w->widgets[widgetIndex];

    // Resolve the absolute ltrb
    ScreenRect rect{ w->windowPos + ScreenCoordsXY{ widget.left, widget.top },
                     w->windowPos + ScreenCoordsXY{ widget.right, widget.bottom } };

    // Get the colour
    uint8_t colour = w->colours[widget.colour];

    gfx_fill_rect_inset(dpi, rect, colour, INSET_RECT_F_60);
    WidgetText(dpi, w, widgetIndex);
}

static std::pair<rct_string_id, void*> WidgetGetStringidAndArgs(const rct_widget& widget)
{
    auto stringId = widget.text;
    void* formatArgs = gCommonFormatArgs;
    if (widget.flags & WIDGET_FLAGS::TEXT_IS_STRING)
    {
        if (widget.string == nullptr || widget.string[0] == '\0')
        {
            stringId = STR_NONE;
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
static void WidgetGroupboxDraw(rct_drawpixelinfo* dpi, rct_window* w, rct_widgetindex widgetIndex)
{
    // Get the widget
    const auto& widget = w->widgets[widgetIndex];

    // Resolve the absolute ltrb
    auto l = w->windowPos.x + widget.left + 5;
    auto t = w->windowPos.y + widget.top;
    auto textRight = l;

    // Text
    auto [stringId, formatArgs] = WidgetGetStringidAndArgs(widget);
    if (stringId != STR_NONE)
    {
        uint8_t colour = w->colours[widget.colour] & 0x7F;
        if (WidgetIsDisabled(w, widgetIndex))
            colour |= COLOUR_FLAG_INSET;

        utf8 buffer[512] = { 0 };
        format_string(buffer, sizeof(buffer), stringId, formatArgs);
        auto ft = Formatter();
        ft.Add<utf8*>(buffer);
        DrawTextBasic(dpi, { l, t }, STR_STRING, ft, { colour });
        textRight = l + gfx_get_string_width(buffer, FontSpriteBase::MEDIUM) + 1;
    }

    // Border
    // Resolve the absolute ltrb
    l = w->windowPos.x + widget.left;
    t = w->windowPos.y + widget.top + 4;
    const auto r = w->windowPos.x + widget.right;
    const auto b = w->windowPos.y + widget.bottom;

    // Get the colour
    uint8_t colour = w->colours[widget.colour] & 0x7F;

    // Border left of text
    gfx_fill_rect(dpi, { { l, t }, { l + 4, t } }, ColourMapA[colour].mid_dark);
    gfx_fill_rect(dpi, { { l + 1, t + 1 }, { l + 4, t + 1 } }, ColourMapA[colour].lighter);

    // Border right of text
    gfx_fill_rect(dpi, { { textRight, t }, { r - 1, t } }, ColourMapA[colour].mid_dark);
    gfx_fill_rect(dpi, { { textRight, t + 1 }, { r - 2, t + 1 } }, ColourMapA[colour].lighter);

    // Border right
    gfx_fill_rect(dpi, { { r - 1, t + 1 }, { r - 1, b - 1 } }, ColourMapA[colour].mid_dark);
    gfx_fill_rect(dpi, { { r, t }, { r, b } }, ColourMapA[colour].lighter);

    // Border bottom
    gfx_fill_rect(dpi, { { l, b - 1 }, { r - 2, b - 1 } }, ColourMapA[colour].mid_dark);
    gfx_fill_rect(dpi, { { l, b }, { r - 1, b } }, ColourMapA[colour].lighter);

    // Border left
    gfx_fill_rect(dpi, { { l, t + 1 }, { l, b - 2 } }, ColourMapA[colour].mid_dark);
    gfx_fill_rect(dpi, { { l + 1, t + 2 }, { l + 1, b - 2 } }, ColourMapA[colour].lighter);
}

/**
 *
 *  rct2: 0x006EB2F9
 */
static void WidgetCaptionDraw(rct_drawpixelinfo* dpi, rct_window* w, rct_widgetindex widgetIndex)
{
    // Get the widget
    const auto* widget = &w->widgets[widgetIndex];

    // Resolve the absolute ltrb
    auto topLeft = w->windowPos + ScreenCoordsXY{ widget->left, widget->top };
    auto bottomRight = w->windowPos + ScreenCoordsXY{ widget->right, widget->bottom };

    // Get the colour
    uint8_t colour = w->colours[widget->colour];

    uint8_t press = INSET_RECT_F_60;
    if (w->flags & WF_10)
        press |= INSET_RECT_FLAG_FILL_MID_LIGHT;

    gfx_fill_rect_inset(dpi, { topLeft, bottomRight }, colour, press);

    // Black caption bars look slightly green, this fixes that
    if (colour == 0)
        gfx_fill_rect(
            dpi, { { topLeft + ScreenCoordsXY{ 1, 1 } }, { bottomRight - ScreenCoordsXY{ 1, 1 } } }, ColourMapA[colour].dark);
    else
        gfx_filter_rect(
            dpi, { { topLeft + ScreenCoordsXY{ 1, 1 } }, { bottomRight - ScreenCoordsXY{ 1, 1 } } },
            FilterPaletteID::PaletteDarken3);

    // Draw text
    if (widget->text == STR_NONE)
        return;

    topLeft = w->windowPos + ScreenCoordsXY{ widget->left + 2, widget->top + 1 };
    int32_t width = widget->width() - 4;
    if ((widget + 1)->type == WindowWidgetType::CloseBox)
    {
        width -= 10;
        if ((widget + 2)->type == WindowWidgetType::CloseBox)
            width -= 10;
    }
    topLeft.x += width / 2;
    DrawTextEllipsised(
        dpi, topLeft, width, widget->text, Formatter::Common(), { COLOUR_WHITE | COLOUR_FLAG_OUTLINE, TextAlignment::CENTRE });
}

/**
 *
 *  rct2: 0x006EBB85
 */
static void WidgetCloseboxDraw(rct_drawpixelinfo* dpi, rct_window* w, rct_widgetindex widgetIndex)
{
    // Get the widget
    const auto& widget = w->widgets[widgetIndex];

    // Resolve the absolute ltrb
    auto topLeft = w->windowPos + ScreenCoordsXY{ widget.left, widget.top };
    auto bottomRight = w->windowPos + ScreenCoordsXY{ widget.right, widget.bottom };

    // Check if the button is pressed down
    uint8_t press = 0;
    if (w->flags & WF_10)
        press |= INSET_RECT_FLAG_FILL_MID_LIGHT;
    if (WidgetIsPressed(w, widgetIndex) || WidgetIsActiveTool(w, widgetIndex))
        press |= INSET_RECT_FLAG_BORDER_INSET;

    // Get the colour
    uint8_t colour = w->colours[widget.colour];

    // Draw the button
    gfx_fill_rect_inset(dpi, { topLeft, bottomRight }, colour, press);

    if (widget.text == STR_NONE)
        return;

    topLeft = w->windowPos + ScreenCoordsXY{ widget.midX() - 1, std::max<int32_t>(widget.top, widget.midY() - 5) };

    if (WidgetIsDisabled(w, widgetIndex))
        colour |= COLOUR_FLAG_INSET;

    DrawTextEllipsised(dpi, topLeft, widget.width() - 2, widget.text, Formatter::Common(), { colour, TextAlignment::CENTRE });
}

/**
 *
 *  rct2: 0x006EBAD9
 */
static void WidgetCheckboxDraw(rct_drawpixelinfo* dpi, rct_window* w, rct_widgetindex widgetIndex)
{
    // Get the widget
    const auto& widget = w->widgets[widgetIndex];

    // Resolve the absolute ltb
    ScreenCoordsXY topLeft = w->windowPos + ScreenCoordsXY{ widget.left, widget.top };
    ScreenCoordsXY bottomRight = w->windowPos + ScreenCoordsXY{ widget.right, widget.bottom };
    ScreenCoordsXY midLeft = { topLeft.x, (topLeft.y + bottomRight.y) / 2 };

    // Get the colour
    colour_t colour = w->colours[widget.colour];

    // checkbox
    gfx_fill_rect_inset(dpi, { midLeft - ScreenCoordsXY{ 0, 5 }, midLeft + ScreenCoordsXY{ 9, 4 } }, colour, INSET_RECT_F_60);

    if (WidgetIsDisabled(w, widgetIndex))
    {
        colour |= COLOUR_FLAG_INSET;
    }

    // fill it when checkbox is pressed
    if (WidgetIsPressed(w, widgetIndex))
    {
        gfx_draw_string(
            dpi, { midLeft - ScreenCoordsXY{ 0, 5 } }, static_cast<const char*>(CheckBoxMarkString),
            { static_cast<colour_t>(NOT_TRANSLUCENT(colour)) });
    }

    // draw the text
    if (widget.text == STR_NONE)
        return;

    auto [stringId, formatArgs] = WidgetGetStringidAndArgs(widget);
    gfx_draw_string_left_centred(dpi, stringId, formatArgs, colour, { midLeft + ScreenCoordsXY{ 14, 0 } });
}

/**
 *
 *  rct2: 0x006EBD96
 */
static void WidgetScrollDraw(rct_drawpixelinfo* dpi, rct_window* w, rct_widgetindex widgetIndex)
{
    // Get the widget
    int32_t scrollIndex = window_get_scroll_data_index(w, widgetIndex);
    const auto& widget = w->widgets[widgetIndex];
    const auto& scroll = w->scrolls[scrollIndex];

    // Resolve the absolute ltrb
    ScreenCoordsXY topLeft = w->windowPos + ScreenCoordsXY{ widget.left, widget.top };
    ScreenCoordsXY bottomRight = w->windowPos + ScreenCoordsXY{ widget.right, widget.bottom };

    // Get the colour
    uint8_t colour = w->colours[widget.colour];

    // Draw the border
    gfx_fill_rect_inset(dpi, { topLeft, bottomRight }, colour, INSET_RECT_F_60);

    // Inflate by -1
    topLeft.x++;
    topLeft.y++;
    bottomRight.x--;
    bottomRight.y--;

    // Horizontal scrollbar
    if (scroll.flags & HSCROLLBAR_VISIBLE)
        WidgetHScrollbarDraw(
            dpi, scroll, topLeft.x, bottomRight.y - SCROLLBAR_WIDTH,
            ((scroll.flags & VSCROLLBAR_VISIBLE) ? bottomRight.x - (SCROLLBAR_WIDTH + 1) : bottomRight.x), bottomRight.y,
            colour);

    // Vertical scrollbar
    if (scroll.flags & VSCROLLBAR_VISIBLE)
        WidgetVScrollbarDraw(
            dpi, scroll, bottomRight.x - SCROLLBAR_WIDTH, topLeft.y, bottomRight.x,
            ((scroll.flags & HSCROLLBAR_VISIBLE) ? bottomRight.y - (SCROLLBAR_WIDTH + 1) : bottomRight.y), colour);

    // Contents
    if (scroll.flags & HSCROLLBAR_VISIBLE)
        bottomRight.y -= (SCROLLBAR_WIDTH + 1);
    if (scroll.flags & VSCROLLBAR_VISIBLE)
        bottomRight.x -= (SCROLLBAR_WIDTH + 1);

    bottomRight.y++;
    bottomRight.x++;

    // Create a new inner scroll dpi
    rct_drawpixelinfo scroll_dpi = *dpi;

    // Clip the scroll dpi against the outer dpi
    int32_t cl = std::max<int32_t>(dpi->x, topLeft.x);
    int32_t ct = std::max<int32_t>(dpi->y, topLeft.y);
    int32_t cr = std::min<int32_t>(dpi->x + dpi->width, bottomRight.x);
    int32_t cb = std::min<int32_t>(dpi->y + dpi->height, bottomRight.y);

    // Set the respective dpi attributes
    scroll_dpi.x = cl - topLeft.x + scroll.h_left;
    scroll_dpi.y = ct - topLeft.y + scroll.v_top;
    scroll_dpi.width = cr - cl;
    scroll_dpi.height = cb - ct;
    scroll_dpi.bits += cl - dpi->x;
    scroll_dpi.bits += (ct - dpi->y) * (dpi->width + dpi->pitch);
    scroll_dpi.pitch = (dpi->width + dpi->pitch) - scroll_dpi.width;

    // Draw the scroll contents
    if (scroll_dpi.width > 0 && scroll_dpi.height > 0)
        window_event_scroll_paint_call(w, &scroll_dpi, scrollIndex);
}

static void WidgetHScrollbarDraw(
    rct_drawpixelinfo* dpi, const rct_scroll& scroll, int32_t l, int32_t t, int32_t r, int32_t b, int32_t colour)
{
    colour &= 0x7F;
    // Trough
    gfx_fill_rect(dpi, { { l + SCROLLBAR_WIDTH, t }, { r - SCROLLBAR_WIDTH, b } }, ColourMapA[colour].lighter);
    gfx_fill_rect(dpi, { { l + SCROLLBAR_WIDTH, t }, { r - SCROLLBAR_WIDTH, b } }, 0x1000000 | ColourMapA[colour].mid_dark);
    gfx_fill_rect(dpi, { { l + SCROLLBAR_WIDTH, t + 2 }, { r - SCROLLBAR_WIDTH, t + 2 } }, ColourMapA[colour].mid_dark);
    gfx_fill_rect(dpi, { { l + SCROLLBAR_WIDTH, t + 3 }, { r - SCROLLBAR_WIDTH, t + 3 } }, ColourMapA[colour].lighter);
    gfx_fill_rect(dpi, { { l + SCROLLBAR_WIDTH, t + 7 }, { r - SCROLLBAR_WIDTH, t + 7 } }, ColourMapA[colour].mid_dark);
    gfx_fill_rect(dpi, { { l + SCROLLBAR_WIDTH, t + 8 }, { r - SCROLLBAR_WIDTH, t + 8 } }, ColourMapA[colour].lighter);

    // Left button
    {
        uint8_t flags = (scroll.flags & HSCROLLBAR_LEFT_PRESSED) ? INSET_RECT_FLAG_BORDER_INSET : 0;

        gfx_fill_rect_inset(dpi, { { l, t }, { l + (SCROLLBAR_WIDTH - 1), b } }, colour, flags);
        gfx_draw_string(dpi, { l + 1, t }, static_cast<const char*>(BlackLeftArrowString), {});
    }

    // Thumb
    {
        int16_t left = std::max(l + SCROLLBAR_WIDTH, l + scroll.h_thumb_left - 1);
        int16_t right = std::min(r - SCROLLBAR_WIDTH, l + scroll.h_thumb_right - 1);
        uint8_t flags = (scroll.flags & HSCROLLBAR_THUMB_PRESSED) ? INSET_RECT_FLAG_BORDER_INSET : 0;

        gfx_fill_rect_inset(dpi, { { left, t }, { right, b } }, colour, flags);
    }

    // Right button
    {
        uint8_t flags = (scroll.flags & HSCROLLBAR_RIGHT_PRESSED) ? INSET_RECT_FLAG_BORDER_INSET : 0;

        gfx_fill_rect_inset(dpi, { { r - (SCROLLBAR_WIDTH - 1), t }, { r, b } }, colour, flags);
        gfx_draw_string(dpi, { r - 6, t }, static_cast<const char*>(BlackRightArrowString), {});
    }
}

static void WidgetVScrollbarDraw(
    rct_drawpixelinfo* dpi, const rct_scroll& scroll, int32_t l, int32_t t, int32_t r, int32_t b, int32_t colour)
{
    colour &= 0x7F;
    // Trough
    gfx_fill_rect(dpi, { { l, t + SCROLLBAR_WIDTH }, { r, b - SCROLLBAR_WIDTH } }, ColourMapA[colour].lighter);
    gfx_fill_rect(dpi, { { l, t + SCROLLBAR_WIDTH }, { r, b - SCROLLBAR_WIDTH } }, 0x1000000 | ColourMapA[colour].mid_dark);
    gfx_fill_rect(dpi, { { l + 2, t + SCROLLBAR_WIDTH }, { l + 2, b - SCROLLBAR_WIDTH } }, ColourMapA[colour].mid_dark);
    gfx_fill_rect(dpi, { { l + 3, t + SCROLLBAR_WIDTH }, { l + 3, b - SCROLLBAR_WIDTH } }, ColourMapA[colour].lighter);
    gfx_fill_rect(dpi, { { l + 7, t + SCROLLBAR_WIDTH }, { l + 7, b - SCROLLBAR_WIDTH } }, ColourMapA[colour].mid_dark);
    gfx_fill_rect(dpi, { { l + 8, t + SCROLLBAR_WIDTH }, { l + 8, b - SCROLLBAR_WIDTH } }, ColourMapA[colour].lighter);

    // Up button
    gfx_fill_rect_inset(
        dpi, { { l, t }, { r, t + (SCROLLBAR_WIDTH - 1) } }, colour,
        ((scroll.flags & VSCROLLBAR_UP_PRESSED) ? INSET_RECT_FLAG_BORDER_INSET : 0));
    gfx_draw_string(dpi, { l + 1, t - 1 }, static_cast<const char*>(BlackUpArrowString), {});

    // Thumb
    gfx_fill_rect_inset(
        dpi,
        { { l, std::max(t + SCROLLBAR_WIDTH, t + scroll.v_thumb_top - 1) },
          { r, std::min(b - SCROLLBAR_WIDTH, t + scroll.v_thumb_bottom - 1) } },
        colour, ((scroll.flags & VSCROLLBAR_THUMB_PRESSED) ? INSET_RECT_FLAG_BORDER_INSET : 0));

    // Down button
    gfx_fill_rect_inset(
        dpi, { { l, b - (SCROLLBAR_WIDTH - 1) }, { r, b } }, colour,
        ((scroll.flags & VSCROLLBAR_DOWN_PRESSED) ? INSET_RECT_FLAG_BORDER_INSET : 0));
    gfx_draw_string(dpi, { l + 1, b - (SCROLLBAR_WIDTH - 1) }, static_cast<const char*>(BlackDownArrowString), {});
}

/**
 *
 *  rct2: 0x006EB951
 */
static void WidgetDrawImage(rct_drawpixelinfo* dpi, rct_window* w, rct_widgetindex widgetIndex)
{
    // Get the widget
    const auto& widget = w->widgets[widgetIndex];

    // Get the image
    int32_t image = widget.image;
    if (image == SPR_NONE)
        return;

    // Resolve the absolute ltrb
    auto screenCoords = w->windowPos + ScreenCoordsXY{ widget.left, widget.top };

    // Get the colour
    uint8_t colour = NOT_TRANSLUCENT(w->colours[widget.colour]);

    if (widget.type == WindowWidgetType::ColourBtn || widget.type == WindowWidgetType::TrnBtn
        || widget.type == WindowWidgetType::Tab)
        if (WidgetIsPressed(w, widgetIndex) || WidgetIsActiveTool(w, widgetIndex))
            image++;

    if (WidgetIsDisabled(w, widgetIndex))
    {
        // Draw greyed out (light border bottom right shadow)
        colour = w->colours[widget.colour];
        colour = ColourMapA[NOT_TRANSLUCENT(colour)].lighter;
        gfx_draw_sprite_solid(dpi, image, screenCoords + ScreenCoordsXY{ 1, 1 }, colour);

        // Draw greyed out (dark)
        colour = w->colours[widget.colour];
        colour = ColourMapA[NOT_TRANSLUCENT(colour)].mid_light;
        gfx_draw_sprite_solid(dpi, image, screenCoords, colour);
    }
    else
    {
        if (image & IMAGE_TYPE_REMAP_2_PLUS)
        {
            // ?
        }

        if (image & IMAGE_TYPE_TRANSPARENT)
            image &= ~IMAGE_TYPE_TRANSPARENT;
        else
            image |= colour << 19;

        gfx_draw_sprite(dpi, ImageId::FromUInt32(image), screenCoords);
    }
}

bool WidgetIsDisabled(rct_window* w, rct_widgetindex widgetIndex)
{
    if (w->classification == WC_CUSTOM)
        return w->widgets[widgetIndex].flags & WIDGET_FLAGS::IS_DISABLED;
    return (w->disabled_widgets & (1LL << widgetIndex)) != 0;
}

bool WidgetIsHoldable(rct_window* w, rct_widgetindex widgetIndex)
{
    if (w->classification == WC_CUSTOM)
        return w->widgets[widgetIndex].flags & WIDGET_FLAGS::IS_HOLDABLE;
    return (w->hold_down_widgets & (1LL << widgetIndex)) != 0;
}

bool WidgetIsVisible(rct_window* w, rct_widgetindex widgetIndex)
{
    return w->widgets[widgetIndex].IsVisible();
}

bool WidgetIsPressed(rct_window* w, rct_widgetindex widgetIndex)
{
    if (w->classification == WC_CUSTOM)
    {
        if (w->widgets[widgetIndex].flags & WIDGET_FLAGS::IS_PRESSED)
        {
            return true;
        }
    }
    else
    {
        if (w->pressed_widgets & (1LL << widgetIndex))
        {
            return true;
        }
    }

    if (input_get_state() == InputState::WidgetPressed || input_get_state() == InputState::DropdownActive)
    {
        if (!(input_test_flag(INPUT_FLAG_WIDGET_PRESSED)))
            return false;
        if (gPressedWidget.window_classification != w->classification)
            return false;
        if (gPressedWidget.window_number != w->number)
            return false;
        if (gPressedWidget.widget_index != widgetIndex)
            return false;
        return true;
    }
    return false;
}

bool WidgetIsHighlighted(rct_window* w, rct_widgetindex widgetIndex)
{
    if (gHoverWidget.window_classification != w->classification)
        return false;
    if (gHoverWidget.window_number != w->number)
        return false;
    if (gHoverWidget.widget_index != widgetIndex)
        return false;
    return true;
}

bool WidgetIsActiveTool(rct_window* w, rct_widgetindex widgetIndex)
{
    if (!(input_test_flag(INPUT_FLAG_TOOL_ACTIVE)))
        return false;
    if (gCurrentToolWidget.window_classification != w->classification)
        return false;
    if (gCurrentToolWidget.window_number != w->number)
        return false;
    if (gCurrentToolWidget.widget_index != widgetIndex)
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
    rct_window* w, const rct_widget* widget, const ScreenCoordsXY& screenCoords, ScreenCoordsXY& retScreenCoords,
    int32_t* output_scroll_area, int32_t* scroll_id)
{
    *scroll_id = 0;
    for (rct_widget* iterator = w->widgets; iterator != widget; iterator++)
    {
        if (iterator->type == WindowWidgetType::Scroll)
        {
            *scroll_id += 1;
        }
    }

    const auto& scroll = w->scrolls[*scroll_id];
    if ((scroll.flags & HSCROLLBAR_VISIBLE) && screenCoords.y >= (w->windowPos.y + widget->bottom - (SCROLLBAR_WIDTH + 1)))
    {
        // horizontal scrollbar
        int32_t rightOffset = 0;
        int32_t iteratorLeft = widget->left + w->windowPos.x + SCROLLBAR_WIDTH;
        int32_t iteratorRight = widget->right + w->windowPos.x - SCROLLBAR_WIDTH;
        if (!(scroll.flags & VSCROLLBAR_VISIBLE))
        {
            rightOffset = SCROLLBAR_WIDTH + 1;
        }

        if (screenCoords.x <= iteratorLeft)
        {
            *output_scroll_area = SCROLL_PART_HSCROLLBAR_LEFT;
        }
        else if (screenCoords.x >= iteratorRight + rightOffset)
        {
            *output_scroll_area = SCROLL_PART_NONE;
        }
        else if (screenCoords.x >= iteratorRight + rightOffset - SCROLLBAR_WIDTH)
        {
            *output_scroll_area = SCROLL_PART_HSCROLLBAR_RIGHT;
        }
        else if (screenCoords.x < (widget->left + w->windowPos.x + scroll.h_thumb_left))
        {
            *output_scroll_area = SCROLL_PART_HSCROLLBAR_LEFT_TROUGH;
        }
        else if (screenCoords.x > (widget->left + w->windowPos.x + scroll.h_thumb_right))
        {
            *output_scroll_area = SCROLL_PART_HSCROLLBAR_RIGHT_TROUGH;
        }
        else
        {
            *output_scroll_area = SCROLL_PART_HSCROLLBAR_THUMB;
        }
    }
    else if ((scroll.flags & VSCROLLBAR_VISIBLE) && (screenCoords.x >= w->windowPos.x + widget->right - (SCROLLBAR_WIDTH + 1)))
    {
        // vertical scrollbar
        int32_t bottomOffset = 0;
        int32_t iteratorTop = widget->top + w->windowPos.y + SCROLLBAR_WIDTH;
        int32_t iteratorBottom = widget->bottom + w->windowPos.y;
        if (scroll.flags & HSCROLLBAR_VISIBLE)
        {
            bottomOffset = (SCROLLBAR_WIDTH + 1);
        }

        if (screenCoords.y <= iteratorTop)
        {
            *output_scroll_area = SCROLL_PART_VSCROLLBAR_TOP;
        }
        else if (screenCoords.y >= (iteratorBottom - bottomOffset))
        {
            *output_scroll_area = SCROLL_PART_NONE;
        }
        else if (screenCoords.y >= (iteratorBottom - bottomOffset - SCROLLBAR_WIDTH))
        {
            *output_scroll_area = SCROLL_PART_VSCROLLBAR_BOTTOM;
        }
        else if (screenCoords.y < (widget->top + w->windowPos.y + scroll.v_thumb_top))
        {
            *output_scroll_area = SCROLL_PART_VSCROLLBAR_TOP_TROUGH;
        }
        else if (screenCoords.y > (widget->top + w->windowPos.y + scroll.v_thumb_bottom))
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
        retScreenCoords -= w->windowPos;
        if (retScreenCoords.x <= 0 || retScreenCoords.y <= 0)
        {
            *output_scroll_area = SCROLL_PART_NONE;
        }
        else
        {
            retScreenCoords.x += scroll.h_left - 1;
            retScreenCoords.y += scroll.v_top - 1;
        }
    }
}

void WidgetSetEnabled(rct_window* w, rct_widgetindex widgetIndex, bool enabled)
{
    WidgetSetDisabled(w, widgetIndex, !enabled);
}

void WidgetSetDisabled(rct_window* w, rct_widgetindex widgetIndex, bool value)
{
    if (value)
    {
        w->disabled_widgets |= (1ULL << widgetIndex);
    }
    else
    {
        w->disabled_widgets &= ~(1ULL << widgetIndex);
    }
}

void WidgetSetHoldable(rct_window* w, rct_widgetindex widgetIndex, bool value)
{
    if (value)
    {
        w->hold_down_widgets |= (1ULL << widgetIndex);
    }
    else
    {
        w->hold_down_widgets &= ~(1ULL << widgetIndex);
    }
}

void WidgetSetVisible(rct_window* w, rct_widgetindex widgetIndex, bool value)
{
    if (value)
    {
        w->widgets[widgetIndex].flags &= ~WIDGET_FLAGS::IS_HIDDEN;
    }
    else
    {
        w->widgets[widgetIndex].flags |= WIDGET_FLAGS::IS_HIDDEN;
    }
}

void WidgetSetCheckboxValue(rct_window* w, rct_widgetindex widgetIndex, int32_t value)
{
    if (value)
        w->pressed_widgets |= (1ULL << widgetIndex);
    else
        w->pressed_widgets &= ~(1ULL << widgetIndex);
}

static void WidgetTextBoxDraw(rct_drawpixelinfo* dpi, rct_window* w, rct_widgetindex widgetIndex)
{
    int32_t no_lines = 0;
    char wrapped_string[TEXT_INPUT_SIZE];

    // Get the widget
    const auto& widget = w->widgets[widgetIndex];

    // Resolve the absolute ltrb
    ScreenCoordsXY topLeft{ w->windowPos + ScreenCoordsXY{ widget.left, widget.top } };
    ScreenCoordsXY bottomRight{ w->windowPos + ScreenCoordsXY{ widget.right, widget.bottom } };

    // Get the colour
    uint8_t colour = w->colours[widget.colour];

    bool active = w->classification == gCurrentTextBox.window.classification && w->number == gCurrentTextBox.window.number
        && widgetIndex == gCurrentTextBox.widget_index;

    // gfx_fill_rect_inset(dpi, l, t, r, b, colour, 0x20 | (!active ? 0x40 : 0x00));
    gfx_fill_rect_inset(dpi, { topLeft, bottomRight }, colour, INSET_RECT_F_60);

    // Figure out where the text should be positioned vertically.
    topLeft.y = w->windowPos.y + widget.textTop();

    if (!active || gTextInput == nullptr)
    {
        if (widget.text != 0)
        {
            safe_strcpy(wrapped_string, widget.string, 512);
            gfx_wrap_string(wrapped_string, bottomRight.x - topLeft.x - 5, FontSpriteBase::MEDIUM, &no_lines);
            gfx_draw_string_no_formatting(
                dpi, { topLeft.x + 2, topLeft.y }, wrapped_string, { w->colours[1], FontSpriteBase::MEDIUM });
        }
        return;
    }

    safe_strcpy(wrapped_string, gTextBoxInput, TEXT_INPUT_SIZE);

    // String length needs to add 12 either side of box
    // +13 for cursor when max length.
    gfx_wrap_string(wrapped_string, bottomRight.x - topLeft.x - 5 - 6, FontSpriteBase::MEDIUM, &no_lines);

    gfx_draw_string_no_formatting(dpi, { topLeft.x + 2, topLeft.y }, wrapped_string, { w->colours[1], FontSpriteBase::MEDIUM });

    size_t string_length = get_string_size(wrapped_string) - 1;

    // Make a copy of the string for measuring the width.
    char temp_string[TEXT_INPUT_SIZE] = { 0 };
    std::memcpy(temp_string, wrapped_string, std::min(string_length, gTextInput->SelectionStart));
    int32_t cur_x = topLeft.x + gfx_get_string_width_no_formatting(temp_string, FontSpriteBase::MEDIUM) + 3;

    int32_t width = 6;
    if (static_cast<uint32_t>(gTextInput->SelectionStart) < strlen(gTextBoxInput))
    {
        // Make a new 1 character wide string for measuring the width
        // of the character that the cursor is under.
        temp_string[1] = '\0';
        temp_string[0] = gTextBoxInput[gTextInput->SelectionStart];
        width = std::max(gfx_get_string_width_no_formatting(temp_string, FontSpriteBase::MEDIUM) - 2, 4);
    }

    if (gTextBoxFrameNo <= 15)
    {
        colour = ColourMapA[w->colours[1]].mid_light;
        auto y = topLeft.y + (widget.height() - 1);
        gfx_fill_rect(dpi, { { cur_x, y }, { cur_x + width, y } }, colour + 5);
    }
}

uint32_t GetColourButtonImage(colour_t colour)
{
    return SPRITE_ID_PALETTE_COLOUR_1(colour) | IMAGE_TYPE_TRANSPARENT | SPR_PALETTE_BTN;
}
