/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include <algorithm>
#include <cmath>
#include <openrct2/Context.h>
#include <openrct2/drawing/Drawing.h>
#include <openrct2/Input.h>
#include <openrct2/localisation/Localisation.h>
#include <openrct2/sprites.h>
#include <openrct2/util/Util.h>
#include "Widget.h"
#include "Window.h"

static void widget_frame_draw(rct_drawpixelinfo *dpi, rct_window *w, rct_widgetindex widgetIndex);
static void widget_resize_draw(rct_drawpixelinfo *dpi, rct_window *w, rct_widgetindex widgetIndex);
static void widget_button_draw(rct_drawpixelinfo *dpi, rct_window *w, rct_widgetindex widgetIndex);
static void widget_tab_draw(rct_drawpixelinfo *dpi, rct_window *w, rct_widgetindex widgetIndex);
static void widget_flat_button_draw(rct_drawpixelinfo *dpi, rct_window *w, rct_widgetindex widgetIndex);
static void widget_text_button(rct_drawpixelinfo *dpi, rct_window *w, rct_widgetindex widgetIndex);
static void widget_text_centred(rct_drawpixelinfo *dpi, rct_window *w, rct_widgetindex widgetIndex);
static void widget_text(rct_drawpixelinfo *dpi, rct_window *w, rct_widgetindex widgetIndex);
static void widget_text_inset(rct_drawpixelinfo *dpi, rct_window *w, rct_widgetindex widgetIndex);
static void widget_text_box_draw(rct_drawpixelinfo *dpi, rct_window *w, rct_widgetindex widgetIndex);
static void widget_groupbox_draw(rct_drawpixelinfo *dpi, rct_window *w, rct_widgetindex widgetIndex);
static void widget_caption_draw(rct_drawpixelinfo *dpi, rct_window *w, rct_widgetindex widgetIndex);
static void widget_checkbox_draw(rct_drawpixelinfo *dpi, rct_window *w, rct_widgetindex widgetIndex);
static void widget_closebox_draw(rct_drawpixelinfo *dpi, rct_window *w, rct_widgetindex widgetIndex);
static void widget_scroll_draw(rct_drawpixelinfo *dpi, rct_window *w, rct_widgetindex widgetIndex);
static void widget_hscrollbar_draw(rct_drawpixelinfo *dpi, rct_scroll *scroll, int32_t l, int32_t t, int32_t r, int32_t b, int32_t colour);
static void widget_vscrollbar_draw(rct_drawpixelinfo *dpi, rct_scroll *scroll, int32_t l, int32_t t, int32_t r, int32_t b, int32_t colour);
static void widget_draw_image(rct_drawpixelinfo *dpi, rct_window *w, rct_widgetindex widgetIndex);

/**
 *
 *  rct2: 0x006EB2A8
 */
void widget_draw(rct_drawpixelinfo *dpi, rct_window *w, rct_widgetindex widgetIndex)
{
    switch (w->widgets[widgetIndex].type) {
    case WWT_EMPTY:
        break;
    case WWT_FRAME:
        widget_frame_draw(dpi, w, widgetIndex);
        break;
    case WWT_RESIZE:
        widget_resize_draw(dpi, w, widgetIndex);
        break;
    case WWT_IMGBTN:
        widget_button_draw(dpi, w, widgetIndex);
        break;
    case WWT_COLOURBTN:
    case WWT_TRNBTN:
    case WWT_TAB:
        widget_tab_draw(dpi, w, widgetIndex);
        break;
    case WWT_FLATBTN:
        widget_flat_button_draw(dpi, w, widgetIndex);
        break;
    case WWT_BUTTON:
    case WWT_TABLE_HEADER:
        widget_text_button(dpi, w, widgetIndex);
        break;
    case WWT_LABEL_CENTRED:
        widget_text_centred(dpi, w, widgetIndex);
        break;
    case WWT_LABEL:
        widget_text(dpi, w, widgetIndex);
        break;
    case WWT_SPINNER:
    case WWT_DROPDOWN:
    case WWT_VIEWPORT:
        widget_text_inset(dpi, w, widgetIndex);
        break;
    case WWT_GROUPBOX:
        widget_groupbox_draw(dpi, w, widgetIndex);
        break;
    case WWT_CAPTION:
        widget_caption_draw(dpi, w, widgetIndex);
        break;
    case WWT_CLOSEBOX:
        widget_closebox_draw(dpi, w, widgetIndex);
        break;
    case WWT_SCROLL:
        widget_scroll_draw(dpi, w, widgetIndex);
        break;
    case WWT_CHECKBOX:
        widget_checkbox_draw(dpi, w, widgetIndex);
        break;
    case WWT_PLACEHOLDER:
        break;
    case WWT_TEXT_BOX:
        widget_text_box_draw(dpi, w, widgetIndex);
        break;
    }
}

/**
 *
 *  rct2: 0x006EB6CE
 */
static void widget_frame_draw(rct_drawpixelinfo *dpi, rct_window *w, rct_widgetindex widgetIndex)
{
    // Get the widget
    rct_widget *widget = &w->widgets[widgetIndex];

    // Resolve the absolute ltrb
    int32_t l = w->x + widget->left;
    int32_t t = w->y + widget->top;
    int32_t r = w->x + widget->right;
    int32_t b = w->y + widget->bottom;

    //
    uint8_t press = ((w->flags & WF_10) ? INSET_RECT_FLAG_FILL_MID_LIGHT : 0);

    // Get the colour
    uint8_t colour = w->colours[widget->colour];

    // Draw the frame
    gfx_fill_rect_inset(dpi, l, t, r, b, colour, press);

    // Check if the window can be resized
    if (!(w->flags & WF_RESIZABLE))
        return;
    if (w->min_width == w->max_width && w->min_height == w->max_height)
        return;

    // Draw the resize sprite at the bottom right corner
    l = w->x + widget->right - 18;
    t = w->y + widget->bottom - 18;
    gfx_draw_sprite(dpi, SPR_RESIZE | IMAGE_TYPE_REMAP | ((colour & 0x7F) << 19), l, t, 0);
}

/**
 *
 *  rct2: 0x006EB765
 */
static void widget_resize_draw(rct_drawpixelinfo *dpi, rct_window *w, rct_widgetindex widgetIndex)
{
    // Get the widget
    rct_widget *widget = &w->widgets[widgetIndex];

    // Resolve the absolute ltrb
    int32_t l = w->x + widget->left;
    int32_t t = w->y + widget->top;
    int32_t r = w->x + widget->right;
    int32_t b = w->y + widget->bottom;

    // Get the colour
    uint8_t colour = w->colours[widget->colour];

    // Draw the panel
    gfx_fill_rect_inset(dpi, l, t, r, b, colour, 0);

    // Check if the window can be resized
    if (!(w->flags & WF_RESIZABLE))
        return;
    if (w->min_width == w->max_width && w->min_height == w->max_height)
        return;

    // Draw the resize sprite at the bottom right corner
    l = w->x + widget->right - 18;
    t = w->y + widget->bottom - 18;
    gfx_draw_sprite(dpi, SPR_RESIZE | IMAGE_TYPE_REMAP | ((colour & 0x7F) << 19), l, t, 0);
}

/**
 *
 *  rct2: 0x006EB8E5
 */
static void widget_button_draw(rct_drawpixelinfo *dpi, rct_window *w, rct_widgetindex widgetIndex)
{
    // Get the widget
    rct_widget *widget = &w->widgets[widgetIndex];

    // Resolve the absolute ltrb
    int32_t l = w->x + widget->left;
    int32_t t = w->y + widget->top;
    int32_t r = w->x + widget->right;
    int32_t b = w->y + widget->bottom;

    // Check if the button is pressed down
    uint8_t press = widget_is_pressed(w, widgetIndex) || widget_is_active_tool(w, widgetIndex) ? INSET_RECT_FLAG_BORDER_INSET : 0;

    // Get the colour
    uint8_t colour = w->colours[widget->colour];

    if ((int32_t)widget->image == -2) {
        // Draw border with no fill
        gfx_fill_rect_inset(dpi, l, t, r, b, colour, press | INSET_RECT_FLAG_FILL_NONE);
        return;
    }

    // Draw the border with fill
    gfx_fill_rect_inset(dpi, l, t, r, b, colour, press);

    widget_draw_image(dpi, w, widgetIndex);
}

/**
 *
 *  rct2: 0x006EB806
 */
static void widget_tab_draw(rct_drawpixelinfo *dpi, rct_window *w, rct_widgetindex widgetIndex)
{
    // Get the widget
    rct_widget *widget = &w->widgets[widgetIndex];

    if ((int32_t)widget->image == -1)
        return;

    // Draw widgets that aren't explicitly disabled.
    if (!widget_is_disabled(w, widgetIndex)) {
        widget_draw_image(dpi, w, widgetIndex);
        return;
    }

    // Do not draw hidden tabs, unless given a sprite.
    if (widget->type == WWT_TAB && widget->image != (IMAGE_TYPE_REMAP | SPR_G2_TAB_DISABLED))
        return;

    if (widget->type != WWT_TRNBTN) {
        widget_draw_image(dpi, w, widgetIndex);
        return;
    }

    // Resolve the absolute ltrb
    int32_t l = w->x + widget->left;
    int32_t t = w->y + widget->top;

    // Get the colour and disabled image
    uint8_t colour = w->colours[widget->colour] & 0x7F;
    uint32_t image = widget->image + 2;

    // Draw disabled image
    gfx_draw_sprite(dpi, image | (colour << 19), l, t, 0);
}

/**
 *
 *  rct2: 0x006EB861
 */
static void widget_flat_button_draw(rct_drawpixelinfo *dpi, rct_window *w, rct_widgetindex widgetIndex)
{
    if (!widget_is_disabled(w, widgetIndex) && widget_is_highlighted(w, widgetIndex)) {
        widget_button_draw(dpi, w, widgetIndex);
        return;
    }

    // Get the widget
    rct_widget *widget = &w->widgets[widgetIndex];

    // Resolve the absolute ltrb
    int32_t l = w->x + widget->left;
    int32_t t = w->y + widget->top;
    int32_t r = w->x + widget->right;
    int32_t b = w->y + widget->bottom;

    // Get the colour
    uint8_t colour = w->colours[widget->colour];

    // Check if the button is pressed down
    if (widget_is_pressed(w, widgetIndex) || widget_is_active_tool(w, widgetIndex)) {
        if ((int32_t)widget->image == -2) {
            // Draw border with no fill
            gfx_fill_rect_inset(dpi, l, t, r, b, colour, INSET_RECT_FLAG_BORDER_INSET | INSET_RECT_FLAG_FILL_NONE);
            return;
        }

        // Draw the border with fill
        gfx_fill_rect_inset(dpi, l, t, r, b, colour, INSET_RECT_FLAG_BORDER_INSET);
    }

    // Draw image
    widget_draw_image(dpi, w, widgetIndex);
}

/**
 *
 *  rct2: 0x006EBBEB
 */
static void widget_text_button(rct_drawpixelinfo *dpi, rct_window *w, rct_widgetindex widgetIndex)
{
    // Get the widget
    rct_widget *widget = &w->widgets[widgetIndex];

    // Resolve the absolute ltrb
    int32_t l = w->x + widget->left;
    int32_t t = w->y + widget->top;
    int32_t r = w->x + widget->right;
    int32_t b = w->y + widget->bottom;

    // Get the colour
    uint8_t colour = w->colours[widget->colour];

    // Border
    uint8_t press = widget_is_pressed(w, widgetIndex) || widget_is_active_tool(w, widgetIndex) ? INSET_RECT_FLAG_BORDER_INSET : 0;
    gfx_fill_rect_inset(dpi, l, t, r, b, colour, press);

    // Button caption
    if (widget->type != WWT_TABLE_HEADER)
    {
        widget_text_centred(dpi, w, widgetIndex);
    }
    else
    {
        widget_text(dpi, w, widgetIndex);
    }
}

/**
 *
 *  rct2: 0x006EBC41
 */
static void widget_text_centred(rct_drawpixelinfo *dpi, rct_window *w, rct_widgetindex widgetIndex)
{
    // Get the widget
    rct_widget *widget = &w->widgets[widgetIndex];

    if (widget->text == STR_NONE)
        return;

    // Get the colour
    uint8_t colour = w->colours[widget->colour];
    colour &= ~(COLOUR_FLAG_TRANSLUCENT);
    if (widget_is_disabled(w, widgetIndex))
        colour |= COLOUR_FLAG_INSET;

    // Resolve the absolute ltrb
    int32_t l = w->x + widget->left;
    int32_t r = w->x + widget->right;
    int32_t t;

    if (widget->type == WWT_BUTTON || widget->type == WWT_TABLE_HEADER)
    {
        int32_t height = (widget->bottom - widget->top);
        if (height >= 10)
            t = w->y + std::max<int32_t>(widget->top, widget->top + (height / 2) - 5);
        else
            t = w->y + widget->top - 1;
    }
    else
        t = w->y + widget->top;

    gfx_draw_string_centred_clipped(
        dpi,
        widget->text,
        gCommonFormatArgs,
        colour,
        (l + r + 1) / 2 - 1,
        t,
        widget->right - widget->left - 2
    );
}

/**
 *
 *  rct2: 0x006EBD52
 */
static void widget_text(rct_drawpixelinfo *dpi, rct_window *w, rct_widgetindex widgetIndex)
{
    // Get the widget
    rct_widget *widget = &w->widgets[widgetIndex];

    if (widget->text == STR_NONE || widget->text == STR_VIEWPORT)
        return;

    // Get the colour
    uint8_t colour = w->colours[widget->colour];
    if (widget_is_disabled(w, widgetIndex))
        colour |= COLOUR_FLAG_INSET;

    // Resolve the absolute ltrb
    int32_t l = w->x + widget->left;
    int32_t r = w->x + widget->right;
    int32_t t;

    if (widget->type == WWT_BUTTON || widget->type == WWT_TABLE_HEADER)
    {
        int32_t height = (widget->bottom - widget->top);
        if (height >= 10)
            t = w->y + std::max<int32_t>(widget->top, widget->top + (height / 2) - 5);
        else
            t = w->y + widget->top - 1;
    }
    else
        t = w->y + widget->top;

    gfx_draw_string_left_clipped(dpi, widget->text, gCommonFormatArgs, colour, l + 1, t, r - l);
}

/**
 *
 *  rct2: 0x006EBD1F
 */
static void widget_text_inset(rct_drawpixelinfo *dpi, rct_window *w, rct_widgetindex widgetIndex)
{
    // Get the widget
    rct_widget *widget = &w->widgets[widgetIndex];

    // Resolve the absolute ltrb
    int32_t l = w->x + widget->left;
    int32_t t = w->y + widget->top;
    int32_t r = w->x + widget->right;
    int32_t b = w->y + widget->bottom;

    // Get the colour
    uint8_t colour = w->colours[widget->colour];

    gfx_fill_rect_inset(dpi, l, t, r, b, colour, INSET_RECT_F_60);
    widget_text(dpi, w, widgetIndex);
}

/**
 *
 *  rct2: 0x006EB535
 */
static void widget_groupbox_draw(rct_drawpixelinfo *dpi, rct_window *w, rct_widgetindex widgetIndex)
{
    // Get the widget
    rct_widget *widget = &w->widgets[widgetIndex];

    // Resolve the absolute ltrb
    int32_t l = w->x + widget->left + 5;
    int32_t t = w->y + widget->top;
    int32_t r = w->x + widget->right;
    int32_t b = w->y + widget->bottom;
    int32_t textRight = l;

    // Text
    if (widget->text != STR_NONE) {
        uint8_t colour = w->colours[widget->colour] & 0x7F;
        if (widget_is_disabled(w, widgetIndex))
            colour |= COLOUR_FLAG_INSET;

        utf8 buffer[512] = { 0 };
        uint8_t args[sizeof(uintptr_t)] = { 0 };
        format_string(buffer, sizeof(buffer), widget->text, gCommonFormatArgs);
        set_format_arg_on(args, 0, uintptr_t, buffer);
        gfx_draw_string_left(dpi, STR_STRING, args, colour, l, t);
        textRight = l + gfx_get_string_width(buffer) + 1;
    }

    // Border
    // Resolve the absolute ltrb
    l = w->x + widget->left;
    t = w->y + widget->top + 4;
    r = w->x + widget->right;
    b = w->y + widget->bottom;

    // Get the colour
    uint8_t colour = w->colours[widget->colour] & 0x7F;

    // Border left of text
    gfx_fill_rect(dpi, l, t, l + 4, t, ColourMapA[colour].mid_dark);
    gfx_fill_rect(dpi, l + 1, t + 1, l + 4, t + 1, ColourMapA[colour].lighter);

    // Border right of text
    gfx_fill_rect(dpi, textRight, t, r - 1, t, ColourMapA[colour].mid_dark);
    gfx_fill_rect(dpi, textRight, t + 1, r - 2, t + 1, ColourMapA[colour].lighter);

    // Border right
    gfx_fill_rect(dpi, r - 1, t + 1, r - 1, b - 1, ColourMapA[colour].mid_dark);
    gfx_fill_rect(dpi, r, t, r, b, ColourMapA[colour].lighter);

    // Border bottom
    gfx_fill_rect(dpi, l, b - 1, r - 2, b - 1, ColourMapA[colour].mid_dark);
    gfx_fill_rect(dpi, l, b, r - 1, b, ColourMapA[colour].lighter);

    // Border left
    gfx_fill_rect(dpi, l, t + 1, l, b - 2, ColourMapA[colour].mid_dark);
    gfx_fill_rect(dpi, l + 1, t + 2, l + 1, b - 2, ColourMapA[colour].lighter);
}

/**
 *
 *  rct2: 0x006EB2F9
 */
static void widget_caption_draw(rct_drawpixelinfo *dpi, rct_window *w, rct_widgetindex widgetIndex)
{
    // Get the widget
    rct_widget *widget = &w->widgets[widgetIndex];

    // Resolve the absolute ltrb
    int32_t l = w->x + widget->left;
    int32_t t = w->y + widget->top;
    int32_t r = w->x + widget->right;
    int32_t b = w->y + widget->bottom;

    // Get the colour
    uint8_t colour = w->colours[widget->colour];

    uint8_t press = INSET_RECT_F_60;
    if (w->flags & WF_10)
        press |= INSET_RECT_FLAG_FILL_MID_LIGHT;

    gfx_fill_rect_inset(dpi, l, t, r, b, colour, press);

    // Black caption bars look slightly green, this fixes that
    if (colour == 0)
        gfx_fill_rect(dpi, l + 1, t + 1, r - 1, b - 1, ColourMapA[colour].dark);
    else
        gfx_filter_rect(dpi, l + 1, t + 1, r - 1, b - 1, PALETTE_DARKEN_3);

    // Draw text
    if (widget->text == STR_NONE)
        return;

    l = widget->left + w->x + 2;
    t = widget->top + w->y + 1;
    int32_t width = widget->right - widget->left - 4;
    if ((widget + 1)->type == WWT_CLOSEBOX) {
        width -= 10;
        if ((widget + 2)->type == WWT_CLOSEBOX)
            width -= 10;
    }
    l += width / 2;
    gfx_draw_string_centred_clipped(dpi, widget->text, gCommonFormatArgs, COLOUR_WHITE | COLOUR_FLAG_OUTLINE, l, t, width);
}

/**
 *
 *  rct2: 0x006EBB85
 */
static void widget_closebox_draw(rct_drawpixelinfo *dpi, rct_window *w, rct_widgetindex widgetIndex)
{
    // Get the widget
    rct_widget *widget = &w->widgets[widgetIndex];

    // Resolve the absolute ltrb
    int32_t l = w->x + widget->left;
    int32_t t = w->y + widget->top;
    int32_t r = w->x + widget->right;
    int32_t b = w->y + widget->bottom;

    // Check if the button is pressed down
    uint8_t press = 0;
    if (w->flags & WF_10)
        press |= INSET_RECT_FLAG_FILL_MID_LIGHT;
    if (widget_is_pressed(w, widgetIndex) || widget_is_active_tool(w, widgetIndex))
        press |= INSET_RECT_FLAG_BORDER_INSET;

    // Get the colour
    uint8_t colour = w->colours[widget->colour];

    // Draw the button
    gfx_fill_rect_inset(dpi, l, t, r, b, colour, press);

    if (widget->text == STR_NONE)
        return;

    l = w->x + (widget->left + widget->right) / 2 - 1;
    t = w->y + std::max<int32_t>(widget->top, (widget->top + widget->bottom) / 2 - 5);

    if (widget_is_disabled(w, widgetIndex))
        colour |= COLOUR_FLAG_INSET;

    gfx_draw_string_centred_clipped(dpi, widget->text, gCommonFormatArgs, colour, l, t, widget->right - widget->left - 2);
}

/**
 *
*  rct2: 0x006EBAD9
*/
static void widget_checkbox_draw(rct_drawpixelinfo *dpi, rct_window *w, rct_widgetindex widgetIndex)
{
    // Get the widget
    rct_widget *widget = &w->widgets[widgetIndex];

    // Resolve the absolute ltb
    int32_t l = w->x + widget->left;
    int32_t t = w->y + widget->top;
    int32_t b = w->y + widget->bottom;
    int32_t yMid = (b + t) / 2;

    // Get the colour
    uint8_t colour = w->colours[widget->colour];

    // checkbox
    gfx_fill_rect_inset(dpi, l, yMid - 5, l + 9, yMid + 4, colour, INSET_RECT_F_60);

    // fill it when checkbox is pressed
    if (widget_is_pressed(w, widgetIndex)) {
        gCurrentFontSpriteBase = FONT_SPRITE_BASE_MEDIUM;
        gfx_draw_string(dpi, (char*)CheckBoxMarkString, NOT_TRANSLUCENT(colour), l, yMid - 5);
    }

    // draw the text
    if (widget->text == STR_NONE)
        return;

    if (widget_is_disabled(w, widgetIndex)) {
        colour |= COLOUR_FLAG_INSET;
    }

    gfx_draw_string_left_centred(dpi, widget->text, gCommonFormatArgs, colour, l + 14, yMid);
}

/**
 *
 *  rct2: 0x006EBD96
 */
static void widget_scroll_draw(rct_drawpixelinfo *dpi, rct_window *w, rct_widgetindex widgetIndex)
{
    // Get the widget
    int32_t scrollIndex = window_get_scroll_data_index(w, widgetIndex);
    rct_widget *widget = &w->widgets[widgetIndex];
    rct_scroll* scroll = &w->scrolls[scrollIndex];

    // Resolve the absolute ltrb
    int32_t l = w->x + widget->left;
    int32_t t = w->y + widget->top;
    int32_t r = w->x + widget->right;
    int32_t b = w->y + widget->bottom;

    // Get the colour
    uint8_t colour = w->colours[widget->colour];

    // Draw the border
    gfx_fill_rect_inset(dpi, l, t, r, b, colour, INSET_RECT_F_60);

    // Inflate by -1
    l++;
    t++;
    r--;
    b--;

    gCurrentFontSpriteBase = FONT_SPRITE_BASE_MEDIUM;

    // Horizontal scrollbar
    if (scroll->flags & HSCROLLBAR_VISIBLE)
        widget_hscrollbar_draw(dpi, scroll, l, b - 10, ((scroll->flags & VSCROLLBAR_VISIBLE) ? r - 11 : r), b, colour);

    // Vertical scrollbar
    if (scroll->flags & VSCROLLBAR_VISIBLE)
        widget_vscrollbar_draw(dpi, scroll, r - 10, t, r, ((scroll->flags & HSCROLLBAR_VISIBLE) ? b - 11 : b), colour);

    // Contents
    if (scroll->flags & HSCROLLBAR_VISIBLE)
        b -= 11;
    if (scroll->flags & VSCROLLBAR_VISIBLE)
        r -= 11;

    b++;
    r++;

    // Create a new inner scroll dpi
    rct_drawpixelinfo scroll_dpi = *dpi;

    // Clip the scroll dpi against the outer dpi
    int32_t cl = std::max<int32_t>(dpi->x, l);
    int32_t ct = std::max<int32_t>(dpi->y, t);
    int32_t cr = std::min<int32_t>(dpi->x + dpi->width, r);
    int32_t cb = std::min<int32_t>(dpi->y + dpi->height, b);

    // Set the respective dpi attributes
    scroll_dpi.x = cl - l + scroll->h_left;
    scroll_dpi.y = ct - t + scroll->v_top;
    scroll_dpi.width = cr - cl;
    scroll_dpi.height = cb - ct;
    scroll_dpi.bits += cl - dpi->x;
    scroll_dpi.bits += (ct - dpi->y) * (dpi->width + dpi->pitch);
    scroll_dpi.pitch = (dpi->width + dpi->pitch) - scroll_dpi.width;

    // Draw the scroll contents
    if (scroll_dpi.width > 0 && scroll_dpi.height > 0)
        window_event_scroll_paint_call(w, &scroll_dpi, scrollIndex);
}

static void widget_hscrollbar_draw(rct_drawpixelinfo *dpi, rct_scroll *scroll, int32_t l, int32_t t, int32_t r, int32_t b, int32_t colour)
{
    colour &= 0x7F;
    // Trough
    gfx_fill_rect(dpi, l + 10, t, r - 10, b, ColourMapA[colour].lighter);
    gfx_fill_rect(dpi, l + 10, t, r - 10, b, 0x1000000 | ColourMapA[colour].mid_dark);
    gfx_fill_rect(dpi, l + 10, t + 2, r - 10, t + 2, ColourMapA[colour].mid_dark);
    gfx_fill_rect(dpi, l + 10, t + 3, r - 10, t + 3, ColourMapA[colour].lighter);
    gfx_fill_rect(dpi, l + 10, t + 7, r - 10, t + 7, ColourMapA[colour].mid_dark);
    gfx_fill_rect(dpi, l + 10, t + 8, r - 10, t + 8, ColourMapA[colour].lighter);

    // Left button
    gfx_fill_rect_inset(dpi, l, t, l + 9, b, colour, ((scroll->flags & HSCROLLBAR_LEFT_PRESSED) ? INSET_RECT_FLAG_BORDER_INSET : 0));
    gfx_draw_string(dpi, (char*)BlackLeftArrowString, COLOUR_BLACK, l + 1, t);

    // Thumb
    gfx_fill_rect_inset(dpi,
        std::max(l + 10, l + scroll->h_thumb_left - 1), t,
        std::min(r - 10, l + scroll->h_thumb_right - 1), b,
        colour, ((scroll->flags & HSCROLLBAR_THUMB_PRESSED) ? INSET_RECT_FLAG_BORDER_INSET : 0));

    // Right button
    gfx_fill_rect_inset(dpi, r - 9, t, r, b, colour, ((scroll->flags & HSCROLLBAR_RIGHT_PRESSED) ? INSET_RECT_FLAG_BORDER_INSET : 0));
    gfx_draw_string(dpi, (char*)BlackRightArrowString, COLOUR_BLACK, r - 6, t);
}

static void widget_vscrollbar_draw(rct_drawpixelinfo *dpi, rct_scroll *scroll, int32_t l, int32_t t, int32_t r, int32_t b, int32_t colour)
{
    colour &= 0x7F;
    // Trough
    gfx_fill_rect(dpi, l, t + 10, r, b - 10, ColourMapA[colour].lighter);
    gfx_fill_rect(dpi, l, t + 10, r, b - 10, 0x1000000 | ColourMapA[colour].mid_dark);
    gfx_fill_rect(dpi, l + 2, t + 10, l + 2, b - 10, ColourMapA[colour].mid_dark);
    gfx_fill_rect(dpi, l + 3, t + 10, l + 3, b - 10, ColourMapA[colour].lighter);
    gfx_fill_rect(dpi, l + 7, t + 10, l + 7, b - 10, ColourMapA[colour].mid_dark);
    gfx_fill_rect(dpi, l + 8, t + 10, l + 8, b - 10, ColourMapA[colour].lighter);

    // Up button
    gfx_fill_rect_inset(dpi, l, t, r, t + 9, colour, ((scroll->flags & VSCROLLBAR_UP_PRESSED) ? INSET_RECT_FLAG_BORDER_INSET : 0));
    gfx_draw_string(dpi, (char*)BlackUpArrowString, COLOUR_BLACK, l + 1, t - 1);

    // Thumb
    gfx_fill_rect_inset(dpi,
        l, std::max(t + 10, t + scroll->v_thumb_top - 1),
        r, std::min(b - 10, t + scroll->v_thumb_bottom - 1),
        colour, ((scroll->flags & VSCROLLBAR_THUMB_PRESSED) ? INSET_RECT_FLAG_BORDER_INSET : 0));

    // Down button
    gfx_fill_rect_inset(dpi, l, b - 9, r, b, colour, ((scroll->flags & VSCROLLBAR_DOWN_PRESSED) ? INSET_RECT_FLAG_BORDER_INSET : 0));
    gfx_draw_string(dpi, (char*)BlackDownArrowString, COLOUR_BLACK, l + 1, b - 9);
}

/**
 *
 *  rct2: 0x006EB951
 */
static void widget_draw_image(rct_drawpixelinfo *dpi, rct_window *w, rct_widgetindex widgetIndex)
{
    // Get the widget
    rct_widget *widget = &w->widgets[widgetIndex];

    // Get the image
    int32_t image = widget->image;
    if (image == SPR_NONE)
        return;

    // Resolve the absolute ltrb
    int32_t l = w->x + widget->left;
    int32_t t = w->y + widget->top;

    // Get the colour
    uint8_t colour = NOT_TRANSLUCENT(w->colours[widget->colour]);

    if (widget->type == WWT_COLOURBTN || widget->type == WWT_TRNBTN || widget->type == WWT_TAB)
        if (widget_is_pressed(w, widgetIndex) || widget_is_active_tool(w, widgetIndex))
            image++;

    if (widget_is_disabled(w, widgetIndex)) {
        // Draw greyed out (light border bottom right shadow)
        colour = w->colours[widget->colour];
        colour = ColourMapA[NOT_TRANSLUCENT(colour)].lighter;
        gfx_draw_sprite_solid(dpi, image, l + 1, t + 1, colour);

        // Draw greyed out (dark)
        colour = w->colours[widget->colour];
        colour = ColourMapA[NOT_TRANSLUCENT(colour)].mid_light;
        gfx_draw_sprite_solid(dpi, image, l, t, colour);
    } else {
        if (image & IMAGE_TYPE_REMAP_2_PLUS) {
            // ?
        }

        if (image & IMAGE_TYPE_TRANSPARENT)
            image &= ~IMAGE_TYPE_TRANSPARENT;
        else
            image |= colour << 19;

        gfx_draw_sprite(dpi, image, l, t, 0);
    }
}

bool widget_is_enabled(rct_window *w, rct_widgetindex widgetIndex)
{
    return (w->enabled_widgets & (1LL << widgetIndex)) != 0;
}

bool widget_is_disabled(rct_window *w, rct_widgetindex widgetIndex)
{
    return (w->disabled_widgets & (1LL << widgetIndex)) != 0;
}

bool widget_is_pressed(rct_window *w, rct_widgetindex widgetIndex)
{
    if (w->pressed_widgets & (1LL << widgetIndex)) {
        return true;
    }
    if (input_get_state() == INPUT_STATE_WIDGET_PRESSED || input_get_state() == INPUT_STATE_DROPDOWN_ACTIVE) {
        if (!(input_test_flag(INPUT_FLAG_WIDGET_PRESSED))) return false;
        if (gPressedWidget.window_classification != w->classification) return false;
        if (gPressedWidget.window_number != w->number) return false;
        if (gPressedWidget.widget_index != widgetIndex) return false;
        return true;
    }
    return false;
}

bool widget_is_highlighted(rct_window *w, rct_widgetindex widgetIndex)
{
    if (gHoverWidget.window_classification != w->classification) return false;
    if (gHoverWidget.window_number != w->number) return false;
    if (gHoverWidget.widget_index != widgetIndex) return false;
    return true;
}

bool widget_is_active_tool(rct_window *w, rct_widgetindex widgetIndex)
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
void widget_scroll_get_part(rct_window *w, rct_widget *widget, int32_t x, int32_t y, int32_t *output_x, int32_t *output_y, int32_t *output_scroll_area, int32_t *scroll_id)
{
    rct_widget* iterator = w->widgets;
    *scroll_id = 0;
    while (++iterator != widget)
    {
        if (iterator->type == WWT_SCROLL)
        {
            (*scroll_id)++;
            break;
        }
    }

    if ((w->scrolls[*scroll_id].flags & HSCROLLBAR_VISIBLE) && y >= (w->y + widget->bottom - 11))
    {
        // horizontal scrollbar
        int32_t rightOffset = 0;
        int32_t iteratorLeft = widget->left + w->x + 10;
        int32_t iteratorRight = widget->right + w->x - 10;
        if (!(w->scrolls[*scroll_id].flags & VSCROLLBAR_VISIBLE))
        {
            rightOffset = 11;
        }

        if (x <= iteratorLeft)
        {
            *output_scroll_area = SCROLL_PART_HSCROLLBAR_LEFT;
        }
        else if (x >= iteratorRight + rightOffset)
        {
            *output_scroll_area = SCROLL_PART_NONE;
        }
        else if (x >= iteratorRight + rightOffset - 10)
        {
            *output_scroll_area = SCROLL_PART_HSCROLLBAR_RIGHT;
        }
        else if (x < (widget->left + w->x + w->scrolls[*scroll_id].h_thumb_left))
        {
            *output_scroll_area = SCROLL_PART_HSCROLLBAR_LEFT_TROUGH;
        }
        else if (x >(widget->left + w->x + w->scrolls[*scroll_id].h_thumb_right))
        {
            *output_scroll_area = SCROLL_PART_HSCROLLBAR_RIGHT_TROUGH;
        }
        else
        {
            *output_scroll_area = SCROLL_PART_HSCROLLBAR_THUMB;
        }
    }
    else if ((w->scrolls[*scroll_id].flags & VSCROLLBAR_VISIBLE) && (x >= w->x + widget->right - 11))
    {
        // vertical scrollbar
        int32_t bottomOffset = 0;
        int32_t iteratorTop = widget->top + w->y + 10;
        int32_t iteratorBottom = widget->bottom + w->y;
        if (w->scrolls[*scroll_id].flags & HSCROLLBAR_VISIBLE)
        {
            bottomOffset = 11;
        }

        if (y <= iteratorTop)
        {
            *output_scroll_area = SCROLL_PART_VSCROLLBAR_TOP;
        }
        else if (y >= (iteratorBottom - bottomOffset))
        {
            *output_scroll_area = SCROLL_PART_NONE;
        }
        else if (y >= (iteratorBottom - bottomOffset - 10))
        {
            *output_scroll_area = SCROLL_PART_VSCROLLBAR_BOTTOM;
        }
        else if (y < (widget->top + w->y + w->scrolls[*scroll_id].v_thumb_top))
        {
            *output_scroll_area = SCROLL_PART_VSCROLLBAR_TOP_TROUGH;
        }
        else if (y >(widget->top + w->y + w->scrolls[*scroll_id].v_thumb_bottom))
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
        //view
        *output_scroll_area = SCROLL_PART_VIEW;
        *output_x = x - widget->left;
        *output_y = y - widget->top;
        *output_x -= w->x;
        *output_y -= w->y;
        if (*output_x <= 0 || *output_y <= 0)
        {
            *output_scroll_area = SCROLL_PART_NONE;
        }
        else
        {
            *output_x += w->scrolls[*scroll_id].h_left - 1;
            *output_y += w->scrolls[*scroll_id].v_top - 1;
        }
    }
}

void widget_set_enabled(rct_window *w, rct_widgetindex widgetIndex, bool enabled)
{
    if (enabled) {
        w->enabled_widgets |= (1ULL << widgetIndex);
        w->disabled_widgets &= ~(1ULL << widgetIndex);
    }
    else {
        w->enabled_widgets &= ~(1ULL << widgetIndex);
        w->disabled_widgets |= (1ULL << widgetIndex);
    }
}

void widget_set_checkbox_value(rct_window *w, rct_widgetindex widgetIndex, int32_t value)
{
    if (value)
        w->pressed_widgets |= (1ULL << widgetIndex);
    else
        w->pressed_widgets &= ~(1ULL << widgetIndex);
}

static void widget_text_box_draw(rct_drawpixelinfo *dpi, rct_window *w, rct_widgetindex widgetIndex)
{
    int32_t no_lines = 0;
    int32_t font_height = 0;
    char wrapped_string[TEXT_INPUT_SIZE];

    // Get the widget
    rct_widget *widget = &w->widgets[widgetIndex];

    // Resolve the absolute ltrb
    int32_t l = w->x + widget->left;
    int32_t t = w->y + widget->top;
    int32_t r = w->x + widget->right;
    int32_t b = w->y + widget->bottom;

    // Get the colour
    uint8_t colour = w->colours[widget->colour];

    bool active = w->classification == gCurrentTextBox.window.classification &&
        w->number == gCurrentTextBox.window.number &&
        widgetIndex == gCurrentTextBox.widget_index;

    //gfx_fill_rect_inset(dpi, l, t, r, b, colour, 0x20 | (!active ? 0x40 : 0x00));
    gfx_fill_rect_inset(dpi, l, t, r, b, colour, INSET_RECT_F_60);

    gCurrentFontSpriteBase = FONT_SPRITE_BASE_MEDIUM;
    gCurrentFontFlags = 0;

    if (!active || gTextInput == nullptr) {

        if (w->widgets[widgetIndex].text != 0) {
            safe_strcpy(wrapped_string, w->widgets[widgetIndex].string, 512);
            gfx_wrap_string(wrapped_string, r - l - 5, &no_lines, &font_height);
            gfx_draw_string(dpi, wrapped_string, w->colours[1], l + 2, t);
        }
        return;
    }


    safe_strcpy(wrapped_string, gTextBoxInput, TEXT_INPUT_SIZE);

    // String length needs to add 12 either side of box
    // +13 for cursor when max length.
    gfx_wrap_string(wrapped_string, r - l - 5 - 6, &no_lines, &font_height);


    gfx_draw_string(dpi, wrapped_string, w->colours[1], l + 2, t);


    size_t string_length = get_string_size(wrapped_string) - 1;

    // Make a copy of the string for measuring the width.
    char temp_string[TEXT_INPUT_SIZE] = { 0 };
    memcpy(temp_string, wrapped_string, std::min(string_length, gTextInput->SelectionStart));
    int32_t cur_x = l + gfx_get_string_width(temp_string) + 3;

    int32_t width = 6;
    if ((uint32_t)gTextInput->SelectionStart < strlen(gTextBoxInput)){
        // Make a new 1 character wide string for measuring the width
        // of the character that the cursor is under.
        temp_string[1] = '\0';
        temp_string[0] = gTextBoxInput[gTextInput->SelectionStart];
        width = std::max(gfx_get_string_width(temp_string) - 2, 4);
    }

    if (gTextBoxFrameNo <= 15){
        colour = ColourMapA[w->colours[1]].mid_light;
        gfx_fill_rect(dpi, cur_x, t + 9, cur_x + width, t + 9, colour + 5);
    }
}
