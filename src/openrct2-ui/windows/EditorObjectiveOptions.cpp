/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "../interface/Dropdown.h"
#include "../interface/Widget.h"
#include "../interface/Window.h"
#include "Window.h"

#include <algorithm>
#include <iterator>
#include <openrct2/Context.h>
#include <openrct2/GameState.h>
#include <openrct2/OpenRCT2.h>
#include <openrct2/actions/ParkSetNameAction.hpp>
#include <openrct2/core/String.hpp>
#include <openrct2/drawing/Drawing.h>
#include <openrct2/drawing/Font.h>
#include <openrct2/interface/Colour.h>
#include <openrct2/localisation/Date.h>
#include <openrct2/localisation/Language.h>
#include <openrct2/localisation/Localisation.h>
#include <openrct2/localisation/StringIds.h>
#include <openrct2/ride/Ride.h>
#include <openrct2/scenario/Scenario.h>
#include <openrct2/sprites.h>
#include <openrct2/util/Util.h>
#include <openrct2/world/Park.h>

static constexpr const rct_string_id WINDOW_TITLE = STR_OBJECTIVE_SELECTION;
static constexpr const int32_t WH = 229;
static constexpr const int32_t WW = 450;

#pragma region Widgets

// clang-format off
enum {
    WINDOW_EDITOR_OBJECTIVE_OPTIONS_PAGE_MAIN,
    WINDOW_EDITOR_OBJECTIVE_OPTIONS_PAGE_RIDES,
    WINDOW_EDITOR_OBJECTIVE_OPTIONS_PAGE_COUNT
};

static constexpr const rct_string_id ObjectiveDropdownOptionNames[] = {
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
    WIDX_PARK_NAME,
    WIDX_SCENARIO_NAME,
    WIDX_CATEGORY,
    WIDX_CATEGORY_DROPDOWN,
    WIDX_DETAILS,

    WIDX_RIDES = 6
};

#define MAIN_OBJECTIVE_OPTIONS_WIDGETS \
    WINDOW_SHIM(WINDOW_TITLE, WW, WH), \
    MakeWidget({  0,  43}, {280, 106}, WWT_RESIZE, WindowColour::Secondary), \
    MakeTab   ({  3,  17}, STR_SELECT_OBJECTIVE_AND_PARK_NAME_TIP         ), \
    MakeTab   ({ 34,  17}, STR_SELECT_RIDES_TO_BE_PRESERVED_TIP           )

static rct_widget window_editor_objective_options_main_widgets[] = {
    MAIN_OBJECTIVE_OPTIONS_WIDGETS,
    MakeWidget        ({ 98,  48}, {344,  12}, WWT_DROPDOWN, WindowColour::Secondary, STR_NONE,           STR_SELECT_OBJECTIVE_FOR_THIS_SCENARIO_TIP     ),
    MakeWidget        ({430,  49}, { 11,  10}, WWT_BUTTON,   WindowColour::Secondary, STR_DROPDOWN_GLYPH, STR_SELECT_OBJECTIVE_FOR_THIS_SCENARIO_TIP     ),
    MakeSpinnerWidgets({158,  65}, {120,  12}, WWT_BUTTON,   WindowColour::Secondary                                                                     ), // NB: 3 widgets
    MakeSpinnerWidgets({158,  82}, {120,  12}, WWT_BUTTON,   WindowColour::Secondary                                                                     ), // NB: 3 widgets
    MakeWidget        ({370,  99}, { 75,  12}, WWT_BUTTON,   WindowColour::Secondary, STR_CHANGE,         STR_CHANGE_NAME_OF_PARK_TIP                    ),
    MakeWidget        ({370, 116}, { 75,  12}, WWT_BUTTON,   WindowColour::Secondary, STR_CHANGE,         STR_CHANGE_NAME_OF_SCENARIO_TIP                ),
    MakeWidget        ({ 98, 133}, {180,  12}, WWT_DROPDOWN, WindowColour::Secondary, STR_NONE,           STR_SELECT_WHICH_GROUP_THIS_SCENARIO_APPEARS_IN),
    MakeWidget        ({266, 134}, { 11,  10}, WWT_BUTTON,   WindowColour::Secondary, STR_DROPDOWN_GLYPH, STR_SELECT_WHICH_GROUP_THIS_SCENARIO_APPEARS_IN),
    MakeWidget        ({370, 150}, { 75,  12}, WWT_BUTTON,   WindowColour::Secondary, STR_CHANGE,         STR_CHANGE_DETAIL_NOTES_ABOUT_PARK_SCENARIO_TIP),
    { WIDGETS_END }
};

static rct_widget window_editor_objective_options_rides_widgets[] = {
    MAIN_OBJECTIVE_OPTIONS_WIDGETS,
    MakeWidget({  3,  60}, {374, 161}, WWT_SCROLL, WindowColour::Secondary, SCROLL_VERTICAL),
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
static void window_editor_objective_options_main_mousedown(rct_window *w, rct_widgetindex widgetIndex, rct_widget* widget);
static void window_editor_objective_options_main_dropdown(rct_window *w, rct_widgetindex widgetIndex, int32_t dropdownIndex);
static void window_editor_objective_options_main_update(rct_window *w);
static void window_editor_objective_options_main_textinput(rct_window *w, rct_widgetindex widgetIndex, char *text);
static void window_editor_objective_options_main_invalidate(rct_window *w);
static void window_editor_objective_options_main_paint(rct_window *w, rct_drawpixelinfo *dpi);

static void window_editor_objective_options_rides_mouseup(rct_window *w, rct_widgetindex widgetIndex);
static void window_editor_objective_options_rides_resize(rct_window *w);
static void window_editor_objective_options_rides_update(rct_window *w);
static void window_editor_objective_options_rides_scrollgetheight(rct_window *w, int32_t scrollIndex, int32_t *width, int32_t *height);
static void window_editor_objective_options_rides_scrollmousedown(rct_window *w, int32_t scrollIndex, const ScreenCoordsXY& screenCoords);
static void window_editor_objective_options_rides_scrollmouseover(rct_window *w, int32_t scrollIndex, const ScreenCoordsXY& screenCoords);
static void window_editor_objective_options_rides_invalidate(rct_window *w);
static void window_editor_objective_options_rides_paint(rct_window *w, rct_drawpixelinfo *dpi);
static void window_editor_objective_options_rides_scrollpaint(rct_window *w, rct_drawpixelinfo *dpi, int32_t scrollIndex);

// 0x009A9DF4
static rct_window_event_list window_objective_options_main_events = {
    nullptr,
    window_editor_objective_options_main_mouseup,
    window_editor_objective_options_main_resize,
    window_editor_objective_options_main_mousedown,
    window_editor_objective_options_main_dropdown,
    nullptr,
    window_editor_objective_options_main_update,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    window_editor_objective_options_main_textinput,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    window_editor_objective_options_main_invalidate,
    window_editor_objective_options_main_paint,
    nullptr
};

// 0x009A9F58
static rct_window_event_list window_objective_options_rides_events = {
    nullptr,
    window_editor_objective_options_rides_mouseup,
    window_editor_objective_options_rides_resize,
    nullptr,
    nullptr,
    nullptr,
    window_editor_objective_options_rides_update,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    window_editor_objective_options_rides_scrollgetheight,
    window_editor_objective_options_rides_scrollmousedown,
    nullptr,
    window_editor_objective_options_rides_scrollmouseover,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
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

static uint64_t window_editor_objective_options_page_enabled_widgets[] = {
    (1 << WIDX_CLOSE) |
    (1 << WIDX_TAB_1) |
    (1 << WIDX_TAB_2) |
    (1 << WIDX_OBJECTIVE) |
    (1 << WIDX_OBJECTIVE_DROPDOWN) |
    (1 << WIDX_OBJECTIVE_ARG_1_INCREASE) |
    (1 << WIDX_OBJECTIVE_ARG_1_DECREASE) |
    (1 << WIDX_OBJECTIVE_ARG_2_INCREASE) |
    (1 << WIDX_OBJECTIVE_ARG_2_DECREASE) |
    (1 << WIDX_PARK_NAME) |
    (1 << WIDX_SCENARIO_NAME) |
    (1 << WIDX_CATEGORY) |
    (1 << WIDX_CATEGORY_DROPDOWN) |
    (1 << WIDX_DETAILS),

    (1 << WIDX_CLOSE) |
    (1 << WIDX_TAB_1) |
    (1 << WIDX_TAB_2)
};

static uint64_t window_editor_objective_options_page_hold_down_widgets[] = {
    (1 << WIDX_OBJECTIVE_ARG_1_INCREASE) |
    (1 << WIDX_OBJECTIVE_ARG_1_DECREASE) |
    (1 << WIDX_OBJECTIVE_ARG_2_INCREASE) |
    (1 << WIDX_OBJECTIVE_ARG_2_DECREASE),

    0
};
// clang-format on

#pragma endregion

static void window_editor_objective_options_update_disabled_widgets(rct_window* w);

/**
 *
 *  rct2: 0x0067137D
 */
rct_window* window_editor_objective_options_open()
{
    rct_window* w;

    w = window_bring_to_front_by_class(WC_EDTIOR_OBJECTIVE_OPTIONS);
    if (w != nullptr)
        return w;

    w = window_create_centred(450, 228, &window_objective_options_main_events, WC_EDTIOR_OBJECTIVE_OPTIONS, WF_10);
    w->widgets = window_editor_objective_options_main_widgets;
    w->enabled_widgets = window_editor_objective_options_page_enabled_widgets[WINDOW_EDITOR_OBJECTIVE_OPTIONS_PAGE_MAIN];
    w->pressed_widgets = 0;
    w->hold_down_widgets = window_editor_objective_options_page_hold_down_widgets[WINDOW_EDITOR_OBJECTIVE_OPTIONS_PAGE_MAIN];
    window_init_scroll_widgets(w);
    w->selected_tab = WINDOW_EDITOR_OBJECTIVE_OPTIONS_PAGE_MAIN;
    w->no_list_items = 0;
    w->selected_list_item = -1;
    window_editor_objective_options_update_disabled_widgets(w);

    return w;
}

static void window_editor_objective_options_set_pressed_tab(rct_window* w)
{
    int32_t i;
    for (i = 0; i < 2; i++)
        w->pressed_widgets &= ~(1 << (WIDX_TAB_1 + i));
    w->pressed_widgets |= 1LL << (WIDX_TAB_1 + w->page);
}

static void window_editor_objective_options_anchor_border_widgets(rct_window* w)
{
    w->widgets[WIDX_BACKGROUND].right = w->width - 1;
    w->widgets[WIDX_BACKGROUND].bottom = w->height - 1;
    w->widgets[WIDX_PAGE_BACKGROUND].right = w->width - 1;
    w->widgets[WIDX_PAGE_BACKGROUND].bottom = w->height - 1;
    w->widgets[WIDX_TITLE].right = w->width - 2;
    w->widgets[WIDX_CLOSE].left = w->width - 13;
    w->widgets[WIDX_CLOSE].right = w->width - 3;
}

static void window_editor_objective_options_draw_tab_images(rct_window* w, rct_drawpixelinfo* dpi)
{
    rct_widget* widget;
    int32_t spriteIndex;

    // Tab 1
    widget = &w->widgets[WIDX_TAB_1];

    spriteIndex = SPR_TAB_OBJECTIVE_0;
    if (w->page == WINDOW_EDITOR_OBJECTIVE_OPTIONS_PAGE_MAIN)
        spriteIndex += (w->frame_no / 4) % 16;

    gfx_draw_sprite(dpi, spriteIndex, w->windowPos + ScreenCoordsXY{ widget->left, widget->top }, 0);

    // Tab 2
    if (!(w->disabled_widgets & (1 << WIDX_TAB_2)))
    {
        widget = &w->widgets[WIDX_TAB_2];
        spriteIndex = SPR_TAB_RIDE_0;
        if (w->page == WINDOW_EDITOR_OBJECTIVE_OPTIONS_PAGE_RIDES)
            spriteIndex += (w->frame_no / 4) % 16;

        gfx_draw_sprite(dpi, spriteIndex, w->windowPos + ScreenCoordsXY{ widget->left, widget->top }, 0);
    }
}

/**
 *
 *  rct2: 0x00668496
 */
static void window_editor_objective_options_set_page(rct_window* w, int32_t page)
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
    w->Invalidate();
    window_editor_objective_options_update_disabled_widgets(w);
    window_event_resize_call(w);
    window_event_invalidate_call(w);
    window_init_scroll_widgets(w);
    w->Invalidate();
}

/**
 *
 *  rct2: 0x0067201D
 */
static void window_editor_objective_options_set_objective(rct_window* w, int32_t objective)
{
    gScenarioObjectiveType = objective;
    w->Invalidate();

    // Set default objective arguments
    switch (objective)
    {
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
            gScenarioObjectiveCurrency = MONEY(50000, 00);
            break;
        case OBJECTIVE_GUESTS_AND_RATING:
            gScenarioObjectiveNumGuests = 2000;
            break;
        case OBJECTIVE_MONTHLY_RIDE_INCOME:
            gScenarioObjectiveCurrency = MONEY(10000, 00);
            break;
        case OBJECTIVE_10_ROLLERCOASTERS_LENGTH:
            gScenarioObjectiveNumGuests = 1200;
            break;
        case OBJECTIVE_FINISH_5_ROLLERCOASTERS:
            gScenarioObjectiveCurrency = FIXED_2DP(6, 70);
            break;
        case OBJECTIVE_REPLAY_LOAN_AND_PARK_VALUE:
            gScenarioObjectiveCurrency = MONEY(50000, 00);
            break;
        case OBJECTIVE_MONTHLY_FOOD_INCOME:
            gScenarioObjectiveCurrency = MONEY(1000, 00);
            break;
    }
}

/**
 *
 *  rct2: 0x006719CA
 */
static void window_editor_objective_options_main_mouseup(rct_window* w, rct_widgetindex widgetIndex)
{
    switch (widgetIndex)
    {
        case WIDX_CLOSE:
            window_close(w);
            break;
        case WIDX_TAB_1:
        case WIDX_TAB_2:
            window_editor_objective_options_set_page(w, widgetIndex - WIDX_TAB_1);
            break;
        case WIDX_PARK_NAME:
        {
            auto& park = OpenRCT2::GetContext()->GetGameState()->GetPark();
            window_text_input_raw_open(w, WIDX_PARK_NAME, STR_PARK_NAME, STR_ENTER_PARK_NAME, park.Name.c_str(), 32);
            break;
        }
        case WIDX_SCENARIO_NAME:
            window_text_input_raw_open(w, WIDX_SCENARIO_NAME, STR_SCENARIO_NAME, STR_ENTER_SCENARIO_NAME, gS6Info.name, 64);
            break;
        case WIDX_DETAILS:
            window_text_input_raw_open(
                w, WIDX_DETAILS, STR_PARK_SCENARIO_DETAILS, STR_ENTER_SCENARIO_DESCRIPTION, gS6Info.details, 256);
            break;
    }
}

/**
 *
 *  rct2: 0x00672254
 */
static void window_editor_objective_options_main_resize(rct_window* w)
{
    window_set_resize(w, 450, 229, 450, 229);
}

static void window_editor_objective_options_show_objective_dropdown(rct_window* w)
{
    int32_t numItems = 0, objectiveType;
    rct_widget* dropdownWidget;
    uint32_t parkFlags;

    dropdownWidget = &w->widgets[WIDX_OBJECTIVE];
    parkFlags = gParkFlags;

    for (auto i = 0; i < OBJECTIVE_COUNT; i++)
    {
        if (i == OBJECTIVE_NONE || i == OBJECTIVE_BUILD_THE_BEST)
            continue;

        const bool objectiveAllowedByMoneyUsage = !(parkFlags & PARK_FLAGS_NO_MONEY_SCENARIO) || !ObjectiveNeedsMoney(i);
        // This objective can only work if the player can ask money for rides.
        const bool objectiveAllowedByPaymentSettings = (i != OBJECTIVE_MONTHLY_RIDE_INCOME) || park_ride_prices_unlocked();
        if (objectiveAllowedByMoneyUsage && objectiveAllowedByPaymentSettings)
        {
            gDropdownItemsFormat[numItems] = STR_DROPDOWN_MENU_LABEL;
            gDropdownItemsArgs[numItems] = ObjectiveDropdownOptionNames[i];
            numItems++;
        }
    }

    window_dropdown_show_text_custom_width(
        { w->windowPos.x + dropdownWidget->left, w->windowPos.y + dropdownWidget->top }, dropdownWidget->height() + 1,
        w->colours[1], 0, DROPDOWN_FLAG_STAY_OPEN, numItems, dropdownWidget->width() - 3);

    objectiveType = gScenarioObjectiveType;
    for (int32_t j = 0; j < numItems; j++)
    {
        if (gDropdownItemsArgs[j] - STR_OBJECTIVE_DROPDOWN_NONE == objectiveType)
        {
            dropdown_set_checked(j, true);
            break;
        }
    }
}

static void window_editor_objective_options_show_category_dropdown(rct_window* w)
{
    int32_t i;
    rct_widget* dropdownWidget;

    dropdownWidget = &w->widgets[WIDX_CATEGORY];

    for (i = SCENARIO_CATEGORY_BEGINNER; i <= SCENARIO_CATEGORY_OTHER; i++)
    {
        gDropdownItemsFormat[i] = STR_DROPDOWN_MENU_LABEL;
        gDropdownItemsArgs[i] = ScenarioCategoryStringIds[i];
    }
    window_dropdown_show_text_custom_width(
        { w->windowPos.x + dropdownWidget->left, w->windowPos.y + dropdownWidget->top }, dropdownWidget->height() + 1,
        w->colours[1], 0, DROPDOWN_FLAG_STAY_OPEN, 5, dropdownWidget->width() - 3);
    dropdown_set_checked(gS6Info.category, true);
}

static void window_editor_objective_options_arg_1_increase(rct_window* w)
{
    switch (gScenarioObjectiveType)
    {
        case OBJECTIVE_PARK_VALUE_BY:
        case OBJECTIVE_MONTHLY_RIDE_INCOME:
        case OBJECTIVE_REPLAY_LOAN_AND_PARK_VALUE:
            if (gScenarioObjectiveCurrency >= MONEY(2000000, 00))
            {
                context_show_error(STR_CANT_INCREASE_FURTHER, STR_NONE);
            }
            else
            {
                gScenarioObjectiveCurrency += MONEY(1000, 0);
                w->Invalidate();
            }
            break;
        case OBJECTIVE_MONTHLY_FOOD_INCOME:
            if (gScenarioObjectiveCurrency >= MONEY(2000000, 00))
            {
                context_show_error(STR_CANT_INCREASE_FURTHER, STR_NONE);
            }
            else
            {
                gScenarioObjectiveCurrency += MONEY(100, 0);
                w->Invalidate();
            }
            break;
        case OBJECTIVE_10_ROLLERCOASTERS_LENGTH:
            if (gScenarioObjectiveNumGuests >= 5000)
            {
                context_show_error(STR_CANT_INCREASE_FURTHER, STR_NONE);
            }
            else
            {
                gScenarioObjectiveNumGuests += 100;
                w->Invalidate();
            }
            break;
        case OBJECTIVE_FINISH_5_ROLLERCOASTERS:
            if (gScenarioObjectiveCurrency >= FIXED_2DP(9, 90))
            {
                context_show_error(STR_CANT_INCREASE_FURTHER, STR_NONE);
            }
            else
            {
                gScenarioObjectiveCurrency += FIXED_2DP(0, 10);
                w->Invalidate();
            }
            break;
        default:
            if (gScenarioObjectiveNumGuests >= 5000)
            {
                context_show_error(STR_CANT_INCREASE_FURTHER, STR_NONE);
            }
            else
            {
                gScenarioObjectiveNumGuests += 50;
                w->Invalidate();
            }
            break;
    }
}

static void window_editor_objective_options_arg_1_decrease(rct_window* w)
{
    switch (gScenarioObjectiveType)
    {
        case OBJECTIVE_PARK_VALUE_BY:
        case OBJECTIVE_MONTHLY_RIDE_INCOME:
        case OBJECTIVE_REPLAY_LOAN_AND_PARK_VALUE:
            if (gScenarioObjectiveCurrency <= MONEY(1000, 00))
            {
                context_show_error(STR_CANT_REDUCE_FURTHER, STR_NONE);
            }
            else
            {
                gScenarioObjectiveCurrency -= MONEY(1000, 0);
                w->Invalidate();
            }
            break;
        case OBJECTIVE_MONTHLY_FOOD_INCOME:
            if (gScenarioObjectiveCurrency <= MONEY(1000, 00))
            {
                context_show_error(STR_CANT_REDUCE_FURTHER, STR_NONE);
            }
            else
            {
                gScenarioObjectiveCurrency -= MONEY(100, 0);
                w->Invalidate();
            }
            break;
        case OBJECTIVE_10_ROLLERCOASTERS_LENGTH:
            if (gScenarioObjectiveNumGuests <= 1000)
            {
                context_show_error(STR_CANT_REDUCE_FURTHER, STR_NONE);
            }
            else
            {
                gScenarioObjectiveNumGuests -= 100;
                w->Invalidate();
            }
            break;
        case OBJECTIVE_FINISH_5_ROLLERCOASTERS:
            if (gScenarioObjectiveCurrency <= FIXED_2DP(4, 00))
            {
                context_show_error(STR_CANT_REDUCE_FURTHER, STR_NONE);
            }
            else
            {
                gScenarioObjectiveCurrency -= FIXED_2DP(0, 10);
                w->Invalidate();
            }
            break;
        default:
            if (gScenarioObjectiveNumGuests <= 250)
            {
                context_show_error(STR_CANT_REDUCE_FURTHER, STR_NONE);
            }
            else
            {
                gScenarioObjectiveNumGuests -= 50;
                w->Invalidate();
            }
            break;
    }
}

static void window_editor_objective_options_arg_2_increase(rct_window* w)
{
    if (gScenarioObjectiveYear >= 25)
    {
        context_show_error(STR_CANT_INCREASE_FURTHER, STR_NONE);
    }
    else
    {
        gScenarioObjectiveYear++;
        w->Invalidate();
    }
}

static void window_editor_objective_options_arg_2_decrease(rct_window* w)
{
    if (gScenarioObjectiveYear <= 1)
    {
        context_show_error(STR_CANT_REDUCE_FURTHER, STR_NONE);
    }
    else
    {
        gScenarioObjectiveYear--;
        w->Invalidate();
    }
}

/**
 *
 *  rct2: 0x00671A0D
 */
static void window_editor_objective_options_main_mousedown(rct_window* w, rct_widgetindex widgetIndex, rct_widget* widget)
{
    switch (widgetIndex)
    {
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
        case WIDX_CATEGORY_DROPDOWN:
            window_editor_objective_options_show_category_dropdown(w);
            break;
    }
}

/**
 *
 *  rct2: 0x00671A54
 */
static void window_editor_objective_options_main_dropdown(rct_window* w, rct_widgetindex widgetIndex, int32_t dropdownIndex)
{
    uint8_t newObjectiveType;

    if (dropdownIndex == -1)
        return;

    switch (widgetIndex)
    {
        case WIDX_OBJECTIVE_DROPDOWN:
            // TODO: Don't rely on string ID order
            newObjectiveType = static_cast<uint8_t>(gDropdownItemsArgs[dropdownIndex] - STR_OBJECTIVE_DROPDOWN_NONE);
            if (gScenarioObjectiveType != newObjectiveType)
                window_editor_objective_options_set_objective(w, newObjectiveType);
            break;
        case WIDX_CATEGORY_DROPDOWN:
            if (gS6Info.category != static_cast<uint8_t>(dropdownIndex))
            {
                gS6Info.category = static_cast<uint8_t>(dropdownIndex);
                w->Invalidate();
            }
            break;
    }
}

/**
 *
 *  rct2: 0x006721E7
 */
static void window_editor_objective_options_main_update(rct_window* w)
{
    uint32_t parkFlags;
    uint8_t objectiveType;

    w->frame_no++;
    window_event_invalidate_call(w);
    widget_invalidate(w, WIDX_TAB_1);

    parkFlags = gParkFlags;
    objectiveType = gScenarioObjectiveType;

    // Check if objective is allowed by money and pay-per-ride settings.
    const bool objectiveAllowedByMoneyUsage = !(parkFlags & PARK_FLAGS_NO_MONEY_SCENARIO)
        || !ObjectiveNeedsMoney(objectiveType);
    // This objective can only work if the player can ask money for rides.
    const bool objectiveAllowedByPaymentSettings = (objectiveType != OBJECTIVE_MONTHLY_RIDE_INCOME)
        || park_ride_prices_unlocked();
    if (!objectiveAllowedByMoneyUsage || !objectiveAllowedByPaymentSettings)
    {
        // Reset objective
        window_editor_objective_options_set_objective(w, OBJECTIVE_GUESTS_AND_RATING);
    }
}

/**
 *
 *  rct2: 0x00671A73
 */
static void window_editor_objective_options_main_textinput(rct_window* w, rct_widgetindex widgetIndex, char* text)
{
    if (text == nullptr)
        return;

    switch (widgetIndex)
    {
        case WIDX_PARK_NAME:
        {
            auto action = ParkSetNameAction(text);
            GameActions::Execute(&action);

            if (gS6Info.name[0] == '\0')
            {
                auto& park = OpenRCT2::GetContext()->GetGameState()->GetPark();
                String::Set(gS6Info.name, sizeof(gS6Info.name), park.Name.c_str());
            }
            break;
        }
        case WIDX_SCENARIO_NAME:
            safe_strcpy(gS6Info.name, text, std::size(gS6Info.name));
            w->Invalidate();
            break;
        case WIDX_DETAILS:
            safe_strcpy(gS6Info.details, text, std::size(gS6Info.details));
            w->Invalidate();
            break;
    }
}

/**
 *
 *  rct2: 0x0067161C
 */
static void window_editor_objective_options_main_invalidate(rct_window* w)
{
    auto widgets = window_editor_objective_options_widgets[w->page];
    if (w->widgets != widgets)
    {
        w->widgets = widgets;
        window_init_scroll_widgets(w);
    }

    window_editor_objective_options_set_pressed_tab(w);

    switch (gScenarioObjectiveType)
    {
        case OBJECTIVE_GUESTS_BY:
        case OBJECTIVE_PARK_VALUE_BY:
            window_editor_objective_options_main_widgets[WIDX_OBJECTIVE_ARG_1].type = WWT_SPINNER;
            window_editor_objective_options_main_widgets[WIDX_OBJECTIVE_ARG_1_INCREASE].type = WWT_BUTTON;
            window_editor_objective_options_main_widgets[WIDX_OBJECTIVE_ARG_1_DECREASE].type = WWT_BUTTON;
            window_editor_objective_options_main_widgets[WIDX_OBJECTIVE_ARG_2].type = WWT_SPINNER;
            window_editor_objective_options_main_widgets[WIDX_OBJECTIVE_ARG_2_INCREASE].type = WWT_BUTTON;
            window_editor_objective_options_main_widgets[WIDX_OBJECTIVE_ARG_2_DECREASE].type = WWT_BUTTON;
            break;
        case OBJECTIVE_GUESTS_AND_RATING:
        case OBJECTIVE_MONTHLY_RIDE_INCOME:
        case OBJECTIVE_10_ROLLERCOASTERS_LENGTH:
        case OBJECTIVE_FINISH_5_ROLLERCOASTERS:
        case OBJECTIVE_REPLAY_LOAN_AND_PARK_VALUE:
        case OBJECTIVE_MONTHLY_FOOD_INCOME:
            window_editor_objective_options_main_widgets[WIDX_OBJECTIVE_ARG_1].type = WWT_SPINNER;
            window_editor_objective_options_main_widgets[WIDX_OBJECTIVE_ARG_1_INCREASE].type = WWT_BUTTON;
            window_editor_objective_options_main_widgets[WIDX_OBJECTIVE_ARG_1_DECREASE].type = WWT_BUTTON;
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

    window_editor_objective_options_main_widgets[WIDX_CLOSE].type = (gScreenFlags & SCREEN_FLAGS_SCENARIO_EDITOR)
        ? WWT_EMPTY
        : WWT_CLOSEBOX;

    window_editor_objective_options_anchor_border_widgets(w);
}

/**
 *
 *  rct2: 0x0067161C
 */
static void window_editor_objective_options_main_paint(rct_window* w, rct_drawpixelinfo* dpi)
{
    int32_t width;
    rct_string_id stringId;
    uint32_t arg;

    window_draw_widgets(w, dpi);
    window_editor_objective_options_draw_tab_images(w, dpi);

    // Objective label
    auto screenCoords = w->windowPos + ScreenCoordsXY{ 8, w->widgets[WIDX_OBJECTIVE].top };
    gfx_draw_string_left(dpi, STR_OBJECTIVE_WINDOW, nullptr, COLOUR_BLACK, screenCoords);

    // Objective value
    screenCoords = w->windowPos + ScreenCoordsXY{ w->widgets[WIDX_OBJECTIVE].left + 1, w->widgets[WIDX_OBJECTIVE].top };
    stringId = ObjectiveDropdownOptionNames[gScenarioObjectiveType];
    gfx_draw_string_left(dpi, STR_WINDOW_COLOUR_2_STRINGID, &stringId, COLOUR_BLACK, screenCoords);

    if (w->widgets[WIDX_OBJECTIVE_ARG_1].type != WWT_EMPTY)
    {
        // Objective argument 1 label
        screenCoords = w->windowPos + ScreenCoordsXY{ 28, w->widgets[WIDX_OBJECTIVE_ARG_1].top };
        switch (gScenarioObjectiveType)
        {
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
        gfx_draw_string_left(dpi, stringId, nullptr, COLOUR_BLACK, screenCoords);

        // Objective argument 1 value
        screenCoords = w->windowPos
            + ScreenCoordsXY{ w->widgets[WIDX_OBJECTIVE_ARG_1].left + 1, w->widgets[WIDX_OBJECTIVE_ARG_1].top };
        switch (gScenarioObjectiveType)
        {
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
        gfx_draw_string_left(dpi, stringId, &arg, COLOUR_BLACK, screenCoords);
    }

    if (w->widgets[WIDX_OBJECTIVE_ARG_2].type != WWT_EMPTY)
    {
        // Objective argument 2 label
        screenCoords = w->windowPos + ScreenCoordsXY{ 28, w->widgets[WIDX_OBJECTIVE_ARG_2].top };
        gfx_draw_string_left(dpi, STR_WINDOW_OBJECTIVE_DATE, nullptr, COLOUR_BLACK, screenCoords);

        // Objective argument 2 value
        screenCoords = w->windowPos
            + ScreenCoordsXY{ w->widgets[WIDX_OBJECTIVE_ARG_2].left + 1, w->widgets[WIDX_OBJECTIVE_ARG_2].top };
        arg = (gScenarioObjectiveYear * MONTH_COUNT) - 1;
        gfx_draw_string_left(dpi, STR_WINDOW_OBJECTIVE_VALUE_DATE, &arg, COLOUR_BLACK, screenCoords);
    }

    // Park name
    screenCoords = w->windowPos + ScreenCoordsXY{ 8, w->widgets[WIDX_PARK_NAME].top };
    width = w->widgets[WIDX_PARK_NAME].left - 16;

    {
        auto& park = OpenRCT2::GetContext()->GetGameState()->GetPark();
        auto parkName = park.Name.c_str();

        auto ft = Formatter::Common();
        ft.Add<rct_string_id>(STR_STRING);
        ft.Add<const char*>(parkName);
        gfx_draw_string_left_clipped(dpi, STR_WINDOW_PARK_NAME, gCommonFormatArgs, COLOUR_BLACK, screenCoords, width);
    }

    // Scenario name
    screenCoords = w->windowPos + ScreenCoordsXY{ 8, w->widgets[WIDX_SCENARIO_NAME].top };
    width = w->widgets[WIDX_SCENARIO_NAME].left - 16;

    auto ft = Formatter::Common();
    ft.Add<rct_string_id>(STR_STRING);
    ft.Add<const char*>(gS6Info.name);
    gfx_draw_string_left_clipped(dpi, STR_WINDOW_SCENARIO_NAME, gCommonFormatArgs, COLOUR_BLACK, screenCoords, width);

    // Scenario details label
    screenCoords = w->windowPos + ScreenCoordsXY{ 8, w->widgets[WIDX_DETAILS].top };
    gfx_draw_string_left(dpi, STR_WINDOW_PARK_DETAILS, nullptr, COLOUR_BLACK, screenCoords);

    // Scenario details value
    screenCoords = w->windowPos + ScreenCoordsXY{ 16, w->widgets[WIDX_DETAILS].top + 10 };
    width = w->widgets[WIDX_DETAILS].left - 4;

    ft = Formatter::Common();
    ft.Add<rct_string_id>(STR_STRING);
    ft.Add<const char*>(gS6Info.details);
    gfx_draw_string_left_wrapped(dpi, gCommonFormatArgs, screenCoords, width, STR_BLACK_STRING, COLOUR_BLACK);

    // Scenario category label
    screenCoords = w->windowPos + ScreenCoordsXY{ 8, w->widgets[WIDX_CATEGORY].top };
    gfx_draw_string_left(dpi, STR_WINDOW_SCENARIO_GROUP, nullptr, COLOUR_BLACK, screenCoords);

    // Scenario category value
    screenCoords = w->windowPos + ScreenCoordsXY{ w->widgets[WIDX_CATEGORY].left + 1, w->widgets[WIDX_CATEGORY].top };
    stringId = ScenarioCategoryStringIds[gS6Info.category];
    gfx_draw_string_left(dpi, STR_WINDOW_COLOUR_2_STRINGID, &stringId, COLOUR_BLACK, screenCoords);
}

/**
 *
 *  rct2: 0x006724A4
 */
static void window_editor_objective_options_rides_mouseup(rct_window* w, rct_widgetindex widgetIndex)
{
    switch (widgetIndex)
    {
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
static void window_editor_objective_options_rides_resize(rct_window* w)
{
    window_set_resize(w, 380, 224, 380, 224);
}

/**
 *
 *  rct2: 0x00672544
 */
static void window_editor_objective_options_rides_update(rct_window* w)
{
    w->frame_no++;
    window_event_invalidate_call(w);
    window_event_resize_call(w);
    widget_invalidate(w, WIDX_TAB_2);

    auto numItems = 0;
    for (auto& ride : GetRideManager())
    {
        if (ride.IsRide())
        {
            w->list_item_positions[numItems] = ride.id;
            numItems++;
        }
    }

    if (w->no_list_items != numItems)
    {
        w->no_list_items = numItems;
        w->Invalidate();
    }
}

/**
 *
 *  rct2: 0x006724BF
 */
static void window_editor_objective_options_rides_scrollgetheight(
    rct_window* w, int32_t scrollIndex, int32_t* width, int32_t* height)
{
    *height = w->no_list_items * 12;
}

/**
 *
 *  rct2: 0x006724FC
 */
static void window_editor_objective_options_rides_scrollmousedown(
    rct_window* w, int32_t scrollIndex, const ScreenCoordsXY& screenCoords)
{
    auto i = screenCoords.y / 12;
    if (i < 0 || i >= w->no_list_items)
        return;

    auto ride = get_ride(w->list_item_positions[i]);
    if (ride != nullptr)
    {
        ride->lifecycle_flags ^= RIDE_LIFECYCLE_INDESTRUCTIBLE;
    }
    w->Invalidate();
}

/**
 *
 *  rct2: 0x006724CC
 */
static void window_editor_objective_options_rides_scrollmouseover(
    rct_window* w, int32_t scrollIndex, const ScreenCoordsXY& screenCoords)
{
    int32_t i;

    i = screenCoords.y / 12;
    if (i < 0 || i >= w->no_list_items)
        return;

    if (w->selected_list_item != i)
    {
        w->selected_list_item = i;
        w->Invalidate();
    }
}

/**
 *
 *  rct2: 0x006722B5
 */
static void window_editor_objective_options_rides_invalidate(rct_window* w)
{
    rct_widget* widgets;

    widgets = window_editor_objective_options_widgets[w->page];
    if (w->widgets != widgets)
    {
        w->widgets = widgets;
        window_init_scroll_widgets(w);
    }

    window_editor_objective_options_set_pressed_tab(w);

    window_editor_objective_options_main_widgets[WIDX_CLOSE].type = (gScreenFlags & SCREEN_FLAGS_SCENARIO_EDITOR)
        ? WWT_EMPTY
        : WWT_CLOSEBOX;

    window_editor_objective_options_anchor_border_widgets(w);
}

/**
 *
 *  rct2: 0x00672340
 */
static void window_editor_objective_options_rides_paint(rct_window* w, rct_drawpixelinfo* dpi)
{
    window_draw_widgets(w, dpi);
    window_editor_objective_options_draw_tab_images(w, dpi);

    gfx_draw_string_left(
        dpi, STR_WINDOW_PRESERVATION_ORDER, nullptr, COLOUR_BLACK,
        w->windowPos + ScreenCoordsXY{ 6, w->widgets[WIDX_PAGE_BACKGROUND].top + 3 });
}

/**
 *
 *  rct2: 0x0067236F
 */
static void window_editor_objective_options_rides_scrollpaint(rct_window* w, rct_drawpixelinfo* dpi, int32_t scrollIndex)
{
    int32_t colour = ColourMapA[w->colours[1]].mid_light;
    gfx_fill_rect(dpi, { { dpi->x, dpi->y }, { dpi->x + dpi->width - 1, dpi->y + dpi->height - 1 } }, colour);

    for (int32_t i = 0; i < w->no_list_items; i++)
    {
        int32_t y = i * 12;

        if (y + 12 < dpi->y || y >= dpi->y + dpi->height)
            continue;

        // Checkbox
        gfx_fill_rect_inset(dpi, 2, y, 11, y + 10, w->colours[1], INSET_RECT_F_E0);

        // Highlighted
        auto stringId = STR_BLACK_STRING;
        if (i == w->selected_list_item)
        {
            stringId = STR_WINDOW_COLOUR_2_STRINGID;
            gfx_filter_rect(dpi, 0, y, w->width, y + 11, PALETTE_DARKEN_1);
        }

        // Checkbox mark
        auto ride = get_ride(w->list_item_positions[i]);
        if (ride != nullptr)
        {
            if (ride->lifecycle_flags & RIDE_LIFECYCLE_INDESTRUCTIBLE)
            {
                gCurrentFontSpriteBase = stringId == STR_WINDOW_COLOUR_2_STRINGID ? FONT_SPRITE_BASE_MEDIUM_EXTRA_DARK
                                                                                  : FONT_SPRITE_BASE_MEDIUM_DARK;
                gfx_draw_string(dpi, static_cast<const char*>(CheckBoxMarkString), w->colours[1] & 0x7F, { 2, y });
            }

            // Ride name
            uint8_t args[32]{};

            Formatter ft(args);
            ride->FormatNameTo(ft);
            gfx_draw_string_left(dpi, stringId, args, COLOUR_BLACK, { 15, y });
        }
    }
}

/**
 *
 *  rct2: 0x00672609
 */
static void window_editor_objective_options_update_disabled_widgets(rct_window* w)
{
    // Check if there are any rides (not shops or facilities)
    const auto& rideManager = GetRideManager();
    if (std::any_of(rideManager.begin(), rideManager.end(), [](const Ride& ride) { return ride.IsRide(); }))
    {
        w->disabled_widgets &= ~(1 << WIDX_TAB_2);
    }
    else
    {
        w->disabled_widgets |= (1 << WIDX_TAB_2);
    }
}
