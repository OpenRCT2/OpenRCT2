/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "../interface/Theme.h"

#include <openrct2-ui/interface/Dropdown.h>
#include <openrct2-ui/interface/Viewport.h>
#include <openrct2-ui/interface/Widget.h>
#include <openrct2-ui/windows/Window.h>
#include <openrct2/Context.h>
#include <openrct2/Game.h>
#include <openrct2/Input.h>
#include <openrct2/actions/PeepPickupAction.h>
#include <openrct2/actions/StaffSetCostumeAction.h>
#include <openrct2/actions/StaffSetNameAction.h>
#include <openrct2/actions/StaffSetOrdersAction.h>
#include <openrct2/actions/StaffSetPatrolAreaAction.h>
#include <openrct2/config/Config.h>
#include <openrct2/entity/EntityRegistry.h>
#include <openrct2/entity/Staff.h>
#include <openrct2/localisation/Formatter.h>
#include <openrct2/localisation/Localisation.h>
#include <openrct2/management/Finance.h>
#include <openrct2/network/network.h>
#include <openrct2/sprites.h>
#include <openrct2/windows/Intent.h>
#include <openrct2/world/Footpath.h>
#include <openrct2/world/Park.h>

static constexpr const rct_string_id WINDOW_TITLE = STR_STRINGID;
static constexpr const int32_t WW = 190;
static constexpr const int32_t WH = 180;

enum WindowStaffPage
{
    WINDOW_STAFF_OVERVIEW,
    WINDOW_STAFF_OPTIONS,
    WINDOW_STAFF_STATISTICS,
};

enum WindowStaffWidgetIdx
{
    WIDX_BACKGROUND,
    WIDX_TITLE,
    WIDX_CLOSE,
    WIDX_RESIZE,
    WIDX_TAB_1,
    WIDX_TAB_2,
    WIDX_TAB_3,

    WIDX_VIEWPORT = 7,
    WIDX_BTM_LABEL,
    WIDX_PICKUP,
    WIDX_PATROL,
    WIDX_RENAME,
    WIDX_LOCATE,
    WIDX_FIRE,

    WIDX_CHECKBOX_1 = 7,
    WIDX_CHECKBOX_2,
    WIDX_CHECKBOX_3,
    WIDX_CHECKBOX_4,
    WIDX_COSTUME_BOX,
    WIDX_COSTUME_BTN,
};

validate_global_widx(WC_PEEP, WIDX_PATROL);
validate_global_widx(WC_STAFF, WIDX_PICKUP);

// clang-format off
#define MAIN_STAFF_WIDGETS \
    WINDOW_SHIM(WINDOW_TITLE, WW, WH), \
    MakeWidget({ 0, 43}, {190, 137}, WindowWidgetType::Resize, WindowColour::Secondary), /* Resize */ \
    MakeTab   ({ 3, 17}, STR_STAFF_OVERVIEW_TIP                         ), /* Tab 1 */ \
    MakeTab   ({34, 17}, STR_STAFF_OPTIONS_TIP                          ), /* Tab 2 */ \
    MakeTab   ({65, 17}, STR_STAFF_STATS_TIP                            )  /* Tab 3 */

static rct_widget window_staff_overview_widgets[] = {
    MAIN_STAFF_WIDGETS,
    MakeWidget     ({      3,      47}, {162, 120}, WindowWidgetType::Viewport,      WindowColour::Secondary                                        ), // Viewport
    MakeWidget     ({      3, WH - 13}, {162,  11}, WindowWidgetType::LabelCentred, WindowColour::Secondary                                        ), // Label at bottom of viewport
    MakeWidget     ({WW - 25,      45}, { 24,  24}, WindowWidgetType::FlatBtn,       WindowColour::Secondary, SPR_PICKUP_BTN, STR_PICKUP_TIP        ), // Pickup Button
    MakeWidget     ({WW - 25,      69}, { 24,  24}, WindowWidgetType::FlatBtn,       WindowColour::Secondary, SPR_PATROL_BTN, STR_SET_PATROL_TIP    ), // Patrol Button
    MakeWidget     ({WW - 25,      93}, { 24,  24}, WindowWidgetType::FlatBtn,       WindowColour::Secondary, SPR_RENAME,     STR_NAME_STAFF_TIP    ), // Rename Button
    MakeWidget     ({WW - 25,     117}, { 24,  24}, WindowWidgetType::FlatBtn,       WindowColour::Secondary, SPR_LOCATE,     STR_LOCATE_SUBJECT_TIP), // Locate Button
    MakeWidget     ({WW - 25,     141}, { 24,  24}, WindowWidgetType::FlatBtn,       WindowColour::Secondary, SPR_DEMOLISH,   STR_FIRE_STAFF_TIP    ), // Fire Button
    WIDGETS_END,
};

//0x9AF910
static rct_widget window_staff_options_widgets[] = {
    MAIN_STAFF_WIDGETS,
    MakeWidget     ({      5,  50}, {180,  12}, WindowWidgetType::Checkbox, WindowColour::Secondary                                            ), // Checkbox 1
    MakeWidget     ({      5,  67}, {180,  12}, WindowWidgetType::Checkbox, WindowColour::Secondary                                            ), // Checkbox 2
    MakeWidget     ({      5,  84}, {180,  12}, WindowWidgetType::Checkbox, WindowColour::Secondary                                            ), // Checkbox 3
    MakeWidget     ({      5, 101}, {180,  12}, WindowWidgetType::Checkbox, WindowColour::Secondary                                            ), // Checkbox 4
    MakeWidget     ({      5,  50}, {180,  12}, WindowWidgetType::DropdownMenu, WindowColour::Secondary                                            ), // Costume Dropdown
    MakeWidget     ({WW - 17,  51}, { 11,  10}, WindowWidgetType::Button,   WindowColour::Secondary, STR_DROPDOWN_GLYPH, STR_SELECT_COSTUME_TIP), // Costume Dropdown Button
    WIDGETS_END,
};
// clang-format on

// 0x9AF9F4
static rct_widget window_staff_stats_widgets[] = {
    MAIN_STAFF_WIDGETS,
    WIDGETS_END,
};

static rct_widget* window_staff_page_widgets[] = {
    window_staff_overview_widgets,
    window_staff_options_widgets,
    window_staff_stats_widgets,
};

static void WindowStaffSetPage(rct_window* w, int32_t page);
static void WindowStaffDisableWidgets(rct_window* w);
static void WindowStaffUnknown05(rct_window* w);
static void WindowStaffViewportInit(rct_window* w);

static void WindowStaffOverviewClose(rct_window* w);
static void WindowStaffOverviewMouseup(rct_window* w, rct_widgetindex widgetIndex);
static void WindowStaffOverviewResize(rct_window* w);
static void WindowStaffOverviewMousedown(rct_window* w, rct_widgetindex widgetIndex, rct_widget* widget);
static void WindowStaffOverviewDropdown(rct_window* w, rct_widgetindex widgetIndex, int32_t dropdownIndex);
static void WindowStaffOverviewUpdate(rct_window* w);
static void WindowStaffOverviewInvalidate(rct_window* w);
static void WindowStaffOverviewPaint(rct_window* w, rct_drawpixelinfo* dpi);
static void WindowStaffOverviewTabPaint(rct_window* w, rct_drawpixelinfo* dpi);
static void WindowStaffOverviewToolUpdate(rct_window* w, rct_widgetindex widgetIndex, const ScreenCoordsXY& screenCoords);
static void WindowStaffOverviewToolDown(rct_window* w, rct_widgetindex widgetIndex, const ScreenCoordsXY& screenCoords);
static void WindowStaffOverviewToolDrag(rct_window* w, rct_widgetindex widgetIndex, const ScreenCoordsXY& screenCoords);
static void WindowStaffOverviewToolUp(rct_window* w, rct_widgetindex widgetIndex, const ScreenCoordsXY& screenCoords);
static void WindowStaffOverviewToolAbort(rct_window* w, rct_widgetindex widgetIndex);
static void WindowStaffOverviewTextInput(rct_window* w, rct_widgetindex widgetIndex, char* text);
static void WindowStaffOverviewViewportRotate(rct_window* w);
static void WindowStaffFollow(rct_window* w);
static void WindowStaffShowLocateDropdown(rct_window* w, rct_widget* widget);

static void WindowStaffOptionsMouseup(rct_window* w, rct_widgetindex widgetIndex);
static void WindowStaffOptionsUpdate(rct_window* w);
static void WindowStaffOptionsInvalidate(rct_window* w);
static void WindowStaffOptionsPaint(rct_window* w, rct_drawpixelinfo* dpi);
static void WindowStaffOptionsTabPaint(rct_window* w, rct_drawpixelinfo* dpi);
static void WindowStaffOptionsMousedown(rct_window* w, rct_widgetindex widgetIndex, rct_widget* widget);
static void WindowStaffOptionsDropdown(rct_window* w, rct_widgetindex widgetIndex, int32_t dropdownIndex);

static void WindowStaffStatsMouseup(rct_window* w, rct_widgetindex widgetIndex);
static void WindowStaffStatsResize(rct_window* w);
static void WindowStaffStatsUpdate(rct_window* w);
static void WindowStaffStatsInvalidate(rct_window* w);
static void WindowStaffStatsPaint(rct_window* w, rct_drawpixelinfo* dpi);
static void WindowStaffStatsTabPaint(rct_window* w, rct_drawpixelinfo* dpi);

void WindowStaffSetColours();

// 0x992AEC
static rct_window_event_list window_staff_overview_events([](auto& events) {
    events.close = &WindowStaffOverviewClose;
    events.mouse_up = &WindowStaffOverviewMouseup;
    events.resize = &WindowStaffOverviewResize;
    events.mouse_down = &WindowStaffOverviewMousedown;
    events.dropdown = &WindowStaffOverviewDropdown;
    events.update = &WindowStaffOverviewUpdate;
    events.tool_update = &WindowStaffOverviewToolUpdate;
    events.tool_down = &WindowStaffOverviewToolDown;
    events.tool_drag = &WindowStaffOverviewToolDrag;
    events.tool_up = &WindowStaffOverviewToolUp;
    events.tool_abort = &WindowStaffOverviewToolAbort;
    events.text_input = &WindowStaffOverviewTextInput;
    events.viewport_rotate = &WindowStaffOverviewViewportRotate;
    events.invalidate = &WindowStaffOverviewInvalidate;
    events.paint = &WindowStaffOverviewPaint;
});

// 0x992B5C
static rct_window_event_list window_staff_options_events([](auto& events) {
    events.mouse_up = &WindowStaffOptionsMouseup;
    events.resize = &WindowStaffStatsResize;
    events.mouse_down = &WindowStaffOptionsMousedown;
    events.dropdown = &WindowStaffOptionsDropdown;
    events.unknown_05 = &WindowStaffUnknown05;
    events.update = &WindowStaffOptionsUpdate;
    events.invalidate = &WindowStaffOptionsInvalidate;
    events.paint = &WindowStaffOptionsPaint;
});

// 0x992BCC
static rct_window_event_list window_staff_stats_events([](auto& events) {
    events.mouse_up = &WindowStaffStatsMouseup;
    events.resize = &WindowStaffStatsResize;
    events.unknown_05 = &WindowStaffUnknown05;
    events.update = &WindowStaffStatsUpdate;
    events.invalidate = &WindowStaffStatsInvalidate;
    events.paint = &WindowStaffStatsPaint;
});

static rct_window_event_list* window_staff_page_events[] = {
    &window_staff_overview_events,
    &window_staff_options_events,
    &window_staff_stats_events,
};

static EntertainerCostume _availableCostumes[static_cast<uint8_t>(EntertainerCostume::Count)];

enum class PatrolAreaValue
{
    UNSET = 0,
    SET = 1,
    NONE = -1,
};

static PatrolAreaValue _staffPatrolAreaPaintValue = PatrolAreaValue::NONE;

static Staff* GetStaff(rct_window* w)
{
    auto staff = GetEntity<Staff>(w->number);
    if (staff == nullptr)
    {
        window_close(w);
        return nullptr;
    }
    return staff;
}

/**
 *
 *  rct2: 0x006BEE98
 */
rct_window* WindowStaffOpen(Peep* peep)
{
    rct_window* w = window_bring_to_front_by_number(WC_PEEP, peep->sprite_index);
    if (w == nullptr)
    {
        w = WindowCreateAutoPos(WW, WH, &window_staff_overview_events, WC_PEEP, WF_10 | WF_RESIZABLE);

        w->number = peep->sprite_index;
        w->page = 0;
        w->frame_no = 0;
        w->highlighted_item = 0;

        WindowStaffDisableWidgets(w);

        w->min_width = WW;
        w->min_height = WH;
        w->max_width = 500;
        w->max_height = 450;
    }
    w->page = 0;
    w->Invalidate();

    w->widgets = window_staff_overview_widgets;
    w->hold_down_widgets = 0;
    w->event_handlers = window_staff_page_events[0];
    w->pressed_widgets = 0;
    WindowStaffDisableWidgets(w);
    WindowInitScrollWidgets(w);
    WindowStaffViewportInit(w);

    if (peep->State == PeepState::Picked)
        window_event_mouse_up_call(w, WIDX_CHECKBOX_3);

    return w;
}

/**
 * rct2: 0x006BED21
 * Disable the staff pickup if not in pickup state.
 */
void WindowStaffDisableWidgets(rct_window* w)
{
    const auto peep = GetStaff(w);
    if (peep == nullptr)
    {
        return;
    }
    uint64_t disabled_widgets = 0;

    if (peep != nullptr && peep->AssignedStaffType == StaffType::Security)
    {
        disabled_widgets |= (1ULL << WIDX_TAB_2);
    }

    if (w->page == WINDOW_STAFF_OVERVIEW)
    {
        if (peep->CanBePickedUp())
        {
            if (w->disabled_widgets & (1ULL << WIDX_PICKUP))
                w->Invalidate();
        }
        else
        {
            disabled_widgets |= (1ULL << WIDX_PICKUP);
            if (!(w->disabled_widgets & (1ULL << WIDX_PICKUP)))
                w->Invalidate();
        }
    }

    w->disabled_widgets = disabled_widgets;
}

/**
 * Same as window_peep_overview_close.
 *  rct2: 0x006BDFF8
 */
void WindowStaffOverviewClose(rct_window* w)
{
    if (input_test_flag(INPUT_FLAG_TOOL_ACTIVE))
    {
        if (w->classification == gCurrentToolWidget.window_classification && w->number == gCurrentToolWidget.window_number)
            tool_cancel();
    }
}

/**
 * Mostly similar to window_peep_set_page.
 *  rct2: 0x006BE023
 */
void WindowStaffSetPage(rct_window* w, int32_t page)
{
    if (input_test_flag(INPUT_FLAG_TOOL_ACTIVE))
    {
        if (w->number == gCurrentToolWidget.window_number && w->classification == gCurrentToolWidget.window_classification)
            tool_cancel();
    }

    int32_t listen = 0;
    if (page == WINDOW_STAFF_OVERVIEW && w->page == WINDOW_STAFF_OVERVIEW && w->viewport != nullptr)
    {
        if (!(w->viewport->flags & VIEWPORT_FLAG_SOUND_ON))
            listen = 1;
    }

    w->page = page;
    w->frame_no = 0;

    w->RemoveViewport();

    w->hold_down_widgets = 0;
    w->event_handlers = window_staff_page_events[page];
    w->pressed_widgets = 0;
    w->widgets = window_staff_page_widgets[page];

    WindowStaffDisableWidgets(w);
    w->Invalidate();

    window_event_resize_call(w);
    window_event_invalidate_call(w);

    WindowInitScrollWidgets(w);
    w->Invalidate();

    if (listen && w->viewport != nullptr)
        w->viewport->flags |= VIEWPORT_FLAG_SOUND_ON;
}

/**
 *
 *  rct2: 0x006BDF55
 */
void WindowStaffOverviewMouseup(rct_window* w, rct_widgetindex widgetIndex)
{
    const auto peep = GetStaff(w);
    if (peep == nullptr)
    {
        return;
    }

    switch (widgetIndex)
    {
        case WIDX_CLOSE:
            window_close(w);
            break;
        case WIDX_TAB_1:
        case WIDX_TAB_2:
        case WIDX_TAB_3:
            WindowStaffSetPage(w, widgetIndex - WIDX_TAB_1);
            break;
        case WIDX_PICKUP:
        {
            w->picked_peep_old_x = peep->x;
            CoordsXYZ nullLoc{};
            nullLoc.SetNull();
            PeepPickupAction pickupAction{ PeepPickupType::Pickup, w->number, nullLoc, network_get_current_player_id() };
            pickupAction.SetCallback([peepnum = w->number](const GameAction* ga, const GameActions::Result* result) {
                if (result->Error != GameActions::Status::Ok)
                    return;
                rct_window* wind = window_find_by_number(WC_PEEP, peepnum);
                if (wind != nullptr)
                {
                    tool_set(wind, WC_STAFF__WIDX_PICKUP, Tool::Picker);
                }
            });
            GameActions::Execute(&pickupAction);
        }
        break;
        case WIDX_FIRE:
        {
            auto intent = Intent(WC_FIRE_PROMPT);
            intent.putExtra(INTENT_EXTRA_PEEP, peep);
            context_open_intent(&intent);
            break;
        }
        case WIDX_RENAME:
        {
            auto peepName = peep->GetName();
            WindowTextInputRawOpen(
                w, widgetIndex, STR_STAFF_TITLE_STAFF_MEMBER_NAME, STR_STAFF_PROMPT_ENTER_NAME, {}, peepName.c_str(), 32);
            break;
        }
    }
}

/**
 *
 *  rct2: 0x006BE558
 */
void WindowStaffOverviewResize(rct_window* w)
{
    WindowStaffDisableWidgets(w);

    w->min_width = WW;
    w->max_width = 500;
    w->min_height = WH;
    w->max_height = 450;

    if (w->width < w->min_width)
    {
        w->width = w->min_width;
        w->Invalidate();
    }

    if (w->width > w->max_width)
    {
        w->Invalidate();
        w->width = w->max_width;
    }

    if (w->height < w->min_height)
    {
        w->height = w->min_height;
        w->Invalidate();
    }

    if (w->height > w->max_height)
    {
        w->Invalidate();
        w->height = w->max_height;
    }

    rct_viewport* viewport = w->viewport;
    if (viewport != nullptr)
    {
        int32_t new_width = w->width - 30;
        int32_t new_height = w->height - 62;

        // Update the viewport size
        if (viewport->width != new_width || viewport->height != new_height)
        {
            viewport->width = new_width;
            viewport->height = new_height;
            viewport->view_width = new_width * viewport->zoom;
            viewport->view_height = new_height * viewport->zoom;
        }
    }

    WindowStaffViewportInit(w);
}

/**
 * Handle the dropdown of patrol button.
 *  rct2: 0x006BDF98
 */
void WindowStaffOverviewMousedown(rct_window* w, rct_widgetindex widgetIndex, rct_widget* widget)
{
    switch (widgetIndex)
    {
        case WIDX_LOCATE:
            WindowStaffShowLocateDropdown(w, widget);
            break;
        case WIDX_PATROL:
        {
            // Dropdown names
            gDropdownItemsFormat[0] = STR_SET_PATROL_AREA;
            gDropdownItemsFormat[1] = STR_CLEAR_PATROL_AREA;

            auto dropdownPos = ScreenCoordsXY{ widget->left + w->windowPos.x, widget->top + w->windowPos.y };
            int32_t extray = widget->height() + 1;
            WindowDropdownShowText(dropdownPos, extray, w->colours[1], 0, 2);
            gDropdownDefaultIndex = 0;

            const auto peep = GetStaff(w);
            if (peep == nullptr)
            {
                return;
            }

            // Disable clear patrol area if no area is set.
            if (!peep->HasPatrolArea())
            {
                Dropdown::SetDisabled(1, true);
            }
        }
    }
}

/**
 *
 *  rct2: 0x006BDFA3
 */
void WindowStaffOverviewDropdown(rct_window* w, rct_widgetindex widgetIndex, int32_t dropdownIndex)
{
    switch (widgetIndex)
    {
        case WIDX_LOCATE:
        {
            if (dropdownIndex == 0)
            {
                w->ScrollToViewport();
            }
            else if (dropdownIndex == 1)
            {
                WindowStaffFollow(w);
            }
            break;
        }
        case WIDX_PATROL:
        {
            // Clear patrol
            if (dropdownIndex == 1)
            {
                const auto peep = GetStaff(w);
                if (peep == nullptr)
                {
                    return;
                }

                auto staffSetPatrolAreaAction = StaffSetPatrolAreaAction(
                    peep->sprite_index, {}, StaffSetPatrolAreaMode::ClearAll);
                GameActions::Execute(&staffSetPatrolAreaAction);
            }
            else
            {
                if (!tool_set(w, widgetIndex, Tool::WalkDown))
                {
                    show_gridlines();
                    gStaffDrawPatrolAreas = w->number;
                    gfx_invalidate_screen();
                }
            }
            break;
        }
    }
}

static void WindowStaffShowLocateDropdown(rct_window* w, rct_widget* widget)
{
    gDropdownItemsFormat[0] = STR_LOCATE_SUBJECT_TIP;
    gDropdownItemsFormat[1] = STR_FOLLOW_SUBJECT_TIP;

    WindowDropdownShowText(
        { w->windowPos.x + widget->left, w->windowPos.y + widget->top }, widget->height() + 1, w->colours[1], 0, 2);
    gDropdownDefaultIndex = 0;
}

static void WindowStaffFollow(rct_window* w)
{
    rct_window* w_main = window_get_main();
    window_follow_sprite(w_main, w->number);
}

/**
 * Update the animation frame of the tab icon.
 *  rct2: 0x6BE602
 */
void WindowStaffOverviewUpdate(rct_window* w)
{
    int32_t newAnimationFrame = w->var_496;
    newAnimationFrame++;
    if (newAnimationFrame >= 24)
    {
        newAnimationFrame = 0;
    }
    w->var_496 = newAnimationFrame;
    widget_invalidate(w, WIDX_TAB_1);
}

/**
 *
 *  rct2: 0x006BE814
 */
static void WindowStaffSetOrder(rct_window* w, int32_t order_id)
{
    const auto peep = GetStaff(w);
    if (peep == nullptr)
    {
        return;
    }

    uint8_t newOrders = peep->StaffOrders ^ (1 << order_id);
    auto staffSetOrdersAction = StaffSetOrdersAction(w->number, newOrders);
    GameActions::Execute(&staffSetOrdersAction);
}

/**
 *
 *  rct2: 0x006BE7DB
 */
void WindowStaffOptionsMouseup(rct_window* w, rct_widgetindex widgetIndex)
{
    switch (widgetIndex)
    {
        case WIDX_CLOSE:
            window_close(w);
            break;
        case WIDX_TAB_1:
        case WIDX_TAB_2:
        case WIDX_TAB_3:
            WindowStaffSetPage(w, widgetIndex - WIDX_TAB_1);
            break;
        case WIDX_CHECKBOX_1:
        case WIDX_CHECKBOX_2:
        case WIDX_CHECKBOX_3:
        case WIDX_CHECKBOX_4:
            WindowStaffSetOrder(w, widgetIndex - WIDX_CHECKBOX_1);
            break;
    }
}

/**
 *
 *  rct2: 0x006BE960
 */
void WindowStaffOptionsUpdate(rct_window* w)
{
    w->frame_no++;
    widget_invalidate(w, WIDX_TAB_2);
}

/**
 *
 *  rct2: 0x006BEBCF
 */
void WindowStaffStatsMouseup(rct_window* w, rct_widgetindex widgetIndex)
{
    switch (widgetIndex)
    {
        case WIDX_CLOSE:
            window_close(w);
            break;
        case WIDX_TAB_1:
        case WIDX_TAB_2:
        case WIDX_TAB_3:
            WindowStaffSetPage(w, widgetIndex - WIDX_TAB_1);
            break;
    }
}

/**
 *
 *  rct2: 0x006BEC1B, 0x006BE975
 */
void WindowStaffStatsResize(rct_window* w)
{
    w->min_width = 190;
    w->max_width = 190;
    w->min_height = 126;
    w->max_height = 126;

    if (w->width < w->min_width)
    {
        w->width = w->min_width;
        w->Invalidate();
    }

    if (w->width > w->max_width)
    {
        w->Invalidate();
        w->width = w->max_width;
    }

    if (w->height < w->min_height)
    {
        w->height = w->min_height;
        w->Invalidate();
    }

    if (w->height > w->max_height)
    {
        w->Invalidate();
        w->height = w->max_height;
    }
}

/**
 *
 *  rct2: 0x006BEBEA
 */
void WindowStaffStatsUpdate(rct_window* w)
{
    w->frame_no++;
    widget_invalidate(w, WIDX_TAB_3);

    auto peep = GetStaff(w);
    if (peep == nullptr)
    {
        return;
    }
    if (peep->WindowInvalidateFlags & PEEP_INVALIDATE_STAFF_STATS)
    {
        peep->WindowInvalidateFlags &= ~PEEP_INVALIDATE_STAFF_STATS;
        w->Invalidate();
    }
}

/**
 *
 *  rct2: 0x6BEC80, 0x6BE9DA
 */
void WindowStaffUnknown05(rct_window* w)
{
    widget_invalidate(w, WIDX_TAB_1);
}

/**
 *
 *  rct2: 0x006BE9E9
 */
void WindowStaffStatsInvalidate(rct_window* w)
{
    ColourSchemeUpdateByClass(w, static_cast<rct_windowclass>(WC_STAFF));

    if (window_staff_page_widgets[w->page] != w->widgets)
    {
        w->widgets = window_staff_page_widgets[w->page];
        WindowInitScrollWidgets(w);
    }

    w->pressed_widgets |= 1ULL << (w->page + WIDX_TAB_1);

    const auto peep = GetStaff(w);
    if (peep == nullptr)
    {
        return;
    }

    auto ft = Formatter::Common();
    peep->FormatNameTo(ft);

    window_staff_stats_widgets[WIDX_BACKGROUND].right = w->width - 1;
    window_staff_stats_widgets[WIDX_BACKGROUND].bottom = w->height - 1;

    window_staff_stats_widgets[WIDX_RESIZE].right = w->width - 1;
    window_staff_stats_widgets[WIDX_RESIZE].bottom = w->height - 1;

    window_staff_stats_widgets[WIDX_TITLE].right = w->width - 2;

    window_staff_stats_widgets[WIDX_CLOSE].left = w->width - 13;
    window_staff_stats_widgets[WIDX_CLOSE].right = w->width - 3;

    window_align_tabs(w, WIDX_TAB_1, WIDX_TAB_3);
}

/**
 *
 *  rct2: 0x006BE62B
 */
void WindowStaffOptionsInvalidate(rct_window* w)
{
    ColourSchemeUpdateByClass(w, static_cast<rct_windowclass>(WC_STAFF));

    if (window_staff_page_widgets[w->page] != w->widgets)
    {
        w->widgets = window_staff_page_widgets[w->page];
        WindowInitScrollWidgets(w);
    }

    w->pressed_widgets |= 1ULL << (w->page + WIDX_TAB_1);

    const auto peep = GetStaff(w);
    if (peep == nullptr)
    {
        return;
    }
    auto ft = Formatter::Common();
    peep->FormatNameTo(ft);

    switch (peep->AssignedStaffType)
    {
        case StaffType::Entertainer:
            window_staff_options_widgets[WIDX_CHECKBOX_1].type = WindowWidgetType::Empty;
            window_staff_options_widgets[WIDX_CHECKBOX_2].type = WindowWidgetType::Empty;
            window_staff_options_widgets[WIDX_CHECKBOX_3].type = WindowWidgetType::Empty;
            window_staff_options_widgets[WIDX_CHECKBOX_4].type = WindowWidgetType::Empty;
            window_staff_options_widgets[WIDX_COSTUME_BOX].type = WindowWidgetType::DropdownMenu;
            window_staff_options_widgets[WIDX_COSTUME_BTN].type = WindowWidgetType::Button;
            window_staff_options_widgets[WIDX_COSTUME_BOX].text = StaffCostumeNames[EnumValue(peep->SpriteType) - 4];
            break;
        case StaffType::Handyman:
            window_staff_options_widgets[WIDX_CHECKBOX_1].type = WindowWidgetType::Checkbox;
            window_staff_options_widgets[WIDX_CHECKBOX_1].text = STR_STAFF_OPTION_SWEEP_FOOTPATHS;
            window_staff_options_widgets[WIDX_CHECKBOX_2].type = WindowWidgetType::Checkbox;
            window_staff_options_widgets[WIDX_CHECKBOX_2].text = STR_STAFF_OPTION_WATER_GARDENS;
            window_staff_options_widgets[WIDX_CHECKBOX_3].type = WindowWidgetType::Checkbox;
            window_staff_options_widgets[WIDX_CHECKBOX_3].text = STR_STAFF_OPTION_EMPTY_LITTER;
            window_staff_options_widgets[WIDX_CHECKBOX_4].type = WindowWidgetType::Checkbox;
            window_staff_options_widgets[WIDX_CHECKBOX_4].text = STR_STAFF_OPTION_MOW_GRASS;
            window_staff_options_widgets[WIDX_COSTUME_BOX].type = WindowWidgetType::Empty;
            window_staff_options_widgets[WIDX_COSTUME_BTN].type = WindowWidgetType::Empty;
            w->pressed_widgets &= ~(
                (1ULL << WIDX_CHECKBOX_1) | (1ULL << WIDX_CHECKBOX_2) | (1ULL << WIDX_CHECKBOX_3) | (1ULL << WIDX_CHECKBOX_4));
            w->pressed_widgets |= peep->StaffOrders << WIDX_CHECKBOX_1;
            break;
        case StaffType::Mechanic:
            window_staff_options_widgets[WIDX_CHECKBOX_1].type = WindowWidgetType::Checkbox;
            window_staff_options_widgets[WIDX_CHECKBOX_1].text = STR_INSPECT_RIDES;
            window_staff_options_widgets[WIDX_CHECKBOX_2].type = WindowWidgetType::Checkbox;
            window_staff_options_widgets[WIDX_CHECKBOX_2].text = STR_FIX_RIDES;
            window_staff_options_widgets[WIDX_CHECKBOX_3].type = WindowWidgetType::Empty;
            window_staff_options_widgets[WIDX_CHECKBOX_4].type = WindowWidgetType::Empty;
            window_staff_options_widgets[WIDX_COSTUME_BOX].type = WindowWidgetType::Empty;
            window_staff_options_widgets[WIDX_COSTUME_BTN].type = WindowWidgetType::Empty;
            w->pressed_widgets &= ~((1ULL << WIDX_CHECKBOX_1) | (1ULL << WIDX_CHECKBOX_2));
            w->pressed_widgets |= peep->StaffOrders << WIDX_CHECKBOX_1;
            break;
        case StaffType::Security:
            // Security guards don't have an options screen.
            break;
        case StaffType::Count:
            break;
    }

    window_staff_options_widgets[WIDX_BACKGROUND].right = w->width - 1;
    window_staff_options_widgets[WIDX_BACKGROUND].bottom = w->height - 1;

    window_staff_options_widgets[WIDX_RESIZE].right = w->width - 1;
    window_staff_options_widgets[WIDX_RESIZE].bottom = w->height - 1;

    window_staff_options_widgets[WIDX_TITLE].right = w->width - 2;
    window_staff_options_widgets[WIDX_CLOSE].left = w->width - 13;
    window_staff_options_widgets[WIDX_CLOSE].right = w->width - 3;

    window_align_tabs(w, WIDX_TAB_1, WIDX_TAB_3);
}

/**
 *
 *  rct2: 0x006BDD91
 */
void WindowStaffOverviewInvalidate(rct_window* w)
{
    ColourSchemeUpdateByClass(w, static_cast<rct_windowclass>(WC_STAFF));

    if (window_staff_page_widgets[w->page] != w->widgets)
    {
        w->widgets = window_staff_page_widgets[w->page];
        WindowInitScrollWidgets(w);
    }

    w->pressed_widgets |= 1ULL << (w->page + WIDX_TAB_1);

    const auto peep = GetStaff(w);
    if (peep == nullptr)
    {
        return;
    }
    auto ft = Formatter::Common();
    peep->FormatNameTo(ft);

    window_staff_overview_widgets[WIDX_BACKGROUND].right = w->width - 1;
    window_staff_overview_widgets[WIDX_BACKGROUND].bottom = w->height - 1;

    window_staff_overview_widgets[WIDX_RESIZE].right = w->width - 1;
    window_staff_overview_widgets[WIDX_RESIZE].bottom = w->height - 1;

    window_staff_overview_widgets[WIDX_TITLE].right = w->width - 2;

    window_staff_overview_widgets[WIDX_VIEWPORT].right = w->width - 26;
    window_staff_overview_widgets[WIDX_VIEWPORT].bottom = w->height - 14;

    window_staff_overview_widgets[WIDX_BTM_LABEL].right = w->width - 26;
    window_staff_overview_widgets[WIDX_BTM_LABEL].top = w->height - 13;
    window_staff_overview_widgets[WIDX_BTM_LABEL].bottom = w->height - 3;

    window_staff_overview_widgets[WIDX_CLOSE].left = w->width - 13;
    window_staff_overview_widgets[WIDX_CLOSE].right = w->width - 3;

    window_staff_overview_widgets[WIDX_PICKUP].left = w->width - 25;
    window_staff_overview_widgets[WIDX_PICKUP].right = w->width - 2;

    window_staff_overview_widgets[WIDX_PATROL].left = w->width - 25;
    window_staff_overview_widgets[WIDX_PATROL].right = w->width - 2;

    window_staff_overview_widgets[WIDX_RENAME].left = w->width - 25;
    window_staff_overview_widgets[WIDX_RENAME].right = w->width - 2;

    window_staff_overview_widgets[WIDX_LOCATE].left = w->width - 25;
    window_staff_overview_widgets[WIDX_LOCATE].right = w->width - 2;

    window_staff_overview_widgets[WIDX_FIRE].left = w->width - 25;
    window_staff_overview_widgets[WIDX_FIRE].right = w->width - 2;

    window_align_tabs(w, WIDX_TAB_1, WIDX_TAB_3);
}

/**
 *
 *  rct2: 0x6BDEAF
 */
void WindowStaffOverviewPaint(rct_window* w, rct_drawpixelinfo* dpi)
{
    WindowDrawWidgets(w, dpi);
    WindowStaffOverviewTabPaint(w, dpi);
    WindowStaffOptionsTabPaint(w, dpi);
    WindowStaffStatsTabPaint(w, dpi);

    // Draw the viewport no sound sprite
    if (w->viewport != nullptr)
    {
        window_draw_viewport(dpi, w);
        rct_viewport* viewport = w->viewport;
        if (viewport->flags & VIEWPORT_FLAG_SOUND_ON)
        {
            gfx_draw_sprite(dpi, ImageId(SPR_HEARING_VIEWPORT), w->windowPos + ScreenCoordsXY{ 2, 2 });
        }
    }

    // Draw the centred label
    const auto peep = GetStaff(w);
    if (peep == nullptr)
    {
        return;
    }
    auto ft = Formatter();
    peep->FormatActionTo(ft);
    const auto& widget = w->widgets[WIDX_BTM_LABEL];
    auto screenPos = w->windowPos + ScreenCoordsXY{ widget.midX(), widget.top };
    int32_t width = widget.width();
    DrawTextEllipsised(dpi, screenPos, width, STR_BLACK_STRING, ft, { TextAlignment::CENTRE });
}

/**
 *
 *  rct2: 0x6BEC8F
 */
void WindowStaffOptionsTabPaint(rct_window* w, rct_drawpixelinfo* dpi)
{
    if (w->disabled_widgets & (1ULL << WIDX_TAB_2))
        return;

    int32_t image_id = SPR_TAB_STAFF_OPTIONS_0;
    if (w->page == WINDOW_STAFF_OPTIONS)
    {
        image_id += (w->frame_no / 2) % 7;
    }

    const auto& widget = w->widgets[WIDX_TAB_2];
    auto screenCoords = w->windowPos + ScreenCoordsXY{ widget.left, widget.top };
    gfx_draw_sprite(dpi, ImageId(image_id), screenCoords);
}

/**
 *
 *  rct2: 0x6BECD3
 */
void WindowStaffStatsTabPaint(rct_window* w, rct_drawpixelinfo* dpi)
{
    if (w->disabled_widgets & (1ULL << WIDX_TAB_3))
        return;

    int32_t image_id = SPR_TAB_STATS_0;
    if (w->page == WINDOW_STAFF_STATISTICS)
    {
        image_id += (w->frame_no / 4) % 7;
    }

    const auto& widget = w->widgets[WIDX_TAB_3];
    auto screenCoords = w->windowPos + ScreenCoordsXY{ widget.left, widget.top };
    gfx_draw_sprite(dpi, ImageId(image_id), screenCoords);
}

/**
 * Based on rct2: 0x6983dd in window_guest to be remerged into one when peep file added.
 */
void WindowStaffOverviewTabPaint(rct_window* w, rct_drawpixelinfo* dpi)
{
    if (w->disabled_widgets & (1ULL << WIDX_TAB_1))
        return;

    const auto& widget = w->widgets[WIDX_TAB_1];
    int32_t width = widget.width() - 1;
    int32_t height = widget.height() - 1;
    auto screenCoords = w->windowPos + ScreenCoordsXY{ widget.left + 1, widget.top + 1 };
    if (w->page == WINDOW_STAFF_OVERVIEW)
        height++;

    rct_drawpixelinfo clip_dpi;
    if (!clip_drawpixelinfo(&clip_dpi, dpi, screenCoords, width, height))
    {
        return;
    }

    screenCoords = ScreenCoordsXY{ 14, 20 };

    const auto peep = GetStaff(w);
    if (peep == nullptr)
    {
        return;
    }

    if (peep->Is<Staff>() && peep->AssignedStaffType == StaffType::Entertainer)
        screenCoords.y++;

    int32_t ebx = GetPeepAnimation(peep->SpriteType).base_image + 1;

    int32_t eax = 0;

    if (w->page == WINDOW_STAFF_OVERVIEW)
    {
        eax = w->var_496;
        eax &= 0xFFFC;
    }
    ebx += eax;

    gfx_draw_sprite(&clip_dpi, ImageId(ebx, peep->TshirtColour, peep->TrousersColour), screenCoords);
}

/**
 *
 *  rct2: 0x6BE7C6
 */
void WindowStaffOptionsPaint(rct_window* w, rct_drawpixelinfo* dpi)
{
    WindowDrawWidgets(w, dpi);
    WindowStaffOverviewTabPaint(w, dpi);
    WindowStaffOptionsTabPaint(w, dpi);
    WindowStaffStatsTabPaint(w, dpi);
}

/**
 *
 *  rct2: 0x6BEA86
 */
void WindowStaffStatsPaint(rct_window* w, rct_drawpixelinfo* dpi)
{
    WindowDrawWidgets(w, dpi);
    WindowStaffOverviewTabPaint(w, dpi);
    WindowStaffOptionsTabPaint(w, dpi);
    WindowStaffStatsTabPaint(w, dpi);

    const auto peep = GetStaff(w);
    if (peep == nullptr)
    {
        return;
    }

    auto screenCoords = w->windowPos
        + ScreenCoordsXY{ window_staff_stats_widgets[WIDX_RESIZE].left + 4, window_staff_stats_widgets[WIDX_RESIZE].top + 4 };

    if (!(gParkFlags & PARK_FLAGS_NO_MONEY))
    {
        auto ft = Formatter();
        ft.Add<money64>(GetStaffWage(peep->AssignedStaffType));
        DrawTextBasic(dpi, screenCoords, STR_STAFF_STAT_WAGES, ft);
        screenCoords.y += LIST_ROW_HEIGHT;
    }

    auto ft = Formatter();
    ft.Add<int32_t>(peep->GetHireDate());
    DrawTextBasic(dpi, screenCoords, STR_STAFF_STAT_EMPLOYED_FOR, ft);
    screenCoords.y += LIST_ROW_HEIGHT;

    switch (peep->AssignedStaffType)
    {
        case StaffType::Handyman:
            ft = Formatter();
            ft.Add<uint16_t>(peep->StaffLawnsMown);
            DrawTextBasic(dpi, screenCoords, STR_STAFF_STAT_LAWNS_MOWN, ft);
            screenCoords.y += LIST_ROW_HEIGHT;

            ft = Formatter();
            ft.Add<uint16_t>(peep->StaffGardensWatered);
            DrawTextBasic(dpi, screenCoords, STR_STAFF_STAT_GARDENS_WATERED, ft);
            screenCoords.y += LIST_ROW_HEIGHT;

            ft = Formatter();
            ft.Add<uint16_t>(peep->StaffLitterSwept);
            DrawTextBasic(dpi, screenCoords, STR_STAFF_STAT_LITTER_SWEPT, ft);
            screenCoords.y += LIST_ROW_HEIGHT;

            ft = Formatter();
            ft.Add<uint16_t>(peep->StaffBinsEmptied);
            DrawTextBasic(dpi, screenCoords, STR_STAFF_STAT_BINS_EMPTIED, ft);
            break;
        case StaffType::Mechanic:
            ft = Formatter();
            ft.Add<uint16_t>(peep->StaffRidesInspected);
            DrawTextBasic(dpi, screenCoords, STR_STAFF_STAT_RIDES_INSPECTED, ft);
            screenCoords.y += LIST_ROW_HEIGHT;

            ft = Formatter();
            ft.Add<uint16_t>(peep->StaffRidesFixed);
            DrawTextBasic(dpi, screenCoords, STR_STAFF_STAT_RIDES_FIXED, ft);
            break;
        case StaffType::Security:
            ft = Formatter();
            ft.Add<uint16_t>(peep->StaffVandalsStopped);
            DrawTextBasic(dpi, screenCoords, STR_STAFF_STAT_VANDALS_STOPPED, ft);
            break;
        case StaffType::Entertainer:
        case StaffType::Count:
            break;
    }
}

/**
 *
 *  rct2: 0x006BDFD8
 */
void WindowStaffOverviewToolUpdate(rct_window* w, rct_widgetindex widgetIndex, const ScreenCoordsXY& screenCoords)
{
    if (widgetIndex != WIDX_PICKUP)
        return;

    map_invalidate_selection_rect();

    gMapSelectFlags &= ~MAP_SELECT_FLAG_ENABLE;

    auto mapCoords = footpath_get_coordinates_from_pos({ screenCoords.x, screenCoords.y + 16 }, nullptr, nullptr);
    if (!mapCoords.IsNull())
    {
        gMapSelectFlags |= MAP_SELECT_FLAG_ENABLE;
        gMapSelectType = MAP_SELECT_TYPE_FULL;
        gMapSelectPositionA = mapCoords;
        gMapSelectPositionB = mapCoords;
        map_invalidate_selection_rect();
    }

    gPickupPeepImage = ImageId();

    auto info = get_map_coordinates_from_pos(screenCoords, ViewportInteractionItemAll);
    if (info.SpriteType == ViewportInteractionItem::None)
        return;

    gPickupPeepX = screenCoords.x - 1;
    gPickupPeepY = screenCoords.y + 16;
    w->picked_peep_frame++;
    if (w->picked_peep_frame >= 48)
    {
        w->picked_peep_frame = 0;
    }

    const auto peep = GetStaff(w);
    if (peep == nullptr)
    {
        return;
    }

    auto baseImageId = GetPeepAnimation(peep->SpriteType, PeepActionSpriteType::Ui).base_image;
    baseImageId += w->picked_peep_frame >> 2;
    gPickupPeepImage = ImageId(baseImageId, peep->TshirtColour, peep->TrousersColour);
}

/**
 *
 *  rct2: 0x006BDFC3
 */
void WindowStaffOverviewToolDown(rct_window* w, rct_widgetindex widgetIndex, const ScreenCoordsXY& screenCoords)
{
    if (widgetIndex == WIDX_PICKUP)
    {
        TileElement* tileElement;
        auto destCoords = footpath_get_coordinates_from_pos({ screenCoords.x, screenCoords.y + 16 }, nullptr, &tileElement);

        if (destCoords.IsNull())
            return;

        PeepPickupAction pickupAction{
            PeepPickupType::Place, w->number, { destCoords, tileElement->GetBaseZ() }, network_get_current_player_id()
        };
        pickupAction.SetCallback([](const GameAction* ga, const GameActions::Result* result) {
            if (result->Error != GameActions::Status::Ok)
                return;
            tool_cancel();
            gPickupPeepImage = ImageId();
        });
        GameActions::Execute(&pickupAction);
    }
    else if (widgetIndex == WIDX_PATROL)
    {
        auto destCoords = footpath_get_coordinates_from_pos(screenCoords, nullptr, nullptr);

        if (destCoords.IsNull())
            return;

        auto staff = TryGetEntity<Staff>(w->number);
        if (staff == nullptr)
            return;

        if (staff->IsPatrolAreaSet(destCoords))
        {
            _staffPatrolAreaPaintValue = PatrolAreaValue::UNSET;
        }
        else
        {
            _staffPatrolAreaPaintValue = PatrolAreaValue::SET;
        }
        auto staffSetPatrolAreaAction = StaffSetPatrolAreaAction(
            w->number, destCoords,
            _staffPatrolAreaPaintValue == PatrolAreaValue::SET ? StaffSetPatrolAreaMode::Set : StaffSetPatrolAreaMode::Unset);
        GameActions::Execute(&staffSetPatrolAreaAction);
    }
}

void WindowStaffOverviewToolDrag(rct_window* w, rct_widgetindex widgetIndex, const ScreenCoordsXY& screenCoords)
{
    if (widgetIndex != WIDX_PATROL)
        return;

    if (network_get_mode() != NETWORK_MODE_NONE)
        return;

    // This works only for singleplayer if the game_do_command can not be prevented
    // to send packets more often than patrol area is updated.

    if (_staffPatrolAreaPaintValue == PatrolAreaValue::NONE)
        return; // Do nothing if we do not have a paintvalue(this should never happen)

    auto destCoords = footpath_get_coordinates_from_pos(screenCoords, nullptr, nullptr);

    if (destCoords.IsNull())
        return;

    auto staff = TryGetEntity<Staff>(w->number);
    if (staff == nullptr)
        return;

    bool patrolAreaValue = staff->IsPatrolAreaSet(destCoords);
    if (_staffPatrolAreaPaintValue == PatrolAreaValue::SET && patrolAreaValue)
        return; // Since area is already the value we want, skip...
    if (_staffPatrolAreaPaintValue == PatrolAreaValue::UNSET && !patrolAreaValue)
        return; // Since area is already the value we want, skip...

    auto staffSetPatrolAreaAction = StaffSetPatrolAreaAction(
        w->number, destCoords,
        _staffPatrolAreaPaintValue == PatrolAreaValue::SET ? StaffSetPatrolAreaMode::Set : StaffSetPatrolAreaMode::Unset);
    GameActions::Execute(&staffSetPatrolAreaAction);
}

void WindowStaffOverviewToolUp(rct_window* w, rct_widgetindex widgetIndex, const ScreenCoordsXY& screenCoords)
{
    if (widgetIndex != WIDX_PATROL)
        return;

    _staffPatrolAreaPaintValue = PatrolAreaValue::NONE;
}

/**
 *
 *  rct2: 0x6BDFAE
 */
void WindowStaffOverviewToolAbort(rct_window* w, rct_widgetindex widgetIndex)
{
    if (widgetIndex == WIDX_PICKUP)
    {
        PeepPickupAction pickupAction{
            PeepPickupType::Cancel, w->number, { w->picked_peep_old_x, 0, 0 }, network_get_current_player_id()
        };
        GameActions::Execute(&pickupAction);
    }
    else if (widgetIndex == WIDX_PATROL)
    {
        hide_gridlines();
        gStaffDrawPatrolAreas = 0xFFFF;
        gfx_invalidate_screen();
    }
}

/* rct2: 0x6BDFED */
void WindowStaffOverviewTextInput(rct_window* w, rct_widgetindex widgetIndex, char* text)
{
    if (widgetIndex != WIDX_RENAME)
        return;

    if (text == nullptr)
        return;

    auto gameAction = StaffSetNameAction(w->number, text);
    GameActions::Execute(&gameAction);
}

/**
 *
 *  rct2: 0x006BE5FC
 */
void WindowStaffOverviewViewportRotate(rct_window* w)
{
    WindowStaffViewportInit(w);
}

/**
 *
 *  rct2: 0x006BEDA3
 */
void WindowStaffViewportInit(rct_window* w)
{
    if (w->page != WINDOW_STAFF_OVERVIEW)
        return;

    std::optional<Focus> focus;

    const auto peep = GetStaff(w);
    if (peep == nullptr)
    {
        return;
    }

    if (peep->State != PeepState::Picked)
    {
        focus = Focus(peep->sprite_index);
    }

    uint16_t viewport_flags;

    if (w->viewport != nullptr)
    {
        if (focus == w->focus)
            return;

        viewport_flags = w->viewport->flags;
        w->RemoveViewport();
    }
    else
    {
        viewport_flags = 0;
        if (gConfigGeneral.always_show_gridlines)
            viewport_flags |= VIEWPORT_FLAG_GRIDLINES;
    }

    window_event_invalidate_call(w);

    w->focus = focus;

    if (peep->State != PeepState::Picked)
    {
        if (w->viewport == nullptr)
        {
            const auto& view_widget = w->widgets[WIDX_VIEWPORT];

            auto screenPos = ScreenCoordsXY{ view_widget.left + 1 + w->windowPos.x, view_widget.top + 1 + w->windowPos.y };
            int32_t width = view_widget.width() - 1;
            int32_t height = view_widget.height() - 1;

            viewport_create(w, screenPos, width, height, focus.value());
            w->flags |= WF_NO_SCROLLING;
            w->Invalidate();
        }
    }

    if (w->viewport != nullptr)
        w->viewport->flags = viewport_flags;
    w->Invalidate();
}

/**
 * Handle the costume of staff member.
 * rct2: 0x006BE802
 */
void WindowStaffOptionsMousedown(rct_window* w, rct_widgetindex widgetIndex, rct_widget* widget)
{
    if (widgetIndex != WIDX_COSTUME_BTN)
    {
        return;
    }

    const auto peep = GetStaff(w);
    if (peep == nullptr)
    {
        return;
    }
    int32_t checkedIndex = -1;
    // This will be moved below where Items Checked is when all
    // of dropdown related functions are finished. This prevents
    // the dropdown from not working on first click.
    int32_t numCostumes = staff_get_available_entertainer_costume_list(_availableCostumes);
    for (int32_t i = 0; i < numCostumes; i++)
    {
        EntertainerCostume costume = _availableCostumes[i];
        if (peep->SpriteType == EntertainerCostumeToSprite(costume))
        {
            checkedIndex = i;
        }
        gDropdownItemsArgs[i] = StaffCostumeNames[static_cast<uint8_t>(costume)];
        gDropdownItemsFormat[i] = STR_DROPDOWN_MENU_LABEL;
    }

    // Get the dropdown box widget instead of button.
    widget--;

    auto dropdownPos = ScreenCoordsXY{ widget->left + w->windowPos.x, widget->top + w->windowPos.y };
    int32_t extray = widget->height() + 1;
    int32_t width = widget->width() - 3;
    WindowDropdownShowTextCustomWidth(dropdownPos, extray, w->colours[1], 0, Dropdown::Flag::StayOpen, numCostumes, width);

    // See above note.
    if (checkedIndex != -1)
    {
        Dropdown::SetChecked(checkedIndex, true);
    }
}

/**
 *
 *  rct2: 0x6BE809
 */
void WindowStaffOptionsDropdown(rct_window* w, rct_widgetindex widgetIndex, int32_t dropdownIndex)
{
    if (widgetIndex != WIDX_COSTUME_BTN)
    {
        return;
    }

    if (dropdownIndex == -1)
        return;

    EntertainerCostume costume = _availableCostumes[dropdownIndex];
    auto staffSetCostumeAction = StaffSetCostumeAction(w->number, costume);
    GameActions::Execute(&staffSetCostumeAction);
}
