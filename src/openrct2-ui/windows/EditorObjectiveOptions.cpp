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
static constexpr const int32_t WH = 595;
static constexpr const int32_t WW = 600;
uint32_t TimedObjectiveIndex = 0;
std::optional<size_t> _highlightedIndexUntimed = 0;
std::optional<size_t> _highlightedIndexTimed = 0;

#pragma region Widgets

// clang-format off
enum {
    WINDOW_EDITOR_OBJECTIVE_OPTIONS_PAGE_MAIN,
    WINDOW_EDITOR_OBJECTIVE_OPTIONS_PAGE_RIDES,
    WINDOW_EDITOR_OBJECTIVE_OPTIONS_PAGE_COUNT
};

enum {
    WIDX_BACKGROUND,
    WIDX_TITLE,
    WIDX_CLOSE,
    WIDX_PAGE_BACKGROUND,
    WIDX_TAB_1,
    WIDX_TAB_2,
    WIDX_PARK_NAME = 6,
    WIDX_SCENARIO_NAME,
    WIDX_CATEGORY,
    WIDX_CATEGORY_DROPDOWN,
    WIDX_DETAILS,
    WIDX_OBJECTIVE_DESCRIPTION,
    WIDX_PERMANENT_SCROLL,
    WIDX_PHASED_SCROLL,
    WIDX_ADD_OBJECTIVE_PERMANENT,
    WIDX_ADD_OBJECTIVE_PERMANENT_DROPDOWN,
    WIDX_ADD_OBJECTIVE_PHASED,
    WIDX_ADD_OBJECTIVE_PHASED_DROPDOWN,
    WIDX_ADD_TIMED_GOALGROUP,
    WIDX_EDIT_TIMED_GOALGROUP,
    WIDX_REMOVE_TIMED_GOALGROUP,
    WIDX_PREVIOUS_GOALGROUP,
    WIDX_NEXT_GOALGROUP,
    WIDX_SET_GOALGROUP_CURRENT,

    WIDX_RIDES = 6
};

#define MAIN_OBJECTIVE_OPTIONS_WIDGETS \
    WINDOW_SHIM(WINDOW_TITLE, WW, WH), \
    MakeWidget({  0,  43}, {280, 106}, WindowWidgetType::Resize, WindowColour::Secondary), \
    MakeTab   ({  3,  17}, STR_SELECT_OBJECTIVE_AND_PARK_NAME_TIP         ), \
    MakeTab   ({ 34,  17}, STR_SELECT_RIDES_TO_BE_PRESERVED_TIP           )

static rct_widget window_editor_objective_options_main_widgets[] = {
    MAIN_OBJECTIVE_OPTIONS_WIDGETS,
    MakeWidget        ({514,  48}, { 80,  12}, WindowWidgetType::Button,   WindowColour::Secondary, STR_CHANGE,         STR_CHANGE_NAME_OF_PARK_TIP                    ),
    MakeWidget        ({514, 65}, { 80,  12}, WindowWidgetType::Button,   WindowColour::Secondary, STR_CHANGE,         STR_CHANGE_NAME_OF_SCENARIO_TIP                ),
    MakeWidget        ({ 98, 82}, {180,  12}, WindowWidgetType::DropdownMenu, WindowColour::Secondary, STR_NONE,           STR_SELECT_WHICH_GROUP_THIS_SCENARIO_APPEARS_IN),
    MakeWidget        ({266, 83}, { 11,  10}, WindowWidgetType::Button,   WindowColour::Secondary, STR_DROPDOWN_GLYPH, STR_SELECT_WHICH_GROUP_THIS_SCENARIO_APPEARS_IN),
    MakeWidget        ({514, 99}, { 80,  12}, WindowWidgetType::Button,   WindowColour::Secondary, STR_CHANGE,         STR_CHANGE_DETAIL_NOTES_ABOUT_PARK_SCENARIO_TIP),
    MakeWidget        ({514, 177}, { 80,  12}, WindowWidgetType::Button,   WindowColour::Secondary, STR_CHANGE,         STR_CHANGE_OBJECTIVE_DESCRIPTION_TIP),
    
    MakeWidget        ({5, 295}, {292, 278}, WindowWidgetType::Scroll, WindowColour::Secondary, SCROLL_VERTICAL), //permanent goal scroll
    MakeWidget        ({303, 295}, {292, 278}, WindowWidgetType::Scroll, WindowColour::Secondary, SCROLL_VERTICAL), //phased goal scroll       
    MakeWidget({ 5,  578}, {292,  12}, WindowWidgetType::DropdownMenu, WindowColour::Primary, STR_ADD_OBJECTIVE,STR_ADD_OBJECTIVE     ), //add objective to permanent
    MakeWidget({ 5,  578}, {292,  12}, WindowWidgetType::Button,   WindowColour::Primary, STR_ADD_OBJECTIVE, STR_ADD_OBJECTIVE), 
    MakeWidget({ 303,  578}, {292,  12}, WindowWidgetType::DropdownMenu, WindowColour::Primary, STR_ADD_OBJECTIVE,STR_ADD_OBJECTIVE     ), //add objective to phased
    MakeWidget({ 303,  578}, {292,  12}, WindowWidgetType::Button,   WindowColour::Primary, STR_ADD_OBJECTIVE, STR_ADD_OBJECTIVE), 
    MakeWidget        ({458, 247}, { 80,  12}, WindowWidgetType::Button,   WindowColour::Secondary, STR_ADD_PHASE), //Add goalgroup
    MakeWidget        ({542, 235}, { 24,  24}, WindowWidgetType::ImgBtn,   WindowColour::Secondary, SPR_RENAME,         STR_EDIT), //Edit goalgroup
    MakeWidget        ({570, 235}, { 24,  24}, WindowWidgetType::ImgBtn,   WindowColour::Secondary, SPR_DEMOLISH,         STR_REMOVE), //Remove goalgroup
    MakeWidget        ({308, 263}, {24, 24}, WindowWidgetType::Button,   WindowColour::Secondary, STR_SMALLER_THAN_ARROW, STR_PREVIOUS_PHASE), //Previous goalgroup
    MakeWidget        ({570, 263}, {24, 24}, WindowWidgetType::Button,   WindowColour::Secondary, STR_BIGGER_THAN_ARROW, STR_NEXT_PHASE), //Next goalgroup
    MakeWidget        ({373, 247}, {80,  12}, WindowWidgetType::Button,   WindowColour::Secondary, STR_SET_GROUP_CURRENT), //Set goalgroup current
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
static void WindowEditorObjectiveOptionsScrollpaint(rct_window* w, rct_drawpixelinfo* dpi, int32_t scrollIndex);
static void WindowEditorObjectiveOptionsScrollgetheight(rct_window *w, int32_t scrollIndex, int32_t *width, int32_t *height);
static void WindowEditorObjectiveOptionsScrollmousedown(rct_window *w, int32_t scrollIndex, const ScreenCoordsXY& screenCoords);
static void WindowEditorObjectiveOptionsScrollmouseover(rct_window *w, int32_t scrollIndex, const ScreenCoordsXY& screenCoords);



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
    
    events.scroll_paint = &WindowEditorObjectiveOptionsScrollpaint;
    events.get_scroll_size = &WindowEditorObjectiveOptionsScrollgetheight;
    events.scroll_mousedown = &WindowEditorObjectiveOptionsScrollmousedown;
    events.scroll_mouseover = &WindowEditorObjectiveOptionsScrollmouseover;
    //events.cursor = &WindowEditorObjectiveOptionsCursor;
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

static uint64_t window_editor_objective_options_page_enabled_widgets[] = {
    (1ULL << WIDX_CLOSE) |
    (1ULL << WIDX_TAB_1) |
    (1ULL << WIDX_TAB_2) |
    (1ULL << WIDX_PARK_NAME) |
    (1ULL << WIDX_SCENARIO_NAME) |
    (1ULL << WIDX_CATEGORY) |
    (1ULL << WIDX_CATEGORY_DROPDOWN) |
    (1ULL << WIDX_DETAILS),

    (1ULL << WIDX_CLOSE) |
    (1ULL << WIDX_TAB_1) |
    (1ULL << WIDX_TAB_2),
};

static uint64_t window_editor_objective_options_page_hold_down_widgets[] = {
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

    TimedObjectiveIndex = 0;
    w = WindowCreateCentred(450, 228, &window_objective_options_main_events, WC_EDITOR_OBJECTIVE_OPTIONS, WF_10);
    w->widgets = window_editor_objective_options_main_widgets;
    w->pressed_widgets = 0;
    // w->hold_down_widgets = window_editor_objective_options_page_hold_down_widgets[WINDOW_EDITOR_OBJECTIVE_OPTIONS_PAGE_MAIN];
    WindowInitScrollWidgets(w);
    w->selected_tab = WINDOW_EDITOR_OBJECTIVE_OPTIONS_PAGE_MAIN;
    w->no_list_items = 0;
    w->selected_list_item = -1;
    WindowEditorObjectiveOptionsUpdateDisabledWidgets(w);

    if (!gScenarioObjective.MoneySettingsOkay())
    {
        // Don't reset objective, as that can now be quite a lot of work gone and resetted away
        context_show_error(STR_ERROR_OBJECTIVE_MONEY_SETTINGS_CHANGED, STR_NONE, {});
    }

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
    if (!(w->disabled_widgets & (1ULL << WIDX_TAB_2)))
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
    // w->hold_down_widgets = window_editor_objective_options_page_hold_down_widgets[page];
    w->event_handlers = window_editor_objective_options_page_events[page];
    w->widgets = window_editor_objective_options_widgets[page];
    w->Invalidate();
    WindowEditorObjectiveOptionsUpdateDisabledWidgets(w);
    window_event_resize_call(w);
    window_event_invalidate_call(w);
    WindowInitScrollWidgets(w);
    w->Invalidate();
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

static void WindowEditorObjectiveOptionsShowAddObjectiveDropdown(rct_window* w, rct_widget* dropdownWidget)
{
    int32_t i;

    for (i = 0; i < 28; i++)
    {
        gDropdownItems[i].Format = STR_DROPDOWN_MENU_LABEL;
        gDropdownItems[i].Args = GoalShortStringIds[i + 1]; // skip dummygoal
    }
    WindowDropdownShowTextCustomWidth(
        { w->windowPos.x + dropdownWidget->left, w->windowPos.y + dropdownWidget->top }, dropdownWidget->height() + 1,
        w->colours[1], 0, Dropdown::Flag::StayOpen, 29, dropdownWidget->width() - 3);
}

/**
 *
 *  rct2: 0x00671A0D
 */
static void WindowEditorObjectiveOptionsMainMousedown(rct_window* w, rct_widgetindex widgetIndex, rct_widget* widget)
{
    switch (widgetIndex)
    {
        case WIDX_CATEGORY_DROPDOWN:
            WindowEditorObjectiveOptionsShowCategoryDropdown(w);
            break;
        case WIDX_ADD_OBJECTIVE_PERMANENT_DROPDOWN:
        case WIDX_ADD_OBJECTIVE_PHASED_DROPDOWN:
            WindowEditorObjectiveOptionsShowAddObjectiveDropdown(w, widget);
            break;
    }
}
#pragma region Main Window

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
        case WIDX_OBJECTIVE_DESCRIPTION:
            WindowTextInputRawOpen(
                w, WIDX_OBJECTIVE_DESCRIPTION, STR_OBJECTIVE_DESCRIPTION, STR_ENTER_OBJECTIVE_DESCRIPTION, {},
                gScenarioObjectiveDescription.c_str(), 256);
            break;
        case WIDX_ADD_TIMED_GOALGROUP:
            WindowEditorObjectiveOptionsAddGroupOpen(nullptr, gScenarioObjective.PhasedGoals.size());
            break;
        case WIDX_EDIT_TIMED_GOALGROUP:
            WindowEditorObjectiveOptionsAddGroupOpen(&gScenarioObjective.PhasedGoals[TimedObjectiveIndex], TimedObjectiveIndex);
            break;
        case WIDX_NEXT_GOALGROUP:
            if (TimedObjectiveIndex < gScenarioObjective.PhasedGoals.size())
                TimedObjectiveIndex++;
            break;
        case WIDX_PREVIOUS_GOALGROUP:
            if (TimedObjectiveIndex > 0)
                TimedObjectiveIndex--;
            break;
        case WIDX_SET_GOALGROUP_CURRENT:
            gScenarioObjective.SetPhasedGoalIndex(TimedObjectiveIndex, true);
            break;
        case WIDX_REMOVE_TIMED_GOALGROUP:
            gScenarioObjective.RemovePhasedGoalGroup(TimedObjectiveIndex);
            if (TimedObjectiveIndex >= gScenarioObjective.PhasedGoals.size() && TimedObjectiveIndex != 0)
            {
                TimedObjectiveIndex--;
            }
            break;
    }
}

/**
 *
 *  rct2: 0x00672254
 */
static void WindowEditorObjectiveOptionsMainResize(rct_window* w)
{
    window_set_resize(w, 600, 595, 600, 595);
}

/**
 *
 *  rct2: 0x00671A54
 */
static void WindowEditorObjectiveOptionsMainDropdown(rct_window* w, rct_widgetindex widgetIndex, int32_t dropdownIndex)
{
    if (dropdownIndex == -1)
        return;

    switch (widgetIndex)
    {
        case WIDX_CATEGORY_DROPDOWN:
            if (gScenarioCategory != static_cast<uint8_t>(dropdownIndex + 1))
            {
                gScenarioCategory = static_cast<SCENARIO_CATEGORY>(dropdownIndex + 1); // skip dummygoal
                w->Invalidate();
            }
            break;
        case WIDX_ADD_OBJECTIVE_PHASED_DROPDOWN:
            if (gScenarioObjective.PhasedGoals.size() > 0)
                WindowEditorObjectiveOptionsAddObjectiveOpen(
                    &gScenarioObjective.PhasedGoals[TimedObjectiveIndex], static_cast<GoalID>(dropdownIndex + 1));
            else
                context_show_error(STR_ERROR_NO_PHASES, STR_NONE, {});
            break;
        case WIDX_ADD_OBJECTIVE_PERMANENT_DROPDOWN:
            WindowEditorObjectiveOptionsAddObjectiveOpen(
                &gScenarioObjective.PermanentGoals, static_cast<GoalID>(dropdownIndex + 1));
            break;
    }
}

/**
 *
 *  rct2: 0x006721E7
 */
static void WindowEditorObjectiveOptionsMainUpdate(rct_window* w)
{
    w->frame_no++;
    window_event_invalidate_call(w);
    widget_invalidate(w, WIDX_TAB_1);
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
        case WIDX_OBJECTIVE_DESCRIPTION:
            gScenarioObjectiveDescription = text;
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
    if (TimedObjectiveIndex < gScenarioObjective.PhasedGoals.size() - 1 && gScenarioObjective.PhasedGoals.size() > 0)
        window_editor_objective_options_main_widgets[WIDX_NEXT_GOALGROUP].type = WindowWidgetType::Button;
    else
        window_editor_objective_options_main_widgets[WIDX_NEXT_GOALGROUP].type = WindowWidgetType::Empty;

    if (TimedObjectiveIndex > 0 && gScenarioObjective.PhasedGoals.size() > 0)
        window_editor_objective_options_main_widgets[WIDX_PREVIOUS_GOALGROUP].type = WindowWidgetType::Button;
    else
        window_editor_objective_options_main_widgets[WIDX_PREVIOUS_GOALGROUP].type = WindowWidgetType::Empty;

    window_editor_objective_options_main_widgets[WIDX_CLOSE].type = (gScreenFlags & SCREEN_FLAGS_SCENARIO_EDITOR)
        ? WindowWidgetType::Empty
        : WindowWidgetType::CloseBox;
    window_editor_objective_options_main_widgets[WIDX_SET_GOALGROUP_CURRENT].type = (gScreenFlags & SCREEN_FLAGS_SCENARIO_EDITOR
                                                                                     || gScenarioObjective.PhasedGoals.size()
                                                                                         == 0)
        ? WindowWidgetType::Empty
        : WindowWidgetType::Button;

    WindowEditorObjectiveOptionsAnchorBorderWidgets(w);
}

/**
 *
 *  rct2: 0x0067161C
 */
static void WindowEditorObjectiveOptionsMainPaint(rct_window* w, rct_drawpixelinfo* dpi)
{
    int32_t width;
    WindowDrawWidgets(w, dpi);
    WindowEditorObjectiveOptionsDrawTabImages(w, dpi);

    // Objective label
    auto screenCoords = w->windowPos + ScreenCoordsXY{ 8, w->widgets[WIDX_SCENARIO_NAME].top };
    auto ft = Formatter();

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
    width = w->widgets[WIDX_DETAILS].left - 20;

    ft = Formatter();
    ft.Add<rct_string_id>(STR_STRING);
    ft.Add<const char*>(gScenarioDetails.c_str());
    DrawTextWrapped(dpi, screenCoords, width, STR_BLACK_STRING, ft);

    // Objective description label
    screenCoords = w->windowPos + ScreenCoordsXY{ 8, w->widgets[WIDX_OBJECTIVE_DESCRIPTION].top };
    DrawTextBasic(dpi, screenCoords, STR_OBJECTIVE_DESCRIPTION);

    // Objective description value
    screenCoords = w->windowPos + ScreenCoordsXY{ 16, w->widgets[WIDX_OBJECTIVE_DESCRIPTION].top + 10 };
    width = w->widgets[WIDX_OBJECTIVE_DESCRIPTION].left - 20;

    ft = Formatter();
    ft.Add<rct_string_id>(STR_STRING);
    ft.Add<const char*>(gScenarioObjectiveDescription.c_str());
    DrawTextWrapped(dpi, screenCoords, width, STR_BLACK_STRING, ft);

    // Scenario category label
    screenCoords = w->windowPos + ScreenCoordsXY{ 8, w->widgets[WIDX_CATEGORY].top };
    DrawTextBasic(dpi, screenCoords, STR_WINDOW_SCENARIO_GROUP);

    // Scenario category value
    screenCoords = w->windowPos + ScreenCoordsXY{ w->widgets[WIDX_CATEGORY].left + 1, w->widgets[WIDX_CATEGORY].top };
    ft = Formatter();
    ft.Add<rct_string_id>(ScenarioCategoryStringIds[gScenarioCategory]);
    DrawTextBasic(dpi, screenCoords, STR_WINDOW_COLOUR_2_STRINGID, ft);
    screenCoords = w->windowPos + ScreenCoordsXY{ 300, 263 };
    DrawTextBasic(dpi, screenCoords - ScreenCoordsXY{ 150, 0 }, STR_PERMANENT_OBJECTIVES, ft, { TextAlignment::CENTRE });
    ft = Formatter();
    if (gScenarioObjective.PhasedGoals.size() > 0)
        ft.Add<uint32_t>(TimedObjectiveIndex + 1);
    else
    {
        ft.Add<uint32_t>(0);
    }
    ft.Add<uint32_t>(gScenarioObjective.PhasedGoals.size());
    DrawTextBasic(dpi, screenCoords + ScreenCoordsXY{ 150, 0 }, STR_PHASED_OBJECTIVES, ft, { TextAlignment::CENTRE });

    screenCoords.y += LIST_ROW_HEIGHT * 1.2;
    if (gScenarioObjective.PhasedGoals.size() > 0)
    {
        ft = Formatter();
        ft.Add<rct_string_id>(STR_STRING);
        std::string s = gScenarioObjective.PhasedGoals[TimedObjectiveIndex].ToString(true);
        ft.Add<const char*>(s.c_str());
        DrawTextBasic(dpi, screenCoords + ScreenCoordsXY{ 150, 0 }, STR_BLACK_STRING, ft, { TextAlignment::CENTRE });
    }
    else
        DrawTextBasic(dpi, screenCoords + ScreenCoordsXY{ 150, 0 }, STR_OBJECTIVE_NO_TIMED, ft, { TextAlignment::CENTRE });

    if (!gScenarioObjective.PermanentGoals.Initialized())
        DrawTextBasic(dpi, screenCoords + ScreenCoordsXY{ -150, 0 }, STR_OBJECTIVE_NO_TIMED, ft, { TextAlignment::CENTRE });
}

static void WindowEditorObjectiveOptionsScrollgetheight(rct_window* w, int32_t scrollIndex, int32_t* width, int32_t* height)
{
    *height = 0;
    if (scrollIndex == 0)
    {
        *height += static_cast<int32_t>(gScenarioObjective.PermanentGoals.goals.size()) * LIST_ROW_HEIGHT;
    }
    else
    {
        if (gScenarioObjective.PhasedGoals.size() != 0)
        {
            *height += static_cast<int32_t>(gScenarioObjective.PhasedGoals[TimedObjectiveIndex].goals.size())
                * SCROLLABLE_ROW_HEIGHT;
        }
    }

    if (_highlightedIndexTimed)
    {
        _highlightedIndexTimed = {};
    }
    if (_highlightedIndexUntimed)
    {
        _highlightedIndexUntimed = {};
    }
}

static ObjectiveGoalPtr WindowEditorObjectiveOptionsGetItemFromScrollY(int32_t scrollIndex, int32_t y, uint32_t* index)
{
    if (scrollIndex == 1 && gScenarioObjective.PhasedGoals.size() == 0)
        return nullptr;

    auto& goalList = scrollIndex == 0 ? gScenarioObjective.PermanentGoals.goals
                                      : gScenarioObjective.PhasedGoals[TimedObjectiveIndex].goals;
    (*index) = 0;
    for (auto& goal : goalList)
    {
        y -= SCROLLABLE_ROW_HEIGHT;
        if (y < 0)
        {
            return goal;
        }
        (*index)++;
    }

    return nullptr;
}

static void WindowEditorObjectiveOptionsScrollmouseover(rct_window* w, int32_t scrollIndex, const ScreenCoordsXY& screenCoords)
{
    auto i = static_cast<size_t>(screenCoords.y / SCROLLABLE_ROW_HEIGHT);
    if (i != _highlightedIndexUntimed && scrollIndex == 0)
    {
        _highlightedIndexUntimed = static_cast<size_t>(i);
        w->Invalidate();
    }
    else if (i != _highlightedIndexTimed && scrollIndex == 1)
    {
        _highlightedIndexTimed = static_cast<size_t>(i);
        w->Invalidate();
    }
}

static void WindowEditorObjectiveOptionsScrollmousedown(rct_window* w, int32_t scrollIndex, const ScreenCoordsXY& screenCoords)
{
    ObjectiveGoalPtr goal;
    uint32_t index;
    goal = WindowEditorObjectiveOptionsGetItemFromScrollY(scrollIndex, screenCoords.y, &index);
    if (goal == nullptr)
        return;

    w->Invalidate();
    if (goal->GetGoalID() != GoalID::DummyGoal) // don't open dummies.
    {
        if (scrollIndex == 0)
            WindowEditorObjectiveOptionsEditObjectiveOpen(&gScenarioObjective.PermanentGoals, goal, index);
        else
            WindowEditorObjectiveOptionsEditObjectiveOpen(&gScenarioObjective.PhasedGoals[TimedObjectiveIndex], goal, index);
    }
}

static void WindowEditorObjectiveOptionsScrollpaint(rct_window* w, rct_drawpixelinfo* dpi, int32_t scrollIndex)
{
    // Draw background
    uint8_t paletteIndex = ColourMapA[w->colours[1]].mid_light;
    gfx_clear(dpi, paletteIndex);

    int32_t itemY = 0; //-SCROLLABLE_ROW_HEIGHT;
    int16_t boxWidth = w->widgets[WIDX_PHASED_SCROLL].width();

    if (scrollIndex == 1 && gScenarioObjective.PhasedGoals.size() == 0)
        return;

    const auto& goalGroup = scrollIndex == 0 ? gScenarioObjective.PermanentGoals
                                             : gScenarioObjective.PhasedGoals[TimedObjectiveIndex];

    size_t i = 0;
    for (const auto& goal : goalGroup.goals)
    {
        DrawTextEllipsised(dpi, { 1, itemY }, boxWidth, goal->ToStringShort());

        if (i == _highlightedIndexUntimed && scrollIndex == 0 || i == _highlightedIndexTimed && scrollIndex == 1)
        {
            gfx_filter_rect(dpi, { 0, itemY, boxWidth, itemY + (SCROLLABLE_ROW_HEIGHT - 1) }, FilterPaletteID::PaletteDarken1);
        }

        itemY += SCROLLABLE_ROW_HEIGHT;
        i++;
    }
}
#pragma endregion

#pragma region SelectRides
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
