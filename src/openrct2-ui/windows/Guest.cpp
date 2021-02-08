/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include <openrct2-ui/interface/Viewport.h>
#include <openrct2-ui/interface/Widget.h>
#include <openrct2-ui/windows/Window.h>
#include <openrct2/Context.h>
#include <openrct2/Game.h>
#include <openrct2/GameState.h>
#include <openrct2/Input.h>
#include <openrct2/actions/GuestSetFlagsAction.h>
#include <openrct2/actions/PeepPickupAction.h>
#include <openrct2/config/Config.h>
#include <openrct2/localisation/Localisation.h>
#include <openrct2/management/Marketing.h>
#include <openrct2/network/network.h>
#include <openrct2/peep/Peep.h>
#include <openrct2/peep/Staff.h>
#include <openrct2/ride/RideData.h>
#include <openrct2/ride/ShopItem.h>
#include <openrct2/scenario/Scenario.h>
#include <openrct2/sprites.h>
#include <openrct2/util/Util.h>
#include <openrct2/windows/Intent.h>
#include <openrct2/world/Footpath.h>
#include <openrct2/world/Park.h>

static constexpr const rct_string_id WINDOW_TITLE = STR_STRINGID;
static constexpr const int32_t WH = 157;
static constexpr const int32_t WW = 192;

// clang-format off
enum WINDOW_GUEST_PAGE {
    WINDOW_GUEST_OVERVIEW,
    WINDOW_GUEST_STATS,
    WINDOW_GUEST_RIDES,
    WINDOW_GUEST_FINANCE,
    WINDOW_GUEST_THOUGHTS,
    WINDOW_GUEST_INVENTORY,
    WINDOW_GUEST_DEBUG
};

enum WINDOW_GUEST_WIDGET_IDX {
    WIDX_BACKGROUND,
    WIDX_TITLE,
    WIDX_CLOSE,
    WIDX_PAGE_BACKGROUND,
    WIDX_TAB_1,
    WIDX_TAB_2,
    WIDX_TAB_3,
    WIDX_TAB_4,
    WIDX_TAB_5,
    WIDX_TAB_6,
    WIDX_TAB_7,

    WIDX_MARQUEE = 11,
    WIDX_VIEWPORT,
    WIDX_ACTION_LBL,
    WIDX_PICKUP,
    WIDX_RENAME,
    WIDX_LOCATE,
    WIDX_TRACK,

    WIDX_RIDE_SCROLL = 11
};

validate_global_widx(WC_PEEP, WIDX_PICKUP);

static constexpr int32_t TabWidth = 30;

#define MAIN_GUEST_WIDGETS \
    WINDOW_SHIM(WINDOW_TITLE, WW, WH), \
    MakeWidget({  0, 43}, {192, 114}, WindowWidgetType::Resize, WindowColour::Secondary), /* Resize */ \
    MakeTab   ({  3, 17}, STR_SHOW_GUEST_VIEW_TIP                        ), /* Tab 1 */ \
    MakeTab   ({ 34, 17}, STR_SHOW_GUEST_NEEDS_TIP                       ), /* Tab 2 */ \
    MakeTab   ({ 65, 17}, STR_SHOW_GUEST_VISITED_RIDES_TIP               ), /* Tab 3 */ \
    MakeTab   ({ 96, 17}, STR_SHOW_GUEST_FINANCE_TIP                     ), /* Tab 4 */ \
    MakeTab   ({127, 17}, STR_SHOW_GUEST_THOUGHTS_TIP                    ), /* Tab 5 */ \
    MakeTab   ({158, 17}, STR_SHOW_GUEST_ITEMS_TIP                       ), /* Tab 6 */ \
    MakeTab   ({189, 17}, STR_DEBUG_TIP                                  )  /* Tab 7 */

static rct_widget window_guest_overview_widgets[] = {
    MAIN_GUEST_WIDGETS,
    MakeWidget({  3,  45}, {164, 12}, WindowWidgetType::LabelCentred, WindowColour::Secondary                                               ), // Label Thought marquee
    MakeWidget({  3,  57}, {164, 87}, WindowWidgetType::Viewport,      WindowColour::Secondary                                               ), // Viewport
    MakeWidget({  3, 144}, {164, 11}, WindowWidgetType::LabelCentred, WindowColour::Secondary                                               ), // Label Action
    MakeWidget({167,  45}, { 24, 24}, WindowWidgetType::FlatBtn,       WindowColour::Secondary, SPR_PICKUP_BTN, STR_PICKUP_TIP               ), // Pickup Button
    MakeWidget({167,  69}, { 24, 24}, WindowWidgetType::FlatBtn,       WindowColour::Secondary, SPR_RENAME,     STR_NAME_GUEST_TIP           ), // Rename Button
    MakeWidget({167,  93}, { 24, 24}, WindowWidgetType::FlatBtn,       WindowColour::Secondary, SPR_LOCATE,     STR_LOCATE_SUBJECT_TIP       ), // Locate Button
    MakeWidget({167, 117}, { 24, 24}, WindowWidgetType::FlatBtn,       WindowColour::Secondary, SPR_TRACK_PEEP, STR_TOGGLE_GUEST_TRACKING_TIP), // Track Button
    { WIDGETS_END },
};

static rct_widget window_guest_stats_widgets[] = {
    MAIN_GUEST_WIDGETS,
    { WIDGETS_END },
};

static rct_widget window_guest_rides_widgets[] = {
    MAIN_GUEST_WIDGETS,
    MakeWidget({3, 57}, {186, 87}, WindowWidgetType::Scroll, WindowColour::Secondary, SCROLL_VERTICAL),
    { WIDGETS_END },
};

static rct_widget window_guest_finance_widgets[] = {
    MAIN_GUEST_WIDGETS,
    { WIDGETS_END },
};

static rct_widget window_guest_thoughts_widgets[] = {
    MAIN_GUEST_WIDGETS,
    { WIDGETS_END },
};

static rct_widget window_guest_inventory_widgets[] = {
    MAIN_GUEST_WIDGETS,
    { WIDGETS_END },
};

static rct_widget window_guest_debug_widgets[] = {
    MAIN_GUEST_WIDGETS,
    { WIDGETS_END },
};

// 0x981D0C
static rct_widget *window_guest_page_widgets[] = {
    window_guest_overview_widgets,
    window_guest_stats_widgets,
    window_guest_rides_widgets,
    window_guest_finance_widgets,
    window_guest_thoughts_widgets,
    window_guest_inventory_widgets,
    window_guest_debug_widgets
};

static void window_guest_set_page(rct_window* w, int32_t page);
static void window_guest_disable_widgets(rct_window* w);
static void window_guest_viewport_init(rct_window* w);
static void window_guest_common_resize(rct_window* w);
static void window_guest_common_invalidate(rct_window* w);

static void window_guest_overview_close(rct_window *w);
static void window_guest_overview_resize(rct_window *w);
static void window_guest_overview_mouse_up(rct_window *w, rct_widgetindex widgetIndex);
static void window_guest_overview_paint(rct_window *w, rct_drawpixelinfo *dpi);
static void window_guest_overview_invalidate(rct_window *w);
static void window_guest_overview_viewport_rotate(rct_window *w);
static void window_guest_overview_update(rct_window* w);
static void window_guest_overview_text_input(rct_window *w, rct_widgetindex widgetIndex, char *text);
static void window_guest_overview_tool_update(rct_window* w, rct_widgetindex widgetIndex, const ScreenCoordsXY& screenCoords);
static void window_guest_overview_tool_down(rct_window* w, rct_widgetindex widgetIndex, const ScreenCoordsXY& screenCoords);
static void window_guest_overview_tool_abort(rct_window *w, rct_widgetindex widgetIndex);

static void window_guest_mouse_up(rct_window *w, rct_widgetindex widgetIndex);

static void window_guest_stats_update(rct_window *w);
static void window_guest_stats_paint(rct_window *w, rct_drawpixelinfo *dpi);

static void window_guest_rides_update(rct_window *w);
static void window_guest_rides_scroll_get_size(rct_window *w, int32_t scrollIndex, int32_t *width, int32_t *height);
static void window_guest_rides_scroll_mouse_down(rct_window *w, int32_t scrollIndex, const ScreenCoordsXY& screenCoords);
static void window_guest_rides_scroll_mouse_over(rct_window *w, int32_t scrollIndex, const ScreenCoordsXY& screenCoords);
static void window_guest_rides_invalidate(rct_window *w);
static void window_guest_rides_paint(rct_window *w, rct_drawpixelinfo *dpi);
static void window_guest_rides_scroll_paint(rct_window *w, rct_drawpixelinfo *dpi, int32_t scrollIndex);

static void window_guest_finance_update(rct_window *w);
static void window_guest_finance_paint(rct_window *w, rct_drawpixelinfo *dpi);

static void window_guest_thoughts_update(rct_window *w);
static void window_guest_thoughts_paint(rct_window *w, rct_drawpixelinfo *dpi);

static void window_guest_inventory_update(rct_window *w);
static void window_guest_inventory_paint(rct_window *w, rct_drawpixelinfo *dpi);

static void window_guest_debug_update(rct_window *w);
static void window_guest_debug_paint(rct_window *w, rct_drawpixelinfo* dpi);

static rct_window_event_list window_guest_overview_events([](auto& events)
{
    events.close = &window_guest_overview_close;
    events.mouse_up = &window_guest_overview_mouse_up;
    events.resize = &window_guest_overview_resize;
    events.update = &window_guest_overview_update;
    events.tool_update = &window_guest_overview_tool_update;
    events.tool_down = &window_guest_overview_tool_down;
    events.tool_abort = &window_guest_overview_tool_abort;
    events.text_input = &window_guest_overview_text_input;
    events.viewport_rotate = &window_guest_overview_viewport_rotate;
    events.invalidate = &window_guest_overview_invalidate;
    events.paint = &window_guest_overview_paint;
});

static rct_window_event_list window_guest_stats_events([](auto& events)
{
    events.mouse_up = &window_guest_mouse_up;
    events.resize = &window_guest_common_resize;
    events.update = &window_guest_stats_update;
    events.invalidate = &window_guest_common_invalidate;
    events.paint = &window_guest_stats_paint;
});

static rct_window_event_list window_guest_rides_events([](auto& events)
{
    events.mouse_up = &window_guest_mouse_up;
    events.resize = &window_guest_common_resize;
    events.update = &window_guest_rides_update;
    events.get_scroll_size = &window_guest_rides_scroll_get_size;
    events.scroll_mousedown = &window_guest_rides_scroll_mouse_down;
    events.scroll_mouseover = &window_guest_rides_scroll_mouse_over;
    events.invalidate = &window_guest_rides_invalidate;
    events.paint = &window_guest_rides_paint;
    events.scroll_paint = &window_guest_rides_scroll_paint;
});

static rct_window_event_list window_guest_finance_events([](auto& events)
{
    events.mouse_up = &window_guest_mouse_up;
    events.resize = &window_guest_common_resize;
    events.update = &window_guest_finance_update;
    events.invalidate = &window_guest_common_invalidate;
    events.paint = &window_guest_finance_paint;
});

static rct_window_event_list window_guest_thoughts_events([](auto& events)
{
    events.mouse_up = &window_guest_mouse_up;
    events.resize = &window_guest_common_resize;
    events.update = &window_guest_thoughts_update;
    events.invalidate = &window_guest_common_invalidate;
    events.paint = &window_guest_thoughts_paint;
});

static rct_window_event_list window_guest_inventory_events([](auto& events)
{
    events.mouse_up = &window_guest_mouse_up;
    events.resize = &window_guest_common_resize;
    events.update = &window_guest_inventory_update;
    events.invalidate = &window_guest_common_invalidate;
    events.paint = &window_guest_inventory_paint;
});

static rct_window_event_list window_guest_debug_events([](auto& events)
{
    events.mouse_up = &window_guest_mouse_up;
    events.resize = &window_guest_common_resize;
    events.update = &window_guest_debug_update;
    events.invalidate = &window_guest_common_invalidate;
    events.paint = &window_guest_debug_paint;
});

// 0x981D24
static rct_window_event_list *window_guest_page_events[] = {
    &window_guest_overview_events,
    &window_guest_stats_events,
    &window_guest_rides_events,
    &window_guest_finance_events,
    &window_guest_thoughts_events,
    &window_guest_inventory_events,
    &window_guest_debug_events
};

void window_guest_set_colours();

// 0x981D3C
static constexpr const uint32_t window_guest_page_enabled_widgets[] = {
    (1 << WIDX_CLOSE) |
    (1 << WIDX_TAB_1) |
    (1 << WIDX_TAB_2) |
    (1 << WIDX_TAB_3) |
    (1 << WIDX_TAB_4) |
    (1 << WIDX_TAB_5) |
    (1 << WIDX_TAB_6) |
    (1 << WIDX_TAB_7) |
    (1 << WIDX_RENAME)|
    (1 << WIDX_PICKUP)|
    (1 << WIDX_LOCATE)|
    (1 << WIDX_TRACK),

    (1 << WIDX_CLOSE) |
    (1 << WIDX_TAB_1) |
    (1 << WIDX_TAB_2) |
    (1 << WIDX_TAB_3) |
    (1 << WIDX_TAB_4) |
    (1 << WIDX_TAB_5) |
    (1 << WIDX_TAB_6) |
    (1 << WIDX_TAB_7),

    (1 << WIDX_CLOSE) |
    (1 << WIDX_TAB_1) |
    (1 << WIDX_TAB_2) |
    (1 << WIDX_TAB_3) |
    (1 << WIDX_TAB_4) |
    (1 << WIDX_TAB_5) |
    (1 << WIDX_TAB_6) |
    (1 << WIDX_TAB_7) |
    (1 << WIDX_RIDE_SCROLL),

    (1 << WIDX_CLOSE) |
    (1 << WIDX_TAB_1) |
    (1 << WIDX_TAB_2) |
    (1 << WIDX_TAB_3) |
    (1 << WIDX_TAB_4) |
    (1 << WIDX_TAB_5) |
    (1 << WIDX_TAB_6) |
    (1 << WIDX_TAB_7),

    (1 << WIDX_CLOSE) |
    (1 << WIDX_TAB_1) |
    (1 << WIDX_TAB_2) |
    (1 << WIDX_TAB_3) |
    (1 << WIDX_TAB_4) |
    (1 << WIDX_TAB_5) |
    (1 << WIDX_TAB_6) |
    (1 << WIDX_TAB_7),

    (1 << WIDX_CLOSE) |
    (1 << WIDX_TAB_1) |
    (1 << WIDX_TAB_2) |
    (1 << WIDX_TAB_3) |
    (1 << WIDX_TAB_4) |
    (1 << WIDX_TAB_5) |
    (1 << WIDX_TAB_6) |
    (1 << WIDX_TAB_7),

    (1 << WIDX_CLOSE) |
    (1 << WIDX_TAB_1) |
    (1 << WIDX_TAB_2) |
    (1 << WIDX_TAB_3) |
    (1 << WIDX_TAB_4) |
    (1 << WIDX_TAB_5) |
    (1 << WIDX_TAB_6) |
    (1 << WIDX_TAB_7)
};

static constexpr const rct_size16 window_guest_page_sizes[][2] = {
    { 192, 159, 500, 450 },     // WINDOW_GUEST_OVERVIEW
    { 192, 180, 192, 180 },     // WINDOW_GUEST_STATS
    { 192, 180, 500, 400 },     // WINDOW_GUEST_RIDES
    { 210, 148, 210, 148 },     // WINDOW_GUEST_FINANCE
    { 192, 159, 500, 450 },     // WINDOW_GUEST_THOUGHTS
    { 192, 159, 500, 450 },     // WINDOW_GUEST_INVENTORY
    { 192, 171, 192, 171 }      // WINDOW_GUEST_DEBUG
};
// clang-format on

static Guest* GetGuest(rct_window* w)
{
    auto guest = GetEntity<Guest>(w->number);
    if (guest == nullptr)
    {
        window_close(w);
        return nullptr;
    }
    return guest;
}

/**
 *
 *  rct2: 0x006989E9
 *
 */
rct_window* window_guest_open(Peep* peep)
{
    if (peep == nullptr)
    {
        return nullptr;
    }
    if (peep->AssignedPeepType == PeepType::Staff)
    {
        return window_staff_open(peep);
    }

    rct_window* window;

    window = window_bring_to_front_by_number(WC_PEEP, peep->sprite_index);
    if (window == nullptr)
    {
        int32_t windowWidth = 192;
        if (gConfigGeneral.debugging_tools)
            windowWidth += TabWidth;

        window = WindowCreateAutoPos(windowWidth, 157, &window_guest_overview_events, WC_PEEP, WF_RESIZABLE);
        window->widgets = window_guest_overview_widgets;
        window->enabled_widgets = window_guest_page_enabled_widgets[0];
        window->number = peep->sprite_index;
        window->page = 0;
        window->viewport_focus_coordinates.y = 0;
        window->frame_no = 0;
        window->list_information_type = 0;
        window->picked_peep_frame = 0;
        window->highlighted_item = 0;
        window_guest_disable_widgets(window);
        window->min_width = windowWidth;
        window->min_height = 157;
        window->max_width = 500;
        window->max_height = 450;
        window->no_list_items = 0;
        window->selected_list_item = -1;

        window->viewport_focus_coordinates.y = -1;
    }

    window->page = 0;
    window->Invalidate();

    window->widgets = window_guest_page_widgets[WINDOW_GUEST_OVERVIEW];
    window->enabled_widgets = window_guest_page_enabled_widgets[WINDOW_GUEST_OVERVIEW];
    window->hold_down_widgets = 0;
    window->event_handlers = window_guest_page_events[WINDOW_GUEST_OVERVIEW];
    window->pressed_widgets = 0;

    window_guest_disable_widgets(window);
    WindowInitScrollWidgets(window);
    window_guest_viewport_init(window);

    return window;
}

static void window_guest_common_resize(rct_window* w)
{
    // Get page specific min and max size
    int32_t minWidth = window_guest_page_sizes[w->page][0].width;
    int32_t minHeight = window_guest_page_sizes[w->page][0].height;
    int32_t maxWidth = window_guest_page_sizes[w->page][1].width;
    int32_t maxHeight = window_guest_page_sizes[w->page][1].height;

    // Ensure min size is large enough for all tabs to fit
    for (int32_t i = WIDX_TAB_1; i <= WIDX_TAB_7; i++)
    {
        if (!(w->disabled_widgets & (1ULL << i)))
        {
            minWidth = std::max(minWidth, w->widgets[i].right + 3);
        }
    }
    maxWidth = std::max(minWidth, maxWidth);

    window_set_resize(w, minWidth, minHeight, maxWidth, maxHeight);
}

static void window_guest_common_invalidate(rct_window* w)
{
    if (window_guest_page_widgets[w->page] != w->widgets)
    {
        w->widgets = window_guest_page_widgets[w->page];
        WindowInitScrollWidgets(w);
    }

    w->pressed_widgets |= 1ULL << (w->page + WIDX_TAB_1);

    const auto peep = GetGuest(w);
    if (peep == nullptr)
    {
        return;
    }
    auto ft = Formatter::Common();
    peep->FormatNameTo(ft);

    w->widgets[WIDX_BACKGROUND].right = w->width - 1;
    w->widgets[WIDX_BACKGROUND].bottom = w->height - 1;
    w->widgets[WIDX_PAGE_BACKGROUND].right = w->width - 1;
    w->widgets[WIDX_PAGE_BACKGROUND].bottom = w->height - 1;
    w->widgets[WIDX_TITLE].right = w->width - 2;
    w->widgets[WIDX_CLOSE].left = w->width - 13;
    w->widgets[WIDX_CLOSE].right = w->width - 3;

    window_align_tabs(w, WIDX_TAB_1, WIDX_TAB_7);
}

/**
 * Disables the finance tab when no money.
 * Disables peep pickup when in certain no pickup states.
 *  rct2: 0x006987A6
 */
void window_guest_disable_widgets(rct_window* w)
{
    const auto peep = GetGuest(w);
    if (peep == nullptr)
    {
        return;
    }
    uint64_t disabled_widgets = 0;

    if (peep->CanBePickedUp())
    {
        if (w->disabled_widgets & (1 << WIDX_PICKUP))
            w->Invalidate();
    }
    else
    {
        disabled_widgets = (1 << WIDX_PICKUP);
        if (!(w->disabled_widgets & (1 << WIDX_PICKUP)))
            w->Invalidate();
    }
    if (gParkFlags & PARK_FLAGS_NO_MONEY)
    {
        disabled_widgets |= (1 << WIDX_TAB_4); // Disable finance tab if no money
    }
    if (!gConfigGeneral.debugging_tools)
    {
        disabled_widgets |= (1 << WIDX_TAB_7); // Disable debug tab when debug tools not turned on
    }
    w->disabled_widgets = disabled_widgets;
}

/**
 *
 *  rct2: 0x00696A75
 */
void window_guest_overview_close(rct_window* w)
{
    if (input_test_flag(INPUT_FLAG_TOOL_ACTIVE))
    {
        if (w->classification == gCurrentToolWidget.window_classification && w->number == gCurrentToolWidget.window_number)
            tool_cancel();
    }
}

/**
 *
 *  rct2: 0x00696FBE
 */
void window_guest_overview_resize(rct_window* w)
{
    window_guest_disable_widgets(w);
    window_event_invalidate_call(w);

    widget_invalidate(w, WIDX_MARQUEE);

    window_guest_common_resize(w);

    auto viewport = w->viewport;
    if (viewport != nullptr)
    {
        auto reqViewportWidth = w->width - 30;
        auto reqViewportHeight = w->height - 72;
        if (viewport->width != reqViewportWidth || viewport->height != reqViewportHeight)
        {
            viewport->width = reqViewportWidth;
            viewport->height = reqViewportHeight;
            viewport->view_width = viewport->width / viewport->zoom;
            viewport->view_height = viewport->height / viewport->zoom;
        }
    }
    window_guest_viewport_init(w);
}

/**
 *
 *  rct2: 0x00696A06
 */
void window_guest_overview_mouse_up(rct_window* w, rct_widgetindex widgetIndex)
{
    const auto peep = GetGuest(w);
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
        case WIDX_TAB_4:
        case WIDX_TAB_5:
        case WIDX_TAB_6:
        case WIDX_TAB_7:
            window_guest_set_page(w, widgetIndex - WIDX_TAB_1);
            break;
        case WIDX_PICKUP:
        {
            if (!peep->CanBePickedUp())
            {
                return;
            }
            w->picked_peep_old_x = peep->x;
            CoordsXYZ nullLoc{};
            nullLoc.setNull();
            PeepPickupAction pickupAction{ PeepPickupType::Pickup, w->number, nullLoc, network_get_current_player_id() };
            pickupAction.SetCallback([peepnum = w->number](const GameAction* ga, const GameActions::Result* result) {
                if (result->Error != GameActions::Status::Ok)
                    return;
                rct_window* wind = window_find_by_number(WC_PEEP, peepnum);
                if (wind)
                {
                    tool_set(wind, WC_PEEP__WIDX_PICKUP, Tool::Picker);
                }
            });
            GameActions::Execute(&pickupAction);
        }
        break;
        case WIDX_RENAME:
        {
            auto peepName = peep->GetName();
            window_text_input_raw_open(w, widgetIndex, STR_GUEST_RENAME_TITLE, STR_GUEST_RENAME_PROMPT, peepName.c_str(), 32);
            break;
        }
        case WIDX_LOCATE:
            w->ScrollToViewport();
            break;
        case WIDX_TRACK:
        {
            uint32_t flags = peep->PeepFlags ^ PEEP_FLAGS_TRACKING;

            auto guestSetFlagsAction = GuestSetFlagsAction(w->number, flags);
            GameActions::Execute(&guestSetFlagsAction);
        }
        break;
    }
}

/**
 *
 *  rct2: 0x696AA0
 */
void window_guest_set_page(rct_window* w, int32_t page)
{
    if (input_test_flag(INPUT_FLAG_TOOL_ACTIVE))
    {
        if (w->number == gCurrentToolWidget.window_number && w->classification == gCurrentToolWidget.window_classification)
            tool_cancel();
    }
    int32_t listen = 0;
    if (page == WINDOW_GUEST_OVERVIEW && w->page == WINDOW_GUEST_OVERVIEW && w->viewport)
    {
        if (!(w->viewport->flags & VIEWPORT_FLAG_SOUND_ON))
            listen = 1;
    }

    w->page = page;
    w->frame_no = 0;
    w->no_list_items = 0;
    w->selected_list_item = -1;

    w->RemoveViewport();

    w->enabled_widgets = window_guest_page_enabled_widgets[page];
    w->hold_down_widgets = 0;
    w->event_handlers = window_guest_page_events[page];
    w->pressed_widgets = 0;
    w->widgets = window_guest_page_widgets[page];
    window_guest_disable_widgets(w);
    w->Invalidate();
    window_event_resize_call(w);
    window_event_invalidate_call(w);
    WindowInitScrollWidgets(w);
    w->Invalidate();

    if (listen && w->viewport)
        w->viewport->flags |= VIEWPORT_FLAG_SOUND_ON;
}

void window_guest_overview_viewport_rotate(rct_window* w)
{
    window_guest_viewport_init(w);
}

/**
 *
 *  rct2: 0x0069883C
 */
void window_guest_viewport_init(rct_window* w)
{
    if (w->page != WINDOW_GUEST_OVERVIEW)
        return;

    const auto peep = GetGuest(w);
    if (peep == nullptr)
    {
        return;
    }

    auto focus = viewport_update_smart_guest_follow(w, peep);
    bool reCreateViewport = false;
    uint16_t origViewportFlags{};
    if (w->viewport != nullptr)
    {
        // Check all combos, for now skipping y and rot
        if (focus.coordinate.x == w->viewport_focus_coordinates.x
            && (focus.coordinate.y & VIEWPORT_FOCUS_Y_MASK) == w->viewport_focus_coordinates.y
            && focus.coordinate.z == w->viewport_focus_coordinates.z
            && focus.coordinate.rotation == w->viewport_focus_coordinates.rotation)
            return;

        origViewportFlags = w->viewport->flags;

        reCreateViewport = true;
        w->RemoveViewport();
    }

    window_event_invalidate_call(w);

    w->viewport_focus_coordinates.x = focus.coordinate.x;
    w->viewport_focus_coordinates.y = focus.coordinate.y;
    w->viewport_focus_coordinates.z = focus.coordinate.z;
    w->viewport_focus_coordinates.rotation = focus.coordinate.rotation;

    if (peep->State != PeepState::Picked && w->viewport == nullptr)
    {
        auto view_widget = &w->widgets[WIDX_VIEWPORT];
        auto screenPos = ScreenCoordsXY{ view_widget->left + 1 + w->windowPos.x, view_widget->top + 1 + w->windowPos.y };
        int32_t width = view_widget->width() - 1;
        int32_t height = view_widget->height() - 1;

        viewport_create(
            w, screenPos, width, height, 0,
            { focus.coordinate.x, focus.coordinate.y & VIEWPORT_FOCUS_Y_MASK, focus.coordinate.z },
            focus.sprite.type & VIEWPORT_FOCUS_TYPE_MASK, focus.sprite.sprite_id);
        if (w->viewport != nullptr && reCreateViewport)
        {
            w->viewport->flags = origViewportFlags;
        }
        w->flags |= WF_NO_SCROLLING;
        w->Invalidate();
    }
    w->Invalidate();
}

/**
 *
 *  rct2: 0x6983dd
 * used by window_staff as well
 */
static void window_guest_overview_tab_paint(rct_window* w, rct_drawpixelinfo* dpi)
{
    if (w->disabled_widgets & (1 << WIDX_TAB_1))
        return;

    rct_widget* widget = &w->widgets[WIDX_TAB_1];
    int32_t width = widget->width() - 1;
    int32_t height = widget->height() - 1;
    auto screenCoords = w->windowPos + ScreenCoordsXY{ widget->left + 1, widget->top + 1 };
    if (w->page == WINDOW_GUEST_OVERVIEW)
        height++;

    rct_drawpixelinfo clip_dpi;
    if (!clip_drawpixelinfo(&clip_dpi, dpi, screenCoords, width, height))
    {
        return;
    }

    screenCoords = ScreenCoordsXY{ 14, 20 };

    const Peep* peep = GetEntity<Peep>(w->number);
    if (peep == nullptr)
    {
        window_close(w);
        return;
    }

    if (peep->AssignedPeepType == PeepType::Staff && peep->AssignedStaffType == StaffType::Entertainer)
        screenCoords.y++;

    int32_t animationFrame = GetPeepAnimation(peep->SpriteType).base_image + 1;

    int32_t animationFrameOffset = 0;

    if (w->page == WINDOW_GUEST_OVERVIEW)
    {
        animationFrameOffset = w->var_496;
        animationFrameOffset &= 0xFFFC;
    }
    animationFrame += animationFrameOffset;

    int32_t sprite_id = animationFrame | SPRITE_ID_PALETTE_COLOUR_2(peep->TshirtColour, peep->TrousersColour);
    gfx_draw_sprite(&clip_dpi, sprite_id, screenCoords, 0);

    // If holding a balloon
    if (animationFrame >= 0x2A1D && animationFrame < 0x2A3D)
    {
        animationFrame += 32;
        animationFrame |= SPRITE_ID_PALETTE_COLOUR_1(peep->BalloonColour);
        gfx_draw_sprite(&clip_dpi, animationFrame, screenCoords, 0);
    }

    // If holding umbrella
    if (animationFrame >= 0x2BBD && animationFrame < 0x2BDD)
    {
        animationFrame += 32;
        animationFrame |= SPRITE_ID_PALETTE_COLOUR_1(peep->UmbrellaColour);
        gfx_draw_sprite(&clip_dpi, animationFrame, screenCoords, 0);
    }

    // If wearing hat
    if (animationFrame >= 0x29DD && animationFrame < 0x29FD)
    {
        animationFrame += 32;
        animationFrame |= SPRITE_ID_PALETTE_COLOUR_1(peep->HatColour);
        gfx_draw_sprite(&clip_dpi, animationFrame, screenCoords, 0);
    }
}

/**
 *
 *  rct2: 0x69869b
 */
static void window_guest_stats_tab_paint(rct_window* w, rct_drawpixelinfo* dpi)
{
    if (w->disabled_widgets & (1 << WIDX_TAB_2))
        return;

    rct_widget* widget = &w->widgets[WIDX_TAB_2];
    auto screenCoords = w->windowPos + ScreenCoordsXY{ widget->left, widget->top };

    const auto peep = GetGuest(w);
    if (peep == nullptr)
    {
        return;
    }
    int32_t image_id = get_peep_face_sprite_large(peep);
    if (w->page == WINDOW_GUEST_STATS)
    {
        // If currently viewing this tab animate tab
        // if it is very sick or angry.
        switch (image_id)
        {
            case SPR_PEEP_LARGE_FACE_VERY_VERY_SICK_0:
                image_id += (w->frame_no / 4) & 0xF;
                break;
            case SPR_PEEP_LARGE_FACE_VERY_SICK_0:
                image_id += (w->frame_no / 8) & 0x3;
                break;
            case SPR_PEEP_LARGE_FACE_ANGRY_0:
                image_id += (w->frame_no / 8) & 0x3;
                break;
        }
    }
    gfx_draw_sprite(dpi, image_id, screenCoords, 0);
}

/**
 *
 *  rct2: 0x69861F
 */
static void window_guest_rides_tab_paint(rct_window* w, rct_drawpixelinfo* dpi)
{
    if (w->disabled_widgets & (1 << WIDX_TAB_3))
        return;

    rct_widget* widget = &w->widgets[WIDX_TAB_3];
    auto screenCoords = w->windowPos + ScreenCoordsXY{ widget->left, widget->top };

    int32_t image_id = SPR_TAB_RIDE_0;

    if (w->page == WINDOW_GUEST_RIDES)
    {
        image_id += (w->frame_no / 4) & 0xF;
    }

    gfx_draw_sprite(dpi, image_id, screenCoords, 0);
}

/**
 *
 *  rct2: 0x698597
 */
static void window_guest_finance_tab_paint(rct_window* w, rct_drawpixelinfo* dpi)
{
    if (w->disabled_widgets & (1 << WIDX_TAB_4))
        return;

    rct_widget* widget = &w->widgets[WIDX_TAB_4];
    auto screenCoords = w->windowPos + ScreenCoordsXY{ widget->left, widget->top };

    int32_t image_id = SPR_TAB_FINANCES_SUMMARY_0;

    if (w->page == WINDOW_GUEST_FINANCE)
    {
        image_id += (w->frame_no / 2) & 0x7;
    }

    gfx_draw_sprite(dpi, image_id, screenCoords, 0);
}

/**
 *
 *  rct2: 0x6985D8
 */
static void window_guest_thoughts_tab_paint(rct_window* w, rct_drawpixelinfo* dpi)
{
    if (w->disabled_widgets & (1 << WIDX_TAB_5))
        return;

    rct_widget* widget = &w->widgets[WIDX_TAB_5];
    auto screenCoords = w->windowPos + ScreenCoordsXY{ widget->left, widget->top };

    int32_t image_id = SPR_TAB_THOUGHTS_0;

    if (w->page == WINDOW_GUEST_THOUGHTS)
    {
        image_id += (w->frame_no / 2) & 0x7;
    }

    gfx_draw_sprite(dpi, image_id, screenCoords, 0);
}

/**
 *
 *  rct2: 0x698661
 */
static void window_guest_inventory_tab_paint(rct_window* w, rct_drawpixelinfo* dpi)
{
    if (w->disabled_widgets & (1 << WIDX_TAB_6))
        return;

    rct_widget* widget = &w->widgets[WIDX_TAB_6];
    auto screenCoords = w->windowPos + ScreenCoordsXY{ widget->left, widget->top };

    int32_t image_id = SPR_TAB_GUEST_INVENTORY;

    gfx_draw_sprite(dpi, image_id, screenCoords, 0);
}

static void window_guest_debug_tab_paint(rct_window* w, rct_drawpixelinfo* dpi)
{
    if (w->disabled_widgets & (1 << WIDX_TAB_7))
        return;

    rct_widget* widget = &w->widgets[WIDX_TAB_7];
    auto screenCoords = w->windowPos + ScreenCoordsXY{ widget->left, widget->top };

    int32_t image_id = SPR_TAB_GEARS_0;
    if (w->page == WINDOW_GUEST_DEBUG)
    {
        image_id += (w->frame_no / 2) & 0x3;
    }

    gfx_draw_sprite(dpi, image_id, screenCoords, 0);
}

/**
 *
 *  rct2: 0x696887
 */
void window_guest_overview_paint(rct_window* w, rct_drawpixelinfo* dpi)
{
    WindowDrawWidgets(w, dpi);
    window_guest_overview_tab_paint(w, dpi);
    window_guest_stats_tab_paint(w, dpi);
    window_guest_rides_tab_paint(w, dpi);
    window_guest_finance_tab_paint(w, dpi);
    window_guest_thoughts_tab_paint(w, dpi);
    window_guest_inventory_tab_paint(w, dpi);
    window_guest_debug_tab_paint(w, dpi);

    // Draw the viewport no sound sprite
    if (w->viewport)
    {
        window_draw_viewport(dpi, w);
        rct_viewport* viewport = w->viewport;
        if (viewport->flags & VIEWPORT_FLAG_SOUND_ON)
        {
            gfx_draw_sprite(dpi, SPR_HEARING_VIEWPORT, w->windowPos + ScreenCoordsXY{ 2, 2 }, 0);
        }
    }

    // Draw the centred label
    const auto peep = GetGuest(w);
    if (peep == nullptr)
    {
        return;
    }

    rct_widget* widget = &w->widgets[WIDX_ACTION_LBL];
    auto screenPos = w->windowPos + ScreenCoordsXY{ widget->midX(), widget->top - 1 };

    {
        auto ft = Formatter();
        peep->FormatActionTo(ft);
        int32_t width = widget->width();
        DrawTextEllipsised(dpi, screenPos, width, STR_BLACK_STRING, ft, COLOUR_BLACK, TextAlignment::CENTRE);
    }

    // Draw the marquee thought
    widget = &w->widgets[WIDX_MARQUEE];
    auto width = widget->width() - 3;
    int32_t left = widget->left + 2 + w->windowPos.x;
    int32_t top = widget->top + w->windowPos.y;
    int32_t height = widget->height();
    rct_drawpixelinfo dpi_marquee;
    if (!clip_drawpixelinfo(&dpi_marquee, dpi, { left, top }, width, height))
    {
        return;
    }

    int32_t i = 0;
    for (; i < PEEP_MAX_THOUGHTS; ++i)
    {
        if (peep->Thoughts[i].type == PeepThoughtType::None)
        {
            w->list_information_type = 0;
            return;
        }
        if (peep->Thoughts[i].freshness == 1)
        { // If a fresh thought
            break;
        }
    }
    if (i == PEEP_MAX_THOUGHTS)
    {
        w->list_information_type = 0;
        return;
    }

    screenPos.x = widget->width() - w->list_information_type;
    {
        auto ft = Formatter();
        peep_thought_set_format_args(&peep->Thoughts[i], ft);
        DrawTextBasic(&dpi_marquee, { screenPos.x, 0 }, STR_WINDOW_COLOUR_2_STRINGID, ft, COLOUR_BLACK);
    }
}

/**
 *
 *  rct2: 0x696749
 */
void window_guest_overview_invalidate(rct_window* w)
{
    window_guest_common_invalidate(w);

    const auto peep = GetGuest(w);
    if (peep == nullptr)
    {
        return;
    }
    w->pressed_widgets &= ~(1 << WIDX_TRACK);
    if (peep->PeepFlags & PEEP_FLAGS_TRACKING)
    {
        w->pressed_widgets |= (1 << WIDX_TRACK);
    }

    window_guest_overview_widgets[WIDX_VIEWPORT].right = w->width - 26;
    window_guest_overview_widgets[WIDX_VIEWPORT].bottom = w->height - 14;

    window_guest_overview_widgets[WIDX_ACTION_LBL].top = w->height - 12;
    window_guest_overview_widgets[WIDX_ACTION_LBL].bottom = w->height - 3;
    window_guest_overview_widgets[WIDX_ACTION_LBL].right = w->width - 24;

    window_guest_overview_widgets[WIDX_MARQUEE].right = w->width - 24;

    window_guest_overview_widgets[WIDX_PICKUP].right = w->width - 2;
    window_guest_overview_widgets[WIDX_RENAME].right = w->width - 2;
    window_guest_overview_widgets[WIDX_LOCATE].right = w->width - 2;
    window_guest_overview_widgets[WIDX_TRACK].right = w->width - 2;

    window_guest_overview_widgets[WIDX_PICKUP].left = w->width - 25;
    window_guest_overview_widgets[WIDX_RENAME].left = w->width - 25;
    window_guest_overview_widgets[WIDX_LOCATE].left = w->width - 25;
    window_guest_overview_widgets[WIDX_TRACK].left = w->width - 25;
}

/**
 *
 *  rct2: 0x696F45
 */
void window_guest_overview_update(rct_window* w)
{
    int32_t newAnimationFrame = w->var_496;
    newAnimationFrame++;
    newAnimationFrame %= 24;
    w->var_496 = newAnimationFrame;

    widget_invalidate(w, WIDX_TAB_1);
    widget_invalidate(w, WIDX_TAB_2);

    const auto peep = GetGuest(w);
    if (peep == nullptr)
    {
        return;
    }
    if (peep->WindowInvalidateFlags & PEEP_INVALIDATE_PEEP_ACTION)
    {
        peep->WindowInvalidateFlags &= ~PEEP_INVALIDATE_PEEP_ACTION;
        widget_invalidate(w, WIDX_ACTION_LBL);
    }

    w->list_information_type += 2;

    if ((w->highlighted_item & 0xFFFF) == 0xFFFF)
        w->highlighted_item &= 0xFFFF0000;
    else
        w->highlighted_item++;

    // Disable peep watching thought for multiplayer as it's client specific
    if (network_get_mode() == NETWORK_MODE_NONE)
    {
        // Create the "I have the strangest feeling I am being watched thought"
        if ((w->highlighted_item & 0xFFFF) >= 3840)
        {
            if (!(w->highlighted_item & 0x3FF))
            {
                int32_t random = util_rand() & 0xFFFF;
                if (random <= 0x2AAA)
                {
                    peep->InsertNewThought(PeepThoughtType::Watched, PEEP_THOUGHT_ITEM_NONE);
                }
            }
        }
    }
}

/* rct2: 0x696A6A */
void window_guest_overview_text_input(rct_window* w, rct_widgetindex widgetIndex, char* text)
{
    if (widgetIndex != WIDX_RENAME)
        return;

    if (text == nullptr)
        return;
    guest_set_name(w->number, text);
}

/**
 *
 *  rct2: 0x696A5F
 */
void window_guest_overview_tool_update(rct_window* w, rct_widgetindex widgetIndex, const ScreenCoordsXY& screenCoords)
{
    if (widgetIndex != WIDX_PICKUP)
        return;

    map_invalidate_selection_rect();

    gMapSelectFlags &= ~MAP_SELECT_FLAG_ENABLE;

    auto mapCoords = footpath_get_coordinates_from_pos({ screenCoords.x, screenCoords.y + 16 }, nullptr, nullptr);
    if (!mapCoords.isNull())
    {
        gMapSelectFlags |= MAP_SELECT_FLAG_ENABLE;
        gMapSelectType = MAP_SELECT_TYPE_FULL;
        gMapSelectPositionA = mapCoords;
        gMapSelectPositionB = mapCoords;
        map_invalidate_selection_rect();
    }

    gPickupPeepImage = UINT32_MAX;

    auto info = get_map_coordinates_from_pos(screenCoords, VIEWPORT_INTERACTION_MASK_NONE);
    if (info.SpriteType == ViewportInteractionItem::None)
        return;

    gPickupPeepX = screenCoords.x - 1;
    gPickupPeepY = screenCoords.y + 16;
    w->picked_peep_frame++;
    if (w->picked_peep_frame >= 48)
    {
        w->picked_peep_frame = 0;
    }

    const auto peep = GetGuest(w);
    if (peep == nullptr)
    {
        return;
    }

    uint32_t imageId = GetPeepAnimation(peep->SpriteType, PeepActionSpriteType::Ui).base_image;
    imageId += w->picked_peep_frame >> 2;

    imageId |= (peep->TshirtColour << 19) | (peep->TrousersColour << 24) | IMAGE_TYPE_REMAP | IMAGE_TYPE_REMAP_2_PLUS;
    gPickupPeepImage = imageId;
}

/**
 *
 *  rct2: 0x696A54
 */
void window_guest_overview_tool_down(rct_window* w, rct_widgetindex widgetIndex, const ScreenCoordsXY& screenCoords)
{
    if (widgetIndex != WIDX_PICKUP)
        return;

    TileElement* tileElement;
    auto destCoords = footpath_get_coordinates_from_pos({ screenCoords.x, screenCoords.y + 16 }, nullptr, &tileElement);

    if (destCoords.isNull())
        return;

    PeepPickupAction pickupAction{
        PeepPickupType::Place, w->number, { destCoords, tileElement->GetBaseZ() }, network_get_current_player_id()
    };
    pickupAction.SetCallback([](const GameAction* ga, const GameActions::Result* result) {
        if (result->Error != GameActions::Status::Ok)
            return;
        tool_cancel();
        gPickupPeepImage = UINT32_MAX;
    });
    GameActions::Execute(&pickupAction);
}

/**
 *
 *  rct2: 0x696A49
 */
void window_guest_overview_tool_abort(rct_window* w, rct_widgetindex widgetIndex)
{
    if (widgetIndex != WIDX_PICKUP)
        return;

    PeepPickupAction pickupAction{
        PeepPickupType::Cancel, w->number, { w->picked_peep_old_x, 0, 0 }, network_get_current_player_id()
    };
    GameActions::Execute(&pickupAction);
}

/**
 * This is a combination of 5 functions that were identical
 *  rct2: 0x69744F, 0x697795, 0x697BDD, 0x697E18, 0x698279
 */
void window_guest_mouse_up(rct_window* w, rct_widgetindex widgetIndex)
{
    switch (widgetIndex)
    {
        case WIDX_CLOSE:
            window_close(w);
            break;
        case WIDX_TAB_1:
        case WIDX_TAB_2:
        case WIDX_TAB_3:
        case WIDX_TAB_4:
        case WIDX_TAB_5:
        case WIDX_TAB_6:
        case WIDX_TAB_7:
            window_guest_set_page(w, widgetIndex - WIDX_TAB_1);
            break;
    }
}

/**
 *
 *  rct2: 0x69746A
 */
void window_guest_stats_update(rct_window* w)
{
    w->frame_no++;
    auto peep = GetGuest(w);
    if (peep == nullptr)
    {
        return;
    }
    peep->WindowInvalidateFlags &= ~PEEP_INVALIDATE_PEEP_STATS;

    w->Invalidate();
}

/**
 *
 *  rct2: 0x0066ECC1
 *
 */
static void window_guest_stats_bars_paint(
    int32_t value, int32_t x, int32_t y, rct_window* w, rct_drawpixelinfo* dpi, int32_t colour, bool blinkFlag)
{
    if (font_get_line_height(gCurrentFontSpriteBase) > 10)
    {
        y += 1;
    }

    gfx_fill_rect_inset(dpi, x + 61, y + 1, x + 61 + 121, y + 9, w->colours[1], INSET_RECT_F_30);

    if (!blinkFlag || game_is_paused() || (gCurrentRealTimeTicks & 8) == 0)
    {
        value *= 118;
        value >>= 8;

        if (value <= 2)
            return;

        gfx_fill_rect_inset(dpi, x + 63, y + 2, x + 63 + value - 1, y + 8, colour, 0);
    }
}

/**
 * Takes a guest stat value (scaled to currMax) and adjusts it to be scaled out of 255.
 * Then clamp the value to between newMin and 255.
 */
static int32_t NormalizeGuestStatValue(int32_t value, int32_t currMax, int32_t newMin)
{
    int32_t newValue = (value * 255) / currMax;
    return std::clamp(newValue, newMin, 255);
}

/**
 *
 *  rct2: 0x0069711D
 */
void window_guest_stats_paint(rct_window* w, rct_drawpixelinfo* dpi)
{
    WindowDrawWidgets(w, dpi);
    window_guest_overview_tab_paint(w, dpi);
    window_guest_stats_tab_paint(w, dpi);
    window_guest_rides_tab_paint(w, dpi);
    window_guest_finance_tab_paint(w, dpi);
    window_guest_thoughts_tab_paint(w, dpi);
    window_guest_inventory_tab_paint(w, dpi);
    window_guest_debug_tab_paint(w, dpi);

    // ebx
    const auto peep = GetGuest(w);
    if (peep == nullptr)
    {
        return;
    }

    // Not sure why this is not stats widgets
    // cx dx
    auto screenCoords = w->windowPos
        + ScreenCoordsXY{ window_guest_rides_widgets[WIDX_PAGE_BACKGROUND].left + 4,
                          window_guest_rides_widgets[WIDX_PAGE_BACKGROUND].top + 4 };

    // Happiness
    gfx_draw_string_left(dpi, STR_GUEST_STAT_HAPPINESS_LABEL, nullptr, COLOUR_BLACK, screenCoords);

    int32_t happiness = NormalizeGuestStatValue(peep->Happiness, PEEP_MAX_HAPPINESS, 10);
    int32_t barColour = COLOUR_BRIGHT_GREEN;
    bool barBlink = happiness < 50;
    window_guest_stats_bars_paint(happiness, screenCoords.x, screenCoords.y, w, dpi, barColour, barBlink);

    // Energy
    screenCoords.y += LIST_ROW_HEIGHT;
    gfx_draw_string_left(dpi, STR_GUEST_STAT_ENERGY_LABEL, nullptr, COLOUR_BLACK, screenCoords);

    int32_t energy = NormalizeGuestStatValue(peep->Energy - PEEP_MIN_ENERGY, PEEP_MAX_ENERGY - PEEP_MIN_ENERGY, 10);
    barColour = COLOUR_BRIGHT_GREEN;
    barBlink = energy < 50;
    window_guest_stats_bars_paint(energy, screenCoords.x, screenCoords.y, w, dpi, barColour, barBlink);

    // Hunger
    screenCoords.y += LIST_ROW_HEIGHT;
    gfx_draw_string_left(dpi, STR_GUEST_STAT_HUNGER_LABEL, nullptr, COLOUR_BLACK, screenCoords);

    int32_t hunger = NormalizeGuestStatValue(peep->Hunger - 32, 158, 0);
    hunger = 255 - hunger; // the bar should be longer when peep->Hunger is low
    barColour = COLOUR_BRIGHT_RED;
    barBlink = hunger > 170;
    window_guest_stats_bars_paint(hunger, screenCoords.x, screenCoords.y, w, dpi, barColour, barBlink);

    // Thirst
    screenCoords.y += LIST_ROW_HEIGHT;
    gfx_draw_string_left(dpi, STR_GUEST_STAT_THIRST_LABEL, nullptr, COLOUR_BLACK, screenCoords);

    int32_t thirst = NormalizeGuestStatValue(peep->Thirst - 32, 158, 0);
    thirst = 255 - thirst; // the bar should be longer when peep->Thirst is low
    barColour = COLOUR_BRIGHT_RED;
    barBlink = thirst > 170;
    window_guest_stats_bars_paint(thirst, screenCoords.x, screenCoords.y, w, dpi, barColour, barBlink);

    // Nausea
    screenCoords.y += LIST_ROW_HEIGHT;
    gfx_draw_string_left(dpi, STR_GUEST_STAT_NAUSEA_LABEL, nullptr, COLOUR_BLACK, screenCoords);

    int32_t nausea = NormalizeGuestStatValue(peep->Nausea - 32, 223, 0);
    barColour = COLOUR_BRIGHT_RED;
    barBlink = nausea > 120;
    window_guest_stats_bars_paint(nausea, screenCoords.x, screenCoords.y, w, dpi, barColour, barBlink);

    // Toilet
    screenCoords.y += LIST_ROW_HEIGHT;
    gfx_draw_string_left(dpi, STR_GUEST_STAT_TOILET_LABEL, nullptr, COLOUR_BLACK, screenCoords);

    int32_t toilet = NormalizeGuestStatValue(peep->Toilet - 64, 178, 0);
    barColour = COLOUR_BRIGHT_RED;
    barBlink = toilet > 160;
    window_guest_stats_bars_paint(toilet, screenCoords.x, screenCoords.y, w, dpi, barColour, barBlink);

    // Time in park
    screenCoords.y += LIST_ROW_HEIGHT + 1;
    int32_t guestEntryTime = peep->GetParkEntryTime();
    if (guestEntryTime != -1)
    {
        int32_t timeInPark = (gScenarioTicks - guestEntryTime) >> 11;
        auto ft = Formatter();
        ft.Add<uint16_t>(timeInPark & 0xFFFF);
        gfx_draw_string_left(dpi, STR_GUEST_STAT_TIME_IN_PARK, ft.Data(), COLOUR_BLACK, screenCoords);
    }

    screenCoords.y += LIST_ROW_HEIGHT + 9;
    gfx_fill_rect_inset(
        dpi, screenCoords.x, screenCoords.y - 6, screenCoords.x + 179, screenCoords.y - 5, w->colours[1],
        INSET_RECT_FLAG_BORDER_INSET);

    // Preferred Ride
    gfx_draw_string_left(dpi, STR_GUEST_STAT_PREFERRED_RIDE, nullptr, COLOUR_BLACK, screenCoords);
    screenCoords.y += LIST_ROW_HEIGHT;

    // Intensity
    {
        auto ft = Formatter();
        auto maxIntensity = peep->Intensity.GetMaximum();
        int32_t string_id = STR_GUEST_STAT_PREFERRED_INTESITY_BELOW;
        if (peep->Intensity.GetMinimum() != 0)
        {
            ft.Add<uint16_t>(peep->Intensity.GetMinimum());
            ft.Add<uint16_t>(maxIntensity);
            string_id = STR_GUEST_STAT_PREFERRED_INTESITY_BETWEEN;
            if (maxIntensity == 15)
                string_id = STR_GUEST_STAT_PREFERRED_INTESITY_ABOVE;
        }
        else
        {
            ft.Add<uint16_t>(maxIntensity);
        }

        gfx_draw_string_left(dpi, string_id, ft.Data(), COLOUR_BLACK, screenCoords + ScreenCoordsXY{ 4, 0 });
    }

    // Nausea tolerance
    {
        static constexpr const rct_string_id nauseaTolerances[] = {
            STR_PEEP_STAT_NAUSEA_TOLERANCE_NONE,
            STR_PEEP_STAT_NAUSEA_TOLERANCE_LOW,
            STR_PEEP_STAT_NAUSEA_TOLERANCE_AVERAGE,
            STR_PEEP_STAT_NAUSEA_TOLERANCE_HIGH,
        };
        screenCoords.y += LIST_ROW_HEIGHT;
        auto nausea_tolerance = EnumValue(peep->NauseaTolerance) & 0x3;
        auto ft = Formatter();
        ft.Add<rct_string_id>(nauseaTolerances[nausea_tolerance]);
        gfx_draw_string_left(dpi, STR_GUEST_STAT_NAUSEA_TOLERANCE, ft.Data(), COLOUR_BLACK, screenCoords);
    }
}

/**
 *
 *  rct2: 0x6977B0
 */
void window_guest_rides_update(rct_window* w)
{
    w->frame_no++;

    widget_invalidate(w, WIDX_TAB_2);
    widget_invalidate(w, WIDX_TAB_3);

    const auto guest = GetGuest(w);
    if (guest == nullptr)
    {
        return;
    }

    // Every 2048 ticks do a full window_invalidate
    int32_t number_of_ticks = gScenarioTicks - guest->GetParkEntryTime();
    if (!(number_of_ticks & 0x7FF))
        w->Invalidate();

    uint8_t curr_list_position = 0;
    for (const auto& ride : GetRideManager())
    {
        if (ride.IsRide() && guest->HasRidden(&ride))
        {
            w->list_item_positions[curr_list_position] = ride.id;
            curr_list_position++;
        }
    }

    // If there are new items
    if (w->no_list_items != curr_list_position)
    {
        w->no_list_items = curr_list_position;
        w->Invalidate();
    }
}

/**
 *
 *  rct2: 0x69784E
 */
void window_guest_rides_scroll_get_size(rct_window* w, int32_t scrollIndex, int32_t* width, int32_t* height)
{
    *height = w->no_list_items * 10;

    if (w->selected_list_item != -1)
    {
        w->selected_list_item = -1;
        w->Invalidate();
    }

    int32_t visable_height = *height - window_guest_rides_widgets[WIDX_RIDE_SCROLL].bottom
        + window_guest_rides_widgets[WIDX_RIDE_SCROLL].top + 21;

    if (visable_height < 0)
        visable_height = 0;

    if (visable_height < w->scrolls[0].v_top)
    {
        w->scrolls[0].v_top = visable_height;
        w->Invalidate();
    }
}

/**
 *
 *  rct2: 0x006978CC
 */
void window_guest_rides_scroll_mouse_down(rct_window* w, int32_t scrollIndex, const ScreenCoordsXY& screenCoords)
{
    int32_t index;

    index = screenCoords.y / 10;
    if (index >= w->no_list_items)
        return;

    auto intent = Intent(WC_RIDE);
    intent.putExtra(INTENT_EXTRA_RIDE_ID, w->list_item_positions[index]);
    context_open_intent(&intent);
}

/**
 *
 *  rct2: 0x0069789C
 */
void window_guest_rides_scroll_mouse_over(rct_window* w, int32_t scrollIndex, const ScreenCoordsXY& screenCoords)
{
    int32_t index;

    index = screenCoords.y / 10;
    if (index >= w->no_list_items)
        return;

    if (index == w->selected_list_item)
        return;
    w->selected_list_item = index;

    w->Invalidate();
}

/**
 *
 *  rct2: 0x0069757A
 */
void window_guest_rides_invalidate(rct_window* w)
{
    window_guest_common_invalidate(w);

    window_guest_rides_widgets[WIDX_RIDE_SCROLL].right = w->width - 4;
    window_guest_rides_widgets[WIDX_RIDE_SCROLL].bottom = w->height - 15;
}

/**
 *
 *  rct2: 0x00697637
 */
void window_guest_rides_paint(rct_window* w, rct_drawpixelinfo* dpi)
{
    WindowDrawWidgets(w, dpi);
    window_guest_overview_tab_paint(w, dpi);
    window_guest_stats_tab_paint(w, dpi);
    window_guest_rides_tab_paint(w, dpi);
    window_guest_finance_tab_paint(w, dpi);
    window_guest_thoughts_tab_paint(w, dpi);
    window_guest_inventory_tab_paint(w, dpi);
    window_guest_debug_tab_paint(w, dpi);

    const auto peep = GetGuest(w);
    if (peep == nullptr)
    {
        return;
    }

    // cx dx
    auto screenCoords = w->windowPos
        + ScreenCoordsXY{ window_guest_rides_widgets[WIDX_PAGE_BACKGROUND].left + 2,
                          window_guest_rides_widgets[WIDX_PAGE_BACKGROUND].top + 2 };

    gfx_draw_string_left(dpi, STR_GUEST_LABEL_RIDES_BEEN_ON, nullptr, COLOUR_BLACK, screenCoords);

    screenCoords.y = w->windowPos.y + window_guest_rides_widgets[WIDX_PAGE_BACKGROUND].bottom - 12;

    auto ft = Formatter();
    auto ride = get_ride(peep->FavouriteRide);
    if (ride != nullptr)
    {
        ride->FormatNameTo(ft);
    }
    else
    {
        ft.Add<rct_string_id>(STR_PEEP_FAVOURITE_RIDE_NOT_AVAILABLE);
    }

    DrawTextEllipsised(dpi, screenCoords, w->width - 14, STR_FAVOURITE_RIDE, ft, COLOUR_BLACK);
}

/**
 *
 *  rct2: 0x006976FC
 */
void window_guest_rides_scroll_paint(rct_window* w, rct_drawpixelinfo* dpi, int32_t scrollIndex)
{
    auto colour = ColourMapA[w->colours[1]].mid_light;
    gfx_fill_rect(dpi, { { dpi->x, dpi->y }, { dpi->x + dpi->width - 1, dpi->y + dpi->height - 1 } }, colour);

    for (int32_t list_index = 0; list_index < w->no_list_items; list_index++)
    {
        auto y = list_index * 10;
        rct_string_id stringId = STR_BLACK_STRING;
        if (list_index == w->selected_list_item)
        {
            gfx_filter_rect(dpi, 0, y, 800, y + 9, FilterPaletteID::PaletteDarken1);
            stringId = STR_WINDOW_COLOUR_2_STRINGID;
        }

        auto ride = get_ride(w->list_item_positions[list_index]);
        if (ride != nullptr)
        {
            auto ft = Formatter();
            ride->FormatNameTo(ft);
            gfx_draw_string_left(dpi, stringId, ft.Data(), COLOUR_BLACK, { 0, y - 1 });
        }
    }
}

/**
 *
 *  rct2: 0x00697BF8
 */
void window_guest_finance_update(rct_window* w)
{
    w->frame_no++;

    widget_invalidate(w, WIDX_TAB_2);
    widget_invalidate(w, WIDX_TAB_4);
}

/**
 *
 *  rct2: 0x00697A08
 */
void window_guest_finance_paint(rct_window* w, rct_drawpixelinfo* dpi)
{
    WindowDrawWidgets(w, dpi);
    window_guest_overview_tab_paint(w, dpi);
    window_guest_stats_tab_paint(w, dpi);
    window_guest_rides_tab_paint(w, dpi);
    window_guest_finance_tab_paint(w, dpi);
    window_guest_thoughts_tab_paint(w, dpi);
    window_guest_inventory_tab_paint(w, dpi);
    window_guest_debug_tab_paint(w, dpi);

    const auto peep = GetGuest(w);
    if (peep == nullptr)
    {
        return;
    }

    // cx dx
    auto screenCoords = w->windowPos
        + ScreenCoordsXY{ window_guest_finance_widgets[WIDX_PAGE_BACKGROUND].left + 4,
                          window_guest_finance_widgets[WIDX_PAGE_BACKGROUND].top + 4 };

    // Cash in pocket
    {
        auto ft = Formatter();
        ft.Add<money32>(peep->CashInPocket);
        gfx_draw_string_left(dpi, STR_GUEST_STAT_CASH_IN_POCKET, ft.Data(), COLOUR_BLACK, screenCoords);
        screenCoords.y += LIST_ROW_HEIGHT;
    }

    // Cash spent
    {
        auto ft = Formatter();
        ft.Add<money32>(peep->CashSpent);
        gfx_draw_string_left(dpi, STR_GUEST_STAT_CASH_SPENT, ft.Data(), COLOUR_BLACK, screenCoords);
        screenCoords.y += LIST_ROW_HEIGHT * 2;
    }

    gfx_fill_rect_inset(
        dpi, screenCoords.x, screenCoords.y - 6, screenCoords.x + 179, screenCoords.y - 5, w->colours[1],
        INSET_RECT_FLAG_BORDER_INSET);

    // Paid to enter
    {
        auto ft = Formatter();
        ft.Add<money32>(peep->PaidToEnter);
        gfx_draw_string_left(dpi, STR_GUEST_EXPENSES_ENTRANCE_FEE, ft.Data(), COLOUR_BLACK, screenCoords);
        screenCoords.y += LIST_ROW_HEIGHT;
    }
    // Paid on rides
    {
        auto ft = Formatter();
        ft.Add<money32>(peep->PaidOnRides);
        ft.Add<uint16_t>(peep->GuestNumRides);
        if (peep->GuestNumRides != 1)
        {
            gfx_draw_string_left(dpi, STR_GUEST_EXPENSES_RIDE_PLURAL, ft.Data(), COLOUR_BLACK, screenCoords);
        }
        else
        {
            gfx_draw_string_left(dpi, STR_GUEST_EXPENSES_RIDE, ft.Data(), COLOUR_BLACK, screenCoords);
        }
        screenCoords.y += LIST_ROW_HEIGHT;
    }
    // Paid on food
    {
        auto ft = Formatter();
        ft.Add<money32>(peep->PaidOnFood);
        ft.Add<uint16_t>(peep->AmountOfFood);
        if (peep->AmountOfFood != 1)
        {
            gfx_draw_string_left(dpi, STR_GUEST_EXPENSES_FOOD_PLURAL, ft.Data(), COLOUR_BLACK, screenCoords);
        }
        else
        {
            gfx_draw_string_left(dpi, STR_GUEST_EXPENSES_FOOD, ft.Data(), COLOUR_BLACK, screenCoords);
        }
        screenCoords.y += LIST_ROW_HEIGHT;
    }

    // Paid on drinks
    {
        auto ft = Formatter();
        ft.Add<money32>(peep->PaidOnDrink);
        ft.Add<uint16_t>(peep->AmountOfDrinks);
        if (peep->AmountOfDrinks != 1)
        {
            gfx_draw_string_left(dpi, STR_GUEST_EXPENSES_DRINK_PLURAL, ft.Data(), COLOUR_BLACK, screenCoords);
        }
        else
        {
            gfx_draw_string_left(dpi, STR_GUEST_EXPENSES_DRINK, ft.Data(), COLOUR_BLACK, screenCoords);
        }
        screenCoords.y += LIST_ROW_HEIGHT;
    }
    // Paid on souvenirs
    {
        auto ft = Formatter();
        ft.Add<money32>(peep->PaidOnSouvenirs);
        ft.Add<uint16_t>(peep->AmountOfSouvenirs);
        if (peep->AmountOfSouvenirs != 1)
        {
            gfx_draw_string_left(dpi, STR_GUEST_EXPENSES_SOUVENIR_PLURAL, ft.Data(), COLOUR_BLACK, screenCoords);
        }
        else
        {
            gfx_draw_string_left(dpi, STR_GUEST_EXPENSES_SOUVENIR, ft.Data(), COLOUR_BLACK, screenCoords);
        }
    }
}

/**
 *
 *  rct2: 0x00697EB4
 */
void window_guest_thoughts_update(rct_window* w)
{
    w->frame_no++;

    widget_invalidate(w, WIDX_TAB_2);
    widget_invalidate(w, WIDX_TAB_5);

    auto peep = GetGuest(w);
    if (peep == nullptr)
    {
        return;
    }
    if (peep->WindowInvalidateFlags & PEEP_INVALIDATE_PEEP_THOUGHTS)
    {
        peep->WindowInvalidateFlags &= ~PEEP_INVALIDATE_PEEP_THOUGHTS;
        w->Invalidate();
    }
}

/**
 *
 *  rct2: 0x00697D2A
 */
void window_guest_thoughts_paint(rct_window* w, rct_drawpixelinfo* dpi)
{
    WindowDrawWidgets(w, dpi);
    window_guest_overview_tab_paint(w, dpi);
    window_guest_stats_tab_paint(w, dpi);
    window_guest_rides_tab_paint(w, dpi);
    window_guest_finance_tab_paint(w, dpi);
    window_guest_thoughts_tab_paint(w, dpi);
    window_guest_inventory_tab_paint(w, dpi);
    window_guest_debug_tab_paint(w, dpi);

    const auto peep = GetGuest(w);
    if (peep == nullptr)
    {
        return;
    }

    // cx dx
    auto screenCoords = w->windowPos
        + ScreenCoordsXY{ window_guest_thoughts_widgets[WIDX_PAGE_BACKGROUND].left + 4,
                          window_guest_thoughts_widgets[WIDX_PAGE_BACKGROUND].top + 4 };

    gfx_draw_string_left(dpi, STR_GUEST_RECENT_THOUGHTS_LABEL, nullptr, COLOUR_BLACK, screenCoords);

    screenCoords.y += 10;
    for (rct_peep_thought* thought = peep->Thoughts; thought < &peep->Thoughts[PEEP_MAX_THOUGHTS]; ++thought)
    {
        if (thought->type == PeepThoughtType::None)
            return;
        if (thought->freshness == 0)
            continue;

        int32_t width = window_guest_thoughts_widgets[WIDX_PAGE_BACKGROUND].right
            - window_guest_thoughts_widgets[WIDX_PAGE_BACKGROUND].left - 8;

        auto ft = Formatter();
        peep_thought_set_format_args(thought, ft);
        screenCoords.y += gfx_draw_string_left_wrapped(dpi, ft.Data(), screenCoords, width, STR_BLACK_STRING, COLOUR_BLACK);

        // If this is the last visible line end drawing.
        if (screenCoords.y > w->windowPos.y + window_guest_thoughts_widgets[WIDX_PAGE_BACKGROUND].bottom - 32)
            return;
    }
}

/**
 *
 *  rct2: 0x00698315
 */
void window_guest_inventory_update(rct_window* w)
{
    w->frame_no++;

    widget_invalidate(w, WIDX_TAB_2);
    widget_invalidate(w, WIDX_TAB_6);

    auto peep = GetGuest(w);
    if (peep == nullptr)
    {
        return;
    }
    if (peep->WindowInvalidateFlags & PEEP_INVALIDATE_PEEP_INVENTORY)
    {
        peep->WindowInvalidateFlags &= ~PEEP_INVALIDATE_PEEP_INVENTORY;
        w->Invalidate();
    }
}

static std::pair<rct_string_id, Formatter> window_guest_inventory_format_item(Peep* peep, ShopItem item)
{
    auto& park = OpenRCT2::GetContext()->GetGameState()->GetPark();
    auto parkName = park.Name.c_str();

    // Default arguments
    auto ft = Formatter();
    ft.Add<uint32_t>(GetShopItemDescriptor(item).Image);
    ft.Add<rct_string_id>(GetShopItemDescriptor(item).Naming.Display);
    ft.Add<rct_string_id>(STR_STRING);
    ft.Add<const char*>(parkName);

    // Special overrides
    Ride* ride{};
    switch (item)
    {
        case ShopItem::Balloon:
            ft.Rewind();
            ft.Add<uint32_t>(SPRITE_ID_PALETTE_COLOUR_1(peep->BalloonColour) | GetShopItemDescriptor(item).Image);
            break;
        case ShopItem::Photo:
            ride = get_ride(peep->Photo1RideRef);
            if (ride != nullptr)
            {
                ft.Rewind();
                ft.Increment(6);
                ride->FormatNameTo(ft);
            }

            break;
        case ShopItem::Umbrella:
            ft.Rewind();
            ft.Add<uint32_t>(SPRITE_ID_PALETTE_COLOUR_1(peep->UmbrellaColour) | GetShopItemDescriptor(item).Image);
            break;
        case ShopItem::Voucher:
            switch (peep->VoucherType)
            {
                case VOUCHER_TYPE_PARK_ENTRY_FREE:
                    ft.Rewind();
                    ft.Increment(6);
                    ft.Add<rct_string_id>(STR_PEEP_INVENTORY_VOUCHER_PARK_ENTRY_FREE);
                    ft.Add<rct_string_id>(STR_STRING);
                    ft.Add<const char*>(parkName);
                    break;
                case VOUCHER_TYPE_RIDE_FREE:
                    ride = get_ride(peep->VoucherRideId);
                    if (ride != nullptr)
                    {
                        ft.Rewind();
                        ft.Increment(6);
                        ft.Add<rct_string_id>(STR_PEEP_INVENTORY_VOUCHER_RIDE_FREE);
                        ride->FormatNameTo(ft);
                    }
                    break;
                case VOUCHER_TYPE_PARK_ENTRY_HALF_PRICE:
                    ft.Rewind();
                    ft.Increment(6);
                    ft.Add<rct_string_id>(STR_PEEP_INVENTORY_VOUCHER_PARK_ENTRY_HALF_PRICE);
                    ft.Add<rct_string_id>(STR_STRING);
                    ft.Add<const char*>(parkName);
                    break;
                case VOUCHER_TYPE_FOOD_OR_DRINK_FREE:
                    ft.Rewind();
                    ft.Increment(6);
                    ft.Add<rct_string_id>(STR_PEEP_INVENTORY_VOUCHER_FOOD_OR_DRINK_FREE);
                    ft.Add<rct_string_id>(GetShopItemDescriptor(peep->VoucherShopItem).Naming.Singular);
                    break;
            }
            break;
        case ShopItem::Hat:
            ft.Rewind();
            ft.Add<uint32_t>(SPRITE_ID_PALETTE_COLOUR_1(peep->HatColour) | GetShopItemDescriptor(item).Image);
            break;
        case ShopItem::TShirt:
            ft.Rewind();
            ft.Add<uint32_t>(SPRITE_ID_PALETTE_COLOUR_1(peep->TshirtColour) | GetShopItemDescriptor(item).Image);
            break;
        case ShopItem::Photo2:
            ride = get_ride(peep->Photo2RideRef);
            if (ride != nullptr)
            {
                ft.Rewind();
                ft.Increment(6);
                ride->FormatNameTo(ft);
            }
            break;
        case ShopItem::Photo3:
            ride = get_ride(peep->Photo3RideRef);
            if (ride != nullptr)
            {
                ft.Rewind();
                ft.Increment(6);
                ride->FormatNameTo(ft);
            }
            break;
        case ShopItem::Photo4:
            ride = get_ride(peep->Photo4RideRef);
            if (ride != nullptr)
            {
                ft.Rewind();
                ft.Increment(6);
                ride->FormatNameTo(ft);
            }
            break;
        default:
            // This switch handles only items visible in peep window
            break;
    }

    return std::make_pair(STR_GUEST_ITEM_FORMAT, ft);
}

/**
 *
 *  rct2: 0x00697F81
 */
void window_guest_inventory_paint(rct_window* w, rct_drawpixelinfo* dpi)
{
    WindowDrawWidgets(w, dpi);
    window_guest_overview_tab_paint(w, dpi);
    window_guest_stats_tab_paint(w, dpi);
    window_guest_rides_tab_paint(w, dpi);
    window_guest_finance_tab_paint(w, dpi);
    window_guest_thoughts_tab_paint(w, dpi);
    window_guest_inventory_tab_paint(w, dpi);
    window_guest_debug_tab_paint(w, dpi);

    const auto guest = GetGuest(w);
    if (guest == nullptr)
    {
        return;
    }

    rct_widget* pageBackgroundWidget = &window_guest_inventory_widgets[WIDX_PAGE_BACKGROUND];
    auto screenCoords = w->windowPos + ScreenCoordsXY{ pageBackgroundWidget->left + 4, pageBackgroundWidget->top + 2 };
    int32_t itemNameWidth = pageBackgroundWidget->width() - 8;

    int32_t maxY = w->windowPos.y + w->height - 22;
    int32_t numItems = 0;

    gfx_draw_string_left(dpi, STR_CARRYING, nullptr, COLOUR_BLACK, screenCoords);
    screenCoords.y += 10;

    for (ShopItem item = ShopItem::Balloon; item < ShopItem::Count; item++)
    {
        if (screenCoords.y >= maxY)
            break;
        if (!guest->HasItem(item))
            continue;

        auto [stringId, ft] = window_guest_inventory_format_item(guest, item);
        screenCoords.y += gfx_draw_string_left_wrapped(dpi, ft.Data(), screenCoords, itemNameWidth, stringId, COLOUR_BLACK);
        numItems++;
    }

    if (numItems == 0)
    {
        gfx_draw_string_left(dpi, STR_NOTHING, nullptr, COLOUR_BLACK, screenCoords);
    }
}

/**
 *
 *  rct2: 0x00698315
 */
void window_guest_debug_update(rct_window* w)
{
    w->frame_no++;
    w->Invalidate();
}

void window_guest_debug_paint(rct_window* w, rct_drawpixelinfo* dpi)
{
    char buffer[512]{};
    char buffer2[512]{};

    WindowDrawWidgets(w, dpi);
    window_guest_overview_tab_paint(w, dpi);
    window_guest_stats_tab_paint(w, dpi);
    window_guest_rides_tab_paint(w, dpi);
    window_guest_finance_tab_paint(w, dpi);
    window_guest_thoughts_tab_paint(w, dpi);
    window_guest_inventory_tab_paint(w, dpi);
    window_guest_debug_tab_paint(w, dpi);

    const auto peep = GetGuest(w);
    if (peep == nullptr)
    {
        return;
    }
    auto screenCoords = w->windowPos
        + ScreenCoordsXY{ window_guest_debug_widgets[WIDX_PAGE_BACKGROUND].left + 4,
                          window_guest_debug_widgets[WIDX_PAGE_BACKGROUND].top + 4 };
    {
        auto ft = Formatter();
        ft.Add<uint32_t>(peep->sprite_index);
        gfx_draw_string_left(dpi, STR_PEEP_DEBUG_SPRITE_INDEX, ft.Data(), 0, screenCoords);
    }
    screenCoords.y += LIST_ROW_HEIGHT;
    {
        auto ft = Formatter();
        ft.Add<int32_t>(peep->x);
        ft.Add<int32_t>(peep->y);
        ft.Add<int32_t>(peep->z);
        gfx_draw_string_left(dpi, STR_PEEP_DEBUG_POSITION, ft.Data(), 0, screenCoords);
    }
    screenCoords.y += LIST_ROW_HEIGHT;
    {
        auto ft = Formatter();
        ft.Add<int32_t>(peep->NextLoc.x);
        ft.Add<int32_t>(peep->NextLoc.y);
        ft.Add<int32_t>(peep->NextLoc.z);
        format_string(buffer, sizeof(buffer), STR_PEEP_DEBUG_NEXT, ft.Data());
        if (peep->GetNextIsSurface())
        {
            format_string(buffer2, sizeof(buffer2), STR_PEEP_DEBUG_NEXT_SURFACE, nullptr);
            safe_strcat(buffer, buffer2, sizeof(buffer));
        }
        if (peep->GetNextIsSloped())
        {
            auto ft2 = Formatter();
            ft2.Add<int32_t>(peep->GetNextDirection());
            format_string(buffer2, sizeof(buffer2), STR_PEEP_DEBUG_NEXT_SLOPE, ft2.Data());
            safe_strcat(buffer, buffer2, sizeof(buffer));
        }
        gfx_draw_string(dpi, buffer, 0, screenCoords);
    }
    screenCoords.y += LIST_ROW_HEIGHT;
    {
        auto ft = Formatter();
        ft.Add<int32_t>(peep->DestinationX);
        ft.Add<int32_t>(peep->DestinationY);
        ft.Add<int32_t>(peep->DestinationTolerance);
        gfx_draw_string_left(dpi, STR_PEEP_DEBUG_DEST, ft.Data(), 0, screenCoords);
    }
    screenCoords.y += LIST_ROW_HEIGHT;
    {
        auto ft = Formatter();
        ft.Add<int32_t>(peep->PathfindGoal.x);
        ft.Add<int32_t>(peep->PathfindGoal.y);
        ft.Add<int32_t>(peep->PathfindGoal.z);
        ft.Add<int32_t>(peep->PathfindGoal.direction);
        gfx_draw_string_left(dpi, STR_PEEP_DEBUG_PATHFIND_GOAL, ft.Data(), 0, screenCoords);
    }
    screenCoords.y += LIST_ROW_HEIGHT;
    gfx_draw_string_left(dpi, STR_PEEP_DEBUG_PATHFIND_HISTORY, nullptr, 0, screenCoords);
    screenCoords.y += LIST_ROW_HEIGHT;

    screenCoords.x += 10;
    for (auto& point : peep->PathfindHistory)
    {
        auto ft = Formatter();
        ft.Add<int32_t>(point.x);
        ft.Add<int32_t>(point.y);
        ft.Add<int32_t>(point.z);
        ft.Add<int32_t>(point.direction);
        gfx_draw_string_left(dpi, STR_PEEP_DEBUG_PATHFIND_HISTORY_ITEM, ft.Data(), 0, screenCoords);
        screenCoords.y += LIST_ROW_HEIGHT;
    }
    screenCoords.x -= 10;
}
