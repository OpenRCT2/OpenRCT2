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

#include "../config/Config.h"
#include "../network/network.h"

extern "C"
{
#include "../interface/widget.h"
#include "../localisation/localisation.h"
#include "../sprites.h"
#include "../util/util.h"
#include "dropdown.h"
}

enum {
    WINDOW_MULTIPLAYER_PAGE_INFORMATION,
    WINDOW_MULTIPLAYER_PAGE_PLAYERS,
    WINDOW_MULTIPLAYER_PAGE_GROUPS,
    WINDOW_MULTIPLAYER_PAGE_OPTIONS
};

enum WINDOW_MULTIPLAYER_WIDGET_IDX {
    WIDX_BACKGROUND,
    WIDX_TITLE,
    WIDX_CLOSE,
    WIDX_CONTENT_PANEL,
    WIDX_TAB1,
    WIDX_TAB2,
    WIDX_TAB3,
    WIDX_TAB4,

    WIDX_LIST = 8,

    WIDX_DEFAULT_GROUP = 8,
    WIDX_DEFAULT_GROUP_DROPDOWN,
    WIDX_ADD_GROUP,
    WIDX_REMOVE_GROUP,
    WIDX_RENAME_GROUP,
    WIDX_SELECTED_GROUP,
    WIDX_SELECTED_GROUP_DROPDOWN,
    WIDX_PERMISSIONS_LIST,

    WIDX_LOG_CHAT_CHECKBOX = 8,
    WIDX_LOG_SERVER_ACTIONS_CHECKBOX,
    WIDX_KNOWN_KEYS_ONLY_CHECKBOX,
};

#define MAIN_MULTIPLAYER_WIDGETS \
    { WWT_FRAME,            0,  0,      339,    0,      239,    0xFFFFFFFF,                 STR_NONE },                 /* panel / background   */  \
    { WWT_CAPTION,          0,  1,      338,    1,      14,     STR_MULTIPLAYER,            STR_WINDOW_TITLE_TIP },     /* title bar            */  \
    { WWT_CLOSEBOX,         0,  327,    337,    2,      13,     STR_CLOSE_X,                STR_CLOSE_WINDOW_TIP },     /* close x button       */  \
    { WWT_RESIZE,           1,  0,      339,    43,     239,    0xFFFFFFFF,                 STR_NONE },                 /* content panel        */  \
    { WWT_TAB,              1,  3,      33,     17,     43,     IMAGE_TYPE_REMAP | SPR_TAB,       STR_SHOW_SERVER_INFO_TIP }, /* tab                  */  \
    { WWT_TAB,              1,  3,      33,     17,     43,     IMAGE_TYPE_REMAP | SPR_TAB,       STR_PLAYERS_TIP },          /* tab                  */  \
    { WWT_TAB,              1,  3,      33,     17,     43,     IMAGE_TYPE_REMAP | SPR_TAB,       STR_GROUPS_TIP },           /* tab                  */  \
    { WWT_TAB,              1,  3,      33,     17,     43,     IMAGE_TYPE_REMAP | SPR_TAB,       STR_OPTIONS_TIP }           /* tab                  */  \

static rct_widget window_multiplayer_information_widgets[] = {
    MAIN_MULTIPLAYER_WIDGETS,
    { WIDGETS_END }
};

static rct_widget window_multiplayer_players_widgets[] = {
    MAIN_MULTIPLAYER_WIDGETS,
    { WWT_SCROLL,           1,  3,      336,    60,     236,    SCROLL_VERTICAL,            STR_NONE },                 // list
    { WIDGETS_END }
};

static rct_widget window_multiplayer_groups_widgets[] = {
    MAIN_MULTIPLAYER_WIDGETS,
    { WWT_DROPDOWN,         1,  141,    315,    46,     57,     0xFFFFFFFF,                 STR_NONE },                 // default group
    { WWT_DROPDOWN_BUTTON,  1,  305,    315,    47,     56,     STR_DROPDOWN_GLYPH,         STR_NONE },                 //
    { WWT_DROPDOWN_BUTTON,  1,  11,     102,    65,     76,     STR_ADD_GROUP,              STR_NONE },                 // add group button
    { WWT_DROPDOWN_BUTTON,  1,  113,    204,    65,     76,     STR_REMOVE_GROUP,           STR_NONE },                 // remove group button
    { WWT_DROPDOWN_BUTTON,  1,  215,    306,    65,     76,     STR_RENAME_GROUP,           STR_NONE },                 // rename group button
    { WWT_DROPDOWN,         1,  72,     246,    80,     91,     0xFFFFFFFF,                 STR_NONE },                 // selected group
    { WWT_DROPDOWN_BUTTON,  1,  236,    246,    81,     90,     STR_DROPDOWN_GLYPH,         STR_NONE },                 //
    { WWT_SCROLL,           1,  3,      316,    94,     300,    SCROLL_VERTICAL,                            STR_NONE },                 // permissions list
    { WIDGETS_END }
};

static rct_widget window_multiplayer_options_widgets[] = {
    MAIN_MULTIPLAYER_WIDGETS,
    { WWT_CHECKBOX,         1,  3,      297,    50,     61,     STR_LOG_CHAT,               STR_LOG_CHAT_TIP },
    { WWT_CHECKBOX,         1,  3,      297,    64,     75,     STR_LOG_SERVER_ACTIONS,     STR_LOG_SERVER_ACTIONS_TIP },
    { WWT_CHECKBOX,         1,  3,      297,    78,     89,     STR_ALLOW_KNOWN_KEYS_ONLY,  STR_ALLOW_KNOWN_KEYS_ONLY_TIP },
    { WIDGETS_END }
};

static rct_widget *window_multiplayer_page_widgets[] = {
    window_multiplayer_information_widgets,
    window_multiplayer_players_widgets,
    window_multiplayer_groups_widgets,
    window_multiplayer_options_widgets
};

const uint64 window_multiplayer_page_enabled_widgets[] = {
    (1 << WIDX_CLOSE) | (1 << WIDX_TAB1) | (1 << WIDX_TAB2) | (1 << WIDX_TAB3) | (1 << WIDX_TAB4),
    (1 << WIDX_CLOSE) | (1 << WIDX_TAB1) | (1 << WIDX_TAB2) | (1 << WIDX_TAB3) | (1 << WIDX_TAB4),
    (1 << WIDX_CLOSE) | (1 << WIDX_TAB1) | (1 << WIDX_TAB2) | (1 << WIDX_TAB3) | (1 << WIDX_TAB4) | (1 << WIDX_DEFAULT_GROUP) | (1 << WIDX_DEFAULT_GROUP_DROPDOWN) | (1 << WIDX_ADD_GROUP) | (1 << WIDX_REMOVE_GROUP) | (1 << WIDX_RENAME_GROUP) | (1 << WIDX_SELECTED_GROUP) | (1 << WIDX_SELECTED_GROUP_DROPDOWN),
    (1 << WIDX_CLOSE) | (1 << WIDX_TAB1) | (1 << WIDX_TAB2) | (1 << WIDX_TAB3) | (1 << WIDX_TAB4) | (1 << WIDX_LOG_CHAT_CHECKBOX) | (1 << WIDX_LOG_SERVER_ACTIONS_CHECKBOX) | (1 << WIDX_KNOWN_KEYS_ONLY_CHECKBOX),
};

static uint8 _selectedGroup = 0;

static void window_multiplayer_information_mouseup(rct_window *w, rct_widgetindex widgetIndex);
static void window_multiplayer_information_resize(rct_window *w);
static void window_multiplayer_information_update(rct_window *w);
static void window_multiplayer_information_invalidate(rct_window *w);
static void window_multiplayer_information_paint(rct_window *w, rct_drawpixelinfo *dpi);

static void window_multiplayer_players_mouseup(rct_window *w, rct_widgetindex widgetIndex);
static void window_multiplayer_players_resize(rct_window *w);
static void window_multiplayer_players_update(rct_window *w);
static void window_multiplayer_players_scrollgetsize(rct_window *w, sint32 scrollIndex, sint32 *width, sint32 *height);
static void window_multiplayer_players_scrollmousedown(rct_window *w, sint32 scrollIndex, sint32 x, sint32 y);
static void window_multiplayer_players_scrollmouseover(rct_window *w, sint32 scrollIndex, sint32 x, sint32 y);
static void window_multiplayer_players_invalidate(rct_window *w);
static void window_multiplayer_players_paint(rct_window *w, rct_drawpixelinfo *dpi);
static void window_multiplayer_players_scrollpaint(rct_window *w, rct_drawpixelinfo *dpi, sint32 scrollIndex);

static void window_multiplayer_groups_mouseup(rct_window *w, rct_widgetindex widgetIndex);
static void window_multiplayer_groups_resize(rct_window *w);
static void window_multiplayer_groups_mousedown(rct_window *w, rct_widgetindex widgetIndex, rct_widget* widget);
static void window_multiplayer_groups_dropdown(rct_window *w, rct_widgetindex widgetIndex, sint32 dropdownIndex);
static void window_multiplayer_groups_update(rct_window *w);
static void window_multiplayer_groups_scrollgetsize(rct_window *w, sint32 scrollIndex, sint32 *width, sint32 *height);
static void window_multiplayer_groups_scrollmousedown(rct_window *w, sint32 scrollIndex, sint32 x, sint32 y);
static void window_multiplayer_groups_scrollmouseover(rct_window *w, sint32 scrollIndex, sint32 x, sint32 y);
static void window_multiplayer_groups_text_input(rct_window *w, rct_widgetindex widgetIndex, char *text);
static void window_multiplayer_groups_invalidate(rct_window *w);
static void window_multiplayer_groups_paint(rct_window *w, rct_drawpixelinfo *dpi);
static void window_multiplayer_groups_scrollpaint(rct_window *w, rct_drawpixelinfo *dpi, sint32 scrollIndex);

static void window_multiplayer_options_mouseup(rct_window *w, rct_widgetindex widgetIndex);
static void window_multiplayer_options_resize(rct_window *w);
static void window_multiplayer_options_update(rct_window *w);
static void window_multiplayer_options_invalidate(rct_window *w);
static void window_multiplayer_options_paint(rct_window *w, rct_drawpixelinfo *dpi);

static rct_window_event_list window_multiplayer_information_events = {
    NULL,
    window_multiplayer_information_mouseup,
    window_multiplayer_information_resize,
    NULL,
    NULL,
    NULL,
    window_multiplayer_information_update,
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
    window_multiplayer_information_invalidate,
    window_multiplayer_information_paint,
    NULL
};

static rct_window_event_list window_multiplayer_players_events = {
    NULL,
    window_multiplayer_players_mouseup,
    window_multiplayer_players_resize,
    NULL,
    NULL,
    NULL,
    window_multiplayer_players_update,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    window_multiplayer_players_scrollgetsize,
    window_multiplayer_players_scrollmousedown,
    NULL,
    window_multiplayer_players_scrollmouseover,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    window_multiplayer_players_invalidate,
    window_multiplayer_players_paint,
    window_multiplayer_players_scrollpaint
};

static rct_window_event_list window_multiplayer_groups_events = {
    NULL,
    window_multiplayer_groups_mouseup,
    window_multiplayer_groups_resize,
    window_multiplayer_groups_mousedown,
    window_multiplayer_groups_dropdown,
    NULL,
    window_multiplayer_groups_update,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    window_multiplayer_groups_scrollgetsize,
    window_multiplayer_groups_scrollmousedown,
    NULL,
    window_multiplayer_groups_scrollmouseover,
    window_multiplayer_groups_text_input,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    window_multiplayer_groups_invalidate,
    window_multiplayer_groups_paint,
    window_multiplayer_groups_scrollpaint
};

static rct_window_event_list window_multiplayer_options_events = {
    NULL,
    window_multiplayer_options_mouseup,
    window_multiplayer_options_resize,
    NULL,
    NULL,
    NULL,
    window_multiplayer_options_update,
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
    window_multiplayer_options_invalidate,
    window_multiplayer_options_paint,
    NULL
};

static rct_window_event_list *window_multiplayer_page_events[] = {
    &window_multiplayer_information_events,
    &window_multiplayer_players_events,
    &window_multiplayer_groups_events,
    &window_multiplayer_options_events
};

static const sint32 window_multiplayer_animation_divisor[] = { 4, 4, 2, 2 };
static const sint32 window_multiplayer_animation_frames[] = { 8, 8, 7, 4 };

static void window_multiplayer_draw_tab_images(rct_window *w, rct_drawpixelinfo *dpi);
static void window_multiplayer_set_page(rct_window* w, sint32 page);

static bool _windowInformationSizeDirty;
static rct_xy16 _windowInformationSize;

void window_multiplayer_open()
{
    // Check if window is already open
    rct_window *window = window_bring_to_front_by_class(WC_MULTIPLAYER);
    if (window == NULL) {
        window = window_create_auto_pos(320, 144, &window_multiplayer_players_events, WC_MULTIPLAYER, WF_10 | WF_RESIZABLE);
        window_multiplayer_set_page(window, WINDOW_MULTIPLAYER_PAGE_INFORMATION);
    }
}

static void window_multiplayer_set_page(rct_window* w, sint32 page)
{
    _windowInformationSizeDirty = true;

    w->page = page;
    w->frame_no = 0;
    w->no_list_items = 0;
    w->selected_list_item = -1;

    w->enabled_widgets = window_multiplayer_page_enabled_widgets[page];
    w->hold_down_widgets = 0;
    w->event_handlers = window_multiplayer_page_events[page];
    w->pressed_widgets = 0;
    w->widgets = window_multiplayer_page_widgets[page];

    window_event_resize_call(w);
    window_event_invalidate_call(w);
    window_init_scroll_widgets(w);
    window_invalidate(w);
}

static void window_multiplayer_anchor_border_widgets(rct_window *w)
{
    w->widgets[WIDX_BACKGROUND].right = w->width - 1;
    w->widgets[WIDX_BACKGROUND].bottom = w->height - 1;
    w->widgets[WIDX_TITLE].right = w->width - 2;
    w->widgets[WIDX_CONTENT_PANEL].right = w->width - 1;
    w->widgets[WIDX_CONTENT_PANEL].bottom = w->height - 1;
    w->widgets[WIDX_CLOSE].left = w->width - 13;
    w->widgets[WIDX_CLOSE].right = w->width - 3;
}

static void window_multiplayer_set_pressed_tab(rct_window *w)
{
    for (sint32 i = 0; i < 2; i++) {
        w->pressed_widgets &= ~(1 << (WIDX_TAB1 + i));
    }
    w->pressed_widgets |= 1LL << (WIDX_TAB1 + w->page);
}

static void window_multiplayer_groups_show_group_dropdown(rct_window *w, rct_widget *widget)
{
    rct_widget *dropdownWidget;
    sint32 numItems, i;

    dropdownWidget = widget - 1;

    numItems = network_get_num_groups();

    window_dropdown_show_text_custom_width(
        w->x + dropdownWidget->left,
        w->y + dropdownWidget->top,
        dropdownWidget->bottom - dropdownWidget->top + 1,
        w->colours[1],
        0,
        0,
        numItems,
        widget->right - dropdownWidget->left
    );

    for (i = 0; i < network_get_num_groups(); i++) {
        gDropdownItemsFormat[i] = STR_OPTIONS_DROPDOWN_ITEM;
        gDropdownItemsArgs[i] = (uintptr_t)network_get_group_name(i);
    }
    if (widget == &window_multiplayer_groups_widgets[WIDX_DEFAULT_GROUP_DROPDOWN]) {
        dropdown_set_checked(network_get_group_index(network_get_default_group()), true);
    } else
    if (widget == &window_multiplayer_groups_widgets[WIDX_SELECTED_GROUP_DROPDOWN]) {
        dropdown_set_checked(network_get_group_index(_selectedGroup), true);
    }
}

#pragma region Information page

static void window_multiplayer_information_mouseup(rct_window *w, rct_widgetindex widgetIndex)
{
    switch (widgetIndex) {
    case WIDX_CLOSE:
        window_close(w);
        break;
    case WIDX_TAB1:
    case WIDX_TAB2:
    case WIDX_TAB3:
    case WIDX_TAB4:
        if (w->page != widgetIndex - WIDX_TAB1) {
            window_multiplayer_set_page(w, widgetIndex - WIDX_TAB1);
        }
        break;
    }
}

static rct_xy16 window_multiplayer_information_get_size()
{
    if (!_windowInformationSizeDirty) {
        return _windowInformationSize;
    }

    sint32 width = 450;
    sint32 height = 110;
    sint32 numLines, fontSpriteBase;

    gCurrentFontSpriteBase = FONT_SPRITE_BASE_MEDIUM;
    utf8 * buffer = _strdup(network_get_server_description());
    gfx_wrap_string(buffer, width, &numLines, &fontSpriteBase);
    free(buffer);

    sint32 lineHeight = font_get_line_height(fontSpriteBase);
    height += (numLines + 1) * lineHeight;

    _windowInformationSizeDirty = false;
    _windowInformationSize = (rct_xy16){ (sint16) width, (sint16) height };
    return _windowInformationSize;
}

static void window_multiplayer_information_resize(rct_window *w)
{
    rct_xy16 size = window_multiplayer_information_get_size();
    window_set_resize(w, size.x, size.y, size.x, size.y);
}

static void window_multiplayer_information_update(rct_window *w)
{
    w->frame_no++;
    widget_invalidate(w, WIDX_TAB1 + w->page);
}

static void window_multiplayer_information_invalidate(rct_window *w)
{
    window_multiplayer_set_pressed_tab(w);
    window_multiplayer_anchor_border_widgets(w);
    window_align_tabs(w, WIDX_TAB1, WIDX_TAB4);
}

static void window_multiplayer_information_paint(rct_window *w, rct_drawpixelinfo *dpi)
{
    window_draw_widgets(w, dpi);
    window_multiplayer_draw_tab_images(w, dpi);

    rct_drawpixelinfo clippedDPI;
    if (clip_drawpixelinfo(&clippedDPI, dpi, w->x, w->y, w->width, w->height)) {
        dpi = &clippedDPI;

        sint32 x = 3;
        sint32 y = 50;
        sint32 width = w->width - 6;

        const utf8 * name = network_get_server_name();
        {
            gfx_draw_string_left_wrapped(dpi, (void*)&name, x, y, width, STR_STRING, w->colours[1]);
            y += 11;
        }
        y += 3;

        const utf8 * description = network_get_server_description();
        if (!str_is_null_or_empty(description)) {
            gfx_draw_string_left_wrapped(dpi, (void*)&description, x, y, width, STR_STRING, w->colours[1]);
            y += 11;
        }
        y += 8;

        const utf8 * providerName = network_get_server_provider_name();
        if (!str_is_null_or_empty(providerName)) {
            gfx_draw_string_left(dpi, STR_PROVIDER_NAME, (void*)&providerName, COLOUR_BLACK, x, y);
            y += 11;
        }

        const utf8 * providerEmail = network_get_server_provider_email();
        if (!str_is_null_or_empty(providerEmail)) {
            gfx_draw_string_left(dpi, STR_PROVIDER_EMAIL, (void*)&providerEmail, COLOUR_BLACK, x, y);
            y += 11;
        }

        const utf8 * providerWebsite = network_get_server_provider_website();
        if (!str_is_null_or_empty(providerWebsite)) {
            gfx_draw_string_left(dpi, STR_PROVIDER_WEBSITE, (void*)&providerWebsite, COLOUR_BLACK, x, y);
        }
    }
}

#pragma endregion

#pragma region Players page

static void window_multiplayer_players_mouseup(rct_window *w, rct_widgetindex widgetIndex)
{
    switch (widgetIndex) {
    case WIDX_CLOSE:
        window_close(w);
        break;
    case WIDX_TAB1:
    case WIDX_TAB2:
    case WIDX_TAB3:
    case WIDX_TAB4:
        if (w->page != widgetIndex - WIDX_TAB1) {
            window_multiplayer_set_page(w, widgetIndex - WIDX_TAB1);
        }
        break;
    }
}

static void window_multiplayer_players_resize(rct_window *w)
{
    window_set_resize(w, 420, 124, 500, 450);

    w->no_list_items = network_get_num_players();
    w->list_item_positions[0] = 0;

    w->selected_list_item = -1;
    window_invalidate(w);
}

static void window_multiplayer_players_update(rct_window *w)
{
    w->frame_no++;
    widget_invalidate(w, WIDX_TAB1 + w->page);
}

static void window_multiplayer_players_scrollgetsize(rct_window *w, sint32 scrollIndex, sint32 *width, sint32 *height)
{
    sint32 i;

    if (w->selected_list_item != -1) {
        w->selected_list_item = -1;
        window_invalidate(w);
    }

    *height = network_get_num_players() * 10;
    i = *height - window_multiplayer_players_widgets[WIDX_LIST].bottom + window_multiplayer_players_widgets[WIDX_LIST].top + 21;
    if (i < 0)
        i = 0;
    if (i < w->scrolls[0].v_top) {
        w->scrolls[0].v_top = i;
        window_invalidate(w);
    }
}

static void window_multiplayer_players_scrollmousedown(rct_window *w, sint32 scrollIndex, sint32 x, sint32 y)
{
    sint32 index;

    index = y / 10;
    if (index >= w->no_list_items)
        return;

    w->selected_list_item = index;
    window_invalidate(w);

    window_player_open(network_get_player_id(index));
}

static void window_multiplayer_players_scrollmouseover(rct_window *w, sint32 scrollIndex, sint32 x, sint32 y)
{
    sint32 index;

    index = y / 10;
    if (index >= w->no_list_items)
        return;

    w->selected_list_item = index;
    window_invalidate(w);
}

static void window_multiplayer_players_invalidate(rct_window *w)
{
    window_multiplayer_set_pressed_tab(w);
    window_multiplayer_anchor_border_widgets(w);
    window_multiplayer_players_widgets[WIDX_LIST].right = w->width - 4;
    window_multiplayer_players_widgets[WIDX_LIST].bottom = w->height - 0x0F;
    window_align_tabs(w, WIDX_TAB1, WIDX_TAB4);
}

static void window_multiplayer_players_paint(rct_window *w, rct_drawpixelinfo *dpi)
{
    rct_string_id stringId;
    sint32 x, y;

    window_draw_widgets(w, dpi);
    window_multiplayer_draw_tab_images(w, dpi);

    // Columns
    gfx_draw_string_left(dpi, STR_PLAYER, NULL, w->colours[2], w->x + 6, 58 - 12 + w->y + 1);
    gfx_draw_string_left(dpi, STR_GROUP, NULL, w->colours[2], w->x + 180, 58 - 12 + w->y + 1);
    gfx_draw_string_left(dpi, STR_LAST_ACTION, NULL, w->colours[2], w->x + 263, 58 - 12 + w->y + 1);
    gfx_draw_string_left(dpi, STR_PING, NULL, w->colours[2], w->x + 363, 58 - 12 + w->y + 1);

    // Number of players
    stringId = w->no_list_items == 1 ? STR_MULTIPLAYER_PLAYER_COUNT : STR_MULTIPLAYER_PLAYER_COUNT_PLURAL;
    x = w->x + 4;
    y = w->y + w->widgets[WIDX_LIST].bottom + 2;
    gfx_draw_string_left(dpi, stringId, &w->no_list_items, w->colours[2], x, y);
}

static void window_multiplayer_players_scrollpaint(rct_window *w, rct_drawpixelinfo *dpi, sint32 scrollIndex)
{
    sint32 y;

    y = 0;
    for (sint32 i = 0; i < network_get_num_players(); i++) {
        if (y > dpi->y + dpi->height) {
            break;
        }

        if (y + 11 >= dpi->y) {
            char buffer[300];

            // Draw player name
            char* lineCh = buffer;
            sint32 colour = COLOUR_BLACK;
            if (i == w->selected_list_item) {
                gfx_filter_rect(dpi, 0, y, 800, y + 9, PALETTE_DARKEN_1);
                safe_strcpy(buffer, network_get_player_name(i), sizeof(buffer));
                colour = w->colours[2];
            } else {
                if (network_get_player_flags(i) & NETWORK_PLAYER_FLAG_ISSERVER) {
                    lineCh = utf8_write_codepoint(lineCh, FORMAT_BABYBLUE);
                } else {
                    lineCh = utf8_write_codepoint(lineCh, FORMAT_BLACK);
                }
                safe_strcpy(lineCh, network_get_player_name(i), sizeof(buffer) - (lineCh - buffer));
            }
            gfx_clip_string(buffer, 230);
            gfx_draw_string(dpi, buffer, colour, 0, y - 1);

            // Draw group name
            lineCh = buffer;
            sint32 group = network_get_group_index(network_get_player_group(i));
            if (group != -1) {
                lineCh = utf8_write_codepoint(lineCh, FORMAT_BLACK);
                safe_strcpy(lineCh, network_get_group_name(group), sizeof(buffer) - (lineCh - buffer));
                gfx_clip_string(buffer, 80);
                gfx_draw_string(dpi, buffer, colour, 173, y - 1);
            }

            // Draw last action
            sint32 action = network_get_player_last_action(i, 2000);
            set_format_arg(0, rct_string_id, STR_ACTION_NA);
            if (action != -999) {
                set_format_arg(0, rct_string_id, network_get_action_name_string_id(action));
            }
            gfx_draw_string_left_clipped(dpi, STR_BLACK_STRING, gCommonFormatArgs, COLOUR_BLACK, 256, y - 1, 100);

            // Draw ping
            lineCh = buffer;
            sint32 ping = network_get_player_ping(i);
            if (ping <= 100) {
                lineCh = utf8_write_codepoint(lineCh, FORMAT_GREEN);
            } else
            if (ping <= 250) {
                lineCh = utf8_write_codepoint(lineCh, FORMAT_YELLOW);
            } else {
                lineCh = utf8_write_codepoint(lineCh, FORMAT_RED);
            }
            snprintf(lineCh, sizeof(buffer) - (lineCh - buffer), "%d ms", ping);
            gfx_draw_string(dpi, buffer, colour, 356, y - 1);
        }
        y += 10;
    }
}

#pragma endregion

#pragma region Groups page

static void window_multiplayer_groups_mouseup(rct_window *w, rct_widgetindex widgetIndex)
{
    switch (widgetIndex) {
    case WIDX_CLOSE:
        window_close(w);
        break;
    case WIDX_TAB1:
    case WIDX_TAB2:
    case WIDX_TAB3:
    case WIDX_TAB4:
        if (w->page != widgetIndex - WIDX_TAB1) {
            window_multiplayer_set_page(w, widgetIndex - WIDX_TAB1);
        }
        break;
    case WIDX_ADD_GROUP:
        game_do_command(0, GAME_COMMAND_FLAG_APPLY, 0, 0, GAME_COMMAND_MODIFY_GROUPS, 0, 0);
        break;
    case WIDX_REMOVE_GROUP:
        game_do_command(1 | (_selectedGroup << 8), GAME_COMMAND_FLAG_APPLY, 0, 0, GAME_COMMAND_MODIFY_GROUPS, 0, 0);
        break;
    case WIDX_RENAME_GROUP:;
        sint32 groupIndex = network_get_group_index(_selectedGroup);
        const utf8 *groupName = network_get_group_name(groupIndex);
        window_text_input_raw_open(w, widgetIndex, STR_GROUP_NAME, STR_ENTER_NEW_NAME_FOR_THIS_GROUP, (utf8*)groupName, 32);
        break;
    }
}

static void window_multiplayer_groups_resize(rct_window *w)
{
    window_set_resize(w, 320, 200, 320, 500);

    w->no_list_items = network_get_num_actions();
    w->list_item_positions[0] = 0;

    w->selected_list_item = -1;
    window_invalidate(w);
}

static void window_multiplayer_groups_mousedown(rct_window *w, rct_widgetindex widgetIndex, rct_widget* widget)
{
    switch (widgetIndex) {
    case WIDX_DEFAULT_GROUP_DROPDOWN:
        window_multiplayer_groups_show_group_dropdown(w, widget);
        break;
    case WIDX_SELECTED_GROUP_DROPDOWN:
        window_multiplayer_groups_show_group_dropdown(w, widget);
        break;
    }
}

static void window_multiplayer_groups_dropdown(rct_window *w, rct_widgetindex widgetIndex, sint32 dropdownIndex)
{
    if (dropdownIndex == -1) {
        return;
    }

    switch(widgetIndex){
    case WIDX_DEFAULT_GROUP_DROPDOWN:
        game_do_command(4 | (network_get_group_id(dropdownIndex) << 8), GAME_COMMAND_FLAG_APPLY, 0, 0, GAME_COMMAND_MODIFY_GROUPS, 0, 0);
        break;
    case WIDX_SELECTED_GROUP_DROPDOWN:
        _selectedGroup = network_get_group_id(dropdownIndex);
        break;
    }

    window_invalidate(w);
}

static void window_multiplayer_groups_update(rct_window *w)
{
    w->frame_no++;
    widget_invalidate(w, WIDX_TAB1 + w->page);
}

static void window_multiplayer_groups_scrollgetsize(rct_window *w, sint32 scrollIndex, sint32 *width, sint32 *height)
{
    sint32 i;

    if (w->selected_list_item != -1) {
        w->selected_list_item = -1;
        window_invalidate(w);
    }

    *height = network_get_num_actions() * 10;
    i = *height - window_multiplayer_groups_widgets[WIDX_LIST].bottom + window_multiplayer_groups_widgets[WIDX_LIST].top + 21;
    if (i < 0)
        i = 0;
    if (i < w->scrolls[0].v_top) {
        w->scrolls[0].v_top = i;
        window_invalidate(w);
    }
}

static void window_multiplayer_groups_scrollmousedown(rct_window *w, sint32 scrollIndex, sint32 x, sint32 y)
{
    sint32 index;

    index = y / 10;
    if (index >= w->no_list_items)
        return;

    w->selected_list_item = index;
    window_invalidate(w);

    game_do_command(2 | (_selectedGroup << 8), GAME_COMMAND_FLAG_APPLY, index, 0, GAME_COMMAND_MODIFY_GROUPS, 0, 0);
}

static void window_multiplayer_groups_scrollmouseover(rct_window *w, sint32 scrollIndex, sint32 x, sint32 y)
{
    sint32 index;

    index = y / 10;
    if (index >= w->no_list_items)
        return;

    w->selected_list_item = index;
    window_invalidate(w);
}

static void window_multiplayer_groups_text_input(rct_window *w, rct_widgetindex widgetIndex, char *text)
{
    if (widgetIndex != WIDX_RENAME_GROUP)
        return;

    if (text == NULL)
        return;

    game_do_command(3 | (_selectedGroup << 8) | (1 << 16), GAME_COMMAND_FLAG_APPLY, w->number, *((sint32*)(text + 0)), GAME_COMMAND_MODIFY_GROUPS, *((sint32*)(text + 8)), *((sint32*)(text + 4)));
    game_do_command(3 | (_selectedGroup << 8) | (2 << 16), GAME_COMMAND_FLAG_APPLY, w->number, *((sint32*)(text + 12)), GAME_COMMAND_MODIFY_GROUPS, *((sint32*)(text + 20)), *((sint32*)(text + 16)));
    game_do_command(3 | (_selectedGroup << 8) | (0 << 16), GAME_COMMAND_FLAG_APPLY, w->number, *((sint32*)(text + 24)), GAME_COMMAND_MODIFY_GROUPS, *((sint32*)(text + 32)), *((sint32*)(text + 28)));
}

static void window_multiplayer_groups_invalidate(rct_window *w)
{
    window_multiplayer_set_pressed_tab(w);
    window_multiplayer_anchor_border_widgets(w);
    window_multiplayer_groups_widgets[WIDX_PERMISSIONS_LIST].right = w->width - 4;
    window_multiplayer_groups_widgets[WIDX_PERMISSIONS_LIST].bottom = w->height - 0x0F;
    window_align_tabs(w, WIDX_TAB1, WIDX_TAB4);

    // select other group if one is removed
    while (network_get_group_index(_selectedGroup) == -1 && _selectedGroup > 0) {
        _selectedGroup--;
    }
}

static void window_multiplayer_groups_paint(rct_window *w, rct_drawpixelinfo *dpi)
{
    window_draw_widgets(w, dpi);
    window_multiplayer_draw_tab_images(w, dpi);

    rct_widget* widget = &window_multiplayer_groups_widgets[WIDX_DEFAULT_GROUP];
    sint32 group = network_get_group_index(network_get_default_group());
    if (group != -1) {
        char buffer[300];
        char* lineCh;
        lineCh = buffer;
        lineCh = utf8_write_codepoint(lineCh, FORMAT_WINDOW_COLOUR_2);
        safe_strcpy(lineCh, network_get_group_name(group), sizeof(buffer) - (lineCh - buffer));
        set_format_arg(0, const char *, buffer);
        gfx_draw_string_centred_clipped(
            dpi,
            STR_STRING,
            gCommonFormatArgs,
            COLOUR_BLACK,
            w->x + (widget->left + widget->right - 11) / 2,
            w->y + widget->top,
            widget->right - widget->left - 8
        );
    }

    sint32 x = w->x + window_multiplayer_groups_widgets[WIDX_CONTENT_PANEL].left + 4;
    sint32 y = w->y + window_multiplayer_groups_widgets[WIDX_CONTENT_PANEL].top + 4;

    gfx_draw_string_left(dpi, STR_DEFAULT_GROUP, NULL, w->colours[2], x, y);

    y += 20;

    gfx_fill_rect_inset(dpi, x, y - 6, x + 310, y - 5, w->colours[1], INSET_RECT_FLAG_BORDER_INSET);

    widget = &window_multiplayer_groups_widgets[WIDX_SELECTED_GROUP];
    group = network_get_group_index(_selectedGroup);
    if (group != -1) {
        char buffer[300];
        char* lineCh;
        lineCh = buffer;
        lineCh = utf8_write_codepoint(lineCh, FORMAT_WINDOW_COLOUR_2);
        safe_strcpy(lineCh, network_get_group_name(group), sizeof(buffer) - (lineCh - buffer));
        set_format_arg(0, const char *, buffer);
        gfx_draw_string_centred_clipped(
            dpi,
            STR_STRING,
            gCommonFormatArgs,
            COLOUR_BLACK,
            w->x + (widget->left + widget->right - 11) / 2,
            w->y + widget->top,
            widget->right - widget->left - 8
        );
    }
}

static void window_multiplayer_groups_scrollpaint(rct_window *w, rct_drawpixelinfo *dpi, sint32 scrollIndex)
{
    sint32 y = 0;

    gfx_fill_rect(dpi, dpi->x, dpi->y, dpi->x + dpi->width - 1, dpi->y + dpi->height - 1, ColourMapA[w->colours[1]].mid_light);

    for (sint32 i = 0; i < network_get_num_actions(); i++) {
        if (i == w->selected_list_item) {
            gfx_filter_rect(dpi, 0, y, 800, y + 9, PALETTE_DARKEN_1);
        }
        if (y > dpi->y + dpi->height) {
            break;
        }

        if (y + 11 >= dpi->y) {
            char buffer[300] = {0};
            sint32 groupindex = network_get_group_index(_selectedGroup);
            if (groupindex != -1){
                if (network_can_perform_action(groupindex, i)) {
                    char* lineCh = buffer;
                    lineCh = utf8_write_codepoint(lineCh, FORMAT_WINDOW_COLOUR_2);
                    lineCh = utf8_write_codepoint(lineCh, FORMAT_TICK);
                    gfx_draw_string(dpi, buffer, COLOUR_BLACK, 0, y - 1);
                }
            }

            // Draw action name
            set_format_arg(0, uint16, network_get_action_name_string_id(i));
            gfx_draw_string_left(dpi, STR_WINDOW_COLOUR_2_STRINGID, gCommonFormatArgs, COLOUR_BLACK, 10, y - 1);
        }
        y += 10;
    }
}

#pragma endregion

#pragma region Options page

static void window_multiplayer_options_mouseup(rct_window *w, rct_widgetindex widgetIndex)
{
    switch (widgetIndex) {
    case WIDX_CLOSE:
        window_close(w);
        break;
    case WIDX_TAB1:
    case WIDX_TAB2:
    case WIDX_TAB3:
    case WIDX_TAB4:
        if (w->page != widgetIndex - WIDX_TAB1) {
            window_multiplayer_set_page(w, widgetIndex - WIDX_TAB1);
        }
        break;
    case WIDX_LOG_CHAT_CHECKBOX:
        gConfigNetwork.log_chat = !gConfigNetwork.log_chat;
        config_save_default();
        break;
    case WIDX_LOG_SERVER_ACTIONS_CHECKBOX:
        gConfigNetwork.log_server_actions = !gConfigNetwork.log_server_actions;
        config_save_default();
        break;
    case WIDX_KNOWN_KEYS_ONLY_CHECKBOX:
        gConfigNetwork.known_keys_only = !gConfigNetwork.known_keys_only;
        config_save_default();
        break;
    }
}

static void window_multiplayer_options_resize(rct_window *w)
{
    window_set_resize(w, 300, 100, 300, 100);
}

static void window_multiplayer_options_update(rct_window *w)
{
    w->frame_no++;
    widget_invalidate(w, WIDX_TAB1 + w->page);
}

static void window_multiplayer_options_invalidate(rct_window *w)
{
    window_multiplayer_set_pressed_tab(w);
    window_multiplayer_anchor_border_widgets(w);
    window_align_tabs(w, WIDX_TAB1, WIDX_TAB4);

    if (network_get_mode() == NETWORK_MODE_CLIENT) {
        w->widgets[WIDX_KNOWN_KEYS_ONLY_CHECKBOX].type = WWT_EMPTY;
    }

    widget_set_checkbox_value(w, WIDX_LOG_CHAT_CHECKBOX, gConfigNetwork.log_chat);
    widget_set_checkbox_value(w, WIDX_LOG_SERVER_ACTIONS_CHECKBOX, gConfigNetwork.log_server_actions);
    widget_set_checkbox_value(w, WIDX_KNOWN_KEYS_ONLY_CHECKBOX, gConfigNetwork.known_keys_only);
}

static void window_multiplayer_options_paint(rct_window *w, rct_drawpixelinfo *dpi)
{
    window_draw_widgets(w, dpi);
    window_multiplayer_draw_tab_images(w, dpi);
}

#pragma endregion

static void window_multiplayer_draw_tab_image(rct_window *w, rct_drawpixelinfo *dpi, sint32 page, sint32 spriteIndex)
{
    rct_widgetindex widgetIndex = WIDX_TAB1 + page;

    if (!widget_is_disabled(w, widgetIndex)) {
        if (w->page == page) {
            sint32 numFrames = window_multiplayer_animation_frames[w->page];
            if (numFrames > 1) {
                sint32 frame = w->frame_no / window_multiplayer_animation_divisor[w->page];
                spriteIndex += (frame % numFrames);
            }
        }

        gfx_draw_sprite(dpi, spriteIndex, w->x + w->widgets[widgetIndex].left, w->y + w->widgets[widgetIndex].top, 0);
    }
}

static void window_multiplayer_draw_tab_images(rct_window *w, rct_drawpixelinfo *dpi)
{
    window_multiplayer_draw_tab_image(w, dpi, WINDOW_MULTIPLAYER_PAGE_INFORMATION, SPR_TAB_KIOSKS_AND_FACILITIES_0);
    window_multiplayer_draw_tab_image(w, dpi, WINDOW_MULTIPLAYER_PAGE_PLAYERS, SPR_TAB_GUESTS_0);
    window_multiplayer_draw_tab_image(w, dpi, WINDOW_MULTIPLAYER_PAGE_GROUPS, SPR_TAB_STAFF_OPTIONS_0);
    window_multiplayer_draw_tab_image(w, dpi, WINDOW_MULTIPLAYER_PAGE_OPTIONS, SPR_TAB_GEARS_0);
}
