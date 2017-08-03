#pragma region Copyright (c) 2014-2017 OpenRCT2 Developers
/*****************************************************************************
 * OpenRCT2, an open source clone of Roller Coaster Tycoon 2.
 *
 * OpenRCT2 is the work of many authors, a full list can be found in contributors.md
 * For more information, visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * A full copy of the GNU General Public License can be found in licence.txt
 *****************************************************************************/
#pragma endregion

#include "../Context.h"
#include "../core/Util.hpp"
#include "../core/Math.hpp"

extern "C"
{
#include "../input.h"
#include "../interface/widget.h"
#include "../localisation/localisation.h"
#include "../sprites.h"
#include "dropdown.h"
}

// The maximum number of rows to list before items overflow into new columns
#define DROPDOWN_TEXT_MAX_ROWS 32

sint32 gAppropriateImageDropdownItemsPerRow[] = {
    1, 1, 1, 1, 2, 2, 3, 3, 4,
    3, 5, 4, 4, 5, 5, 5, 4, 5,
    6, 5, 5, 7, 4, 5, 6, 5, 6,
    6, 6, 6, 6, 8, 8, 0
};

enum {
    WIDX_BACKGROUND,
};

static rct_widget window_dropdown_widgets[] = {
    { WWT_IMGBTN, 0, 0, 0, 0, 0, (uint32) SPR_NONE, STR_NONE },
    { WIDGETS_END },
};

sint32 _dropdown_num_columns;
sint32 _dropdown_num_rows;
sint32 _dropdown_item_width;
sint32 _dropdown_item_height;
bool _dropdown_list_vertically;

sint32 gDropdownNumItems;
rct_string_id gDropdownItemsFormat[DROPDOWN_ITEMS_MAX_SIZE];
sint64 gDropdownItemsArgs[DROPDOWN_ITEMS_MAX_SIZE];
static bool _dropdownItemsChecked[DROPDOWN_ITEMS_MAX_SIZE];
static bool _dropdownItemsDisabled[DROPDOWN_ITEMS_MAX_SIZE];
bool gDropdownIsColour;
sint32 gDropdownLastColourHover;
sint32 gDropdownHighlightedIndex;
sint32 gDropdownDefaultIndex;

bool dropdown_is_checked(sint32 index)
{
    if (index < 0 || index >= (sint32)Util::CountOf(_dropdownItemsDisabled))
    {
        return false;
    }
    return _dropdownItemsChecked[index];
}

bool dropdown_is_disabled(sint32 index)
{
    if (index < 0 || index >= (sint32)Util::CountOf(_dropdownItemsDisabled))
    {
        return true;
    }
    return _dropdownItemsDisabled[index];
}

void dropdown_set_checked(sint32 index, bool value)
{
    if (index < 0 || index >= (sint32)Util::CountOf(_dropdownItemsDisabled))
    {
        return;
    }
    _dropdownItemsChecked[index] = value;
}

void dropdown_set_disabled(sint32 index, bool value)
{
    if (index < 0 || index >= (sint32)Util::CountOf(_dropdownItemsDisabled))
    {
        return;
    }
    _dropdownItemsDisabled[index] = value;
}

static void window_dropdown_paint(rct_window *w, rct_drawpixelinfo *dpi);

static rct_window_event_list window_dropdown_events = {
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    window_dropdown_paint,
    NULL
};

/**
 * Shows a text dropdown menu.
 *  rct2: 0x006ECFB9
 *
 * @param x (cx)
 * @param y (dx)
 * @param extray (di)
 * @param flags (bh)
 * @param num_items (bx)
 * @param colour (al)
 */
void window_dropdown_show_text(sint32 x, sint32 y, sint32 extray, uint8 colour, uint8 flags, size_t num_items)
{
    sint32 string_width, max_string_width;
    char buffer[256];

    // Calculate the longest string width
    max_string_width = 0;
    for (size_t i = 0; i < num_items; i++) {
        format_string(buffer, 256, gDropdownItemsFormat[i], (void*)(&gDropdownItemsArgs[i]));
        gCurrentFontSpriteBase = FONT_SPRITE_BASE_MEDIUM;
        string_width = gfx_get_string_width(buffer);
        max_string_width = Math::Max(string_width, max_string_width);
    }

    window_dropdown_show_text_custom_width(x, y, extray, colour, 0, flags, num_items, max_string_width + 3);
}

/**
 * Shows a text dropdown menu.
 *  rct2: 0x006ECFB9, although 0x006ECE50 is real version
 *
 * @param x (cx)
 * @param y (dx)
 * @param extray (di)
 * @param flags (bh)
 * @param num_items (bx)
 * @param colour (al)
 * @param custom_height (ah) requires flag set as well
 */
void window_dropdown_show_text_custom_width(sint32 x, sint32 y, sint32 extray, uint8 colour, uint8 custom_height, uint8 flags, size_t num_items, sint32 width)
{
    rct_window* w;

    input_set_flag((INPUT_FLAGS) (INPUT_FLAG_DROPDOWN_STAY_OPEN | INPUT_FLAG_DROPDOWN_MOUSE_UP), false);
    if (flags & DROPDOWN_FLAG_STAY_OPEN)
        input_set_flag(INPUT_FLAG_DROPDOWN_STAY_OPEN, true);

    window_dropdown_close();
    
    // Set and calculate num items, rows and columns
    _dropdown_item_width = width;
    _dropdown_item_height = (flags & DROPDOWN_FLAG_CUSTOM_HEIGHT) ? custom_height : 10;
    gDropdownNumItems = (sint32)num_items;
    // There must always be at least one column to prevent dividing by zero
    if (gDropdownNumItems == 0)
    {
        _dropdown_num_columns = 1;
        _dropdown_num_rows = 0;
    }
    else
    {
        _dropdown_num_columns = (gDropdownNumItems + DROPDOWN_TEXT_MAX_ROWS - 1) / DROPDOWN_TEXT_MAX_ROWS;
        _dropdown_num_rows = (gDropdownNumItems + _dropdown_num_columns - 1) / _dropdown_num_columns;
    }
    
    // Text dropdowns are listed horizontally
    _dropdown_list_vertically = true;

    width = _dropdown_item_width * _dropdown_num_columns + 3;
    sint32 height = _dropdown_item_height * _dropdown_num_rows + 3;
    sint32 screenWidth = context_get_width();
    sint32 screenHeight = context_get_height();
    if (x + width > screenWidth)
        x = Math::Max(0, screenWidth - width);
    if (y + height > screenHeight)
        y = Math::Max(0, screenHeight - height);

    window_dropdown_widgets[WIDX_BACKGROUND].right = width;
    window_dropdown_widgets[WIDX_BACKGROUND].bottom = height;

    // Create the window
    w = window_create(
        x, y + extray,
        window_dropdown_widgets[WIDX_BACKGROUND].right + 1,
        window_dropdown_widgets[WIDX_BACKGROUND].bottom + 1,
        &window_dropdown_events,
        WC_DROPDOWN,
        WF_STICK_TO_FRONT
    );
    w->widgets = window_dropdown_widgets;
    if (colour & COLOUR_FLAG_TRANSLUCENT)
        w->flags |= WF_TRANSPARENT;
    w->colours[0] = colour;

    // Input state
    gDropdownHighlightedIndex = -1;
    memset(_dropdownItemsDisabled, 0, sizeof(_dropdownItemsDisabled));
    memset(_dropdownItemsChecked, 0, sizeof(_dropdownItemsChecked));
    gDropdownIsColour = false;
    gDropdownDefaultIndex = -1;
    input_set_state(INPUT_STATE_DROPDOWN_ACTIVE);
}

/**
 * Shows an image dropdown menu.
 *  rct2: 0x006ECFB9
 *
 * @param x (cx)
 * @param y (dx)
 * @param extray (di)
 * @param flags (bh)
 * @param numItems (bx)
 * @param colour (al)
 * @param itemWidth (bp)
 * @param itemHeight (ah)
 * @param numColumns (bl)
 */
void window_dropdown_show_image(sint32 x, sint32 y, sint32 extray, uint8 colour, uint8 flags, sint32 numItems, sint32 itemWidth, sint32 itemHeight, sint32 numColumns)
{
    sint32 width, height;
    rct_window* w;

    input_set_flag((INPUT_FLAGS) (INPUT_FLAG_DROPDOWN_STAY_OPEN | INPUT_FLAG_DROPDOWN_MOUSE_UP), false);
    if (flags & DROPDOWN_FLAG_STAY_OPEN)
        input_set_flag(INPUT_FLAG_DROPDOWN_STAY_OPEN, true);

    // Close existing dropdown
    window_dropdown_close();

    // Set and calculate num items, rows and columns
    _dropdown_item_width = itemWidth;
    _dropdown_item_height = itemHeight;
    gDropdownNumItems = numItems;
    // There must always be at least one column to prevent dividing by zero
    if (gDropdownNumItems == 0)
    {
        _dropdown_num_columns = 1;
        _dropdown_num_rows = 0;
    }
    else
    {
        _dropdown_num_columns = numColumns;
        _dropdown_num_rows = gDropdownNumItems / _dropdown_num_columns;
        if (gDropdownNumItems % _dropdown_num_columns != 0)
            _dropdown_num_rows++;
    }

    // image dropdowns are listed horizontally
    _dropdown_list_vertically = false;

    // Calculate position and size
    width = _dropdown_item_width * _dropdown_num_columns + 3;
    height = _dropdown_item_height * _dropdown_num_rows + 3;

    sint32 screenWidth = context_get_width();
    sint32 screenHeight = context_get_height();
    if (x + width > screenWidth)
        x = Math::Max(0, screenWidth - width);
    if (y + height > screenHeight)
        y = Math::Max(0, screenHeight - height);
    window_dropdown_widgets[WIDX_BACKGROUND].right = width;
    window_dropdown_widgets[WIDX_BACKGROUND].bottom = height;

    // Create the window
    w = window_create(
        x, y + extray,
        window_dropdown_widgets[WIDX_BACKGROUND].right + 1,
        window_dropdown_widgets[WIDX_BACKGROUND].bottom + 1,
        &window_dropdown_events,
        WC_DROPDOWN,
        WF_STICK_TO_FRONT
    );
    w->widgets = window_dropdown_widgets;
    if (colour & COLOUR_FLAG_TRANSLUCENT)
        w->flags |= WF_TRANSPARENT;
    w->colours[0] = colour;

    // Input state
    gDropdownHighlightedIndex = -1;
    memset(_dropdownItemsDisabled, 0, sizeof(_dropdownItemsDisabled));
    memset(_dropdownItemsChecked, 0, sizeof(_dropdownItemsChecked));
    gDropdownIsColour = false;
    gDropdownDefaultIndex = -1;
    input_set_state(INPUT_STATE_DROPDOWN_ACTIVE);
}

void window_dropdown_close()
{
    window_close_by_class(WC_DROPDOWN);
}

static void window_dropdown_paint(rct_window *w, rct_drawpixelinfo *dpi)
{
    sint32 cell_x, cell_y, l, t, r, b, item, image, colour;

    window_draw_widgets(w, dpi);

    sint32 highlightedIndex = gDropdownHighlightedIndex;
    for (sint32 i = 0; i < gDropdownNumItems; i++) {
        if (_dropdown_list_vertically) {
            cell_x = i / _dropdown_num_rows;
            cell_y = i % _dropdown_num_rows;
        }
        else {
            cell_x = i % _dropdown_num_columns;
            cell_y = i / _dropdown_num_columns;
        }

        if (gDropdownItemsFormat[i] == DROPDOWN_SEPARATOR) {
            l = w->x + 2 + (cell_x * _dropdown_item_width);
            t = w->y + 2 + (cell_y * _dropdown_item_height);
            r = l + _dropdown_item_width - 1;
            t += (_dropdown_item_height / 2);
            b = t;

            if (w->colours[0] & COLOUR_FLAG_TRANSLUCENT) {
                translucent_window_palette palette = TranslucentWindowPalettes[BASE_COLOUR(w->colours[0])];
                gfx_filter_rect(dpi, l, t, r, b, palette.highlight);
                gfx_filter_rect(dpi, l, t + 1, r, b + 1, palette.shadow);
            } else {
                gfx_fill_rect(dpi, l, t, r, b, ColourMapA[w->colours[0]].mid_dark);
                gfx_fill_rect(dpi, l, t + 1, r, b + 1, ColourMapA[w->colours[0]].lightest);
            }
        } else {
            //
            if (i == highlightedIndex) {
                l = w->x + 2 + (cell_x * _dropdown_item_width);
                t = w->y + 2 + (cell_y * _dropdown_item_height);
                r = l + _dropdown_item_width - 1;
                b = t + _dropdown_item_height - 1;
                gfx_filter_rect(dpi, l, t, r, b, PALETTE_DARKEN_3);
            }

            item = gDropdownItemsFormat[i];
            if (item == DROPDOWN_FORMAT_LAND_PICKER || item == DROPDOWN_FORMAT_COLOUR_PICKER) {
                // Image item
                image = (uint32)gDropdownItemsArgs[i];
                if (item == DROPDOWN_FORMAT_COLOUR_PICKER && highlightedIndex == i)
                    image++;

                gfx_draw_sprite(
                    dpi,
                    image,
                    w->x + 2 + (cell_x * _dropdown_item_width),
                    w->y + 2 + (cell_y * _dropdown_item_height), 0
                );
            } else {
                // Text item
                if (i < DROPDOWN_ITEMS_MAX_SIZE) {
                    if (dropdown_is_checked(i)) {
                        item++;
                    }
                }

                // Calculate colour
                colour = NOT_TRANSLUCENT(w->colours[0]);
                if (i == highlightedIndex)
                    colour = COLOUR_WHITE;
                if (dropdown_is_disabled(i))
                    if (i < DROPDOWN_ITEMS_MAX_SIZE)
                        colour = NOT_TRANSLUCENT(w->colours[0]) | COLOUR_FLAG_INSET;

                // Draw item string
                gfx_draw_string_left_clipped(
                    dpi,
                    item,
                    (void*)(&gDropdownItemsArgs[i]), colour,
                    w->x + 2 + (cell_x * _dropdown_item_width),
                    w->y + 1 + (cell_y * _dropdown_item_height),
                    w->width - 5
                );
            }
        }
    }
}

/**
 * New function based on 6e914e
 * returns -1 if index is invalid
 */
sint32 dropdown_index_from_point(sint32 x, sint32 y, rct_window *w)
{
    sint32 top = y - w->y - 2;
    if (top < 0) return -1;

    sint32 left = x - w->x;
    if (left >= w->width) return -1;
    left -= 2;
    if (left < 0) return -1;

    sint32 column_no = left / _dropdown_item_width;
    if (column_no >= _dropdown_num_columns) return -1;

    sint32 row_no = top / _dropdown_item_height;
    if (row_no >= _dropdown_num_rows) return -1;

    sint32 dropdown_index;
    if (_dropdown_list_vertically)
        dropdown_index = column_no * _dropdown_num_rows + row_no;
    else
        dropdown_index = row_no * _dropdown_num_columns + column_no;

    if (dropdown_index >= gDropdownNumItems) return -1;

    return dropdown_index;
}

/**
 *  rct2: 0x006ED43D
 */
void window_dropdown_show_colour(rct_window *w, rct_widget *widget, uint8 dropdownColour, uint8 selectedColour)
{
    sint32 defaultIndex = -1;
    // Set items
    for (uint64 i = 0; i < COLOUR_COUNT; i++)
    {
        if (selectedColour == i)
            defaultIndex = i;

        gDropdownItemsFormat[i] = DROPDOWN_FORMAT_COLOUR_PICKER;
        gDropdownItemsArgs[i] = (i << 32) | (SPRITE_ID_PALETTE_COLOUR_1(i) | SPR_PALETTE_BTN);
    }

    // Show dropdown
    window_dropdown_show_image(
        w->x + widget->left,
        w->y + widget->top,
        widget->bottom - widget->top + 1,
        dropdownColour,
        DROPDOWN_FLAG_STAY_OPEN,
        COLOUR_COUNT,
        12,
        12,
        gAppropriateImageDropdownItemsPerRow[COLOUR_COUNT]
    );

    gDropdownIsColour = true;
    gDropdownLastColourHover = -1;
    gDropdownDefaultIndex = defaultIndex;
}
