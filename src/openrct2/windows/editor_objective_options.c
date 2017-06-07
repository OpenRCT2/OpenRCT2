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
#include "../interface/widget.h"
#include "../interface/window.h"
#include "../localisation/date.h"
#include "../localisation/localisation.h"
#include "../rct2.h"
#include "../scenario/scenario.h"
#include "../sprites.h"
#include "../util/util.h"
#include "../world/Climate.h"
#include "../world/park.h"
#include "dropdown.h"
#include "error.h"

#pragma region Widgets

enum {
    WINDOW_EDITOR_OBJECTIVE_OPTIONS_PAGE_MAIN,
    WINDOW_EDITOR_OBJECTIVE_OPTIONS_PAGE_RIDES,
    WINDOW_EDITOR_OBJECTIVE_OPTIONS_PAGE_COUNT
};

static const rct_string_id ClimateNames[] = {
    STR_CLIMATE_COOL_AND_WET,
    STR_CLIMATE_WARM,
    STR_CLIMATE_HOT_AND_DRY,
    STR_CLIMATE_COLD,
};

static const rct_string_id ObjectiveDropdownOptionNames[] = {
    STR_OBJECTIVE_DROPDOWN_NONE,
    STR_OBJECTIVE_DROPDOWN_NUMBER_OF_GUESTS_AT_A_GIVEN_DATE,
    STR_OBJECTIVE_DROPDOWN_PARK_VALUE_AT_A_GIVEN_DATE,
    STR_OBJECTIVE_DROPDOWN_HAVE_FUN,
    STR_OBJECTIVE_DROPDOWN_BUILD_THE_BEST_RIDE_YOU_CAN,
    STR_OBJECTIVE_DROPDOWN_BUILD_10_ROLLER_COASTERS,
    STR_OBJECTIVE_DROPDOWN_NUMBER_OF_GUESTS_IN_PARK,
    STR_OBJECTIVE_DROPDOWN_MONTHLY_INCOME_FROM_RIDE_TICKETS,
    STR_OBJECTIVE_DROPDOWN_BUILD_10_ROLLER_COASTERS_OF_A_GIVEN_LENGTH,
    STR_OBJECTIVE_DROPDOWN_FINISH_BUILDING_5_ROLLER_COASTERS,
    STR_OBJECTIVE_DROPDOWN_REPAY_LOAN_AND_ACHIEVE_A_GIVEN_PARK_VALUE,
    STR_OBJECTIVE_DROPDOWN_MONTHLY_PROFIT_FROM_FOOD_MERCHANDISE,
};

enum {
    WIDX_BACKGROUND,
    WIDX_TITLE,
    WIDX_CLOSE,
    WIDX_PAGE_BACKGROUND,
    WIDX_TAB_1,
    WIDX_TAB_2,

    WIDX_OBJECTIVE = 6,
    WIDX_OBJECTIVE_DROPDOWN,
    WIDX_OBJECTIVE_ARG_1,
    WIDX_OBJECTIVE_ARG_1_INCREASE,
    WIDX_OBJECTIVE_ARG_1_DECREASE,
    WIDX_OBJECTIVE_ARG_2,
    WIDX_OBJECTIVE_ARG_2_INCREASE,
    WIDX_OBJECTIVE_ARG_2_DECREASE,
    WIDX_CLIMATE,
    WIDX_CLIMATE_DROPDOWN,
    WIDX_PARK_NAME,
    WIDX_SCENARIO_NAME,
    WIDX_CATEGORY,
    WIDX_CATEGORY_DROPDOWN,
    WIDX_DETAILS,

    WIDX_RIDES = 6
};

#define MAIN_OBJECTIVE_OPTIONS_WIDGETS \
    { WWT_FRAME,            0,  0,      449,    0,      228,    STR_NONE,                   STR_NONE                                            }, \
    { WWT_CAPTION,          0,  1,      448,    1,      14,     STR_OBJECTIVE_SELECTION,    STR_WINDOW_TITLE_TIP                                }, \
    { WWT_CLOSEBOX,         0,  437,    447,    2,      13,     STR_CLOSE_X,                STR_CLOSE_WINDOW_TIP                                }, \
    { WWT_RESIZE,           1,  0,      279,    43,     148,    STR_NONE,                   STR_NONE                                            }, \
    { WWT_TAB,              1,  3,      33,     17,     43,     0x20000000 | SPR_TAB,       STR_SELECT_OBJECTIVE_AND_PARK_NAME_TIP              }, \
    { WWT_TAB,              1,  34,     64,     17,     43,     0x20000000 | SPR_TAB,       STR_SELECT_RIDES_TO_BE_PRESERVED_TIP                }

static rct_widget window_editor_objective_options_main_widgets[] = {
    MAIN_OBJECTIVE_OPTIONS_WIDGETS,
    { WWT_DROPDOWN,         1,  98,     441,    48,     59,     STR_NONE,                   STR_SELECT_OBJECTIVE_FOR_THIS_SCENARIO_TIP          },
    { WWT_DROPDOWN_BUTTON,  1,  430,    440,    49,     58,     STR_DROPDOWN_GLYPH,         STR_SELECT_OBJECTIVE_FOR_THIS_SCENARIO_TIP          },
    { WWT_SPINNER,          1,  158,    237,    65,     76,     STR_NONE,                   STR_NONE                                            },
    { WWT_DROPDOWN_BUTTON,  1,  226,    236,    66,     70,     STR_NUMERIC_UP,             STR_NONE                                            },
    { WWT_DROPDOWN_BUTTON,  1,  226,    236,    71,     75,     STR_NUMERIC_DOWN,           STR_NONE                                            },
    { WWT_SPINNER,          1,  158,    277,    82,     93,     STR_NONE,                   STR_NONE                                            },
    { WWT_DROPDOWN_BUTTON,  1,  266,    276,    83,     87,     STR_NUMERIC_UP,             STR_NONE                                            },
    { WWT_DROPDOWN_BUTTON,  1,  266,    276,    88,     92,     STR_NUMERIC_DOWN,           STR_NONE                                            },
    { WWT_DROPDOWN,         1,  98,     277,    99,     110,    STR_NONE,                   STR_SELECT_CLIMATE_TIP                              },
    { WWT_DROPDOWN_BUTTON,  1,  266,    276,    100,    109,    STR_DROPDOWN_GLYPH,         STR_SELECT_CLIMATE_TIP                              },
    { WWT_DROPDOWN_BUTTON,  1,  370,    444,    116,    127,    STR_CHANGE,                 STR_CHANGE_NAME_OF_PARK_TIP                         },
    { WWT_DROPDOWN_BUTTON,  1,  370,    444,    133,    144,    STR_CHANGE,                 STR_CHANGE_NAME_OF_SCENARIO_TIP                     },
    { WWT_DROPDOWN,         1,  98,     277,    150,    161,    STR_NONE,                   STR_SELECT_WHICH_GROUP_THIS_SCENARIO_APPEARS_IN     },
    { WWT_DROPDOWN_BUTTON,  1,  266,    276,    151,    160,    STR_DROPDOWN_GLYPH,         STR_SELECT_WHICH_GROUP_THIS_SCENARIO_APPEARS_IN     },
    { WWT_DROPDOWN_BUTTON,  1,  370,    444,    167,    178,    STR_CHANGE,                 STR_CHANGE_DETAIL_NOTES_ABOUT_PARK_SCENARIO_TIP     },
    { WIDGETS_END }
};

static rct_widget window_editor_objective_options_rides_widgets[] = {
    MAIN_OBJECTIVE_OPTIONS_WIDGETS,
    { WWT_SCROLL,           1,  3,      376,    60,     220,    SCROLL_VERTICAL,                        STR_NONE                                },
    { WIDGETS_END }
};

static rct_widget *window_editor_objective_options_widgets[] = {
    window_editor_objective_options_main_widgets,
    window_editor_objective_options_rides_widgets
};

#pragma endregion

#pragma region Events

static void window_editor_objective_options_main_mouseup(rct_window *w, rct_widgetindex widgetIndex);
static void window_editor_objective_options_main_resize(rct_window *w);
static void window_editor_objective_options_main_mousedown(rct_widgetindex widgetIndex, rct_window*w, rct_widget* widget);
static void window_editor_objective_options_main_dropdown(rct_window *w, rct_widgetindex widgetIndex, sint32 dropdownIndex);
static void window_editor_objective_options_main_update(rct_window *w);
static void window_editor_objective_options_main_textinput(rct_window *w, rct_widgetindex widgetIndex, char *text);
static void window_editor_objective_options_main_invalidate(rct_window *w);
static void window_editor_objective_options_main_paint(rct_window *w, rct_drawpixelinfo *dpi);

static void window_editor_objective_options_rides_mouseup(rct_window *w, rct_widgetindex widgetIndex);
static void window_editor_objective_options_rides_resize(rct_window *w);
static void window_editor_objective_options_rides_update(rct_window *w);
static void window_editor_objective_options_rides_scrollgetheight(rct_window *w, sint32 scrollIndex, sint32 *width, sint32 *height);
static void window_editor_objective_options_rides_scrollmousedown(rct_window *w, sint32 scrollIndex, sint32 x, sint32 y);
static void window_editor_objective_options_rides_scrollmouseover(rct_window *w, sint32 scrollIndex, sint32 x, sint32 y);
static void window_editor_objective_options_rides_invalidate(rct_window *w);
static void window_editor_objective_options_rides_paint(rct_window *w, rct_drawpixelinfo *dpi);
static void window_editor_objective_options_rides_scrollpaint(rct_window *w, rct_drawpixelinfo *dpi, sint32 scrollIndex);

// 0x009A9DF4
static rct_window_event_list window_objective_options_main_events = {
    NULL,
    window_editor_objective_options_main_mouseup,
    window_editor_objective_options_main_resize,
    window_editor_objective_options_main_mousedown,
    window_editor_objective_options_main_dropdown,
    NULL,
    window_editor_objective_options_main_update,
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
    window_editor_objective_options_main_textinput,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    window_editor_objective_options_main_invalidate,
    window_editor_objective_options_main_paint,
    NULL
};

// 0x009A9F58
static rct_window_event_list window_objective_options_rides_events = {
    NULL,
    window_editor_objective_options_rides_mouseup,
    window_editor_objective_options_rides_resize,
    NULL,
    NULL,
    NULL,
    window_editor_objective_options_rides_update,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    window_editor_objective_options_rides_scrollgetheight,
    window_editor_objective_options_rides_scrollmousedown,
    NULL,
    window_editor_objective_options_rides_scrollmouseover,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    window_editor_objective_options_rides_invalidate,
    window_editor_objective_options_rides_paint,
    window_editor_objective_options_rides_scrollpaint
};

static rct_window_event_list *window_editor_objective_options_page_events[] = {
    &window_objective_options_main_events,
    &window_objective_options_rides_events
};

#pragma endregion

#pragma region Enabled widgets

static uint64 window_editor_objective_options_page_enabled_widgets[] = {
    (1 << WIDX_CLOSE) |
    (1 << WIDX_TAB_1) |
    (1 << WIDX_TAB_2) |
    (1 << WIDX_OBJECTIVE) |
    (1 << WIDX_OBJECTIVE_DROPDOWN) |
    (1 << WIDX_OBJECTIVE_ARG_1_INCREASE) |
    (1 << WIDX_OBJECTIVE_ARG_1_DECREASE) |
    (1 << WIDX_OBJECTIVE_ARG_2_INCREASE) |
    (1 << WIDX_OBJECTIVE_ARG_2_DECREASE) |
    (1 << WIDX_CLIMATE) |
    (1 << WIDX_CLIMATE_DROPDOWN) |
    (1 << WIDX_PARK_NAME) |
    (1 << WIDX_SCENARIO_NAME) |
    (1 << WIDX_CATEGORY) |
    (1 << WIDX_CATEGORY_DROPDOWN) |
    (1 << WIDX_DETAILS),

    (1 << WIDX_CLOSE) |
    (1 << WIDX_TAB_1) |
    (1 << WIDX_TAB_2)
};

static uint64 window_editor_objective_options_page_hold_down_widgets[] = {
    (1 << WIDX_OBJECTIVE_ARG_1_INCREASE) |
    (1 << WIDX_OBJECTIVE_ARG_1_DECREASE) |
    (1 << WIDX_OBJECTIVE_ARG_2_INCREASE) |
    (1 << WIDX_OBJECTIVE_ARG_2_DECREASE),

    0
};

#pragma endregion

static void window_editor_objective_options_update_disabled_widgets(rct_window *w);

/**
 *
 *  rct2: 0x0067137D
 */
void window_editor_objective_options_open()
{
    rct_window *w;

    w = window_bring_to_front_by_class(WC_EDTIOR_OBJECTIVE_OPTIONS);
    if (w != NULL)
        return;

    w = window_create_centred(
        450,
        228,
        &window_objective_options_main_events,
        WC_EDTIOR_OBJECTIVE_OPTIONS,
        WF_10
    );
    w->widgets = window_editor_objective_options_main_widgets;
    w->enabled_widgets = window_editor_objective_options_page_enabled_widgets[WINDOW_EDITOR_OBJECTIVE_OPTIONS_PAGE_MAIN];
    w->pressed_widgets = 0;
    w->hold_down_widgets = window_editor_objective_options_page_hold_down_widgets[WINDOW_EDITOR_OBJECTIVE_OPTIONS_PAGE_MAIN];
    window_init_scroll_widgets(w);
    w->var_4AE = 0;
    w->selected_tab = WINDOW_EDITOR_OBJECTIVE_OPTIONS_PAGE_MAIN;
    w->no_list_items = 0;
    w->selected_list_item = -1;
    window_editor_objective_options_update_disabled_widgets(w);
}

static void window_editor_objective_options_set_pressed_tab(rct_window *w)
{
    sint32 i;
    for (i = 0; i < 2; i++)
        w->pressed_widgets &= ~(1 << (WIDX_TAB_1 + i));
    w->pressed_widgets |= 1LL << (WIDX_TAB_1 + w->page);
}

static void window_editor_objective_options_anchor_border_widgets(rct_window *w)
{
    w->widgets[WIDX_BACKGROUND].right = w->width - 1;
    w->widgets[WIDX_BACKGROUND].bottom = w->height - 1;
    w->widgets[WIDX_PAGE_BACKGROUND].right = w->width - 1;
    w->widgets[WIDX_PAGE_BACKGROUND].bottom = w->height - 1;
    w->widgets[WIDX_TITLE].right = w->width - 2;
    w->widgets[WIDX_CLOSE].left = w->width - 13;
    w->widgets[WIDX_CLOSE].right = w->width - 3;
}

static void window_editor_objective_options_draw_tab_images(rct_window *w, rct_drawpixelinfo *dpi)
{
    rct_widget *widget;
    sint32 spriteIndex;

    // Tab 1
    widget = &w->widgets[WIDX_TAB_1];

    spriteIndex = SPR_TAB_OBJECTIVE_0;
    if (w->page == WINDOW_EDITOR_OBJECTIVE_OPTIONS_PAGE_MAIN)
        spriteIndex += (w->frame_no / 4) % 16;

    gfx_draw_sprite(dpi, spriteIndex, w->x + widget->left, w->y + widget->top, 0);

    // Tab 2
    if (!(w->disabled_widgets & (1 << WIDX_TAB_2))) {
        widget = &w->widgets[WIDX_TAB_2];
        spriteIndex = SPR_TAB_RIDE_0;
        if (w->page == WINDOW_EDITOR_OBJECTIVE_OPTIONS_PAGE_RIDES)
            spriteIndex += (w->frame_no / 4) % 16;

        gfx_draw_sprite(dpi, spriteIndex, w->x + widget->left, w->y + widget->top, 0);
    }
}

/**
 *
 *  rct2: 0x00668496
 */
static void window_editor_objective_options_set_page(rct_window *w, sint32 page)
{
    if (w->page == page)
        return;

    w->page = page;
    w->frame_no = 0;
    w->var_492 = 0;
    w->no_list_items = 0;
    w->selected_list_item = -1;
    w->enabled_widgets = window_editor_objective_options_page_enabled_widgets[page];
    w->hold_down_widgets = window_editor_objective_options_page_hold_down_widgets[page];
    w->event_handlers = window_editor_objective_options_page_events[page];
    w->widgets = window_editor_objective_options_widgets[page];
    window_invalidate(w);
    window_editor_objective_options_update_disabled_widgets(w);
    window_event_resize_call(w);
    window_event_invalidate_call(w);
    window_init_scroll_widgets(w);
    window_invalidate(w);
}

/**
 *
 *  rct2: 0x0067201D
 */
static void window_editor_objective_options_set_objective(rct_window *w, sint32 objective)
{
    gScenarioObjectiveType = objective;
    window_invalidate(w);

    // Set default objective arguments
    switch (objective) {
    case OBJECTIVE_NONE:
    case OBJECTIVE_HAVE_FUN:
    case OBJECTIVE_BUILD_THE_BEST:
    case OBJECTIVE_10_ROLLERCOASTERS:
        break;
    case OBJECTIVE_GUESTS_BY:
        gScenarioObjectiveYear = 3;
        gScenarioObjectiveNumGuests = 1500;
        break;
    case OBJECTIVE_PARK_VALUE_BY:
        gScenarioObjectiveYear = 3;
        gScenarioObjectiveCurrency = MONEY(50000,00);
        break;
    case OBJECTIVE_GUESTS_AND_RATING:
        gScenarioObjectiveNumGuests = 2000;
        break;
    case OBJECTIVE_MONTHLY_RIDE_INCOME:
        gScenarioObjectiveCurrency = MONEY(10000,00);
        break;
    case OBJECTIVE_10_ROLLERCOASTERS_LENGTH:
        gScenarioObjectiveNumGuests = 1200;
        break;
    case OBJECTIVE_FINISH_5_ROLLERCOASTERS:
        gScenarioObjectiveCurrency = FIXED_2DP(6,70);
        break;
    case OBJECTIVE_REPLAY_LOAN_AND_PARK_VALUE:
        gScenarioObjectiveCurrency = MONEY(50000,00);
        break;
    case OBJECTIVE_MONTHLY_FOOD_INCOME:
        gScenarioObjectiveCurrency = MONEY(1000,00);
        break;
    }
}

/**
 *
 *  rct2: 0x006719CA
 */
static void window_editor_objective_options_main_mouseup(rct_window *w, rct_widgetindex widgetIndex)
{
    switch (widgetIndex) {
    case WIDX_CLOSE:
        window_close(w);
        break;
    case WIDX_TAB_1:
    case WIDX_TAB_2:
        window_editor_objective_options_set_page(w, widgetIndex - WIDX_TAB_1);
        break;
    case WIDX_PARK_NAME:
        set_format_arg(16, uint32, gParkNameArgs);
        window_text_input_open(w, WIDX_PARK_NAME, STR_PARK_NAME, STR_ENTER_PARK_NAME, gParkName, 0, 32);
        break;
    case WIDX_SCENARIO_NAME:
        window_text_input_raw_open(w, WIDX_SCENARIO_NAME, STR_SCENARIO_NAME, STR_ENTER_SCENARIO_NAME, gS6Info.name, 64);
        break;
    case WIDX_DETAILS:
        window_text_input_raw_open(w, WIDX_DETAILS, STR_PARK_SCENARIO_DETAILS, STR_ENTER_SCENARIO_DESCRIPTION, gS6Info.details, 256);
        break;
    }
}

/**
 *
 *  rct2: 0x00672254
 */
static void window_editor_objective_options_main_resize(rct_window *w)
{
    window_set_resize(w, 450, 229, 450, 229);
}

static void window_editor_objective_options_show_objective_dropdown(rct_window *w)
{
    sint32 i, numItems, objectiveType;
    rct_widget *dropdownWidget;
    uint32 parkFlags;

    dropdownWidget = &w->widgets[WIDX_OBJECTIVE];
    parkFlags = gParkFlags;
    numItems = 0;

    if (!(parkFlags & PARK_FLAGS_NO_MONEY_SCENARIO)) {
        numItems += 2;
        if (parkFlags & PARK_FLAGS_PARK_FREE_ENTRY)
            numItems++;
    }

    numItems += 5;

    i = 0;
    gDropdownItemsFormat[i] = STR_DROPDOWN_MENU_LABEL;
    gDropdownItemsArgs[i] = STR_OBJECTIVE_DROPDOWN_HAVE_FUN;
    i++;

    if (!(parkFlags & PARK_FLAGS_NO_MONEY_SCENARIO)) {
        gDropdownItemsFormat[i] = STR_DROPDOWN_MENU_LABEL;
        gDropdownItemsArgs[i] = STR_OBJECTIVE_DROPDOWN_NUMBER_OF_GUESTS_AT_A_GIVEN_DATE;
        i++;
        gDropdownItemsFormat[i] = STR_DROPDOWN_MENU_LABEL;
        gDropdownItemsArgs[i] = STR_OBJECTIVE_DROPDOWN_MONTHLY_PROFIT_FROM_FOOD_MERCHANDISE;
        i++;
        gDropdownItemsFormat[i] = STR_DROPDOWN_MENU_LABEL;
        gDropdownItemsArgs[i] = STR_OBJECTIVE_DROPDOWN_REPAY_LOAN_AND_ACHIEVE_A_GIVEN_PARK_VALUE;
        i++;
        gDropdownItemsFormat[i] = STR_DROPDOWN_MENU_LABEL;
        gDropdownItemsArgs[i] = STR_OBJECTIVE_DROPDOWN_PARK_VALUE_AT_A_GIVEN_DATE;
        i++;
        if (parkFlags & PARK_FLAGS_PARK_FREE_ENTRY) {
            gDropdownItemsFormat[i] = STR_DROPDOWN_MENU_LABEL;
            gDropdownItemsArgs[i] = STR_OBJECTIVE_DROPDOWN_MONTHLY_INCOME_FROM_RIDE_TICKETS;
            i++;
        }
    }

    gDropdownItemsFormat[i] = STR_DROPDOWN_MENU_LABEL;
    gDropdownItemsArgs[i] = STR_OBJECTIVE_DROPDOWN_NUMBER_OF_GUESTS_IN_PARK;
    i++;

    gDropdownItemsFormat[i] = STR_DROPDOWN_MENU_LABEL;
    gDropdownItemsArgs[i] = STR_OBJECTIVE_DROPDOWN_BUILD_10_ROLLER_COASTERS;
    i++;

    gDropdownItemsFormat[i] = STR_DROPDOWN_MENU_LABEL;
    gDropdownItemsArgs[i] = STR_OBJECTIVE_DROPDOWN_BUILD_10_ROLLER_COASTERS_OF_A_GIVEN_LENGTH;
    i++;

    gDropdownItemsFormat[i] = STR_DROPDOWN_MENU_LABEL;
    gDropdownItemsArgs[i] = STR_OBJECTIVE_DROPDOWN_FINISH_BUILDING_5_ROLLER_COASTERS;
    i++;

    window_dropdown_show_text_custom_width(
        w->x + dropdownWidget->left,
        w->y + dropdownWidget->top,
        dropdownWidget->bottom - dropdownWidget->top + 1,
        w->colours[1],
        0,
        DROPDOWN_FLAG_STAY_OPEN,
        numItems,
        dropdownWidget->right - dropdownWidget->left - 3
    );

    objectiveType = gScenarioObjectiveType;
    for (i = 0; i < numItems; i++) {
        if (gDropdownItemsArgs[i] - STR_OBJECTIVE_DROPDOWN_NONE == objectiveType) {
            dropdown_set_checked(i, true);
            break;
        }
    }
}

static void window_editor_objective_options_show_climate_dropdown(rct_window *w)
{
    sint32 i;
    rct_widget *dropdownWidget;

    dropdownWidget = &w->widgets[WIDX_CLIMATE];

    for (i = 0; i < 4; i++) {
        gDropdownItemsFormat[i] = STR_DROPDOWN_MENU_LABEL;
        gDropdownItemsArgs[i] = ClimateNames[i];
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
    dropdown_set_checked(gClimate, true);
}

static void window_editor_objective_options_show_category_dropdown(rct_window *w)
{
    sint32 i;
    rct_widget *dropdownWidget;

    dropdownWidget = &w->widgets[WIDX_CATEGORY];

    for (i = SCENARIO_CATEGORY_BEGINNER; i <= SCENARIO_CATEGORY_OTHER; i++) {
        gDropdownItemsFormat[i] = STR_DROPDOWN_MENU_LABEL;
        gDropdownItemsArgs[i] = ScenarioCategoryStringIds[i];
    }
    window_dropdown_show_text_custom_width(
        w->x + dropdownWidget->left,
        w->y + dropdownWidget->top,
        dropdownWidget->bottom - dropdownWidget->top + 1,
        w->colours[1],
        0,
        DROPDOWN_FLAG_STAY_OPEN,
        5,
        dropdownWidget->right - dropdownWidget->left - 3
    );
    dropdown_set_checked(gS6Info.category, true);
}

static void window_editor_objective_options_arg_1_increase(rct_window *w)
{
    switch (gScenarioObjectiveType) {
    case OBJECTIVE_PARK_VALUE_BY:
    case OBJECTIVE_MONTHLY_RIDE_INCOME:
    case OBJECTIVE_REPLAY_LOAN_AND_PARK_VALUE:
        if (gScenarioObjectiveCurrency >= MONEY(2000000,00)) {
            window_error_open(STR_CANT_INCREASE_FURTHER, STR_NONE);
        } else {
            gScenarioObjectiveCurrency += MONEY(1000,0);
            window_invalidate(w);
        }
        break;
    case OBJECTIVE_MONTHLY_FOOD_INCOME:
        if (gScenarioObjectiveCurrency >= MONEY(2000000,00)) {
            window_error_open(STR_CANT_INCREASE_FURTHER, STR_NONE);
        } else {
            gScenarioObjectiveCurrency += MONEY(100,0);
            window_invalidate(w);
        }
        break;
    case OBJECTIVE_10_ROLLERCOASTERS_LENGTH:
        if (gScenarioObjectiveNumGuests >= 5000) {
            window_error_open(STR_CANT_INCREASE_FURTHER, STR_NONE);
        } else {
            gScenarioObjectiveNumGuests += 100;
            window_invalidate(w);
        }
        break;
    case OBJECTIVE_FINISH_5_ROLLERCOASTERS:
        if (gScenarioObjectiveCurrency >= FIXED_2DP(9,90)) {
            window_error_open(STR_CANT_INCREASE_FURTHER, STR_NONE);
        } else {
            gScenarioObjectiveCurrency += FIXED_2DP(0,10);
            window_invalidate(w);
        }
        break;
    default:
        if (gScenarioObjectiveNumGuests >= 5000) {
            window_error_open(STR_CANT_INCREASE_FURTHER, STR_NONE);
        } else {
            gScenarioObjectiveNumGuests += 50;
            window_invalidate(w);
        }
        break;
    }
}

static void window_editor_objective_options_arg_1_decrease(rct_window *w)
{
    switch (gScenarioObjectiveType) {
    case OBJECTIVE_PARK_VALUE_BY:
    case OBJECTIVE_MONTHLY_RIDE_INCOME:
    case OBJECTIVE_REPLAY_LOAN_AND_PARK_VALUE:
        if (gScenarioObjectiveCurrency <= MONEY(1000,00)) {
            window_error_open(STR_CANT_REDUCE_FURTHER, STR_NONE);
        } else {
            gScenarioObjectiveCurrency -= MONEY(1000,0);
            window_invalidate(w);
        }
        break;
    case OBJECTIVE_MONTHLY_FOOD_INCOME:
        if (gScenarioObjectiveCurrency <= MONEY(1000,00)) {
            window_error_open(STR_CANT_REDUCE_FURTHER, STR_NONE);
        } else {
            gScenarioObjectiveCurrency -= MONEY(100,0);
            window_invalidate(w);
        }
        break;
    case OBJECTIVE_10_ROLLERCOASTERS_LENGTH:
        if (gScenarioObjectiveNumGuests <= 1000) {
            window_error_open(STR_CANT_REDUCE_FURTHER, STR_NONE);
        } else {
            gScenarioObjectiveNumGuests -= 100;
            window_invalidate(w);
        }
        break;
    case OBJECTIVE_FINISH_5_ROLLERCOASTERS:
        if (gScenarioObjectiveCurrency <= FIXED_2DP(4,00)) {
            window_error_open(STR_CANT_REDUCE_FURTHER, STR_NONE);
        } else {
            gScenarioObjectiveCurrency -= FIXED_2DP(0,10);
            window_invalidate(w);
        }
        break;
    default:
        if (gScenarioObjectiveNumGuests <= 250) {
            window_error_open(STR_CANT_REDUCE_FURTHER, STR_NONE);
        } else {
            gScenarioObjectiveNumGuests -= 50;
            window_invalidate(w);
        }
        break;
    }
}

static void window_editor_objective_options_arg_2_increase(rct_window *w)
{
    if (gScenarioObjectiveYear >= 25) {
        window_error_open(STR_CANT_INCREASE_FURTHER, STR_NONE);
    } else {
        gScenarioObjectiveYear++;
        window_invalidate(w);
    }
}

static void window_editor_objective_options_arg_2_decrease(rct_window *w)
{
    if (gScenarioObjectiveYear <= 1) {
        window_error_open(STR_CANT_REDUCE_FURTHER, STR_NONE);
    } else {
        gScenarioObjectiveYear--;
        window_invalidate(w);
    }
}

/**
 *
 *  rct2: 0x00671A0D
 */
static void window_editor_objective_options_main_mousedown(rct_widgetindex widgetIndex, rct_window *w, rct_widget* widget)
{
    switch (widgetIndex) {
    case WIDX_OBJECTIVE_DROPDOWN:
        window_editor_objective_options_show_objective_dropdown(w);
        break;
    case WIDX_OBJECTIVE_ARG_1_INCREASE:
        window_editor_objective_options_arg_1_increase(w);
        break;
    case WIDX_OBJECTIVE_ARG_1_DECREASE:
        window_editor_objective_options_arg_1_decrease(w);
        break;
    case WIDX_OBJECTIVE_ARG_2_INCREASE:
        window_editor_objective_options_arg_2_increase(w);
        break;
    case WIDX_OBJECTIVE_ARG_2_DECREASE:
        window_editor_objective_options_arg_2_decrease(w);
        break;
    case WIDX_CLIMATE_DROPDOWN:
        window_editor_objective_options_show_climate_dropdown(w);
        break;
    case WIDX_CATEGORY_DROPDOWN:
        window_editor_objective_options_show_category_dropdown(w);
        break;
    }
}

/**
 *
 *  rct2: 0x00671A54
 */
static void window_editor_objective_options_main_dropdown(rct_window *w, rct_widgetindex widgetIndex, sint32 dropdownIndex)
{
    uint8 newObjectiveType;

    if (dropdownIndex == -1)
        return;

    switch (widgetIndex) {
    case WIDX_OBJECTIVE_DROPDOWN:
        // TODO: Don't rely on string ID order
        newObjectiveType = (uint8)(gDropdownItemsArgs[dropdownIndex] - STR_OBJECTIVE_DROPDOWN_NONE);
        if (gScenarioObjectiveType != newObjectiveType)
            window_editor_objective_options_set_objective(w, newObjectiveType);
        break;
    case WIDX_CLIMATE_DROPDOWN:
        if (gClimate != (uint8)dropdownIndex) {
            gClimate = (uint8)dropdownIndex;
            window_invalidate(w);
        }
        break;
    case WIDX_CATEGORY_DROPDOWN:
        if (gS6Info.category != (uint8)dropdownIndex) {
            gS6Info.category = (uint8)dropdownIndex;
            window_invalidate(w);
        }
        break;
    }
}

/**
 *
 *  rct2: 0x006721E7
 */
static void window_editor_objective_options_main_update(rct_window *w)
{
    uint32 parkFlags;
    uint8 objectiveType;

    w->frame_no++;
    window_event_invalidate_call(w);
    widget_invalidate(w, WIDX_TAB_1);

    parkFlags = gParkFlags;
    objectiveType = gScenarioObjectiveType;

    // Reset objective if invalid
    if ((
        (parkFlags & PARK_FLAGS_NO_MONEY_SCENARIO) &&

        // The following objectives are the only valid objectives when there is no money
        objectiveType != OBJECTIVE_HAVE_FUN &&
        objectiveType != OBJECTIVE_10_ROLLERCOASTERS &&
        objectiveType != OBJECTIVE_GUESTS_AND_RATING &&
        objectiveType != OBJECTIVE_10_ROLLERCOASTERS_LENGTH &&
        objectiveType != OBJECTIVE_FINISH_5_ROLLERCOASTERS
    ) || (
        // The park must be free for the monthly ride income objective
        !(parkFlags & PARK_FLAGS_PARK_FREE_ENTRY) &&
        objectiveType == OBJECTIVE_MONTHLY_RIDE_INCOME
    )) {
        // Reset objective
        window_editor_objective_options_set_objective(w, OBJECTIVE_GUESTS_AND_RATING);
    }
}

/**
 *
 *  rct2: 0x00671A73
 */
static void window_editor_objective_options_main_textinput(rct_window *w, rct_widgetindex widgetIndex, char *text)
{
    if (text == NULL)
        return;

    switch (widgetIndex) {
    case WIDX_PARK_NAME:
        park_set_name(text);

        if (gS6Info.name[0] == 0)
            format_string(gS6Info.name, 64, gParkName, &gParkNameArgs);
        break;
    case WIDX_SCENARIO_NAME:
        safe_strcpy(gS6Info.name, text, 64);
        if (strnlen(gS6Info.name, 64) == 64)
        {
            gS6Info.name[64 - 1] = '\0';
            log_warning("Truncated S6 name: %s", gS6Info.name);
        }
        window_invalidate(w);
        break;
    case WIDX_DETAILS:
        safe_strcpy(gS6Info.details, text, 256);
        if (strnlen(gS6Info.details, 256) == 256)
        {
            gS6Info.details[256 - 1] = '\0';
            log_warning("Truncated S6 name: %s", gS6Info.details);
        }
        window_invalidate(w);
        break;
    }
}

/**
 *
 *  rct2: 0x0067161C
 */
static void window_editor_objective_options_main_invalidate(rct_window *w)
{
    rct_widget *widgets;
    rct_stex_entry *stex;

    stex = g_stexEntries[0];
    if (stex == (rct_stex_entry*)-1)
        stex = NULL;

    widgets = window_editor_objective_options_widgets[w->page];
    if (w->widgets != widgets) {
        w->widgets = widgets;
        window_init_scroll_widgets(w);
    }

    window_editor_objective_options_set_pressed_tab(w);

    if (stex == NULL)
        w->disabled_widgets &= ~((1 << WIDX_PARK_NAME) | (1 << WIDX_SCENARIO_NAME));
    else
        w->disabled_widgets |= ((1 << WIDX_PARK_NAME) | (1 << WIDX_SCENARIO_NAME));

    switch (gScenarioObjectiveType) {
    case OBJECTIVE_GUESTS_BY:
    case OBJECTIVE_PARK_VALUE_BY:
        window_editor_objective_options_main_widgets[WIDX_OBJECTIVE_ARG_1].type = WWT_SPINNER;
        window_editor_objective_options_main_widgets[WIDX_OBJECTIVE_ARG_1_INCREASE].type = WWT_DROPDOWN_BUTTON;
        window_editor_objective_options_main_widgets[WIDX_OBJECTIVE_ARG_1_DECREASE].type = WWT_DROPDOWN_BUTTON;
        window_editor_objective_options_main_widgets[WIDX_OBJECTIVE_ARG_2].type = WWT_SPINNER;
        window_editor_objective_options_main_widgets[WIDX_OBJECTIVE_ARG_2_INCREASE].type = WWT_DROPDOWN_BUTTON;
        window_editor_objective_options_main_widgets[WIDX_OBJECTIVE_ARG_2_DECREASE].type = WWT_DROPDOWN_BUTTON;
        break;
    case OBJECTIVE_GUESTS_AND_RATING:
    case OBJECTIVE_MONTHLY_RIDE_INCOME:
    case OBJECTIVE_10_ROLLERCOASTERS_LENGTH:
    case OBJECTIVE_FINISH_5_ROLLERCOASTERS:
    case OBJECTIVE_REPLAY_LOAN_AND_PARK_VALUE:
    case OBJECTIVE_MONTHLY_FOOD_INCOME:
        window_editor_objective_options_main_widgets[WIDX_OBJECTIVE_ARG_1].type = WWT_SPINNER;
        window_editor_objective_options_main_widgets[WIDX_OBJECTIVE_ARG_1_INCREASE].type = WWT_DROPDOWN_BUTTON;
        window_editor_objective_options_main_widgets[WIDX_OBJECTIVE_ARG_1_DECREASE].type = WWT_DROPDOWN_BUTTON;
        window_editor_objective_options_main_widgets[WIDX_OBJECTIVE_ARG_2].type = WWT_EMPTY;
        window_editor_objective_options_main_widgets[WIDX_OBJECTIVE_ARG_2_INCREASE].type = WWT_EMPTY;
        window_editor_objective_options_main_widgets[WIDX_OBJECTIVE_ARG_2_DECREASE].type = WWT_EMPTY;
        break;
    default:
        window_editor_objective_options_main_widgets[WIDX_OBJECTIVE_ARG_1].type = WWT_EMPTY;
        window_editor_objective_options_main_widgets[WIDX_OBJECTIVE_ARG_1_INCREASE].type = WWT_EMPTY;
        window_editor_objective_options_main_widgets[WIDX_OBJECTIVE_ARG_1_DECREASE].type = WWT_EMPTY;
        window_editor_objective_options_main_widgets[WIDX_OBJECTIVE_ARG_2].type = WWT_EMPTY;
        window_editor_objective_options_main_widgets[WIDX_OBJECTIVE_ARG_2_INCREASE].type = WWT_EMPTY;
        window_editor_objective_options_main_widgets[WIDX_OBJECTIVE_ARG_2_DECREASE].type = WWT_EMPTY;
        break;
    }

    window_editor_objective_options_main_widgets[WIDX_CLOSE].type =
        (gScreenFlags & SCREEN_FLAGS_SCENARIO_EDITOR) ? WWT_EMPTY : WWT_CLOSEBOX;

    window_editor_objective_options_anchor_border_widgets(w);
}

/**
 *
 *  rct2: 0x0067161C
 */
static void window_editor_objective_options_main_paint(rct_window *w, rct_drawpixelinfo *dpi)
{
    rct_stex_entry *stex;
    sint32 x, y, width;
    rct_string_id stringId;
    uint32 arg;

    window_draw_widgets(w, dpi);
    window_editor_objective_options_draw_tab_images(w, dpi);

    stex = g_stexEntries[0];
    if (stex == (rct_stex_entry*)-1)
        stex = NULL;

    // Objective label
    x = w->x + 8;
    y = w->y + w->widgets[WIDX_OBJECTIVE].top;
    gfx_draw_string_left(dpi, STR_OBJECTIVE_WINDOW, NULL, COLOUR_BLACK, x, y);

    // Objective value
    x = w->x + w->widgets[WIDX_OBJECTIVE].left + 1;
    y = w->y + w->widgets[WIDX_OBJECTIVE].top;
    stringId = ObjectiveDropdownOptionNames[gScenarioObjectiveType];
    gfx_draw_string_left(dpi, STR_WINDOW_COLOUR_2_STRINGID, &stringId, COLOUR_BLACK, x, y);

    if (w->widgets[WIDX_OBJECTIVE_ARG_1].type != WWT_EMPTY) {
        // Objective argument 1 label
        x = w->x + 28;
        y = w->y + w->widgets[WIDX_OBJECTIVE_ARG_1].top;
        switch (gScenarioObjectiveType) {
        case OBJECTIVE_GUESTS_BY:
        case OBJECTIVE_GUESTS_AND_RATING:
            stringId = STR_WINDOW_OBJECTIVE_GUEST_COUNT;
            break;
        case OBJECTIVE_PARK_VALUE_BY:
        case OBJECTIVE_REPLAY_LOAN_AND_PARK_VALUE:
            stringId = STR_WINDOW_OBJECTIVE_PARK_VALUE;
            break;
        case OBJECTIVE_MONTHLY_RIDE_INCOME:
            stringId = STR_WINDOW_OBJECTIVE_MONTHLY_INCOME;
            break;
        case OBJECTIVE_MONTHLY_FOOD_INCOME:
            stringId = STR_WINDOW_OBJECTIVE_MONTHLY_PROFIT;
            break;
        case OBJECTIVE_10_ROLLERCOASTERS_LENGTH:
            stringId = STR_WINDOW_OBJECTIVE_MINIMUM_LENGTH;
            break;
        default:
            stringId = STR_WINDOW_OBJECTIVE_EXCITEMENT_RATING;
            break;
        }
        gfx_draw_string_left(dpi, stringId, NULL, COLOUR_BLACK, x, y);

        // Objective argument 1 value
        x = w->x + w->widgets[WIDX_OBJECTIVE_ARG_1].left + 1;
        y = w->y + w->widgets[WIDX_OBJECTIVE_ARG_1].top;
        switch (gScenarioObjectiveType) {
        case OBJECTIVE_GUESTS_BY:
        case OBJECTIVE_GUESTS_AND_RATING:
            stringId = STR_WINDOW_OBJECTIVE_VALUE_GUEST_COUNT;
            arg = gScenarioObjectiveNumGuests;
            break;
        case OBJECTIVE_PARK_VALUE_BY:
        case OBJECTIVE_REPLAY_LOAN_AND_PARK_VALUE:
        case OBJECTIVE_MONTHLY_RIDE_INCOME:
        case OBJECTIVE_MONTHLY_FOOD_INCOME:
            stringId = STR_CURRENCY_FORMAT_LABEL;
            arg = gScenarioObjectiveCurrency;
            break;
        case OBJECTIVE_10_ROLLERCOASTERS_LENGTH:
            stringId = STR_WINDOW_OBJECTIVE_VALUE_LENGTH;
            arg = gScenarioObjectiveNumGuests;
            break;
        default:
            stringId = STR_WINDOW_OBJECTIVE_VALUE_RATING;
            arg = gScenarioObjectiveCurrency;
            break;
        }
        gfx_draw_string_left(dpi, stringId, &arg, COLOUR_BLACK, x, y);
    }

    if (w->widgets[WIDX_OBJECTIVE_ARG_2].type != WWT_EMPTY) {
        // Objective argument 2 label
        x = w->x + 28;
        y = w->y + w->widgets[WIDX_OBJECTIVE_ARG_2].top;
        gfx_draw_string_left(dpi, STR_WINDOW_OBJECTIVE_DATE, NULL, COLOUR_BLACK, x, y);

        // Objective argument 2 value
        x = w->x + w->widgets[WIDX_OBJECTIVE_ARG_2].left + 1;
        y = w->y + w->widgets[WIDX_OBJECTIVE_ARG_2].top;
        arg = (gScenarioObjectiveYear * MONTH_COUNT) - 1;
        gfx_draw_string_left(dpi, STR_WINDOW_OBJECTIVE_VALUE_DATE, &arg, COLOUR_BLACK, x, y);
    }

    // Climate label
    x = w->x + 8;
    y = w->y + w->widgets[WIDX_CLIMATE].top;
    gfx_draw_string_left(dpi, STR_CLIMATE_LABEL, NULL, COLOUR_BLACK, x, y);

    // Climate value
    x = w->x + w->widgets[WIDX_CLIMATE].left + 1;
    y = w->y + w->widgets[WIDX_CLIMATE].top;
    stringId = ClimateNames[gClimate];
    gfx_draw_string_left(dpi, STR_WINDOW_COLOUR_2_STRINGID, &stringId, COLOUR_BLACK, x, y);

    // Park name
    x = w->x + 8;
    y = w->y + w->widgets[WIDX_PARK_NAME].top;
    width = w->widgets[WIDX_PARK_NAME].left - 16;

    if (stex != NULL) {
        set_format_arg(0, rct_string_id, stex->park_name);
    } else {
        set_format_arg(0, rct_string_id, gParkName);
    }
    set_format_arg(2, uint32, gParkNameArgs);
    gfx_draw_string_left_clipped(dpi, STR_WINDOW_PARK_NAME, gCommonFormatArgs, COLOUR_BLACK, x, y, width);

    // Scenario name
    x = w->x + 8;
    y = w->y + w->widgets[WIDX_SCENARIO_NAME].top;
    width = w->widgets[WIDX_SCENARIO_NAME].left - 16;

    if (stex != NULL) {
        set_format_arg(0, rct_string_id, stex->scenario_name);
        set_format_arg(2, uint32, gParkNameArgs);
    } else {
        set_format_arg(0, rct_string_id, STR_STRING);
        set_format_arg(2, const char *, gS6Info.name);
    }

    gfx_draw_string_left_clipped(dpi, STR_WINDOW_SCENARIO_NAME, gCommonFormatArgs, COLOUR_BLACK, x, y, width);

    // Scenario details label
    x = w->x + 8;
    y = w->y + w->widgets[WIDX_DETAILS].top;
    gfx_draw_string_left(dpi, STR_WINDOW_PARK_DETAILS, NULL, COLOUR_BLACK, x, y);

    // Scenario details value
    x = w->x + 16;
    y = w->y + w->widgets[WIDX_DETAILS].top + 10;
    width = w->widgets[WIDX_DETAILS].left - 4;

    if (stex != NULL) {
        set_format_arg(0, rct_string_id, stex->details);
        set_format_arg(2, uint32, gParkNameArgs);
    } else {
        set_format_arg(0, rct_string_id, STR_STRING);
        set_format_arg(2, const char *, gS6Info.details);
    }
    gfx_draw_string_left_wrapped(dpi, gCommonFormatArgs, x, y, width, STR_BLACK_STRING, COLOUR_BLACK);

    // Scenario category label
    x = w->x + 8;
    y = w->y + w->widgets[WIDX_CATEGORY].top;
    gfx_draw_string_left(dpi, STR_WINDOW_SCENARIO_GROUP, NULL, COLOUR_BLACK, x, y);

    // Scenario category value
    x = w->x + w->widgets[WIDX_CATEGORY].left + 1;
    y = w->y + w->widgets[WIDX_CATEGORY].top;
    stringId = ScenarioCategoryStringIds[gS6Info.category];
    gfx_draw_string_left(dpi, STR_WINDOW_COLOUR_2_STRINGID, &stringId, COLOUR_BLACK, x, y);
}

/**
 *
 *  rct2: 0x006724A4
 */
static void window_editor_objective_options_rides_mouseup(rct_window *w, rct_widgetindex widgetIndex)
{
    switch (widgetIndex) {
    case WIDX_CLOSE:
        window_close(w);
        break;
    case WIDX_TAB_1:
    case WIDX_TAB_2:
        window_editor_objective_options_set_page(w, widgetIndex - WIDX_TAB_1);
        break;
    }
}

/**
 *
 *  rct2: 0x006725A8
 */
static void window_editor_objective_options_rides_resize(rct_window *w)
{
    window_set_resize(w, 380, 224, 380, 224);
}

/**
 *
 *  rct2: 0x00672544
 */
static void window_editor_objective_options_rides_update(rct_window *w)
{
    sint32 i, numItems;
    rct_ride *ride;

    w->frame_no++;
    window_event_invalidate_call(w);
    window_event_resize_call(w);
    widget_invalidate(w, WIDX_TAB_2);

    numItems = 0;
    FOR_ALL_RIDES(i, ride) {
        if (gRideClassifications[ride->type] == RIDE_CLASS_RIDE) {
            w->list_item_positions[numItems] = i;
            numItems++;
        }
    }

    if (w->no_list_items != numItems) {
        w->no_list_items = numItems;
        window_invalidate(w);
    }
}

/**
 *
 *  rct2: 0x006724BF
 */
static void window_editor_objective_options_rides_scrollgetheight(rct_window *w, sint32 scrollIndex, sint32 *width, sint32 *height)
{
    *height = w->no_list_items * 12;
}

/**
 *
 *  rct2: 0x006724FC
 */
static void window_editor_objective_options_rides_scrollmousedown(rct_window *w, sint32 scrollIndex, sint32 x, sint32 y)
{
    rct_ride *ride;
    sint32 i;

    i = y / 12;
    if (i < 0 || i >= w->no_list_items)
        return;

    ride = get_ride(w->list_item_positions[i]);
    ride->lifecycle_flags ^= RIDE_LIFECYCLE_INDESTRUCTIBLE;
    window_invalidate(w);
}

/**
 *
 *  rct2: 0x006724CC
 */
static void window_editor_objective_options_rides_scrollmouseover(rct_window *w, sint32 scrollIndex, sint32 x, sint32 y)
{
    sint32 i;

    i = y / 12;
    if (i < 0 || i >= w->no_list_items)
        return;

    if (w->selected_list_item != i) {
        w->selected_list_item = i;
        window_invalidate(w);
    }
}

/**
 *
 *  rct2: 0x006722B5
 */
static void window_editor_objective_options_rides_invalidate(rct_window *w)
{
    rct_widget *widgets;

    widgets = window_editor_objective_options_widgets[w->page];
    if (w->widgets != widgets) {
        w->widgets = widgets;
        window_init_scroll_widgets(w);
    }

    window_editor_objective_options_set_pressed_tab(w);

    window_editor_objective_options_main_widgets[WIDX_CLOSE].type = (gScreenFlags & SCREEN_FLAGS_SCENARIO_EDITOR) ? WWT_EMPTY : WWT_CLOSEBOX;

    window_editor_objective_options_anchor_border_widgets(w);
}

/**
 *
 *  rct2: 0x00672340
 */
static void window_editor_objective_options_rides_paint(rct_window *w, rct_drawpixelinfo *dpi)
{
    window_draw_widgets(w, dpi);
    window_editor_objective_options_draw_tab_images(w, dpi);

    gfx_draw_string_left(dpi, STR_WINDOW_PRESERVATION_ORDER, NULL, COLOUR_BLACK, w->x + 6, w->y + w->widgets[WIDX_PAGE_BACKGROUND].top + 3);
}

/**
 *
 *  rct2: 0x0067236F
 */
static void window_editor_objective_options_rides_scrollpaint(rct_window *w, rct_drawpixelinfo *dpi, sint32 scrollIndex)
{
    rct_string_id stringId;
    rct_ride *ride;

    sint32 colour = ColourMapA[w->colours[1]].mid_light;
    gfx_fill_rect(dpi, dpi->x, dpi->y, dpi->x + dpi->width - 1, dpi->y + dpi->height - 1, colour);

    for (sint32 i = 0; i < w->no_list_items; i++) {
        sint32 y = i * 12;

        if (y + 12 < dpi->y || y >= dpi->y + dpi->height)
            continue;

        // Checkbox
        gfx_fill_rect_inset(dpi, 2, y, 11, y + 10, w->colours[1], INSET_RECT_F_E0);

        // Highlighted
        if (i == w->selected_list_item) {
            stringId = STR_WINDOW_COLOUR_2_STRINGID;
            gfx_filter_rect(dpi, 0, y, w->width, y + 11, PALETTE_DARKEN_1);
        } else {
            stringId = STR_BLACK_STRING;
        }

        // Checkbox mark
        ride = get_ride(w->list_item_positions[i]);
        if (ride->lifecycle_flags & RIDE_LIFECYCLE_INDESTRUCTIBLE) {
            gCurrentFontSpriteBase = stringId == STR_WINDOW_COLOUR_2_STRINGID ? FONT_SPRITE_BASE_MEDIUM_EXTRA_DARK : FONT_SPRITE_BASE_MEDIUM_DARK;
            gfx_draw_string(dpi, (char*)CheckBoxMarkString, w->colours[1] & 0x7F, 2, y);
        }

        // Ride name
        gfx_draw_string_left(dpi, stringId, &ride->name, COLOUR_BLACK, 15, y);
    }
}

/**
 *
 *  rct2: 0x00672609
 */
static void window_editor_objective_options_update_disabled_widgets(rct_window *w)
{
    rct_ride *ride;
    sint32 i, numRides;

    // Check if there are any rides (not shops or facilities)
    numRides = 0;
    FOR_ALL_RIDES(i, ride) {
        if (gRideClassifications[ride->type] == RIDE_CLASS_RIDE) {
            numRides++;
        }
    }

    if (numRides != 0) {
        w->disabled_widgets &= ~(1 << WIDX_TAB_2);
    } else {
        w->disabled_widgets |= (1 << WIDX_TAB_2);
    }
}
