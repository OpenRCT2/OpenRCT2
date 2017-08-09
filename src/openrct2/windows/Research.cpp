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

extern "C"
{
    #include "../game.h"
    #include "../localisation/localisation.h"
    #include "../interface/widget.h"
    #include "../management/news_item.h"
    #include "../sprites.h"
    #include "../world/scenery.h"
    #include "dropdown.h"
}

enum {
    WINDOW_RESEARCH_PAGE_DEVELOPMENT,
    WINDOW_RESEARCH_PAGE_FUNDING,
    WINDOW_RESEARCH_PAGE_COUNT
};

enum {
    WIDX_BACKGROUND,
    WIDX_TITLE,
    WIDX_CLOSE,
    WIDX_PAGE_BACKGROUND,
    WIDX_TAB_1,
    WIDX_TAB_2,

    WIDX_CURRENTLY_IN_DEVELOPMENT_GROUP,
    WIDX_LAST_DEVELOPMENT_GROUP,
    WIDX_LAST_DEVELOPMENT_BUTTON,

    WIDX_FUNDING_GROUP = 6,
    WIDX_RESEARCH_FUNDING,
    WIDX_RESEARCH_FUNDING_DROPDOWN_BUTTON,
    WIDX_PRIORITIES_GROUP,
    WIDX_TRANSPORT_RIDES,
    WIDX_GENTLE_RIDES,
    WIDX_ROLLER_COASTERS,
    WIDX_THRILL_RIDES,
    WIDX_WATER_RIDES,
    WIDX_SHOPS_AND_STALLS,
    WIDX_SCENERY_AND_THEMING,
};

#pragma region Widgets

static rct_widget window_research_development_widgets[] = {
    { WWT_FRAME,            0,  0,      299,    0,      195,    0xFFFFFFFF,                             STR_NONE },
    { WWT_CAPTION,          0,  1,      298,    1,      14,     STR_RESEARCH_AND_DEVELOPMENT,           STR_WINDOW_TITLE_TIP },
    { WWT_CLOSEBOX,         0,  287,    297,    2,      13,     STR_CLOSE_X,                            STR_CLOSE_WINDOW_TIP },
    { WWT_RESIZE,           1,  0,      299,    43,     195,    0xFFFFFFFF,                             STR_NONE },
    { WWT_TAB,              1,  3,      33,     17,     43,     IMAGE_TYPE_REMAP | SPR_TAB,                   STR_RESEARCH_AND_DEVELOPMENT_TIP },
    { WWT_TAB,              1,  34,     64,     17,     43,     IMAGE_TYPE_REMAP | SPR_TAB,                   STR_FINANCES_RESEARCH_TIP },
    { WWT_GROUPBOX,         2,  3,      292,    47,     116,    STR_CURRENTLY_IN_DEVELOPMENT,           STR_NONE },
    { WWT_GROUPBOX,         2,  3,      292,    124,    188,    STR_LAST_DEVELOPMENT,                   STR_NONE },
    { WWT_FLATBTN,          2,  265,    288,    161,    184,    0xFFFFFFFF,                             STR_RESEARCH_SHOW_DETAILS_TIP },
    { WIDGETS_END },
};

static rct_widget window_research_funding_widgets[] = {
    { WWT_FRAME,            0,  0,      319,    0,      206,    0xFFFFFFFF,                             STR_NONE },
    { WWT_CAPTION,          0,  1,      318,    1,      14,     STR_RESEARCH_FUNDING,                   STR_WINDOW_TITLE_TIP },
    { WWT_CLOSEBOX,         0,  307,    317,    2,      13,     STR_CLOSE_X,                            STR_CLOSE_WINDOW_TIP },
    { WWT_RESIZE,           1,  0,      319,    43,     206,    0xFFFFFFFF,                             STR_NONE },
    { WWT_TAB,              1,  3,      33,     17,     43,     IMAGE_TYPE_REMAP | SPR_TAB,                   STR_RESEARCH_AND_DEVELOPMENT_TIP },
    { WWT_TAB,              1,  34,     64,     17,     43,     IMAGE_TYPE_REMAP | SPR_TAB,                   STR_FINANCES_RESEARCH_TIP },
    { WWT_GROUPBOX,         2,  3,      316,    47,     91,     STR_RESEARCH_FUNDING_,                  STR_NONE },
    { WWT_DROPDOWN,         2,  8,      167,    59,     70,     0xFFFFFFFF,                             STR_SELECT_LEVEL_OF_RESEARCH_AND_DEVELOPMENT },
    { WWT_DROPDOWN_BUTTON,  2,  156,    166,    60,     69,     STR_DROPDOWN_GLYPH,                     STR_SELECT_LEVEL_OF_RESEARCH_AND_DEVELOPMENT },
    { WWT_GROUPBOX,         2,  3,      316,    96,     202,    STR_RESEARCH_PRIORITIES,                STR_NONE },
    { WWT_CHECKBOX,         2,  8,      311,    108,    119,    STR_RESEARCH_NEW_TRANSPORT_RIDES,       STR_RESEARCH_NEW_TRANSPORT_RIDES_TIP },
    { WWT_CHECKBOX,         2,  8,      311,    121,    132,    STR_RESEARCH_NEW_GENTLE_RIDES,          STR_RESEARCH_NEW_GENTLE_RIDES_TIP },
    { WWT_CHECKBOX,         2,  8,      311,    134,    145,    STR_RESEARCH_NEW_ROLLER_COASTERS,       STR_RESEARCH_NEW_ROLLER_COASTERS_TIP },
    { WWT_CHECKBOX,         2,  8,      311,    147,    158,    STR_RESEARCH_NEW_THRILL_RIDES,          STR_RESEARCH_NEW_THRILL_RIDES_TIP },
    { WWT_CHECKBOX,         2,  8,      311,    160,    171,    STR_RESEARCH_NEW_WATER_RIDES,           STR_RESEARCH_NEW_WATER_RIDES_TIP },
    { WWT_CHECKBOX,         2,  8,      311,    173,    184,    STR_RESEARCH_NEW_SHOPS_AND_STALLS,      STR_RESEARCH_NEW_SHOPS_AND_STALLS_TIP },
    { WWT_CHECKBOX,         2,  8,      311,    186,    197,    STR_RESEARCH_NEW_SCENERY_AND_THEMING,   STR_RESEARCH_NEW_SCENERY_AND_THEMING_TIP },
    { WIDGETS_END },
};

static rct_widget *window_research_page_widgets[] = {
    window_research_development_widgets,
    window_research_funding_widgets
};

#pragma endregion

#pragma region Events

static void window_research_development_mouseup(rct_window *w, rct_widgetindex widgetIndex);
static void window_research_development_update(rct_window *w);
static void window_research_development_invalidate(rct_window *w);
static void window_research_development_paint(rct_window *w, rct_drawpixelinfo *dpi);

static void window_research_funding_mouseup(rct_window *w, rct_widgetindex widgetIndex);
static void window_research_funding_mousedown(rct_window *w, rct_widgetindex widgetIndex, rct_widget* widget);
static void window_research_funding_dropdown(rct_window *w, rct_widgetindex widgetIndex, sint32 dropdownIndex);
static void window_research_funding_update(rct_window *w);
static void window_research_funding_invalidate(rct_window *w);
static void window_research_funding_paint(rct_window *w, rct_drawpixelinfo *dpi);

//
static rct_window_event_list window_research_development_events = {
    NULL,
    window_research_development_mouseup,
    NULL,
    NULL,
    NULL,
    NULL,
    window_research_development_update,
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
    window_research_development_invalidate,
    window_research_development_paint,
    NULL
};

// 0x009890E8
static rct_window_event_list window_research_funding_events = {
    NULL,
    window_research_funding_mouseup,
    NULL,
    window_research_funding_mousedown,
    window_research_funding_dropdown,
    NULL,
    window_research_funding_update,
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
    window_research_funding_invalidate,
    window_research_funding_paint,
    NULL
};

static rct_window_event_list *window_research_page_events[] = {
    &window_research_development_events,
    &window_research_funding_events
};

#pragma endregion

#pragma region Enabled widgets

static uint32 window_research_page_enabled_widgets[] = {
    (1 << WIDX_CLOSE) |
    (1 << WIDX_TAB_1) |
    (1 << WIDX_TAB_2) |
    (1 << WIDX_LAST_DEVELOPMENT_BUTTON),

    (1 << WIDX_CLOSE) |
    (1 << WIDX_TAB_1) |
    (1 << WIDX_TAB_2) |
    (1 << WIDX_RESEARCH_FUNDING) |
    (1 << WIDX_RESEARCH_FUNDING_DROPDOWN_BUTTON) |
    (1 << WIDX_TRANSPORT_RIDES) |
    (1 << WIDX_GENTLE_RIDES) |
    (1 << WIDX_ROLLER_COASTERS) |
    (1 << WIDX_THRILL_RIDES) |
    (1 << WIDX_WATER_RIDES) |
    (1 << WIDX_SHOPS_AND_STALLS) |
    (1 << WIDX_SCENERY_AND_THEMING)
};

#pragma endregion

const sint32 window_research_tab_animation_loops[] = { 16, 16 };

static const rct_string_id ResearchCategoryNames[] = {
    STR_RESEARCH_CATEGORY_TRANSPORT,
    STR_RESEARCH_CATEGORY_GENTLE,
    STR_RESEARCH_CATEGORY_ROLLERCOASTER,
    STR_RESEARCH_CATEGORY_THRILL,
    STR_RESEARCH_CATEGORY_WATER,
    STR_RESEARCH_CATEGORY_SHOP,
    STR_RESEARCH_CATEGORY_SCENERYSET,
};

static const rct_string_id ResearchStageNames[] = {
    STR_RESEARCH_STAGE_INITIAL_RESEARCH,
    STR_RESEARCH_STAGE_DESIGNING,
    STR_RESEARCH_STAGE_COMPLETING_DESIGN,
    STR_RESEARCH_STAGE_UNKNOWN,
};

static void window_research_set_page(rct_window *w, sint32 page);
static void window_research_set_pressed_tab(rct_window *w);
static void window_research_draw_tab_images(rct_drawpixelinfo *dpi, rct_window *w);

void window_research_open()
{
    rct_window *w;

    w = window_bring_to_front_by_class(WC_RESEARCH);
    if (w == NULL) {
        w = window_create_auto_pos(530, 257, window_research_page_events[0], WC_RESEARCH, WF_10);
        w->widgets = window_research_page_widgets[0];
        w->enabled_widgets = window_research_page_enabled_widgets[0];
        w->number = 0;
        w->page = 0;
        w->frame_no = 0;
        w->disabled_widgets = 0;
        research_update_uncompleted_types();
    }

    w->page = 0;
    window_invalidate(w);
    w->width = 300;
    w->height = 196;
    window_invalidate(w);

    w->widgets = window_research_page_widgets[0];
    w->enabled_widgets = window_research_page_enabled_widgets[0];
    w->hold_down_widgets = 0;
    w->event_handlers = window_research_page_events[0];
    w->pressed_widgets = 0;
    w->disabled_widgets = 0;
    window_init_scroll_widgets(w);
}

#pragma region Development page

/**
 *
 *  rct2: 0x006B6B38
 */
static void window_research_development_mouseup(rct_window *w, rct_widgetindex widgetIndex)
{
    switch (widgetIndex) {
    case WIDX_CLOSE:
        window_close(w);
        break;
    case WIDX_TAB_1:
    case WIDX_TAB_2:
        window_research_set_page(w, widgetIndex - WIDX_TAB_1);
        break;
    case WIDX_LAST_DEVELOPMENT_BUTTON:
        news_item_open_subject(NEWS_ITEM_RESEARCH, (sint32)gResearchLastItemSubject);
        break;
    }
}

/**
 *
 *  rct2: 0x0069CBA6
 */
static void window_research_development_update(rct_window *w)
{
    // Tab animation
    if (++w->frame_no >= window_research_tab_animation_loops[w->page])
        w->frame_no = 0;
    widget_invalidate(w, WIDX_TAB_1);
}

/**
 *
 *  rct2: 0x006B6819
 */
static void window_research_development_invalidate(rct_window *w)
{
    if (w->widgets != window_research_page_widgets[WINDOW_RESEARCH_PAGE_DEVELOPMENT]) {
        w->widgets = window_research_page_widgets[WINDOW_RESEARCH_PAGE_DEVELOPMENT];
        window_init_scroll_widgets(w);
    }

    window_research_set_pressed_tab(w);

    window_research_development_widgets[WIDX_LAST_DEVELOPMENT_BUTTON].type = WWT_EMPTY;
    uint32 typeId = gResearchLastItemSubject;
    if (typeId != 0xFFFFFFFF) {
        window_research_development_widgets[WIDX_LAST_DEVELOPMENT_BUTTON].type = WWT_FLATBTN;
        window_research_development_widgets[WIDX_LAST_DEVELOPMENT_BUTTON].image = typeId >= 0x10000 ? SPR_NEW_RIDE : SPR_NEW_SCENERY;
    }
}

/**
 *
 *  rct2: 0x006B689B
 */
static void window_research_development_paint(rct_window *w, rct_drawpixelinfo *dpi)
{
    window_draw_widgets(w, dpi);
    window_research_draw_tab_images(dpi, w);

    window_research_development_page_paint(w, dpi, WIDX_CURRENTLY_IN_DEVELOPMENT_GROUP);
}

void window_research_development_page_paint(rct_window *w, rct_drawpixelinfo *dpi, rct_widgetindex baseWidgetIndex)
{
    baseWidgetIndex = baseWidgetIndex - WIDX_CURRENTLY_IN_DEVELOPMENT_GROUP;

    sint32 x = w->x + 10;
    sint32 y = w->y + w->widgets[WIDX_CURRENTLY_IN_DEVELOPMENT_GROUP + baseWidgetIndex].top + 12;
    rct_string_id stringId;

    if (gResearchProgressStage == RESEARCH_STAGE_FINISHED_ALL) {
        stringId = STR_RESEARCH_UNKNOWN;
        gfx_draw_string_left_wrapped(dpi, &stringId, x, y, 296, STR_RESEARCH_TYPE_LABEL, COLOUR_BLACK);
        y += 25;

        // Progress
        stringId = STR_RESEARCH_COMPLETED_AL;
        gfx_draw_string_left_wrapped(dpi, &stringId, x, y, 296, STR_RESEARCH_PROGRESS_LABEL, COLOUR_BLACK);
        y += 15;

        set_format_arg(0, rct_string_id, STR_RESEARCH_STAGE_UNKNOWN);
        gfx_draw_string_left(dpi, STR_RESEARCH_EXPECTED_LABEL, gCommonFormatArgs, COLOUR_BLACK, x, y);
    } else {
        // Research type
        stringId = STR_RESEARCH_UNKNOWN;
        if (gResearchProgressStage != RESEARCH_STAGE_INITIAL_RESEARCH) {
            stringId = ResearchCategoryNames[gResearchNextCategory];
            if (gResearchProgressStage != RESEARCH_STAGE_DESIGNING) {
                uint32 typeId = gResearchNextItem;
                if (typeId >= 0x10000) {
                    uint8 baseRideType = (typeId >> 8) & 0xFF;
                    rct_ride_entry *rideEntry = get_ride_entry(typeId & 0xFF);
                    stringId = (rideEntry->flags & RIDE_ENTRY_FLAG_SEPARATE_RIDE_NAME) ?
                        rideEntry->name :
                        get_friendly_track_type_name(baseRideType, rideEntry); // Makes sure the correct track name is displayed, e.g. Hyper-Twister instead of Steel Twister.
                } else {
                    stringId = get_scenery_group_entry(typeId)->name;
                }
            }
        }
        gfx_draw_string_left_wrapped(dpi, &stringId, x, y, 296, STR_RESEARCH_TYPE_LABEL, COLOUR_BLACK);
        y += 25;

        // Progress
        stringId = ResearchStageNames[gResearchProgressStage];
        gfx_draw_string_left_wrapped(dpi, &stringId, x, y, 296, STR_RESEARCH_PROGRESS_LABEL, COLOUR_BLACK);
        y += 15;

        // Expected
        set_format_arg(0, rct_string_id, STR_RESEARCH_STAGE_UNKNOWN);
        if (gResearchProgressStage != 0) {
            uint16 expectedDay = gResearchExpectedDay;
            if (expectedDay != 255) {
                // TODO: Should probably use game date format setting
                set_format_arg(0, rct_string_id, STR_RESEARCH_EXPECTED_FORMAT);
                set_format_arg(2, rct_string_id, DateDayNames[expectedDay]);
                set_format_arg(4, rct_string_id, DateGameMonthNames[gResearchExpectedMonth]);
            }
        }
        gfx_draw_string_left(dpi, STR_RESEARCH_EXPECTED_LABEL, gCommonFormatArgs, COLOUR_BLACK, x, y);
    }

    // Last development
    x = w->x + 10;
    y = w->y + w->widgets[WIDX_LAST_DEVELOPMENT_GROUP + baseWidgetIndex].top + 12;

    uint32 typeId = gResearchLastItemSubject;
    rct_string_id lastDevelopmentFormat;
    if (typeId != 0xFFFFFFFF) {
        if (typeId >= 0x10000) {
            rct_ride_entry *rideEntry = get_ride_entry(typeId & 0xFF);
            if (rideEntry == NULL) {
                return;
            }
            stringId = (rideEntry->flags & RIDE_ENTRY_FLAG_SEPARATE_RIDE_NAME) ?
                rideEntry->name :
                ((typeId >> 8) & 0xFF) + 2;

            lastDevelopmentFormat = STR_RESEARCH_RIDE_LABEL;
        } else {
            rct_scenery_set_entry *sse = get_scenery_group_entry(typeId);
            if (sse == NULL) {
                return;
            }
            stringId = sse->name;
            lastDevelopmentFormat = STR_RESEARCH_SCENERY_LABEL;
        }
        gfx_draw_string_left_wrapped(dpi, &stringId, x, y, 266, lastDevelopmentFormat, COLOUR_BLACK);
    }
}

#pragma endregion

#pragma region Funding page

/**
 *
 *  rct2: 0x0069DB3F
 */
static void window_research_funding_mouseup(rct_window *w, rct_widgetindex widgetIndex)
{
    sint32 activeResearchTypes;

    switch (widgetIndex) {
    case WIDX_CLOSE:
        window_close(w);
        break;
    case WIDX_TAB_1:
    case WIDX_TAB_2:
        window_research_set_page(w, widgetIndex - WIDX_TAB_1);
        break;
    case WIDX_TRANSPORT_RIDES:
    case WIDX_GENTLE_RIDES:
    case WIDX_ROLLER_COASTERS:
    case WIDX_THRILL_RIDES:
    case WIDX_WATER_RIDES:
    case WIDX_SHOPS_AND_STALLS:
    case WIDX_SCENERY_AND_THEMING:
        activeResearchTypes = gResearchPriorities;
        activeResearchTypes ^= 1 << (widgetIndex - WIDX_TRANSPORT_RIDES);
        research_set_priority(activeResearchTypes);
        break;
    }
}

/**
 *
 *  rct2: 0x0069DB66
 */
static void window_research_funding_mousedown(rct_window *w, rct_widgetindex widgetIndex, rct_widget* widget)
{
    rct_widget *dropdownWidget;
    sint32 i;

    if (widgetIndex != WIDX_RESEARCH_FUNDING_DROPDOWN_BUTTON)
        return;

    dropdownWidget = widget - 1;

    for (i = 0; i < 4; i++) {
        gDropdownItemsFormat[i] = STR_DROPDOWN_MENU_LABEL;
        gDropdownItemsArgs[i] = ResearchFundingLevelNames[i];
    }
    window_dropdown_show_text_custom_width(
        w->x + dropdownWidget->left,
        w->y + dropdownWidget->top,
        dropdownWidget->bottom - dropdownWidget->top + 1,
        w->colours[1],
        0,
        DROPDOWN_FLAG_STAY_OPEN,
        4,
        dropdownWidget->right - dropdownWidget->left - 3
    );

    sint32 currentResearchLevel = gResearchFundingLevel;
    dropdown_set_checked(currentResearchLevel, true);
}

/**
 *
 *  rct2: 0x0069DB6D
 */
static void window_research_funding_dropdown(rct_window *w, rct_widgetindex widgetIndex, sint32 dropdownIndex)
{
    if (widgetIndex != WIDX_RESEARCH_FUNDING_DROPDOWN_BUTTON || dropdownIndex == -1)
        return;

    research_set_funding(dropdownIndex);
    window_invalidate(w);
}

/**
 *
 *  rct2: 0x0069DC23
 */
static void window_research_funding_update(rct_window *w)
{
    // Tab animation
    if (++w->frame_no >= window_research_tab_animation_loops[w->page])
        w->frame_no = 0;
    widget_invalidate(w, WIDX_TAB_2);
}

/**
 *
 *  rct2: 0x0069DA64
 */
static void window_research_funding_invalidate(rct_window *w)
{
    if (w->widgets != window_research_page_widgets[WINDOW_RESEARCH_PAGE_FUNDING]) {
        w->widgets = window_research_page_widgets[WINDOW_RESEARCH_PAGE_FUNDING];
        window_init_scroll_widgets(w);
    }

    window_research_set_pressed_tab(w);

    if ((gParkFlags & PARK_FLAGS_NO_MONEY) ||
        (gResearchProgressStage == RESEARCH_STAGE_FINISHED_ALL)) {
        //window_research_funding_widgets[WIDX_FUNDING_GROUP].type = WWT_EMPTY;
        window_research_funding_widgets[WIDX_RESEARCH_FUNDING].type = WWT_EMPTY;
        window_research_funding_widgets[WIDX_RESEARCH_FUNDING_DROPDOWN_BUTTON].type = WWT_EMPTY;
    } else {
        window_research_funding_widgets[WIDX_FUNDING_GROUP].type = WWT_GROUPBOX;
        window_research_funding_widgets[WIDX_RESEARCH_FUNDING].type = WWT_DROPDOWN;
        window_research_funding_widgets[WIDX_RESEARCH_FUNDING_DROPDOWN_BUTTON].type = WWT_DROPDOWN_BUTTON;

        // Current funding
        sint32 currentResearchLevel = gResearchFundingLevel;
        window_research_funding_widgets[WIDX_RESEARCH_FUNDING].text = ResearchFundingLevelNames[currentResearchLevel];
    }

    // Checkboxes
    uint8 activeResearchTypes = gResearchPriorities;
    for (sint32 i = 0; i < 7; i++) {
        sint32 mask = 1 << i;
        sint32 widgetMask = 1 << (i + WIDX_TRANSPORT_RIDES);

        // Set checkbox disabled if research type is complete
        if (gResearchUncompletedCategories & mask) {
            w->disabled_widgets &= ~widgetMask;

            // Set checkbox ticked if research type is active
            if (activeResearchTypes & mask)
                w->pressed_widgets |= widgetMask;
            else
                w->pressed_widgets &= ~widgetMask;
        } else {
            w->disabled_widgets |= widgetMask;
            w->pressed_widgets &= ~widgetMask;
        }
    }
}

/**
 *
 *  rct2: 0x0069DAF0
 */
static void window_research_funding_paint(rct_window *w, rct_drawpixelinfo *dpi)
{
    window_draw_widgets(w, dpi);
    window_research_draw_tab_images(dpi, w);

    window_research_funding_page_paint(w, dpi, WIDX_RESEARCH_FUNDING);
}

void window_research_funding_page_paint(rct_window *w, rct_drawpixelinfo *dpi, rct_widgetindex baseWidgetIndex)
{
    if (gParkFlags & PARK_FLAGS_NO_MONEY)
        return;

    sint32 currentResearchLevel = gResearchFundingLevel;
    money32 currentResearchCostPerWeek = research_cost_table[currentResearchLevel];
    gfx_draw_string_left(dpi, STR_RESEARCH_COST_PER_MONTH, &currentResearchCostPerWeek, COLOUR_BLACK, w->x + 10, w->y + 77);
}

#pragma endregion

#pragma region Common

/**
 *
 *  rct2: 0x0069CAC5
 */
static void window_research_set_page(rct_window *w, sint32 page)
{
    w->page = page;
    w->frame_no = 0;
    if (w->viewport != NULL) {
        w->viewport->width = 0;
        w->viewport = NULL;
    }

    w->enabled_widgets = window_research_page_enabled_widgets[page];
    w->hold_down_widgets = 0;
    w->event_handlers = window_research_page_events[page];
    w->widgets = window_research_page_widgets[page];
    w->disabled_widgets = 0;
    w->pressed_widgets = 0;

    window_invalidate(w);
    if (w->page == WINDOW_RESEARCH_PAGE_DEVELOPMENT) {
        w->width = 300;
        w->height = 196;
    } else {
        w->width = 320;
        w->height = 207;
    }
    window_event_resize_call(w);
    window_event_invalidate_call(w);

    window_init_scroll_widgets(w);
    window_invalidate(w);
}

static void window_research_set_pressed_tab(rct_window *w)
{
    sint32 i;
    for (i = 0; i < WINDOW_RESEARCH_PAGE_COUNT; i++)
        w->pressed_widgets &= ~(1 << (WIDX_TAB_1 + i));
    w->pressed_widgets |= 1LL << (WIDX_TAB_1 + w->page);
}

static void window_research_draw_tab_image(rct_drawpixelinfo *dpi, rct_window *w, sint32 page, sint32 spriteIndex)
{
    rct_widgetindex widgetIndex = WIDX_TAB_1 + page;

    if (!(w->disabled_widgets & (1LL << widgetIndex))) {
        if (w->page == page) {
            sint32 frame = w->frame_no / 2;
            if (page == WINDOW_RESEARCH_PAGE_DEVELOPMENT)
                frame %= 8;
            spriteIndex += frame;
        }

        gfx_draw_sprite(dpi, spriteIndex, w->x + w->widgets[widgetIndex].left, w->y + w->widgets[widgetIndex].top, 0);
    }
}

static void window_research_draw_tab_images(rct_drawpixelinfo *dpi, rct_window *w)
{
    window_research_draw_tab_image(dpi, w, WINDOW_RESEARCH_PAGE_DEVELOPMENT, SPR_TAB_FINANCES_RESEARCH_0);
    window_research_draw_tab_image(dpi, w, WINDOW_RESEARCH_PAGE_FUNDING, SPR_TAB_FINANCES_SUMMARY_0);
}

#pragma endregion
