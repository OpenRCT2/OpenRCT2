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
#include <openrct2-ui/interface/Theme.h>
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

#define MAX_YEAR_DATE 100

static constexpr const int32_t WH = 302;
static constexpr const int32_t WW = 250;

ObjectiveGoalGroup newGroup;
size_t groupIndex = 0;

#pragma region Widgets

// clang-format off



static constexpr const rct_string_id MonthsDropdownNames[] = {
    STR_MONTH_MARCH,
    STR_MONTH_APRIL,
    STR_MONTH_MAY,
    STR_MONTH_JUNE,
    STR_MONTH_JULY,
    STR_MONTH_AUGUST,
    STR_MONTH_SEPTEMBER,
    STR_MONTH_OCTOBER,
};

static constexpr const rct_string_id BeforeAtAfterNames[] = {
    STR_DATELESS,
    STR_BEFORE_DATE,
    STR_AT_DATE,
    STR_AFTER_DATE,
    STR_DURING_PERIOD,
    STR_AT_END_OF_PERIOD,
    STR_AFTER_PERIOD,
};


enum {
    WIDX_ADD_GROUP_BACKGROUND,
    WIDX_ADD_GROUP_TITLE,
    WIDX_ADD_GROUP_CLOSE,
    WIDX_BEFOREATAFTER,
    WIDX_BEFOREATAFTER_DROPDOWN,
    WIDX_MONTH,
    WIDX_MONTH_DROPDOWN,
    WIDX_YEAR,
    WIDX_YEAR_INCREASE,
    WIDX_YEAR_DECREASE,
    WIDX_MONTHSPINNER,
    WIDX_MONTHSPINNER_INCREASE,
    WIDX_MONTHSPINNER_DECREASE,
    WIDX_OVERRIDE_EARLY,
    WIDX_GROUP_TEXT,
    WIDX_OKAY,
    WIDX_CANCEL,
};


#define MAIN_OBJECTIVE_ADD_GROUP_WIDGETS \
    MakeWidget({ 145,  47}, {100,  12}, WindowWidgetType::DropdownMenu, WindowColour::Secondary, STR_NONE, STR_SELECT_RELATION_TO_DATE     ),/*before at after*/ \
    MakeWidget({233, 48}, { 11,  10}, WindowWidgetType::Button,   WindowColour::Secondary, STR_DROPDOWN_GLYPH, STR_SELECT_RELATION_TO_DATE), \
    MakeWidget({ 145,  67}, {100,  12}, WindowWidgetType::DropdownMenu, WindowColour::Secondary, STR_NONE,STR_SELECT_A_MONTH     ), /*month*/\
    MakeWidget({233, 68}, { 11,  10}, WindowWidgetType::Button,   WindowColour::Secondary, STR_DROPDOWN_GLYPH, STR_SELECT_A_MONTH), /*month*/\
    MakeSpinnerWidgets({145,  87}, {100,  12}, WindowWidgetType::Button,   WindowColour::Secondary), /*year NB: 3 widgets*/\
    MakeSpinnerWidgets({ 145,  67}, {100,  12}, WindowWidgetType::Button,   WindowColour::Secondary), /*month as number NB: 3 widgets*/\
    MakeWidget({232,  107}, {12,  12}, WindowWidgetType::Checkbox,   WindowColour::Secondary,STR_NONE, STR_OVERRIDE_EARLY_COMPLETION),/*override early completion*/ \
    MakeWidget({145,  127}, {100,  12}, WindowWidgetType::Button,   WindowColour::Secondary,STR_CHANGE ),/*Enter group text*/ \
    MakeWidget({40,  285}, {70,  12}, WindowWidgetType::Button,   WindowColour::Secondary,STR_GOAL_GROUP_CONFIRM ),/*Confirm*/ \
    MakeWidget({140,  285}, {70,  12}, WindowWidgetType::Button,   WindowColour::Secondary,STR_SAVE_PROMPT_CANCEL ), /*Cancel*/ \
    WIDGETS_END,

static rct_widget window_editor_objective_options_add_group_widets[] = {
    WINDOW_SHIM(STR_ADD_DEADLINE, WW, WH),
    MAIN_OBJECTIVE_ADD_GROUP_WIDGETS
};

static rct_widget window_editor_objective_options_edit_group_widets[] = {
    WINDOW_SHIM(STR_EDIT_PHASE, WW, WH),
    MAIN_OBJECTIVE_ADD_GROUP_WIDGETS
};

#pragma endregion

#pragma region Events

static void WindowEditorObjectiveOptionsAddGroupMouseup(rct_window *w, rct_widgetindex widgetIndex);
static void WindowEditorObjectiveOptionsAddGroupResize(rct_window *w);
static void WindowEditorObjectiveOptionsAddGroupMousedown(rct_window *w, rct_widgetindex widgetIndex, rct_widget* widget);
static void WindowEditorObjectiveOptionsAddGroupDropdown(rct_window *w, rct_widgetindex widgetIndex, int32_t dropdownIndex);
static void WindowEditorObjectiveOptionsAddGroupInvalidate(rct_window *w);
static void WindowEditorObjectiveOptionsAddGroupPaint(rct_window *w, rct_drawpixelinfo *dpi);
static void WindowEditorObjectiveOptionsAddGroupTextinput(rct_window* w, rct_widgetindex widgetIndex, char* text);


static rct_window_event_list window_objective_options_add_group_events([](auto& events)
{
    events.mouse_up = &WindowEditorObjectiveOptionsAddGroupMouseup;
    events.resize = &WindowEditorObjectiveOptionsAddGroupResize;
    events.mouse_down = &WindowEditorObjectiveOptionsAddGroupMousedown;
    events.dropdown = &WindowEditorObjectiveOptionsAddGroupDropdown;
    events.invalidate = &WindowEditorObjectiveOptionsAddGroupInvalidate;
    events.paint = &WindowEditorObjectiveOptionsAddGroupPaint;
    events.text_input = &WindowEditorObjectiveOptionsAddGroupTextinput;
});


#pragma endregion

#pragma region Enabled widgets
static uint64_t window_editor_objective_options_page_add_objective_hold_down_widgets[] = {
    (1ULL << WIDX_YEAR_INCREASE) |
    (1ULL << WIDX_YEAR_DECREASE)|
    (1ULL << WIDX_MONTHSPINNER_INCREASE) |
    (1ULL << WIDX_MONTHSPINNER_DECREASE),
    0,
};
// clang-format on

#pragma endregion

rct_window* WindowEditorObjectiveOptionsAddGroupOpen(ObjectiveGoalGroup* _group, size_t _groupIndex)
{
    rct_window* w;
    if (_group != nullptr)
    {
        newGroup = *_group;
        groupIndex = _groupIndex;
    }
    else
    {
        groupIndex = gScenarioObjective.PhasedGoals.size();
        newGroup = ObjectiveGoalGroup(GoalGroupType::AtDate, 1, 1, false, false, "");
    }

    w = window_bring_to_front_by_class(WC_EDITOR_OBJECTIVE_OPTIONS_ADD_GROUP);
    if (w != nullptr)
        return w;

    w = WindowCreateCentred(
        250, 302, &window_objective_options_add_group_events, WC_EDITOR_OBJECTIVE_OPTIONS_ADD_GROUP, WF_RESIZABLE);
    if (_group == nullptr)
        w->widgets = window_editor_objective_options_add_group_widets;
    else
        w->widgets = window_editor_objective_options_edit_group_widets;
    w->pressed_widgets = 0;
    w->hold_down_widgets = window_editor_objective_options_page_add_objective_hold_down_widgets[0];
    w->no_list_items = 0;
    w->number = 0;
    w->page = 0;
    w->frame_no = 0;
    w->list_information_type = 0;
    w->picked_peep_frame = 0;
    w->highlighted_item = 0;
    w->selected_list_item = -1;

    w->Invalidate();
    window_event_resize_call(w);
    window_event_invalidate_call(w);
    WindowInitScrollWidgets(w);

    return w;
}

static void WindowEditorObjectiveOptionsAddGroupAnchorBorderWidgets(rct_window* w)
{
    w->widgets[WIDX_ADD_GROUP_BACKGROUND].right = w->width - 1;
    w->widgets[WIDX_ADD_GROUP_BACKGROUND].bottom = w->height - 1;
    w->widgets[WIDX_ADD_GROUP_TITLE].right = w->width - 2;
    w->widgets[WIDX_ADD_GROUP_CLOSE].left = w->width - 13;
    w->widgets[WIDX_ADD_GROUP_CLOSE].right = w->width - 3;
}

static void WindowEditorObjectiveOptionsAddGroupMouseup(rct_window* w, rct_widgetindex widgetIndex)
{
    rct_string_id e;
    switch (widgetIndex)
    {
        case WIDX_CANCEL:
        case WIDX_ADD_GROUP_CLOSE:
            window_close(w);
            break;
        case WIDX_GROUP_TEXT:
        {
            WindowTextInputRawOpen(
                w, WIDX_GROUP_TEXT, STR_PARK_SCENARIO_DETAILS, STR_ENTER_PHASE_DESCRIPTION, {},
                newGroup.scenarioDetailsPhase.c_str(), 256);
            break;
        }
        case WIDX_OVERRIDE_EARLY:
            newGroup.disAllowEarlyCompletion = !newGroup.disAllowEarlyCompletion;
            break;
        case WIDX_OKAY:
            if (gScenarioObjective.PhasedGoals.size() <= groupIndex)
            {
                e = gScenarioObjective.AddPhasedGoalGroup(newGroup);
                if (e == 0)
                    window_close(w);
                else
                    context_show_error(e, STR_NONE, {});
            }
            else // if editing an already established group
            {
                e = gScenarioObjective.SetPhasedGoalGroup(newGroup, groupIndex);
                if (e == 0)
                    window_close(w);
                else
                    context_show_error(e, STR_NONE, {});
            }
            break;
    }
}

static void WindowEditorObjectiveOptionsAddGroupResize(rct_window* w)
{
    window_set_resize(w, 250, 302, 250, 302);
}

static void WindowEditorObjectiveOptionsAddGroupDropdown(rct_window* w, rct_widgetindex widgetIndex, int32_t dropdownIndex)
{
    if (dropdownIndex == -1)
        return;

    switch (widgetIndex)
    {
        case WIDX_BEFOREATAFTER_DROPDOWN:
            newGroup.groupType = static_cast<GoalGroupType>(dropdownIndex);
            WindowEditorObjectiveOptionsAddGroupInvalidate(w);
            w->Invalidate();
            break;
        case WIDX_MONTH_DROPDOWN:
            if (newGroup.monthDate != static_cast<uint8_t>(dropdownIndex + 1))
            {
                newGroup.monthDate = static_cast<uint8_t>(dropdownIndex + 1);
                w->Invalidate();
            }
            break;
    }
}

static void WindowEditorObjectiveOptionsAddGroupTextinput(rct_window* w, rct_widgetindex widgetIndex, char* text)
{
    if (text == nullptr)
        return;

    switch (widgetIndex)
    {
        case WIDX_GROUP_TEXT:
            newGroup.scenarioDetailsPhase = text;
            w->Invalidate();
            break;
    }
}

static void WindowEditorObjectiveOptionsAddGroupInvalidate(rct_window* w)
{
    ColourSchemeUpdateByClass(w, static_cast<rct_windowclass>(WC_EDITOR_OBJECTIVE_OPTIONS));
    WindowEditorObjectiveOptionsAddGroupAnchorBorderWidgets(w);
    switch (newGroup.groupType)
    {
        case GoalGroupType::AfterDate:
        case GoalGroupType::AtDate:
        case GoalGroupType::BeforeDate:
            window_editor_objective_options_add_group_widets[WIDX_MONTHSPINNER].type = WindowWidgetType::Empty;
            window_editor_objective_options_edit_group_widets[WIDX_MONTHSPINNER].type = WindowWidgetType::Empty;
            window_editor_objective_options_add_group_widets[WIDX_MONTHSPINNER_INCREASE].type = WindowWidgetType::Empty;
            window_editor_objective_options_edit_group_widets[WIDX_MONTHSPINNER_INCREASE].type = WindowWidgetType::Empty;
            window_editor_objective_options_add_group_widets[WIDX_MONTHSPINNER_DECREASE].type = WindowWidgetType::Empty;
            window_editor_objective_options_edit_group_widets[WIDX_MONTHSPINNER_DECREASE].type = WindowWidgetType::Empty;

            window_editor_objective_options_add_group_widets[WIDX_MONTH].type = WindowWidgetType::DropdownMenu;
            window_editor_objective_options_edit_group_widets[WIDX_MONTH].type = WindowWidgetType::DropdownMenu;
            window_editor_objective_options_add_group_widets[WIDX_MONTH_DROPDOWN].type = WindowWidgetType::Button;
            window_editor_objective_options_edit_group_widets[WIDX_MONTH_DROPDOWN].type = WindowWidgetType::Button;

            window_editor_objective_options_add_group_widets[WIDX_OVERRIDE_EARLY].type = WindowWidgetType::Checkbox;
            window_editor_objective_options_edit_group_widets[WIDX_OVERRIDE_EARLY].type = WindowWidgetType::Checkbox;

            window_editor_objective_options_add_group_widets[WIDX_YEAR].type = WindowWidgetType::Spinner;
            window_editor_objective_options_edit_group_widets[WIDX_YEAR].type = WindowWidgetType::Spinner;
            window_editor_objective_options_add_group_widets[WIDX_YEAR_INCREASE].type = WindowWidgetType::Button;
            window_editor_objective_options_edit_group_widets[WIDX_YEAR_INCREASE].type = WindowWidgetType::Button;
            window_editor_objective_options_add_group_widets[WIDX_YEAR_DECREASE].type = WindowWidgetType::Button;
            window_editor_objective_options_edit_group_widets[WIDX_YEAR_DECREASE].type = WindowWidgetType::Button;

            break;

        case GoalGroupType::DuringPeriod:
        case GoalGroupType::AfterPeriod:
        case GoalGroupType::AtEndOfPeriod:
            window_editor_objective_options_add_group_widets[WIDX_MONTHSPINNER].type = WindowWidgetType::Spinner;
            window_editor_objective_options_edit_group_widets[WIDX_MONTHSPINNER].type = WindowWidgetType::Spinner;
            window_editor_objective_options_add_group_widets[WIDX_MONTHSPINNER_INCREASE].type = WindowWidgetType::Button;
            window_editor_objective_options_edit_group_widets[WIDX_MONTHSPINNER_INCREASE].type = WindowWidgetType::Button;
            window_editor_objective_options_add_group_widets[WIDX_MONTHSPINNER_DECREASE].type = WindowWidgetType::Button;
            window_editor_objective_options_edit_group_widets[WIDX_MONTHSPINNER_DECREASE].type = WindowWidgetType::Button;

            window_editor_objective_options_add_group_widets[WIDX_MONTH].type = WindowWidgetType::Empty;
            window_editor_objective_options_edit_group_widets[WIDX_MONTH].type = WindowWidgetType::Empty;
            window_editor_objective_options_add_group_widets[WIDX_MONTH_DROPDOWN].type = WindowWidgetType::Empty;
            window_editor_objective_options_edit_group_widets[WIDX_MONTH_DROPDOWN].type = WindowWidgetType::Empty;

            window_editor_objective_options_add_group_widets[WIDX_OVERRIDE_EARLY].type = WindowWidgetType::Checkbox;
            window_editor_objective_options_edit_group_widets[WIDX_OVERRIDE_EARLY].type = WindowWidgetType::Checkbox;

            window_editor_objective_options_add_group_widets[WIDX_YEAR].type = WindowWidgetType::Spinner;
            window_editor_objective_options_edit_group_widets[WIDX_YEAR].type = WindowWidgetType::Spinner;
            window_editor_objective_options_add_group_widets[WIDX_YEAR_INCREASE].type = WindowWidgetType::Button;
            window_editor_objective_options_edit_group_widets[WIDX_YEAR_INCREASE].type = WindowWidgetType::Button;
            window_editor_objective_options_add_group_widets[WIDX_YEAR_DECREASE].type = WindowWidgetType::Button;
            window_editor_objective_options_edit_group_widets[WIDX_YEAR_DECREASE].type = WindowWidgetType::Button;

            break;

        case GoalGroupType::Dateless:

            window_editor_objective_options_add_group_widets[WIDX_MONTHSPINNER].type = WindowWidgetType::Empty;
            window_editor_objective_options_edit_group_widets[WIDX_MONTHSPINNER].type = WindowWidgetType::Empty;
            window_editor_objective_options_add_group_widets[WIDX_MONTHSPINNER_INCREASE].type = WindowWidgetType::Empty;
            window_editor_objective_options_edit_group_widets[WIDX_MONTHSPINNER_INCREASE].type = WindowWidgetType::Empty;
            window_editor_objective_options_add_group_widets[WIDX_MONTHSPINNER_DECREASE].type = WindowWidgetType::Empty;
            window_editor_objective_options_edit_group_widets[WIDX_MONTHSPINNER_DECREASE].type = WindowWidgetType::Empty;
            window_editor_objective_options_add_group_widets[WIDX_MONTH].type = WindowWidgetType::Empty;
            window_editor_objective_options_edit_group_widets[WIDX_MONTH].type = WindowWidgetType::Empty;
            window_editor_objective_options_add_group_widets[WIDX_MONTH_DROPDOWN].type = WindowWidgetType::Empty;
            window_editor_objective_options_edit_group_widets[WIDX_MONTH_DROPDOWN].type = WindowWidgetType::Empty;

            window_editor_objective_options_add_group_widets[WIDX_OVERRIDE_EARLY].type = WindowWidgetType::Empty;
            window_editor_objective_options_edit_group_widets[WIDX_OVERRIDE_EARLY].type = WindowWidgetType::Empty;
            window_editor_objective_options_add_group_widets[WIDX_YEAR].type = WindowWidgetType::Empty;
            window_editor_objective_options_edit_group_widets[WIDX_YEAR].type = WindowWidgetType::Empty;
            window_editor_objective_options_add_group_widets[WIDX_YEAR_INCREASE].type = WindowWidgetType::Empty;
            window_editor_objective_options_edit_group_widets[WIDX_YEAR_INCREASE].type = WindowWidgetType::Empty;
            window_editor_objective_options_add_group_widets[WIDX_YEAR_DECREASE].type = WindowWidgetType::Empty;
            window_editor_objective_options_edit_group_widets[WIDX_YEAR_DECREASE].type = WindowWidgetType::Empty;
            break;
    }

    if (newGroup.disAllowEarlyCompletion)
        w->pressed_widgets |= (1ULL << WIDX_OVERRIDE_EARLY);
    else
        w->pressed_widgets &= ~(1ULL << WIDX_OVERRIDE_EARLY);
}

static void WindowEditorObjectiveOptionsAddGroupYearIncrease(rct_window* w)
{
    if (newGroup.groupType == GoalGroupType::AfterDate || newGroup.groupType == GoalGroupType::AtDate
        || newGroup.groupType == GoalGroupType::BeforeDate)
    {
        if (newGroup.yearDate >= MAX_YEAR_DATE)
        {
            context_show_error(STR_CANT_INCREASE_FURTHER, STR_NONE, {});
        }
        else
        {
            newGroup.yearDate++;
            w->Invalidate();
        }
    }
    else if (
        newGroup.groupType == GoalGroupType::AfterPeriod || newGroup.groupType == GoalGroupType::AtEndOfPeriod
        || newGroup.groupType == GoalGroupType::DuringPeriod)
    {
        if (newGroup.yearPeriod >= MAX_YEAR_DATE)
        {
            context_show_error(STR_CANT_INCREASE_FURTHER, STR_NONE, {});
        }
        else
        {
            newGroup.yearPeriod++;
            w->Invalidate();
        }
    }
}

static void WindowEditorObjectiveOptionsAddGroupYearDecrease(rct_window* w)
{
    if (newGroup.groupType == GoalGroupType::AfterDate || newGroup.groupType == GoalGroupType::AtDate
        || newGroup.groupType == GoalGroupType::BeforeDate)
    {
        if (newGroup.yearDate <= 1)
        {
            context_show_error(STR_CANT_REDUCE_FURTHER, STR_NONE, {});
        }
        else
        {
            newGroup.yearDate--;
            w->Invalidate();
        }
    }
    else if (
        newGroup.groupType == GoalGroupType::AfterPeriod || newGroup.groupType == GoalGroupType::AtEndOfPeriod
        || newGroup.groupType == GoalGroupType::DuringPeriod)
    {
        if (newGroup.yearPeriod <= 0)
        {
            context_show_error(STR_CANT_REDUCE_FURTHER, STR_NONE, {});
        }
        else
        {
            newGroup.yearPeriod--;
            w->Invalidate();
        }
    }
}
static void WindowEditorObjectiveOptionsAddGroupBeforeAfterDropdown(rct_window* w)
{
    int32_t i;
    rct_widget* dropdownWidget;

    dropdownWidget = &w->widgets[WIDX_BEFOREATAFTER];
    for (i = 0; i < 7; i++)
    {
        gDropdownItems[i].Format = STR_DROPDOWN_MENU_LABEL;
        gDropdownItems[i].Args = BeforeAtAfterNames[i];
    }
    WindowDropdownShowTextCustomWidth(
        { w->windowPos.x + dropdownWidget->left, w->windowPos.y + dropdownWidget->top }, dropdownWidget->height() + 1,
        w->colours[1], 0, Dropdown::Flag::StayOpen, 7, dropdownWidget->width() - 3);
    Dropdown::SetChecked((int32_t)newGroup.groupType, true);
}

static void WindowEditorObjectiveOptionsAddGroupMonthDropdown(rct_window* w)
{
    int32_t i;
    rct_widget* dropdownWidget;

    dropdownWidget = &w->widgets[WIDX_MONTH];

    for (i = 0; i < MONTH_COUNT; i++)
    {
        gDropdownItems[i].Format = STR_DROPDOWN_MENU_LABEL;
        gDropdownItems[i].Args = MonthsDropdownNames[i];
    }
    WindowDropdownShowTextCustomWidth(
        { w->windowPos.x + dropdownWidget->left, w->windowPos.y + dropdownWidget->top }, dropdownWidget->height() + 1,
        w->colours[1], 0, Dropdown::Flag::StayOpen, MONTH_COUNT, dropdownWidget->width() - 3);

    Dropdown::SetChecked(newGroup.monthDate - 1, true);
}

static void WindowEditorObjectiveOptionsAddGroupMonthIncrease(rct_window* w)
{
    if (newGroup.monthPeriod >= MONTH_COUNT)
    {
        context_show_error(STR_CANT_INCREASE_FURTHER, STR_NONE, {});
    }
    else
    {
        newGroup.monthPeriod++;
        w->Invalidate();
    }
}

static void WindowEditorObjectiveOptionsAddGroupMonthDecrease(rct_window* w)
{
    if (newGroup.monthPeriod <= 0)
    {
        context_show_error(STR_CANT_REDUCE_FURTHER, STR_NONE, {});
    }
    else
    {
        newGroup.monthPeriod--;
        w->Invalidate();
    }
}

static void WindowEditorObjectiveOptionsAddGroupMousedown(rct_window* w, rct_widgetindex widgetIndex, rct_widget* widget)
{
    switch (widgetIndex)
    {
        case WIDX_BEFOREATAFTER_DROPDOWN:
            WindowEditorObjectiveOptionsAddGroupBeforeAfterDropdown(w);
            break;
        case WIDX_YEAR_INCREASE:
            WindowEditorObjectiveOptionsAddGroupYearIncrease(w);
            break;
        case WIDX_YEAR_DECREASE:
            WindowEditorObjectiveOptionsAddGroupYearDecrease(w);
            break;
        case WIDX_MONTH_DROPDOWN:
            WindowEditorObjectiveOptionsAddGroupMonthDropdown(w);
            break;
        case WIDX_MONTHSPINNER_INCREASE:
            WindowEditorObjectiveOptionsAddGroupMonthIncrease(w);
            break;
        case WIDX_MONTHSPINNER_DECREASE:
            WindowEditorObjectiveOptionsAddGroupMonthDecrease(w);
            break;
    }
}

static void WindowEditorObjectiveOptionsAddGroupPaint(rct_window* w, rct_drawpixelinfo* dpi)
{
    WindowDrawWidgets(w, dpi);
    auto screenCoords = w->windowPos + ScreenCoordsXY{ 125, w->widgets[WIDX_BEFOREATAFTER].top - 22 };
    auto ft = Formatter();
    ft.Add<rct_string_id>(STR_STRING);
    std::string s = newGroup.ToString(true);
    ft.Add<const char*>(s.c_str());
    DrawTextBasic(dpi, screenCoords, STR_WINDOW_COLOUR_2_STRINGID, ft, { TextAlignment::CENTRE_HORIZONTAL_ONLY });

    // Date Relation label
    screenCoords = w->windowPos + ScreenCoordsXY{ 8, w->widgets[WIDX_BEFOREATAFTER].top };
    ft = Formatter();
    ft.Add<rct_string_id>(STR_DATE_RELATION);
    DrawTextBasic(dpi, screenCoords, STR_WINDOW_COLOUR_2_STRINGID, ft);

    // Date Relation value
    screenCoords = w->windowPos + ScreenCoordsXY{ w->widgets[WIDX_BEFOREATAFTER].left + 1, w->widgets[WIDX_BEFOREATAFTER].top };
    ft = Formatter();

    ft.Add<rct_string_id>(BeforeAtAfterNames[(int32_t)newGroup.groupType]);

    DrawTextBasic(dpi, screenCoords, STR_WINDOW_COLOUR_2_STRINGID, ft);

    if (newGroup.groupType != GoalGroupType::Dateless)
    {
        // Month label
        screenCoords = w->windowPos + ScreenCoordsXY{ 8, w->widgets[WIDX_MONTH].top };
        DrawTextBasic(dpi, screenCoords, STR_MONTH_WINDOW_COLOUR_2);

        if (newGroup.groupType == GoalGroupType::AfterDate || newGroup.groupType == GoalGroupType::AtDate
            || newGroup.groupType == GoalGroupType::BeforeDate)
        {
            // Month date value
            screenCoords = w->windowPos + ScreenCoordsXY{ w->widgets[WIDX_MONTH].left + 1, w->widgets[WIDX_MONTH].top };
            ft = Formatter();
            ft.Add<rct_string_id>(MonthsDropdownNames[newGroup.monthDate - 1]);
            DrawTextBasic(dpi, screenCoords, STR_WINDOW_COLOUR_2_STRINGID, ft);
        }
        else
        {
            // Month period value
            screenCoords = w->windowPos
                + ScreenCoordsXY{ w->widgets[WIDX_MONTHSPINNER].left + 1, w->widgets[WIDX_MONTHSPINNER].top };
            ft = Formatter();
            ft.Add<rct_string_id>(STR_FORMAT_INTEGER);
            ft.Add<uint16_t>(newGroup.monthPeriod);
            DrawTextBasic(dpi, screenCoords, STR_WINDOW_COLOUR_2_STRINGID, ft);
        }
        // Year label
        screenCoords = w->windowPos + ScreenCoordsXY{ 8, w->widgets[WIDX_YEAR].top };
        DrawTextBasic(dpi, screenCoords, STR_YEAR_WINDOW_COLOUR_2);

        // Year value
        screenCoords = w->windowPos + ScreenCoordsXY{ w->widgets[WIDX_YEAR].left + 1, w->widgets[WIDX_YEAR].top };
        ft = Formatter();
        ft.Add<rct_string_id>(STR_FORMAT_INTEGER);
        if (newGroup.groupType == GoalGroupType::AfterDate || newGroup.groupType == GoalGroupType::AtDate
            || newGroup.groupType == GoalGroupType::BeforeDate)
            ft.Add<uint16_t>(newGroup.yearDate);
        else
            ft.Add<uint16_t>(newGroup.yearPeriod);
        DrawTextBasic(dpi, screenCoords, STR_WINDOW_COLOUR_2_STRINGID, ft);

        // Override early completion label
        screenCoords = w->windowPos + ScreenCoordsXY{ 8, w->widgets[WIDX_OVERRIDE_EARLY].top };
        ft = Formatter();
        ft.Add<rct_string_id>(STR_OVERRIDE_EARLY_COMPLETION);
        DrawTextBasic(dpi, screenCoords, STR_WINDOW_COLOUR_2_STRINGID, ft);
    }
    // Scenario details label
    screenCoords = w->windowPos + ScreenCoordsXY{ 8, w->widgets[WIDX_GROUP_TEXT].top };
    DrawTextBasic(dpi, screenCoords, STR_WINDOW_PARK_DETAILS);

    // Scenario details value
    screenCoords = w->windowPos + ScreenCoordsXY{ 16, w->widgets[WIDX_GROUP_TEXT].top + 12 };

    ft = Formatter();
    ft.Add<rct_string_id>(STR_STRING);
    ft.Add<const char*>(newGroup.scenarioDetailsPhase.c_str());
    DrawTextWrapped(dpi, screenCoords, 218, STR_BLACK_STRING, ft);
}
