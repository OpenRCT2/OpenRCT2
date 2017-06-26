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

#include "../game.h"
#include "../config/Config.h"
#include "../localisation/localisation.h"
#include "../interface/viewport.h"
#include "../interface/widget.h"
#include "../interface/window.h"
#include "../ride/ride.h"
#include "../world/map.h"
#include "../world/banner.h"
#include "../world/scenery.h"
#include "error.h"
#include "dropdown.h"
#include "../drawing/drawing.h"
#include "../sprites.h"

#define WW 113
#define WH 96

enum WINDOW_SIGN_WIDGET_IDX {
    WIDX_BACKGROUND,
    WIDX_TITLE,
    WIDX_CLOSE,
    WIDX_VIEWPORT,
    WIDX_SIGN_TEXT,
    WIDX_SIGN_DEMOLISH,
    WIDX_MAIN_COLOUR,
    WIDX_TEXT_COLOUR
};

// 0x9AEE00
rct_widget window_sign_widgets[] = {
        { WWT_FRAME,    0, 0,       WW - 1,     0,          WH - 1,     0xFFFFFFFF,     STR_NONE },                         // panel / background
        { WWT_CAPTION,  0, 1,       WW - 2,     1,          14,         STR_SIGN,       STR_WINDOW_TITLE_TIP },             // title bar
        { WWT_CLOSEBOX, 0, WW - 13, WW - 3,     2,          13,         STR_CLOSE_X,    STR_CLOSE_WINDOW_TIP },             // close x button
        { WWT_VIEWPORT, 1, 3,       WW - 26,    17,         WH - 20,    0xFFFFFFFE,     STR_NONE },                         // Viewport
        { WWT_FLATBTN,  1, WW - 25, WW - 2,     19,         42,         SPR_RENAME,     STR_CHANGE_SIGN_TEXT_TIP },         // change sign button
        { WWT_FLATBTN,  1, WW - 25, WW - 2,     67,         90,         SPR_DEMOLISH,   STR_DEMOLISH_SIGN_TIP },            // demolish button
        { WWT_COLOURBTN, 1, 5,      16,         WH - 16,    WH - 5,     0xFFFFFFFF,     STR_SELECT_MAIN_SIGN_COLOUR_TIP },  // Main colour
        { WWT_COLOURBTN, 1, 17,     28,         WH - 16,    WH - 5,     0xFFFFFFFF,     STR_SELECT_TEXT_COLOUR_TIP },       // Text colour
        { WIDGETS_END },
};

static void window_sign_mouseup(rct_window *w, rct_widgetindex widgetIndex);
static void window_sign_mousedown(rct_widgetindex widgetIndex, rct_window*w, rct_widget* widget);
static void window_sign_dropdown(rct_window *w, rct_widgetindex widgetIndex, sint32 dropdownIndex);
static void window_sign_textinput(rct_window *w, rct_widgetindex widgetIndex, char *text);
static void window_sign_viewport_rotate(rct_window *w);
static void window_sign_invalidate(rct_window *w);
static void window_sign_paint(rct_window *w, rct_drawpixelinfo *dpi);


// 0x98E44C
static rct_window_event_list window_sign_events = {
    NULL,
    window_sign_mouseup,
    NULL,
    window_sign_mousedown,
    window_sign_dropdown,
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
    window_sign_textinput,
    window_sign_viewport_rotate,
    NULL,
    NULL,
    NULL,
    NULL,
    window_sign_invalidate,
    window_sign_paint,
    NULL
};

static void window_sign_small_mouseup(rct_window *w, rct_widgetindex widgetIndex);
static void window_sign_small_dropdown(rct_window *w, rct_widgetindex widgetIndex, sint32 dropdownIndex);
static void window_sign_small_invalidate(rct_window *w);

// 0x9A410C
static rct_window_event_list window_sign_small_events = {
    NULL,
    window_sign_small_mouseup,
    NULL,
    window_sign_mousedown,
    window_sign_small_dropdown,
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
    window_sign_textinput,
    window_sign_viewport_rotate,
    NULL,
    NULL,
    NULL,
    NULL,
    window_sign_small_invalidate,
    window_sign_paint,
    NULL
};

/**
*
*  rct2: 0x006BA305
*/
void window_sign_open(rct_windownumber number)
{
    rct_window* w;
    rct_widget *viewportWidget;


    // Check if window is already open
    w = window_bring_to_front_by_number(WC_BANNER, number);
    if (w != NULL)
        return;

    w = window_create_auto_pos(WW, WH, &window_sign_events, WC_BANNER, WF_NO_SCROLLING);
    w->widgets = window_sign_widgets;
    w->enabled_widgets =
        (1 << WIDX_CLOSE) |
        (1 << WIDX_SIGN_TEXT) |
        (1 << WIDX_SIGN_DEMOLISH) |
        (1 << WIDX_MAIN_COLOUR) |
        (1 << WIDX_TEXT_COLOUR);

    w->number = number;
    window_init_scroll_widgets(w);

    sint32 view_x = gBanners[w->number].x << 5;
    sint32 view_y = gBanners[w->number].y << 5;

    rct_map_element* map_element = map_get_first_element_at(view_x / 32, view_y / 32);

    while (1){
        if (map_element_get_type(map_element) == MAP_ELEMENT_TYPE_SCENERY_MULTIPLE) {
            rct_scenery_entry* scenery_entry = get_large_scenery_entry(map_element->properties.scenerymultiple.type & MAP_ELEMENT_LARGE_TYPE_MASK);
            if (scenery_entry->large_scenery.var_11 != 0xFF){
                sint32 id = (map_element->type & 0xC0) |
                    ((map_element->properties.scenerymultiple.colour[0] & 0xE0) >> 2) |
                    ((map_element->properties.scenerymultiple.colour[1] & 0xE0) >> 5);
                if (id == w->number)
                    break;
            }
        }
        map_element++;
    }

    sint32 view_z = map_element->base_height << 3;
    w->frame_no = view_z;

    w->list_information_type = map_element->properties.scenerymultiple.colour[0] & 0x1F;
    w->var_492 = map_element->properties.scenerymultiple.colour[1] & 0x1F;
    w->var_48C = map_element->properties.scenerymultiple.type;

    view_x += 16;
    view_y += 16;

    // Create viewport
    viewportWidget = &window_sign_widgets[WIDX_VIEWPORT];
    viewport_create(
        w,
        w->x + viewportWidget->left + 1,
        w->y + viewportWidget->top + 1,
        (viewportWidget->right - viewportWidget->left) - 1,
        (viewportWidget->bottom - viewportWidget->top) - 1,
        0,
        view_x,
        view_y,
        view_z,
        0,
        -1
    );

    w->viewport->flags = gConfigGeneral.always_show_gridlines ? VIEWPORT_FLAG_GRIDLINES : 0;
    window_invalidate(w);
}

/**
 *
 *  rct2: 0x6B9765
 */
static void window_sign_mouseup(rct_window *w, rct_widgetindex widgetIndex)
{
    rct_banner* banner = &gBanners[w->number];
    sint32 x = banner->x << 5;
    sint32 y = banner->y << 5;

    rct_string_id string_id;

    rct_map_element* map_element = map_get_first_element_at(x / 32, y / 32);

    switch (widgetIndex) {
    case WIDX_CLOSE:
        window_close(w);
        break;
    case WIDX_SIGN_DEMOLISH:
        while (1){
            if (map_element_get_type(map_element) == MAP_ELEMENT_TYPE_SCENERY_MULTIPLE) {
                rct_scenery_entry* scenery_entry = get_large_scenery_entry(map_element->properties.scenerymultiple.type & MAP_ELEMENT_LARGE_TYPE_MASK);
                if (scenery_entry->large_scenery.var_11 != 0xFF){
                    sint32 id = (map_element->type & 0xC0) |
                        ((map_element->properties.scenerymultiple.colour[0] & 0xE0) >> 2) |
                        ((map_element->properties.scenerymultiple.colour[1] & 0xE0) >> 5);
                    if (id == w->number)
                        break;
                }
            }
            map_element++;
        }
        game_do_command(
            x,
            1 | ((map_element->type&0x3) << 8),
            y,
            map_element->base_height | ((map_element->properties.scenerymultiple.type >> 10) << 8),
            GAME_COMMAND_REMOVE_LARGE_SCENERY,
            0,
            0);
        break;
    case WIDX_SIGN_TEXT:
        if (banner->flags & BANNER_FLAG_LINKED_TO_RIDE){
            rct_ride* ride = get_ride(banner->colour);
            set_format_arg(16, uint32, ride->name_arguments);
            string_id = ride->name;
        }
        else
        {
            string_id = gBanners[w->number].string_idx;
        }
        window_text_input_open(w, WIDX_SIGN_TEXT, STR_SIGN_TEXT_TITLE, STR_SIGN_TEXT_PROMPT, string_id, 0, 32);
        break;
    }
}

/**
 *
 *  rct2: 0x6B9784
  & 0x6E6164 */
static void window_sign_mousedown(rct_widgetindex widgetIndex, rct_window*w, rct_widget* widget)
{
    switch (widgetIndex) {
    case WIDX_MAIN_COLOUR:
        window_dropdown_show_colour(w, widget, TRANSLUCENT(w->colours[1]), (uint8)w->list_information_type);
        break;
    case WIDX_TEXT_COLOUR:
        window_dropdown_show_colour(w, widget, TRANSLUCENT(w->colours[1]), (uint8)w->var_492);
        break;
    }
}

/**
 *
 *  rct2: 0x6B979C
 */
static void window_sign_dropdown(rct_window *w, rct_widgetindex widgetIndex, sint32 dropdownIndex)
{
    switch (widgetIndex){
    case WIDX_MAIN_COLOUR:
        if (dropdownIndex == -1) return;
        w->list_information_type = dropdownIndex;
        game_do_command(1, GAME_COMMAND_FLAG_APPLY, w->number, dropdownIndex, GAME_COMMAND_SET_SIGN_STYLE, w->var_492, 1);
        break;
    case WIDX_TEXT_COLOUR:
        if (dropdownIndex == -1) return;
        w->var_492 = dropdownIndex;
        game_do_command(1, GAME_COMMAND_FLAG_APPLY, w->number, w->list_information_type, GAME_COMMAND_SET_SIGN_STYLE, dropdownIndex, 1);
        break;
    default:
        return;
    }

    window_invalidate(w);
}

/**
 *
 *  rct2: 0x6B9791, 0x6E6171
 */
static void window_sign_textinput(rct_window *w, rct_widgetindex widgetIndex, char *text)
{
    if (widgetIndex == WIDX_SIGN_TEXT && text != NULL) {
        game_do_command(1, GAME_COMMAND_FLAG_APPLY, w->number, *((sint32*)(text + 0)), GAME_COMMAND_SET_SIGN_NAME, *((sint32*)(text + 8)), *((sint32*)(text + 4)));
        game_do_command(2, GAME_COMMAND_FLAG_APPLY, w->number, *((sint32*)(text + 12)), GAME_COMMAND_SET_SIGN_NAME, *((sint32*)(text + 20)), *((sint32*)(text + 16)));
        game_do_command(0, GAME_COMMAND_FLAG_APPLY, w->number, *((sint32*)(text + 24)), GAME_COMMAND_SET_SIGN_NAME, *((sint32*)(text + 32)), *((sint32*)(text + 28)));
    }
}

/**
 *
 *  rct2: 0x006B96F5
 */
static void window_sign_invalidate(rct_window *w)
{
    rct_widget* main_colour_btn = &window_sign_widgets[WIDX_MAIN_COLOUR];
    rct_widget* text_colour_btn = &window_sign_widgets[WIDX_TEXT_COLOUR];

    rct_scenery_entry* scenery_entry = get_large_scenery_entry(w->var_48C);

    main_colour_btn->type = WWT_EMPTY;
    text_colour_btn->type = WWT_EMPTY;

    if (scenery_entry->large_scenery.flags & LARGE_SCENERY_FLAG_HAS_PRIMARY_COLOUR){
        main_colour_btn->type = WWT_COLOURBTN;
    }
    if (scenery_entry->large_scenery.flags & LARGE_SCENERY_FLAG_HAS_SECONDARY_COLOUR) {
        text_colour_btn->type = WWT_COLOURBTN;
    }

    main_colour_btn->image = (w->list_information_type << 19) | 0x60000000 | SPR_PALETTE_BTN;
    text_colour_btn->image = (w->var_492 << 19) | 0x60000000 | SPR_PALETTE_BTN;
}

/**
 *
 *  rct2: 0x006B9754, 0x006E6134
 */
static void window_sign_paint(rct_window *w, rct_drawpixelinfo *dpi)
{
    window_draw_widgets(w, dpi);

    // Draw viewport
    if (w->viewport != NULL) {
        window_draw_viewport(dpi, w);
    }
}

/**
 *
 *  rct2: 0x6B9A6C, 0x6E6424
 */
static void window_sign_viewport_rotate(rct_window *w)
{
    rct_viewport* view = w->viewport;
    w->viewport = 0;

    view->width = 0;
    viewport_update_pointers();

    rct_banner* banner = &gBanners[w->number];

    sint32 view_x = (banner->x << 5) + 16;
    sint32 view_y = (banner->y << 5) + 16;
    sint32 view_z = w->frame_no;

    // Create viewport
    rct_widget* viewportWidget = &window_sign_widgets[WIDX_VIEWPORT];
    viewport_create(
        w,
        w->x + viewportWidget->left + 1,
        w->y + viewportWidget->top + 1,
        (viewportWidget->right - viewportWidget->left) - 1,
        (viewportWidget->bottom - viewportWidget->top) - 1,
        0,
        view_x,
        view_y,
        view_z,
        0,
        -1
        );

    w->viewport->flags = gConfigGeneral.always_show_gridlines ? VIEWPORT_FLAG_GRIDLINES : 0;
    window_invalidate(w);
}


/**
 *
 *  rct2: 0x6E5F52
 */
void window_sign_small_open(rct_windownumber number){
    rct_window* w;
    rct_widget *viewportWidget;


    // Check if window is already open
    w = window_bring_to_front_by_number(WC_BANNER, number);
    if (w != NULL)
        return;

    w = window_create_auto_pos(WW, WH, &window_sign_small_events, WC_BANNER, 0);
    w->widgets = window_sign_widgets;
    w->enabled_widgets =
        (1 << WIDX_CLOSE) |
        (1 << WIDX_SIGN_TEXT) |
        (1 << WIDX_SIGN_DEMOLISH) |
        (1 << WIDX_MAIN_COLOUR) |
        (1 << WIDX_TEXT_COLOUR);

    w->number = number;
    window_init_scroll_widgets(w);
    w->colours[0] = COLOUR_DARK_BROWN;
    w->colours[1] = COLOUR_DARK_BROWN;
    w->colours[2] = COLOUR_DARK_BROWN;

    sint32 view_x = gBanners[w->number].x << 5;
    sint32 view_y = gBanners[w->number].y << 5;

    rct_map_element* map_element = map_get_first_element_at(view_x / 32, view_y / 32);

    while (1){
        if (map_element_get_type(map_element) == MAP_ELEMENT_TYPE_WALL) {
            rct_scenery_entry* scenery_entry = get_wall_entry(map_element->properties.wall.type);
            if (scenery_entry->wall.scrolling_mode != 0xFF){
                if (map_element->properties.wall.banner_index == w->number)
                    break;
            }
        }
        map_element++;
    }

    sint32 view_z = map_element->base_height << 3;
    w->frame_no = view_z;

    w->list_information_type = map_element->properties.wall.colour_1 & 0x1F;
    w->var_492 = wall_element_get_secondary_colour(map_element);
    w->var_48C = map_element->properties.wall.type;

    view_x += 16;
    view_y += 16;

    // Create viewport
    viewportWidget = &window_sign_widgets[WIDX_VIEWPORT];
    viewport_create(
        w,
        w->x + viewportWidget->left + 1,
        w->y + viewportWidget->top + 1,
        (viewportWidget->right - viewportWidget->left) - 1,
        (viewportWidget->bottom - viewportWidget->top) - 1,
        0,
        view_x,
        view_y,
        view_z,
        0,
        -1
    );

    w->viewport->flags = gConfigGeneral.always_show_gridlines ? VIEWPORT_FLAG_GRIDLINES : 0;
    w->flags |= WF_NO_SCROLLING;
    window_invalidate(w);
}

/**
 *
 *  rct2: 0x6E6145
 */
static void window_sign_small_mouseup(rct_window *w, rct_widgetindex widgetIndex)
{
    rct_banner* banner = &gBanners[w->number];
    sint32 x = banner->x << 5;
    sint32 y = banner->y << 5;

    rct_string_id string_id;

    rct_map_element* map_element = map_get_first_element_at(x / 32, y / 32);

    switch (widgetIndex) {
    case WIDX_CLOSE:
        window_close(w);
        break;
    case WIDX_SIGN_DEMOLISH:
        while (1){
            if (map_element_get_type(map_element) == MAP_ELEMENT_TYPE_WALL) {
                rct_scenery_entry* scenery_entry = get_wall_entry(map_element->properties.wall.type);
                if (scenery_entry->wall.scrolling_mode != 0xFF){
                    if (map_element->properties.wall.banner_index == w->number)
                        break;
                }
            }
            map_element++;
        }
        gGameCommandErrorTitle = STR_CANT_REMOVE_THIS;
        game_do_command(
            x,
            1 | ((map_element->type & 0x3) << 8),
            y,
            (map_element->base_height << 8) | (map_element->type & 0x3),
            GAME_COMMAND_REMOVE_WALL,
            0,
            0);
        break;
    case WIDX_SIGN_TEXT:
        if (banner->flags & BANNER_FLAG_LINKED_TO_RIDE){
            rct_ride* ride = get_ride(banner->colour);
            set_format_arg(16, uint32, ride->name_arguments);
            string_id = ride->name;
        }
        else
        {
            string_id = gBanners[w->number].string_idx;
        }
        window_text_input_open(w, WIDX_SIGN_TEXT, STR_SIGN_TEXT_TITLE, STR_SIGN_TEXT_PROMPT, string_id, 0, 32);
        break;
    }
}

/**
 *
 *  rct2: 0x6E617C
 */
static void window_sign_small_dropdown(rct_window *w, rct_widgetindex widgetIndex, sint32 dropdownIndex)
{
    switch (widgetIndex){
    case WIDX_MAIN_COLOUR:
        if (dropdownIndex == -1) return;
        w->list_information_type = dropdownIndex;
        game_do_command(1, GAME_COMMAND_FLAG_APPLY, w->number, dropdownIndex, GAME_COMMAND_SET_SIGN_STYLE, w->var_492, 0);
        break;
    case WIDX_TEXT_COLOUR:
        if (dropdownIndex == -1) return;
        w->var_492 = dropdownIndex;
        game_do_command(1, GAME_COMMAND_FLAG_APPLY, w->number, w->list_information_type, GAME_COMMAND_SET_SIGN_STYLE, dropdownIndex, 0);
        break;
    default:
        return;
    }

    window_invalidate(w);
}

/**
 *
 *  rct2: 0x006E60D5
 */
static void window_sign_small_invalidate(rct_window *w)
{
    rct_widget* main_colour_btn = &window_sign_widgets[WIDX_MAIN_COLOUR];
    rct_widget* text_colour_btn = &window_sign_widgets[WIDX_TEXT_COLOUR];

    rct_scenery_entry* scenery_entry = get_wall_entry(w->var_48C);

    main_colour_btn->type = WWT_EMPTY;
    text_colour_btn->type = WWT_EMPTY;

    if (scenery_entry->wall.flags & WALL_SCENERY_HAS_PRIMARY_COLOUR) {
        main_colour_btn->type = WWT_COLOURBTN;
    }
    if (scenery_entry->wall.flags & WALL_SCENERY_HAS_SECONDARY_COLOUR) {
        text_colour_btn->type = WWT_COLOURBTN;
    }

    main_colour_btn->image = (w->list_information_type << 19) | 0x60000000 | SPR_PALETTE_BTN;
    text_colour_btn->image = (w->var_492 << 19) | 0x60000000 | SPR_PALETTE_BTN;
}
