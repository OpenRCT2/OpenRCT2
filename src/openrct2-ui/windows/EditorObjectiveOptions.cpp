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
#include <openrct2/actions/ParkSetNameAction.h>
#include <openrct2/core/String.hpp>
#include <openrct2/drawing/Drawing.h>
#include <openrct2/drawing/Font.h>
#include <openrct2/interface/Colour.h>
#include <openrct2/localisation/Date.h>
#include <openrct2/localisation/Formatter.h>
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
    MakeWidget({  0,  43}, {280, 106}, WindowWidgetType::Resize, WindowColour::Secondary), \
    MakeTab   ({  3,  17}, STR_SELECT_OBJECTIVE_AND_PARK_NAME_TIP         ), \
    MakeTab   ({ 34,  17}, STR_SELECT_RIDES_TO_BE_PRESERVED_TIP           )

static rct_widget window_editor_objective_options_main_widgets[] = {
    MAIN_OBJECTIVE_OPTIONS_WIDGETS,
    MakeWidget        ({ 98,  48}, {344,  12}, WindowWidgetType::DropdownMenu, WindowColour::Secondary, STR_NONE,           STR_SELECT_OBJECTIVE_FOR_THIS_SCENARIO_TIP     ),
    MakeWidget        ({430,  49}, { 11,  10}, WindowWidgetType::Button,   WindowColour::Secondary, STR_DROPDOWN_GLYPH, STR_SELECT_OBJECTIVE_FOR_THIS_SCENARIO_TIP     ),
    MakeSpinnerWidgets({158,  65}, {120,  12}, WindowWidgetType::Button,   WindowColour::Secondary                                                                     ), // NB: 3 widgets
    MakeSpinnerWidgets({158,  82}, {120,  12}, WindowWidgetType::Button,   WindowColour::Secondary                                                                     ), // NB: 3 widgets
    MakeWidget        ({370,  99}, { 75,  12}, WindowWidgetType::Button,   WindowColour::Secondary, STR_CHANGE,         STR_CHANGE_NAME_OF_PARK_TIP                    ),
    MakeWidget        ({370, 116}, { 75,  12}, WindowWidgetType::Button,   WindowColour::Secondary, STR_CHANGE,         STR_CHANGE_NAME_OF_SCENARIO_TIP                ),
    MakeWidget        ({ 98, 133}, {180,  12}, WindowWidgetType::DropdownMenu, WindowColour::Secondary, STR_NONE,           STR_SELECT_WHICH_GROUP_THIS_SCENARIO_APPEARS_IN),
    MakeWidget        ({266, 134}, { 11,  10}, WindowWidgetType::Button,   WindowColour::Secondary, STR_DROPDOWN_GLYPH, STR_SELECT_WHICH_GROUP_THIS_SCENARIO_APPEARS_IN),
    MakeWidget        ({370, 150}, { 75,  12}, WindowWidgetType::Button,   WindowColour::Secondary, STR_CHANGE,         STR_CHANGE_DETAIL_NOTES_ABOUT_PARK_SCENARIO_TIP),
    WIDGETS_END,
};

static rct_widget window_editor_objective_options_rides_widgets[] = {
    MAIN_OBJECTIVE_OPTIONS_WIDGETS,
    MakeWidget({  3,  60}, {374, 161}, WindowWidgetType::Scroll, WindowColour::Secondary, SCROLL_VERTICAL),
    WIDGETS_END,
};

static rct_widget *window_editor_objective_options_widgets[] = {
    window_editor_objective_options_main_widgets,
    window_editor_objective_options_rides_widgets,
};

#pragma endregion

#pragma region Events

static void WindowEditorObjectiveOptionsMainMouseup(rct_window *w, rct_widgetindex widgetIndex);
static void WindowEditorObjectiveOptionsMainResize(rct_window *w);
static void WindowEditorObjectiveOptionsMainMousedown(rct_window *w, rct_widgetindex widgetIndex, rct_widget* widget);
static void WindowEditorObjectiveOptionsMainDropdown(rct_window *w, rct_widgetindex widgetIndex, int32_t dropdownIndex);
static void WindowEditorObjectiveOptionsMainUpdate(rct_window *w);
static void WindowEditorObjectiveOptionsMainTextinput(rct_window *w, rct_widgetindex widgetIndex, char *text);
static void WindowEditorObjectiveOptionsMainInvalidate(rct_window *w);
static void WindowEditorObjectiveOptionsMainPaint(rct_window *w, rct_drawpixelinfo *dpi);

static void WindowEditorObjectiveOptionsRidesMouseup(rct_window *w, rct_widgetindex widgetIndex);
static void WindowEditorObjectiveOptionsRidesResize(rct_window *w);
static void WindowEditorObjectiveOptionsRidesUpdate(rct_window *w);
static void WindowEditorObjectiveOptionsRidesScrollgetheight(rct_window *w, int32_t scrollIndex, int32_t *width, int32_t *height);
static void WindowEditorObjectiveOptionsRidesScrollmousedown(rct_window *w, int32_t scrollIndex, const ScreenCoordsXY& screenCoords);
static void WindowEditorObjectiveOptionsRidesScrollmouseover(rct_window *w, int32_t scrollIndex, const ScreenCoordsXY& screenCoords);
static void WindowEditorObjectiveOptionsRidesInvalidate(rct_window *w);
static void WindowEditorObjectiveOptionsRidesPaint(rct_window *w, rct_drawpixelinfo *dpi);
static void WindowEditorObjectiveOptionsRidesScrollpaint(rct_window *w, rct_drawpixelinfo *dpi, int32_t scrollIndex);

// 0x009A9DF4
static rct_window_event_list window_objective_options_main_events([](auto& events)
{
    events.mouse_up = &WindowEditorObjectiveOptionsMainMouseup;
    events.resize = &WindowEditorObjectiveOptionsMainResize;
    events.mouse_down = &WindowEditorObjectiveOptionsMainMousedown;
    events.dropdown = &WindowEditorObjectiveOptionsMainDropdown;
    events.update = &WindowEditorObjectiveOptionsMainUpdate;
    events.text_input = &WindowEditorObjectiveOptionsMainTextinput;
    events.invalidate = &WindowEditorObjectiveOptionsMainInvalidate;
    events.paint = &WindowEditorObjectiveOptionsMainPaint;
});

// 0x009A9F58
static rct_window_event_list window_objective_options_rides_events([](auto& events)
{
    events.mouse_up = &WindowEditorObjectiveOptionsRidesMouseup;
    events.resize = &WindowEditorObjectiveOptionsRidesResize;
    events.update = &WindowEditorObjectiveOptionsRidesUpdate;
    events.get_scroll_size = &WindowEditorObjectiveOptionsRidesScrollgetheight;
    events.scroll_mousedown = &WindowEditorObjectiveOptionsRidesScrollmousedown;
    events.scroll_mouseover = &WindowEditorObjectiveOptionsRidesScrollmouseover;
    events.invalidate = &WindowEditorObjectiveOptionsRidesInvalidate;
    events.paint = &WindowEditorObjectiveOptionsRidesPaint;
    events.scroll_paint = &WindowEditorObjectiveOptionsRidesScrollpaint;
});

static rct_window_event_list *window_editor_objective_options_page_events[] = {
    &window_objective_options_main_events,
    &window_objective_options_rides_events,
};

#pragma endregion

#pragma region Enabled widgets

static uint64_t window_editor_objective_options_page_hold_down_widgets[] = {
    (1ULL << WIDX_OBJECTIVE_ARG_1_INCREASE) |
    (1ULL << WIDX_OBJECTIVE_ARG_1_DECREASE) |
    (1ULL << WIDX_OBJECTIVE_ARG_2_INCREASE) |
    (1ULL << WIDX_OBJECTIVE_ARG_2_DECREASE),

    0,
};
// clang-format on

#pragma endregion

static void WindowEditorObjectiveOptionsUpdateDisabledWidgets(rct_window* w);

/**
 *
 *  rct2: 0x0067137D
 */
rct_window* WindowEditorObjectiveOptionsOpen()
{
    rct_window* w;

    w = window_bring_to_front_by_class(WC_EDITOR_OBJECTIVE_OPTIONS);
    if (w != nullptr)
        return w;

    w = WindowCreateCentred(450, 228, &window_objective_options_main_events, WC_EDITOR_OBJECTIVE_OPTIONS, WF_10);
    w->widgets = window_editor_objective_options_main_widgets;
    w->pressed_widgets = 0;
    w->hold_down_widgets = window_editor_objective_options_page_hold_down_widgets[WINDOW_EDITOR_OBJECTIVE_OPTIONS_PAGE_MAIN];
    WindowInitScrollWidgets(w);
    w->selected_tab = WINDOW_EDITOR_OBJECTIVE_OPTIONS_PAGE_MAIN;
    w->no_list_items = 0;
    w->selected_list_item = -1;
    WindowEditorObjectiveOptionsUpdateDisabledWidgets(w);

    return w;
}

static void WindowEditorObjectiveOptionsSetPressedTab(rct_window* w)
{
    int32_t i;
    for (i = 0; i < 2; i++)
        w->pressed_widgets &= ~(1 << (WIDX_TAB_1 + i));
    w->pressed_widgets |= 1LL << (WIDX_TAB_1 + w->page);
}

static void WindowEditorObjectiveOptionsAnchorBorderWidgets(rct_window* w)
{
    w->widgets[WIDX_BACKGROUND].right = w->width - 1;
    w->widgets[WIDX_BACKGROUND].bottom = w->height - 1;
    w->widgets[WIDX_PAGE_BACKGROUND].right = w->width - 1;
    w->widgets[WIDX_PAGE_BACKGROUND].bottom = w->height - 1;
    w->widgets[WIDX_TITLE].right = w->width - 2;
    w->widgets[WIDX_CLOSE].left = w->width - 13;
    w->widgets[WIDX_CLOSE].right = w->width - 3;
}

static void WindowEditorObjectiveOptionsDrawTabImages(rct_window* w, rct_drawpixelinfo* dpi)
{
    rct_widget* widget;
    int32_t spriteIndex;

    // Tab 1
    widget = &w->widgets[WIDX_TAB_1];

    spriteIndex = SPR_TAB_OBJECTIVE_0;
    if (w->page == WINDOW_EDITOR_OBJECTIVE_OPTIONS_PAGE_MAIN)
        spriteIndex += (w->frame_no / 4) % 16;

    gfx_draw_sprite(dpi, ImageId(spriteIndex), w->windowPos + ScreenCoordsXY{ widget->left, widget->top });

    // Tab 2
    if (!WidgetIsDisabled(w, WIDX_TAB_2))
    {
        widget = &w->widgets[WIDX_TAB_2];
        spriteIndex = SPR_TAB_RIDE_0;
        if (w->page == WINDOW_EDITOR_OBJECTIVE_OPTIONS_PAGE_RIDES)
            spriteIndex += (w->frame_no / 4) % 16;

        gfx_draw_sprite(dpi, ImageId(spriteIndex), w->windowPos + ScreenCoordsXY{ widget->left, widget->top });
    }
}

/**
 *
 *  rct2: 0x00668496
 */
static void WindowEditorObjectiveOptionsSetPage(rct_window* w, int32_t page)
{
    if (w->page == page)
        return;

    w->page = page;
    w->frame_no = 0;
    w->var_492 = 0;
    w->no_list_items = 0;
    w->selected_list_item = -1;
    w->hold_down_widgets = window_editor_objective_options_page_hold_down_widgets[page];
    w->event_handlers = window_editor_objective_options_page_events[page];
    w->widgets = window_editor_objective_options_widgets[page];
    w->Invalidate();
    WindowEditorObjectiveOptionsUpdateDisabledWidgets(w);
    window_event_resize_call(w);
    window_event_invalidate_call(w);
    WindowInitScrollWidgets(w);
    w->Invalidate();
}

/**
 *
 *  rct2: 0x0067201D
 */
static void WindowEditorObjectiveOptionsSetObjective(rct_window* w, int32_t objective)
{
    gScenarioObjective.Type = objective;
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
            gScenarioObjective.Year = 3;
            gScenarioObjective.NumGuests = 1500;
            break;
        case OBJECTIVE_PARK_VALUE_BY:
            gScenarioObjective.Year = 3;
            gScenarioObjective.Currency = 50000.00_GBP;
            break;
        case OBJECTIVE_GUESTS_AND_RATING:
            gScenarioObjective.NumGuests = 2000;
            break;
        case OBJECTIVE_MONTHLY_RIDE_INCOME:
            gScenarioObjective.Currency = 10000.00_GBP;
            break;
        case OBJECTIVE_10_ROLLERCOASTERS_LENGTH:
            gScenarioObjective.MinimumLength = 1200;
            break;
        case OBJECTIVE_FINISH_5_ROLLERCOASTERS:
            gScenarioObjective.MinimumExcitement = FIXED_2DP(6, 70);
            break;
        case OBJECTIVE_REPAY_LOAN_AND_PARK_VALUE:
            gScenarioObjective.Currency = 50000.00_GBP;
            break;
        case OBJECTIVE_MONTHLY_FOOD_INCOME:
            gScenarioObjective.Currency = 1000.00_GBP;
            break;
    }
}

/**
 *
 *  rct2: 0x006719CA
 */
static void WindowEditorObjectiveOptionsMainMouseup(rct_window* w, rct_widgetindex widgetIndex)
{
    switch (widgetIndex)
    {
        case WIDX_CLOSE:
            window_close(w);
            break;
        case WIDX_TAB_1:
        case WIDX_TAB_2:
            WindowEditorObjectiveOptionsSetPage(w, widgetIndex - WIDX_TAB_1);
            break;
        case WIDX_PARK_NAME:
        {
            auto& park = OpenRCT2::GetContext()->GetGameState()->GetPark();
            WindowTextInputRawOpen(w, WIDX_PARK_NAME, STR_PARK_NAME, STR_ENTER_PARK_NAME, {}, park.Name.c_str(), 32);
            break;
        }
        case WIDX_SCENARIO_NAME:
            WindowTextInputRawOpen(
                w, WIDX_SCENARIO_NAME, STR_SCENARIO_NAME, STR_ENTER_SCENARIO_NAME, {}, gScenarioName.c_str(), 64);
            break;
        case WIDX_DETAILS:
            WindowTextInputRawOpen(
                w, WIDX_DETAILS, STR_PARK_SCENARIO_DETAILS, STR_ENTER_SCENARIO_DESCRIPTION, {}, gScenarioDetails.c_str(), 256);
            break;
    }
}

/**
 *
 *  rct2: 0x00672254
 */
static void WindowEditorObjectiveOptionsMainResize(rct_window* w)
{
    window_set_resize(w, 450, 229, 450, 229);
}

static void WindowEditorObjectiveOptionsShowObjectiveDropdown(rct_window* w)
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
            gDropdownItems[numItems].Format = STR_DROPDOWN_MENU_LABEL;
            gDropdownItems[numItems].Args = ObjectiveDropdownOptionNames[i];
            numItems++;
        }
    }

    WindowDropdownShowTextCustomWidth(
        { w->windowPos.x + dropdownWidget->left, w->windowPos.y + dropdownWidget->top }, dropdownWidget->height() + 1,
        w->colours[1], 0, Dropdown::Flag::StayOpen, numItems, dropdownWidget->width() - 3);

    objectiveType = gScenarioObjective.Type;
    for (int32_t j = 0; j < numItems; j++)
    {
        if (gDropdownItems[j].Args - STR_OBJECTIVE_DROPDOWN_NONE == objectiveType)
        {
            Dropdown::SetChecked(j, true);
            break;
        }
    }
}

static void WindowEditorObjectiveOptionsShowCategoryDropdown(rct_window* w)
{
    int32_t i;
    rct_widget* dropdownWidget;

    dropdownWidget = &w->widgets[WIDX_CATEGORY];

    for (i = SCENARIO_CATEGORY_BEGINNER; i <= SCENARIO_CATEGORY_OTHER; i++)
    {
        gDropdownItems[i].Format = STR_DROPDOWN_MENU_LABEL;
        gDropdownItems[i].Args = ScenarioCategoryStringIds[i];
    }
    WindowDropdownShowTextCustomWidth(
        { w->windowPos.x + dropdownWidget->left, w->windowPos.y + dropdownWidget->top }, dropdownWidget->height() + 1,
        w->colours[1], 0, Dropdown::Flag::StayOpen, 5, dropdownWidget->width() - 3);
    Dropdown::SetChecked(gScenarioCategory, true);
}

static void WindowEditorObjectiveOptionsArg1Increase(rct_window* w)
{
    switch (gScenarioObjective.Type)
    {
        case OBJECTIVE_PARK_VALUE_BY:
        case OBJECTIVE_MONTHLY_RIDE_INCOME:
        case OBJECTIVE_REPAY_LOAN_AND_PARK_VALUE:
            if (gScenarioObjective.Currency >= 2000000.00_GBP)
            {
                context_show_error(STR_CANT_INCREASE_FURTHER, STR_NONE, {});
            }
            else
            {
                gScenarioObjective.Currency += 1000.00_GBP;
                w->Invalidate();
            }
            break;
        case OBJECTIVE_MONTHLY_FOOD_INCOME:
            if (gScenarioObjective.Currency >= 2000000.00_GBP)
            {
                context_show_error(STR_CANT_INCREASE_FURTHER, STR_NONE, {});
            }
            else
            {
                gScenarioObjective.Currency += 100.00_GBP;
                w->Invalidate();
            }
            break;
        case OBJECTIVE_10_ROLLERCOASTERS_LENGTH:
            if (gScenarioObjective.MinimumLength >= 5000)
            {
                context_show_error(STR_CANT_INCREASE_FURTHER, STR_NONE, {});
            }
            else
            {
                gScenarioObjective.MinimumLength += 100;
                w->Invalidate();
            }
            break;
        case OBJECTIVE_FINISH_5_ROLLERCOASTERS:
            if (gScenarioObjective.MinimumExcitement >= FIXED_2DP(9, 90))
            {
                context_show_error(STR_CANT_INCREASE_FURTHER, STR_NONE, {});
            }
            else
            {
                gScenarioObjective.MinimumExcitement += FIXED_2DP(0, 10);
                w->Invalidate();
            }
            break;
        default:
            if (gScenarioObjective.NumGuests >= 5000)
            {
                context_show_error(STR_CANT_INCREASE_FURTHER, STR_NONE, {});
            }
            else
            {
                gScenarioObjective.NumGuests += 50;
                w->Invalidate();
            }
            break;
    }
}

static void WindowEditorObjectiveOptionsArg1Decrease(rct_window* w)
{
    switch (gScenarioObjective.Type)
    {
        case OBJECTIVE_PARK_VALUE_BY:
        case OBJECTIVE_MONTHLY_RIDE_INCOME:
        case OBJECTIVE_REPAY_LOAN_AND_PARK_VALUE:
            if (gScenarioObjective.Currency <= 1000.00_GBP)
            {
                context_show_error(STR_CANT_REDUCE_FURTHER, STR_NONE, {});
            }
            else
            {
                gScenarioObjective.Currency -= 1000.00_GBP;
                w->Invalidate();
            }
            break;
        case OBJECTIVE_MONTHLY_FOOD_INCOME:
            if (gScenarioObjective.Currency <= 1000.00_GBP)
            {
                context_show_error(STR_CANT_REDUCE_FURTHER, STR_NONE, {});
            }
            else
            {
                gScenarioObjective.Currency -= 100.00_GBP;
                w->Invalidate();
            }
            break;
        case OBJECTIVE_10_ROLLERCOASTERS_LENGTH:
            if (gScenarioObjective.MinimumLength <= 1000)
            {
                context_show_error(STR_CANT_REDUCE_FURTHER, STR_NONE, {});
            }
            else
            {
                gScenarioObjective.MinimumLength -= 100;
                w->Invalidate();
            }
            break;
        case OBJECTIVE_FINISH_5_ROLLERCOASTERS:
            if (gScenarioObjective.MinimumExcitement <= FIXED_2DP(4, 00))
            {
                context_show_error(STR_CANT_REDUCE_FURTHER, STR_NONE, {});
            }
            else
            {
                gScenarioObjective.MinimumExcitement -= FIXED_2DP(0, 10);
                w->Invalidate();
            }
            break;
        default:
            if (gScenarioObjective.NumGuests <= 250)
            {
                context_show_error(STR_CANT_REDUCE_FURTHER, STR_NONE, {});
            }
            else
            {
                gScenarioObjective.NumGuests -= 50;
                w->Invalidate();
            }
            break;
    }
}

static void WindowEditorObjectiveOptionsArg2Increase(rct_window* w)
{
    if (gScenarioObjective.Year >= 25)
    {
        context_show_error(STR_CANT_INCREASE_FURTHER, STR_NONE, {});
    }
    else
    {
        gScenarioObjective.Year++;
        w->Invalidate();
    }
}

static void WindowEditorObjectiveOptionsArg2Decrease(rct_window* w)
{
    if (gScenarioObjective.Year <= 1)
    {
        context_show_error(STR_CANT_REDUCE_FURTHER, STR_NONE, {});
    }
    else
    {
        gScenarioObjective.Year--;
        w->Invalidate();
    }
}

/**
 *
 *  rct2: 0x00671A0D
 */
static void WindowEditorObjectiveOptionsMainMousedown(rct_window* w, rct_widgetindex widgetIndex, rct_widget* widget)
{
    switch (widgetIndex)
    {
        case WIDX_OBJECTIVE_DROPDOWN:
            WindowEditorObjectiveOptionsShowObjectiveDropdown(w);
            break;
        case WIDX_OBJECTIVE_ARG_1_INCREASE:
            WindowEditorObjectiveOptionsArg1Increase(w);
            break;
        case WIDX_OBJECTIVE_ARG_1_DECREASE:
            WindowEditorObjectiveOptionsArg1Decrease(w);
            break;
        case WIDX_OBJECTIVE_ARG_2_INCREASE:
            WindowEditorObjectiveOptionsArg2Increase(w);
            break;
        case WIDX_OBJECTIVE_ARG_2_DECREASE:
            WindowEditorObjectiveOptionsArg2Decrease(w);
            break;
        case WIDX_CATEGORY_DROPDOWN:
            WindowEditorObjectiveOptionsShowCategoryDropdown(w);
            break;
    }
}

/**
 *
 *  rct2: 0x00671A54
 */
static void WindowEditorObjectiveOptionsMainDropdown(rct_window* w, rct_widgetindex widgetIndex, int32_t dropdownIndex)
{
    uint8_t newObjectiveType;

    if (dropdownIndex == -1)
        return;

    switch (widgetIndex)
    {
        case WIDX_OBJECTIVE_DROPDOWN:
            // TODO: Don't rely on string ID order
            newObjectiveType = static_cast<uint8_t>(gDropdownItems[dropdownIndex].Args - STR_OBJECTIVE_DROPDOWN_NONE);
            if (gScenarioObjective.Type != newObjectiveType)
                WindowEditorObjectiveOptionsSetObjective(w, newObjectiveType);
            break;
        case WIDX_CATEGORY_DROPDOWN:
            if (gScenarioCategory != static_cast<uint8_t>(dropdownIndex))
            {
                gScenarioCategory = static_cast<SCENARIO_CATEGORY>(dropdownIndex);
                w->Invalidate();
            }
            break;
    }
}

/**
 *
 *  rct2: 0x006721E7
 */
static void WindowEditorObjectiveOptionsMainUpdate(rct_window* w)
{
    uint32_t parkFlags;
    uint8_t objectiveType;

    w->frame_no++;
    window_event_invalidate_call(w);
    widget_invalidate(w, WIDX_TAB_1);

    parkFlags = gParkFlags;
    objectiveType = gScenarioObjective.Type;

    // Check if objective is allowed by money and pay-per-ride settings.
    const bool objectiveAllowedByMoneyUsage = !(parkFlags & PARK_FLAGS_NO_MONEY_SCENARIO)
        || !ObjectiveNeedsMoney(objectiveType);
    // This objective can only work if the player can ask money for rides.
    const bool objectiveAllowedByPaymentSettings = (objectiveType != OBJECTIVE_MONTHLY_RIDE_INCOME)
        || park_ride_prices_unlocked();
    if (!objectiveAllowedByMoneyUsage || !objectiveAllowedByPaymentSettings)
    {
        // Reset objective
        WindowEditorObjectiveOptionsSetObjective(w, OBJECTIVE_GUESTS_AND_RATING);
    }
}

/**
 *
 *  rct2: 0x00671A73
 */
static void WindowEditorObjectiveOptionsMainTextinput(rct_window* w, rct_widgetindex widgetIndex, char* text)
{
    if (text == nullptr)
        return;

    switch (widgetIndex)
    {
        case WIDX_PARK_NAME:
        {
            auto action = ParkSetNameAction(text);
            GameActions::Execute(&action);

            if (gScenarioName.empty())
            {
                auto& park = OpenRCT2::GetContext()->GetGameState()->GetPark();
                gScenarioName = park.Name;
            }
            break;
        }
        case WIDX_SCENARIO_NAME:
            gScenarioName = text;
            w->Invalidate();
            break;
        case WIDX_DETAILS:
            gScenarioDetails = text;
            w->Invalidate();
            break;
    }
}

/**
 *
 *  rct2: 0x0067161C
 */
static void WindowEditorObjectiveOptionsMainInvalidate(rct_window* w)
{
    auto widgets = window_editor_objective_options_widgets[w->page];
    if (w->widgets != widgets)
    {
        w->widgets = widgets;
        WindowInitScrollWidgets(w);
    }

    WindowEditorObjectiveOptionsSetPressedTab(w);

    switch (gScenarioObjective.Type)
    {
        case OBJECTIVE_GUESTS_BY:
        case OBJECTIVE_PARK_VALUE_BY:
            window_editor_objective_options_main_widgets[WIDX_OBJECTIVE_ARG_1].type = WindowWidgetType::Spinner;
            window_editor_objective_options_main_widgets[WIDX_OBJECTIVE_ARG_1_INCREASE].type = WindowWidgetType::Button;
            window_editor_objective_options_main_widgets[WIDX_OBJECTIVE_ARG_1_DECREASE].type = WindowWidgetType::Button;
            window_editor_objective_options_main_widgets[WIDX_OBJECTIVE_ARG_2].type = WindowWidgetType::Spinner;
            window_editor_objective_options_main_widgets[WIDX_OBJECTIVE_ARG_2_INCREASE].type = WindowWidgetType::Button;
            window_editor_objective_options_main_widgets[WIDX_OBJECTIVE_ARG_2_DECREASE].type = WindowWidgetType::Button;
            break;
        case OBJECTIVE_GUESTS_AND_RATING:
        case OBJECTIVE_MONTHLY_RIDE_INCOME:
        case OBJECTIVE_10_ROLLERCOASTERS_LENGTH:
        case OBJECTIVE_FINISH_5_ROLLERCOASTERS:
        case OBJECTIVE_REPAY_LOAN_AND_PARK_VALUE:
        case OBJECTIVE_MONTHLY_FOOD_INCOME:
            window_editor_objective_options_main_widgets[WIDX_OBJECTIVE_ARG_1].type = WindowWidgetType::Spinner;
            window_editor_objective_options_main_widgets[WIDX_OBJECTIVE_ARG_1_INCREASE].type = WindowWidgetType::Button;
            window_editor_objective_options_main_widgets[WIDX_OBJECTIVE_ARG_1_DECREASE].type = WindowWidgetType::Button;
            window_editor_objective_options_main_widgets[WIDX_OBJECTIVE_ARG_2].type = WindowWidgetType::Empty;
            window_editor_objective_options_main_widgets[WIDX_OBJECTIVE_ARG_2_INCREASE].type = WindowWidgetType::Empty;
            window_editor_objective_options_main_widgets[WIDX_OBJECTIVE_ARG_2_DECREASE].type = WindowWidgetType::Empty;
            break;
        default:
            window_editor_objective_options_main_widgets[WIDX_OBJECTIVE_ARG_1].type = WindowWidgetType::Empty;
            window_editor_objective_options_main_widgets[WIDX_OBJECTIVE_ARG_1_INCREASE].type = WindowWidgetType::Empty;
            window_editor_objective_options_main_widgets[WIDX_OBJECTIVE_ARG_1_DECREASE].type = WindowWidgetType::Empty;
            window_editor_objective_options_main_widgets[WIDX_OBJECTIVE_ARG_2].type = WindowWidgetType::Empty;
            window_editor_objective_options_main_widgets[WIDX_OBJECTIVE_ARG_2_INCREASE].type = WindowWidgetType::Empty;
            window_editor_objective_options_main_widgets[WIDX_OBJECTIVE_ARG_2_DECREASE].type = WindowWidgetType::Empty;
            break;
    }

    window_editor_objective_options_main_widgets[WIDX_CLOSE].type = (gScreenFlags & SCREEN_FLAGS_SCENARIO_EDITOR)
        ? WindowWidgetType::Empty
        : WindowWidgetType::CloseBox;

    WindowEditorObjectiveOptionsAnchorBorderWidgets(w);
}

/**
 *
 *  rct2: 0x0067161C
 */
static void WindowEditorObjectiveOptionsMainPaint(rct_window* w, rct_drawpixelinfo* dpi)
{
    int32_t width;
    rct_string_id stringId;

    WindowDrawWidgets(w, dpi);
    WindowEditorObjectiveOptionsDrawTabImages(w, dpi);

    // Objective label
    auto screenCoords = w->windowPos + ScreenCoordsXY{ 8, w->widgets[WIDX_OBJECTIVE].top };
    DrawTextBasic(dpi, screenCoords, STR_OBJECTIVE_WINDOW);

    // Objective value
    screenCoords = w->windowPos + ScreenCoordsXY{ w->widgets[WIDX_OBJECTIVE].left + 1, w->widgets[WIDX_OBJECTIVE].top };
    auto ft = Formatter();
    ft.Add<rct_string_id>(ObjectiveDropdownOptionNames[gScenarioObjective.Type]);
    DrawTextBasic(dpi, screenCoords, STR_WINDOW_COLOUR_2_STRINGID, ft);

    if (w->widgets[WIDX_OBJECTIVE_ARG_1].type != WindowWidgetType::Empty)
    {
        // Objective argument 1 label
        screenCoords = w->windowPos + ScreenCoordsXY{ 28, w->widgets[WIDX_OBJECTIVE_ARG_1].top };
        switch (gScenarioObjective.Type)
        {
            case OBJECTIVE_GUESTS_BY:
            case OBJECTIVE_GUESTS_AND_RATING:
                stringId = STR_WINDOW_OBJECTIVE_GUEST_COUNT;
                break;
            case OBJECTIVE_PARK_VALUE_BY:
            case OBJECTIVE_REPAY_LOAN_AND_PARK_VALUE:
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
        DrawTextBasic(dpi, screenCoords, stringId);

        // Objective argument 1 value
        screenCoords = w->windowPos
            + ScreenCoordsXY{ w->widgets[WIDX_OBJECTIVE_ARG_1].left + 1, w->widgets[WIDX_OBJECTIVE_ARG_1].top };
        ft = Formatter();
        switch (gScenarioObjective.Type)
        {
            case OBJECTIVE_GUESTS_BY:
            case OBJECTIVE_GUESTS_AND_RATING:
                stringId = STR_WINDOW_COLOUR_2_COMMA16;
                ft.Add<uint16_t>(gScenarioObjective.NumGuests);
                break;
            case OBJECTIVE_PARK_VALUE_BY:
            case OBJECTIVE_REPAY_LOAN_AND_PARK_VALUE:
            case OBJECTIVE_MONTHLY_RIDE_INCOME:
            case OBJECTIVE_MONTHLY_FOOD_INCOME:
                stringId = STR_CURRENCY_FORMAT_LABEL;
                ft.Add<money64>(gScenarioObjective.Currency);
                break;
            case OBJECTIVE_10_ROLLERCOASTERS_LENGTH:
                stringId = STR_WINDOW_COLOUR_2_LENGTH;
                ft.Add<uint16_t>(gScenarioObjective.MinimumLength);
                break;
            case OBJECTIVE_FINISH_5_ROLLERCOASTERS:
                stringId = STR_WINDOW_COLOUR_2_COMMA2DP32;
                ft.Add<uint16_t>(gScenarioObjective.MinimumExcitement);
                break;
            default:
                stringId = STR_WINDOW_COLOUR_2_COMMA2DP32;
                ft.Add<money64>(gScenarioObjective.Currency);
                break;
        }
        DrawTextBasic(dpi, screenCoords, stringId, ft, COLOUR_BLACK);
    }

    if (w->widgets[WIDX_OBJECTIVE_ARG_2].type != WindowWidgetType::Empty)
    {
        // Objective argument 2 label
        screenCoords = w->windowPos + ScreenCoordsXY{ 28, w->widgets[WIDX_OBJECTIVE_ARG_2].top };
        DrawTextBasic(dpi, screenCoords, STR_WINDOW_OBJECTIVE_DATE);

        // Objective argument 2 value
        screenCoords = w->windowPos
            + ScreenCoordsXY{ w->widgets[WIDX_OBJECTIVE_ARG_2].left + 1, w->widgets[WIDX_OBJECTIVE_ARG_2].top };
        ft = Formatter();
        ft.Add<uint16_t>((gScenarioObjective.Year * MONTH_COUNT) - 1);
        DrawTextBasic(dpi, screenCoords, STR_WINDOW_OBJECTIVE_VALUE_DATE, ft);
    }

    // Park name
    screenCoords = w->windowPos + ScreenCoordsXY{ 8, w->widgets[WIDX_PARK_NAME].top };
    width = w->widgets[WIDX_PARK_NAME].left - 16;

    {
        auto& park = OpenRCT2::GetContext()->GetGameState()->GetPark();
        auto parkName = park.Name.c_str();

        ft = Formatter();
        ft.Add<rct_string_id>(STR_STRING);
        ft.Add<const char*>(parkName);
        DrawTextEllipsised(dpi, screenCoords, width, STR_WINDOW_PARK_NAME, ft);
    }

    // Scenario name
    screenCoords = w->windowPos + ScreenCoordsXY{ 8, w->widgets[WIDX_SCENARIO_NAME].top };
    width = w->widgets[WIDX_SCENARIO_NAME].left - 16;

    ft = Formatter();
    ft.Add<rct_string_id>(STR_STRING);
    ft.Add<const char*>(gScenarioName.c_str());
    DrawTextEllipsised(dpi, screenCoords, width, STR_WINDOW_SCENARIO_NAME, ft);

    // Scenario details label
    screenCoords = w->windowPos + ScreenCoordsXY{ 8, w->widgets[WIDX_DETAILS].top };
    DrawTextBasic(dpi, screenCoords, STR_WINDOW_PARK_DETAILS);

    // Scenario details value
    screenCoords = w->windowPos + ScreenCoordsXY{ 16, w->widgets[WIDX_DETAILS].top + 10 };
    width = w->widgets[WIDX_DETAILS].left - 4;

    ft = Formatter();
    ft.Add<rct_string_id>(STR_STRING);
    ft.Add<const char*>(gScenarioDetails.c_str());
    DrawTextWrapped(dpi, screenCoords, width, STR_BLACK_STRING, ft);

    // Scenario category label
    screenCoords = w->windowPos + ScreenCoordsXY{ 8, w->widgets[WIDX_CATEGORY].top };
    DrawTextBasic(dpi, screenCoords, STR_WINDOW_SCENARIO_GROUP);

    // Scenario category value
    screenCoords = w->windowPos + ScreenCoordsXY{ w->widgets[WIDX_CATEGORY].left + 1, w->widgets[WIDX_CATEGORY].top };
    ft = Formatter();
    ft.Add<rct_string_id>(ScenarioCategoryStringIds[gScenarioCategory]);
    DrawTextBasic(dpi, screenCoords, STR_WINDOW_COLOUR_2_STRINGID, ft);
}

/**
 *
 *  rct2: 0x006724A4
 */
static void WindowEditorObjectiveOptionsRidesMouseup(rct_window* w, rct_widgetindex widgetIndex)
{
    switch (widgetIndex)
    {
        case WIDX_CLOSE:
            window_close(w);
            break;
        case WIDX_TAB_1:
        case WIDX_TAB_2:
            WindowEditorObjectiveOptionsSetPage(w, widgetIndex - WIDX_TAB_1);
            break;
    }
}

/**
 *
 *  rct2: 0x006725A8
 */
static void WindowEditorObjectiveOptionsRidesResize(rct_window* w)
{
    window_set_resize(w, 380, 224, 380, 224);
}

/**
 *
 *  rct2: 0x00672544
 */
static void WindowEditorObjectiveOptionsRidesUpdate(rct_window* w)
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
            w->list_item_positions[numItems] = ride.id.ToUnderlying();
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
static void WindowEditorObjectiveOptionsRidesScrollgetheight(
    rct_window* w, int32_t scrollIndex, int32_t* width, int32_t* height)
{
    *height = w->no_list_items * 12;
}

/**
 *
 *  rct2: 0x006724FC
 */
static void WindowEditorObjectiveOptionsRidesScrollmousedown(
    rct_window* w, int32_t scrollIndex, const ScreenCoordsXY& screenCoords)
{
    auto i = screenCoords.y / 12;
    if (i < 0 || i >= w->no_list_items)
        return;

    const auto rideId = RideId::FromUnderlying(w->list_item_positions[i]);
    auto ride = get_ride(rideId);
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
static void WindowEditorObjectiveOptionsRidesScrollmouseover(
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
static void WindowEditorObjectiveOptionsRidesInvalidate(rct_window* w)
{
    rct_widget* widgets;

    widgets = window_editor_objective_options_widgets[w->page];
    if (w->widgets != widgets)
    {
        w->widgets = widgets;
        WindowInitScrollWidgets(w);
    }

    WindowEditorObjectiveOptionsSetPressedTab(w);

    window_editor_objective_options_main_widgets[WIDX_CLOSE].type = (gScreenFlags & SCREEN_FLAGS_SCENARIO_EDITOR)
        ? WindowWidgetType::Empty
        : WindowWidgetType::CloseBox;

    WindowEditorObjectiveOptionsAnchorBorderWidgets(w);
}

/**
 *
 *  rct2: 0x00672340
 */
static void WindowEditorObjectiveOptionsRidesPaint(rct_window* w, rct_drawpixelinfo* dpi)
{
    WindowDrawWidgets(w, dpi);
    WindowEditorObjectiveOptionsDrawTabImages(w, dpi);

    DrawTextBasic(
        dpi, w->windowPos + ScreenCoordsXY{ 6, w->widgets[WIDX_PAGE_BACKGROUND].top + 3 }, STR_WINDOW_PRESERVATION_ORDER);
}

/**
 *
 *  rct2: 0x0067236F
 */
static void WindowEditorObjectiveOptionsRidesScrollpaint(rct_window* w, rct_drawpixelinfo* dpi, int32_t scrollIndex)
{
    int32_t colour = ColourMapA[w->colours[1]].mid_light;
    gfx_fill_rect(dpi, { { dpi->x, dpi->y }, { dpi->x + dpi->width - 1, dpi->y + dpi->height - 1 } }, colour);

    for (int32_t i = 0; i < w->no_list_items; i++)
    {
        int32_t y = i * 12;

        if (y + 12 < dpi->y || y >= dpi->y + dpi->height)
            continue;

        // Checkbox
        gfx_fill_rect_inset(dpi, { { 2, y }, { 11, y + 10 } }, w->colours[1], INSET_RECT_F_E0);

        // Highlighted
        auto stringId = STR_BLACK_STRING;
        if (i == w->selected_list_item)
        {
            stringId = STR_WINDOW_COLOUR_2_STRINGID;
            gfx_filter_rect(dpi, { 0, y, w->width, y + 11 }, FilterPaletteID::PaletteDarken1);
        }

        // Checkbox mark
        const auto rideId = RideId::FromUnderlying(w->list_item_positions[i]);
        auto ride = get_ride(rideId);
        if (ride != nullptr)
        {
            if (ride->lifecycle_flags & RIDE_LIFECYCLE_INDESTRUCTIBLE)
            {
                FontSpriteBase fontSpriteBase = stringId == STR_WINDOW_COLOUR_2_STRINGID ? FontSpriteBase::MEDIUM_EXTRA_DARK
                                                                                         : FontSpriteBase::MEDIUM_DARK;
                gfx_draw_string(
                    dpi, { 2, y }, static_cast<const char*>(CheckBoxMarkString),
                    { static_cast<colour_t>(w->colours[1] & 0x7F), fontSpriteBase });
            }

            // Ride name

            Formatter ft;
            ride->FormatNameTo(ft);
            DrawTextBasic(dpi, { 15, y }, stringId, ft);
        }
    }
}

/**
 *
 *  rct2: 0x00672609
 */
static void WindowEditorObjectiveOptionsUpdateDisabledWidgets(rct_window* w)
{
    // Check if there are any rides (not shops or facilities)
    const auto& rideManager = GetRideManager();
    if (std::any_of(rideManager.begin(), rideManager.end(), [](const Ride& ride) { return ride.IsRide(); }))
    {
        w->disabled_widgets &= ~(1ULL << WIDX_TAB_2);
    }
    else
    {
        w->disabled_widgets |= (1ULL << WIDX_TAB_2);
    }
}
