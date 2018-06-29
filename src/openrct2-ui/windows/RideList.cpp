/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include <openrct2-ui/windows/Window.h>

#include <openrct2/config/Config.h>
#include <openrct2/Context.h>
#include <openrct2/core/Util.hpp>
#include <openrct2/Game.h>
#include <openrct2-ui/interface/Widget.h>
#include <openrct2/localisation/Localisation.h>
#include <openrct2/network/network.h>
#include <openrct2/sprites.h>
#include <openrct2-ui/interface/Dropdown.h>
#include <openrct2/windows/Intent.h>
#include <openrct2/world/Park.h>
#include <openrct2/interface/Colour.h>
#include <openrct2/drawing/Drawing.h>
#include "../interface/Theme.h"

// clang-format off
enum {
    PAGE_RIDES,
    PAGE_SHOPS_AND_STALLS,
    PAGE_KIOSKS_AND_FACILITIES,
    PAGE_COUNT
};

enum WINDOW_RIDE_LIST_WIDGET_IDX {
    WIDX_BACKGROUND,
    WIDX_TITLE,
    WIDX_CLOSE,
    WIDX_PAGE_BACKGROUND,
    WIDX_OPEN_CLOSE_ALL,
    WIDX_CURRENT_INFORMATION_TYPE,
    WIDX_INFORMATION_TYPE_DROPDOWN,
    WIDX_SORT,
    WIDX_TAB_1,
    WIDX_TAB_2,
    WIDX_TAB_3,
    WIDX_LIST,
    WIDX_CLOSE_LIGHT,
    WIDX_OPEN_LIGHT,
    WIDX_QUICK_DEMOLISH,
};

static rct_widget window_ride_list_widgets[] = {
    { WWT_FRAME,            0,  0,      339,    0,      239,    0xFFFFFFFF,                 STR_NONE },                                 // panel / background
    { WWT_CAPTION,          0,  1,      338,    1,      14,     0xFFFFFFFF,                 STR_WINDOW_TITLE_TIP },                     // title bar
    { WWT_CLOSEBOX,         0,  327,    337,    2,      13,     STR_CLOSE_X,                STR_CLOSE_WINDOW_TIP },                     // close x button
    { WWT_RESIZE,           1,  0,      339,    43,     239,    0xFFFFFFFF,                 STR_NONE },                                 // tab page background
    { WWT_FLATBTN,          1,  315,    338,    60,     83,     SPR_TOGGLE_OPEN_CLOSE,      STR_OPEN_OR_CLOSE_ALL_RIDES },              // open / close all toggle
    { WWT_DROPDOWN,         1,  150,    273,    46,     57,     0xFFFFFFFF,                 STR_NONE },                                 // current information type
    { WWT_BUTTON,           1,  262,    272,    47,     56,     STR_DROPDOWN_GLYPH,         STR_RIDE_LIST_INFORMATION_TYPE_TIP },       // information type dropdown button
    { WWT_BUTTON,           1,  280,    333,    46,     57,     STR_SORT,                   STR_RIDE_LIST_SORT_TIP },                   // sort button
    { WWT_TAB,              1,  3,      33,     17,     43,     IMAGE_TYPE_REMAP | SPR_TAB,       STR_LIST_RIDES_TIP },                       // tab 1
    { WWT_TAB,              1,  34,     64,     17,     43,     IMAGE_TYPE_REMAP | SPR_TAB,       STR_LIST_SHOPS_AND_STALLS_TIP },            // tab 2
    { WWT_TAB,              1,  65,     95,     17,     43,     IMAGE_TYPE_REMAP | SPR_TAB,       STR_LIST_KIOSKS_AND_FACILITIES_TIP },       // tab 3
    { WWT_SCROLL,           1,  3,      336,    60,     236,    SCROLL_VERTICAL,                            STR_NONE },                                 // list
    { WWT_IMGBTN,           1,  320,    333,    62,     75,     SPR_G2_RCT1_CLOSE_BUTTON_0, STR_NONE },
    { WWT_IMGBTN,           1,  320,    333,    76,     89,     SPR_G2_RCT1_OPEN_BUTTON_0,  STR_NONE },
    { WWT_FLATBTN,          1,  315,    338,    90,     113,    SPR_DEMOLISH,               STR_QUICK_DEMOLISH_RIDE },
    { WIDGETS_END },
};

static bool _quickDemolishMode = false;

static void window_ride_list_mouseup(rct_window *w, rct_widgetindex widgetIndex);
static void window_ride_list_resize(rct_window *w);
static void window_ride_list_mousedown(rct_window *w, rct_widgetindex widgetIndex, rct_widget* widget);
static void window_ride_list_dropdown(rct_window *w, rct_widgetindex widgetIndex, int32_t dropdownIndex);
static void window_ride_list_update(rct_window *w);
static void window_ride_list_scrollgetsize(rct_window *w, int32_t scrollIndex, int32_t *width, int32_t *height);
static void window_ride_list_scrollmousedown(rct_window *w, int32_t scrollIndex, int32_t x, int32_t y);
static void window_ride_list_scrollmouseover(rct_window *w, int32_t scrollIndex, int32_t x, int32_t y);
static void window_ride_list_tooltip(rct_window* w, rct_widgetindex widgetIndex, rct_string_id *stringId);
static void window_ride_list_invalidate(rct_window *w);
static void window_ride_list_paint(rct_window *w, rct_drawpixelinfo *dpi);
static void window_ride_list_scrollpaint(rct_window *w, rct_drawpixelinfo *dpi, int32_t scrollIndex);

static rct_window_event_list window_ride_list_events = {
    nullptr,
    window_ride_list_mouseup,
    window_ride_list_resize,
    window_ride_list_mousedown,
    window_ride_list_dropdown,
    nullptr,
    window_ride_list_update,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    window_ride_list_scrollgetsize,
    window_ride_list_scrollmousedown,
    nullptr,
    window_ride_list_scrollmouseover,
    nullptr,
    nullptr,
    nullptr,
    window_ride_list_tooltip,
    nullptr,
    nullptr,
    window_ride_list_invalidate,
    window_ride_list_paint,
    window_ride_list_scrollpaint
};

enum {
    INFORMATION_TYPE_STATUS,
    INFORMATION_TYPE_POPULARITY,
    INFORMATION_TYPE_SATISFACTION,
    INFORMATION_TYPE_PROFIT,
    INFORMATION_TYPE_TOTAL_CUSTOMERS,
    INFORMATION_TYPE_TOTAL_PROFIT,
    INFORMATION_TYPE_CUSTOMERS,
    INFORMATION_TYPE_AGE,
    INFORMATION_TYPE_INCOME,
    INFORMATION_TYPE_RUNNING_COST,
    INFORMATION_TYPE_QUEUE_LENGTH,
    INFORMATION_TYPE_QUEUE_TIME,
    INFORMATION_TYPE_RELIABILITY,
    INFORMATION_TYPE_DOWN_TIME,
    INFORMATION_TYPE_GUESTS_FAVOURITE,
    DROPDOWN_LIST_COUNT
};

static constexpr const rct_string_id ride_info_type_string_mapping[DROPDOWN_LIST_COUNT] = {
    STR_STATUS,
    STR_POPULARITY,
    STR_SATISFACTION,
    STR_PROFIT,
    STR_RIDE_LIST_TOTAL_CUSTOMERS,
    STR_RIDE_LIST_TOTAL_PROFIT,
    STR_RIDE_LIST_CUSTOMERS_PER_HOUR,
    STR_RIDE_LIST_AGE,
    STR_RIDE_LIST_INCOME,
    STR_RIDE_LIST_RUNNING_COST,
    STR_QUEUE_LENGTH,
    STR_QUEUE_TIME,
    STR_RELIABILITY,
    STR_DOWN_TIME,
    STR_GUESTS_FAVOURITE
};

static constexpr const rct_string_id ride_list_statusbar_count_strings[PAGE_COUNT] = {
    STR_NUMBER_RIDES,
    STR_NUMBER_SHOPS_AND_STALLS,
    STR_NUMBER_RESTROOMS_AND_INFORMATION_KIOSKS,
};

static constexpr const bool ride_info_type_money_mapping[DROPDOWN_LIST_COUNT] = {
    false,
    false,
    false,
    true,
    false,
    true,
    false,
    false,
    true,
    true,
    false,
    false,
    false,
    false,
    false
};

static constexpr const rct_string_id page_names[] = {
    STR_RIDES,
    STR_SHOPS_AND_STALLS,
    STR_RESTROOMS_AND_INFORMATION_KIOSKS,
};
// clang-format on

static int32_t _window_ride_list_information_type;

static void window_ride_list_draw_tab_images(rct_drawpixelinfo *dpi, rct_window *w);
static void window_ride_list_close_all(rct_window *w);
static void window_ride_list_open_all(rct_window *w);

/**
 *
 *  rct2: 0x006B30BC
 */
rct_window * window_ride_list_open()
{
    rct_window* window;

    // Check if window is already open
    window = window_bring_to_front_by_class(WC_RIDE_LIST);
    if (window == nullptr) {
        window = window_create_auto_pos(340, 240, &window_ride_list_events, WC_RIDE_LIST, WF_10 | WF_RESIZABLE);
        window->widgets = window_ride_list_widgets;
        window->enabled_widgets =
            (1 << WIDX_CLOSE) |
            (1 << WIDX_OPEN_CLOSE_ALL) |
            (1 << WIDX_CURRENT_INFORMATION_TYPE) |
            (1 << WIDX_INFORMATION_TYPE_DROPDOWN) |
            (1 << WIDX_SORT) |
            (1 << WIDX_TAB_1) |
            (1 << WIDX_TAB_2) |
            (1 << WIDX_TAB_3) |
            (1 << WIDX_CLOSE_LIGHT) |
            (1 << WIDX_OPEN_LIGHT);
        if (network_get_mode() != NETWORK_MODE_CLIENT) {
            window->enabled_widgets |= (1 << WIDX_QUICK_DEMOLISH);
        }
        window_init_scroll_widgets(window);
        window->page = PAGE_RIDES;
        window->no_list_items = 0;
        window->selected_list_item = -1;
        window->frame_no = 0;
        window->min_width = 340;
        window->min_height = 240;
        window->max_width = 400;
        window->max_height = 700;
        window_ride_list_refresh_list(window);
    }
    _window_ride_list_information_type = INFORMATION_TYPE_STATUS;
    window->list_information_type = 0;
    _quickDemolishMode = false;

    return window;
}

/**
 *
 *  rct2: 0x006B3511
 */
static void window_ride_list_mouseup(rct_window *w, rct_widgetindex widgetIndex)
{
    switch (widgetIndex) {
    case WIDX_CLOSE:
        window_close(w);
        break;
    case WIDX_SORT:
        w->list_information_type = _window_ride_list_information_type;
        w->no_list_items = 0;
        w->selected_list_item = -1;
        window_ride_list_refresh_list(w);
        break;
    case WIDX_TAB_1:
    case WIDX_TAB_2:
    case WIDX_TAB_3:
        if (w->page != widgetIndex - WIDX_TAB_1) {
            w->page = widgetIndex - WIDX_TAB_1;
            w->no_list_items = 0;
            w->frame_no = 0;
            w->selected_list_item = -1;
            if (w->page != PAGE_RIDES && _window_ride_list_information_type > INFORMATION_TYPE_RUNNING_COST) {
                _window_ride_list_information_type = INFORMATION_TYPE_STATUS;
            }
            window_ride_list_refresh_list(w);
        }
        break;
    case WIDX_CLOSE_LIGHT:
        window_ride_list_close_all(w);
        break;
    case WIDX_OPEN_LIGHT:
        window_ride_list_open_all(w);
        break;
    case WIDX_QUICK_DEMOLISH:
        if (network_get_mode() != NETWORK_MODE_CLIENT) {
            _quickDemolishMode = !_quickDemolishMode;
        }
        else {
            _quickDemolishMode = false;
        }
        window_invalidate(w);
        break;
    }
}

/**
 *
 *  rct2: 0x006B38A7
 */
static void window_ride_list_resize(rct_window *w)
{
    w->min_width = 340;
    w->min_height = 124;
    if (w->width < w->min_width) {
        window_invalidate(w);
        w->width = w->min_width;
    }
    if (w->height < w->min_height) {
        window_invalidate(w);
        w->height = w->min_height;
    }

    // Refreshing the list can be a very intensive operation
    // owing to its use of ride_has_any_track_elements().
    // This makes sure it's only refreshed every 64 ticks.
    if (!(gCurrentTicks & 0x3f)) {
        window_ride_list_refresh_list(w);
    }
}

/**
 *
 *  rct2: 0x006B3532
 */
static void window_ride_list_mousedown(rct_window *w, rct_widgetindex widgetIndex, rct_widget* widget)
{
    if (widgetIndex == WIDX_OPEN_CLOSE_ALL) {
        gDropdownItemsFormat[0] = STR_CLOSE_ALL;
        gDropdownItemsFormat[1] = STR_OPEN_ALL;
        window_dropdown_show_text(w->x + widget->left, w->y + widget->top, widget->bottom - widget->top, w->colours[1], 0, 2);
    } else if (widgetIndex == WIDX_INFORMATION_TYPE_DROPDOWN) {
        widget--;

        int32_t lastType;
        if (w->page == PAGE_RIDES)
            lastType = INFORMATION_TYPE_GUESTS_FAVOURITE;
        else
            lastType = INFORMATION_TYPE_RUNNING_COST;

        int32_t numItems = 0;
        int32_t selectedIndex = -1;
        for (int32_t type = INFORMATION_TYPE_STATUS; type <= lastType; type++) {
            if ((gParkFlags & PARK_FLAGS_NO_MONEY)) {
                if (ride_info_type_money_mapping[type]) {
                    continue;
                }
            }

            if (type == _window_ride_list_information_type) {
                selectedIndex = numItems;
            }

            gDropdownItemsFormat[numItems] = STR_DROPDOWN_MENU_LABEL;
            gDropdownItemsArgs[numItems] = ride_info_type_string_mapping[type];
            numItems++;
        }

        window_dropdown_show_text_custom_width(
            w->x + widget->left,
            w->y + widget->top,
            widget->bottom - widget->top,
            w->colours[1],
            0,
            DROPDOWN_FLAG_STAY_OPEN,
            numItems,
            widget->right - widget->left - 3
        );
        if (selectedIndex != -1) {
            dropdown_set_checked(selectedIndex, true);
        }
    }
}

/**
 *
 *  rct2: 0x006B3547
 */
static void window_ride_list_dropdown(rct_window *w, rct_widgetindex widgetIndex, int32_t dropdownIndex)
{
    if (widgetIndex == WIDX_OPEN_CLOSE_ALL)
    {
        if (dropdownIndex == 0)
        {
            window_ride_list_close_all(w);
        }
        else if (dropdownIndex == 1)
        {
            window_ride_list_open_all(w);
        }

        window_invalidate(w);
    }
    else if (widgetIndex == WIDX_INFORMATION_TYPE_DROPDOWN)
    {
        if (dropdownIndex == -1)
            return;

        int32_t informationType = INFORMATION_TYPE_STATUS;
        uint32_t arg = (uint32_t)gDropdownItemsArgs[dropdownIndex];
        for (size_t i = 0; i < Util::CountOf(ride_info_type_string_mapping); i++)
        {
            if (arg == ride_info_type_string_mapping[i])
            {
                informationType = (int32_t)i;
            }
        }

        _window_ride_list_information_type = informationType;
        window_invalidate(w);
    }
}

/**
 *
 *  rct2: 0x006B386B
 */
static void window_ride_list_update(rct_window *w)
{
    w->frame_no = (w->frame_no + 1) % 64;
    widget_invalidate(w, WIDX_TAB_1 + w->page);
    if (_window_ride_list_information_type != INFORMATION_TYPE_STATUS)
        window_invalidate(w);
}

/**
 *
 *  rct2: 0x006B35A1
 */
static void window_ride_list_scrollgetsize(rct_window *w, int32_t scrollIndex, int32_t *width, int32_t *height)
{
    int32_t top;

    *height = w->no_list_items * SCROLLABLE_ROW_HEIGHT;
    if (w->selected_list_item != -1) {
        w->selected_list_item = -1;
        window_invalidate(w);
    }

    top = *height - window_ride_list_widgets[WIDX_LIST].bottom + window_ride_list_widgets[WIDX_LIST].top + 21;
    if (top < 0)
        top = 0;
    if (top < w->scrolls[0].v_top) {
        w->scrolls[0].v_top = top;
        window_invalidate(w);
    }
}

/**
 *
 *  rct2: 0x006B361F
 */
static void window_ride_list_scrollmousedown(rct_window *w, int32_t scrollIndex, int32_t x, int32_t y)
{
    int32_t index;

    index = y / SCROLLABLE_ROW_HEIGHT;
    if (index >= w->no_list_items)
        return;

    // Open ride window
    uint8_t rideIndex = w->list_item_positions[index];
    if (_quickDemolishMode && network_get_mode() != NETWORK_MODE_CLIENT) {
        ride_action_modify(rideIndex, RIDE_MODIFY_DEMOLISH, GAME_COMMAND_FLAG_APPLY);
        window_ride_list_refresh_list(w);
    }
    else {
        auto intent = Intent(WC_RIDE);
        intent.putExtra(INTENT_EXTRA_RIDE_ID, rideIndex);
        context_open_intent(&intent);
    }
}

/**
 *
 *  rct2: 0x006B35EF
 */
static void window_ride_list_scrollmouseover(rct_window *w, int32_t scrollIndex, int32_t x, int32_t y)
{
    int32_t index;

    index = y / SCROLLABLE_ROW_HEIGHT;
    if (index >= w->no_list_items)
        return;

    w->selected_list_item = index;
    window_invalidate(w);
}

/**
 *
 *  rct2: 0x006B3861
 */
static void window_ride_list_tooltip(rct_window* w, rct_widgetindex widgetIndex, rct_string_id *stringId)
{
    set_format_arg(0, rct_string_id, STR_LIST);
}

/**
 *
 *  rct2: 0x006B3182
 */
static void window_ride_list_invalidate(rct_window *w)
{
    window_ride_list_widgets[WIDX_CURRENT_INFORMATION_TYPE].text = ride_info_type_string_mapping[_window_ride_list_information_type];

    // Set correct active tab
    for (int32_t i = 0; i < 3; i++)
        w->pressed_widgets &= ~(1 << (WIDX_TAB_1 + i));
    w->pressed_widgets |= 1LL << (WIDX_TAB_1 + w->page);

    window_ride_list_widgets[WIDX_TITLE].text = page_names[w->page];

    if (_quickDemolishMode)
        w->pressed_widgets |= (1ULL << WIDX_QUICK_DEMOLISH);
    else
        w->pressed_widgets &= ~(1ULL << WIDX_QUICK_DEMOLISH);

    w->widgets[WIDX_BACKGROUND].right = w->width - 1;
    w->widgets[WIDX_BACKGROUND].bottom = w->height - 1;
    w->widgets[WIDX_PAGE_BACKGROUND].right = w->width - 1;
    w->widgets[WIDX_PAGE_BACKGROUND].bottom = w->height - 1;
    w->widgets[WIDX_TITLE].right = w->width - 2;
    w->widgets[WIDX_CLOSE].left = w->width - 13;
    w->widgets[WIDX_CLOSE].right = w->width - 3;
    w->widgets[WIDX_LIST].right = w->width - 26;
    w->widgets[WIDX_LIST].bottom = w->height - 15;
    w->widgets[WIDX_OPEN_CLOSE_ALL].right = w->width - 2;
    w->widgets[WIDX_OPEN_CLOSE_ALL].left = w->width - 25;
    w->widgets[WIDX_CLOSE_LIGHT].right = w->width - 7;
    w->widgets[WIDX_CLOSE_LIGHT].left = w->width - 20;
    w->widgets[WIDX_OPEN_LIGHT].right = w->width - 7;
    w->widgets[WIDX_OPEN_LIGHT].left = w->width - 20;
    w->widgets[WIDX_QUICK_DEMOLISH].right = w->width - 2;
    w->widgets[WIDX_QUICK_DEMOLISH].left = w->width - 25;

    if (theme_get_flags() & UITHEME_FLAG_USE_LIGHTS_RIDE) {
        w->widgets[WIDX_OPEN_CLOSE_ALL].type = WWT_EMPTY;
        w->widgets[WIDX_CLOSE_LIGHT].type = WWT_IMGBTN;
        w->widgets[WIDX_OPEN_LIGHT].type = WWT_IMGBTN;

        int8_t allClosed = -1;
        int8_t allOpen = -1;
        int32_t i;
        Ride *ride;
        FOR_ALL_RIDES(i, ride) {
            if (w->page != gRideClassifications[ride->type])
                continue;
            if (ride->status == RIDE_STATUS_OPEN) {
                if (allOpen == -1) allOpen = true;
                allClosed = false;
            }
            else {
                if (allClosed == -1) allClosed = true;
                allOpen = false;
            }
        }
        w->widgets[WIDX_CLOSE_LIGHT].image = SPR_G2_RCT1_CLOSE_BUTTON_0 + (allClosed == 1) * 2 + widget_is_pressed(w, WIDX_CLOSE_LIGHT);
        w->widgets[WIDX_OPEN_LIGHT].image = SPR_G2_RCT1_OPEN_BUTTON_0 + (allOpen == 1) * 2 + widget_is_pressed(w, WIDX_OPEN_LIGHT);
        w->widgets[WIDX_QUICK_DEMOLISH].top = w->widgets[WIDX_OPEN_LIGHT].bottom + 3;
    }
    else {
        w->widgets[WIDX_OPEN_CLOSE_ALL].type = WWT_FLATBTN;
        w->widgets[WIDX_CLOSE_LIGHT].type = WWT_EMPTY;
        w->widgets[WIDX_OPEN_LIGHT].type = WWT_EMPTY;
        w->widgets[WIDX_QUICK_DEMOLISH].top = w->widgets[WIDX_OPEN_CLOSE_ALL].bottom + 3;
    }
    w->widgets[WIDX_QUICK_DEMOLISH].bottom = w->widgets[WIDX_QUICK_DEMOLISH].top + 23;
    w->widgets[WIDX_QUICK_DEMOLISH].type = network_get_mode() != NETWORK_MODE_CLIENT ? WWT_FLATBTN : WWT_EMPTY;
}

/**
 *
 *  rct2: 0x006B3235
 */
static void window_ride_list_paint(rct_window *w, rct_drawpixelinfo *dpi)
{
    window_draw_widgets(w, dpi);
    window_ride_list_draw_tab_images(dpi, w);

    // Draw number of attractions on bottom
    gfx_draw_string_left(dpi, ride_list_statusbar_count_strings[w->page], &w->no_list_items, COLOUR_BLACK, w->x + 4, w->widgets[WIDX_LIST].bottom + w->y + 2);
}

/**
 *
 *  rct2: 0x006B3240
 */
static void window_ride_list_scrollpaint(rct_window *w, rct_drawpixelinfo *dpi, int32_t scrollIndex)
{
    int32_t i, y, argument;
    rct_string_id format, formatSecondary;
    Ride *ride;

    gfx_fill_rect(dpi, dpi->x, dpi->y, dpi->x + dpi->width, dpi->y + dpi->height, ColourMapA[w->colours[1]].mid_light);

    y = 0;
    for (i = 0; i < w->no_list_items; i++) {
        format = (_quickDemolishMode ? STR_RED_STRINGID : STR_BLACK_STRING);

        // Background highlight
        if (i == w->selected_list_item) {
            gfx_filter_rect(dpi, 0, y, 800, y + SCROLLABLE_ROW_HEIGHT - 1, PALETTE_DARKEN_1);
            format = (_quickDemolishMode ? STR_LIGHTPINK_STRINGID : STR_WINDOW_COLOUR_2_STRINGID);
        }

        // Get ride
        ride = get_ride(w->list_item_positions[i]);

        // Ride name
        set_format_arg(0, rct_string_id, ride->name);
        set_format_arg(2, uint32_t, ride->name_arguments);
        gfx_draw_string_left_clipped(dpi, format, gCommonFormatArgs, COLOUR_BLACK, 0, y - 1, 159);

        // Ride information
        formatSecondary = 0;
        switch (_window_ride_list_information_type) {
        case INFORMATION_TYPE_STATUS:
            ride_get_status(w->list_item_positions[i], &formatSecondary, &argument);
            set_format_arg(2, int32_t, argument);
            break;
        case INFORMATION_TYPE_POPULARITY:
            formatSecondary = STR_POPULARITY_UNKNOWN_LABEL;
            if (ride->popularity != 255) {
                formatSecondary = STR_POPULARITY_LABEL;
                set_format_arg(2, uint16_t, ride->popularity * 4);
            }
            break;
        case INFORMATION_TYPE_SATISFACTION:
            formatSecondary = STR_SATISFACTION_UNKNOWN_LABEL;
            if (ride->satisfaction != 255) {
                formatSecondary = STR_SATISFACTION_LABEL;
                set_format_arg(2, uint16_t, ride->satisfaction * 5);
            }
            break;
        case INFORMATION_TYPE_PROFIT:
            formatSecondary = 0;
            if (ride->profit != MONEY32_UNDEFINED) {
                formatSecondary = STR_PROFIT_LABEL;
                set_format_arg(2, int32_t, ride->profit);
            }
            break;
        case INFORMATION_TYPE_TOTAL_CUSTOMERS:
            formatSecondary = STR_RIDE_LIST_TOTAL_CUSTOMERS_LABEL;
            set_format_arg(2, uint32_t, ride->total_customers);
            break;
        case INFORMATION_TYPE_TOTAL_PROFIT:
            formatSecondary = 0;
            if (ride->total_profit != MONEY32_UNDEFINED) {
                formatSecondary = STR_RIDE_LIST_TOTAL_PROFIT_LABEL;
                set_format_arg(2, int32_t, ride->total_profit);
            }
            break;
        case INFORMATION_TYPE_CUSTOMERS:
            formatSecondary = STR_RIDE_LIST_CUSTOMERS_PER_HOUR_LABEL;
            set_format_arg(2, uint32_t, ride_customers_per_hour(ride));
            break;
        case INFORMATION_TYPE_AGE:
        {
            int16_t age = date_get_year(gDateMonthsElapsed - ride->build_date);
            switch (age) {
            case 0:  formatSecondary = STR_RIDE_LIST_BUILT_THIS_YEAR_LABEL; break;
            case 1:  formatSecondary = STR_RIDE_LIST_BUILT_LAST_YEAR_LABEL; break;
            default: formatSecondary = STR_RIDE_LIST_BUILT_X_YEARS_AGO_LABEL; break;
            }
            set_format_arg(2, int16_t, age);
            break;
        }
        case INFORMATION_TYPE_INCOME:
            formatSecondary = 0;
            if (ride->income_per_hour != MONEY32_UNDEFINED) {
                formatSecondary = STR_RIDE_LIST_INCOME_LABEL;
                set_format_arg(2, int32_t, ride->income_per_hour);
            }
            break;
        case INFORMATION_TYPE_RUNNING_COST:
            formatSecondary = STR_RIDE_LIST_RUNNING_COST_UNKNOWN;
            if (ride->upkeep_cost != (money16)(uint16_t)0xFFFF) {
                formatSecondary = STR_RIDE_LIST_RUNNING_COST_LABEL;
                set_format_arg(2, int32_t, ride->upkeep_cost * 16);
            }
            break;
        case INFORMATION_TYPE_QUEUE_LENGTH:
            set_format_arg(2, uint16_t, ride_get_total_queue_length(ride));
            formatSecondary = STR_QUEUE_EMPTY;
            {
                uint16_t arg;
                memcpy(&arg, gCommonFormatArgs + 2, sizeof(uint16_t));

                if (arg == 1)
                    formatSecondary = STR_QUEUE_ONE_PERSON;
                else if (arg > 1)
                    formatSecondary = STR_QUEUE_PEOPLE;
            }
            break;
        case INFORMATION_TYPE_QUEUE_TIME:
            set_format_arg(2, uint16_t, ride_get_max_queue_time(ride));
            formatSecondary = STR_QUEUE_TIME_LABEL;
            {
                uint16_t arg;
                memcpy(&arg, gCommonFormatArgs + 2, sizeof(uint16_t));

                if (arg > 1)
                    formatSecondary = STR_QUEUE_TIME_PLURAL_LABEL;
            }
            break;
        case INFORMATION_TYPE_RELIABILITY:
            set_format_arg(2, uint16_t, ride->reliability_percentage);
            formatSecondary = STR_RELIABILITY_LABEL;
            break;
        case INFORMATION_TYPE_DOWN_TIME:
            set_format_arg(2, uint16_t, ride->downtime);
            formatSecondary = STR_DOWN_TIME_LABEL;
            break;
        case INFORMATION_TYPE_GUESTS_FAVOURITE:
            formatSecondary = 0;
            if (gRideClassifications[ride->type] == RIDE_CLASS_RIDE) {
                set_format_arg(2, uint16_t, ride->guests_favourite);
                formatSecondary = ride->guests_favourite == 1 ? STR_GUESTS_FAVOURITE_LABEL : STR_GUESTS_FAVOURITE_PLURAL_LABEL;
            }
            break;
        }

        // Make test red and bold if broken down or crashed
        if (formatSecondary == STR_BROKEN_DOWN || formatSecondary == STR_CRASHED)
            format = STR_RED_OUTLINED_STRING;

        set_format_arg(0, rct_string_id, formatSecondary);
        gfx_draw_string_left_clipped(dpi, format, gCommonFormatArgs, COLOUR_BLACK, 160, y - 1, 157);
        y += SCROLLABLE_ROW_HEIGHT;
    }
}

/**
 *
 *  rct2: 0x006B38EA
 */
static void window_ride_list_draw_tab_images(rct_drawpixelinfo *dpi, rct_window *w)
{
    int32_t sprite_idx;

    // Rides tab
    sprite_idx = SPR_TAB_RIDE_0;
    if (w->page == PAGE_RIDES)
        sprite_idx += w->frame_no / 4;
    gfx_draw_sprite(dpi, sprite_idx, w->x + w->widgets[WIDX_TAB_1].left, w->y + w->widgets[WIDX_TAB_1].top, 0);

    // Shops and stalls tab
    sprite_idx = SPR_TAB_SHOPS_AND_STALLS_0;
    if (w->page == PAGE_SHOPS_AND_STALLS)
        sprite_idx += w->frame_no / 4;
    gfx_draw_sprite(dpi, sprite_idx, w->x + w->widgets[WIDX_TAB_2].left, w->y + w->widgets[WIDX_TAB_2].top, 0);

    // Information kiosks and facilities tab
    sprite_idx = SPR_TAB_KIOSKS_AND_FACILITIES_0;
    if (w->page == PAGE_KIOSKS_AND_FACILITIES)
        sprite_idx += (w->frame_no / 4) % 8;
    gfx_draw_sprite(dpi, sprite_idx, w->x + w->widgets[WIDX_TAB_3].left, w->y + w->widgets[WIDX_TAB_3].top, 0);
}



/**
 *
 *  rct2: 0x006B39A8
 */
void window_ride_list_refresh_list(rct_window *w)
{
    int32_t i;
    Ride *ride, *otherRide;
    char bufferA[128], bufferB[128];
    int32_t list_index = 0;

    FOR_ALL_RIDES(i, ride) {
        if (w->page != gRideClassifications[ride->type] || (ride->status == RIDE_STATUS_CLOSED && !ride_has_any_track_elements(i)))
            continue;

        if (ride->window_invalidate_flags & RIDE_INVALIDATE_RIDE_LIST) {
            ride->window_invalidate_flags &= ~RIDE_INVALIDATE_RIDE_LIST;
        }

        w->list_item_positions[list_index] = i;
        int32_t current_list_position = list_index;
        switch (w->list_information_type) {
        case INFORMATION_TYPE_STATUS:
            format_string_to_upper(bufferA, 128, ride->name, &ride->name_arguments);
            while (--current_list_position >= 0) {
                otherRide = get_ride(w->list_item_positions[current_list_position]);
                format_string_to_upper(bufferB, 128, otherRide->name, &otherRide->name_arguments);
                if (strcmp(bufferA, bufferB) >= 0)
                    break;

                window_bubble_list_item(w, current_list_position);
            }
            break;
        case INFORMATION_TYPE_POPULARITY:
            while (--current_list_position >= 0) {
                otherRide = get_ride(w->list_item_positions[current_list_position]);
                if (ride->popularity * 4 <= otherRide->popularity * 4)
                    break;

                window_bubble_list_item(w, current_list_position);
            }
            break;
        case INFORMATION_TYPE_SATISFACTION:
            while (--current_list_position >= 0) {
                otherRide = get_ride(w->list_item_positions[current_list_position]);
                if (ride->satisfaction * 5 <= otherRide->satisfaction * 5)
                    break;

                window_bubble_list_item(w, current_list_position);
            }
            break;
        case INFORMATION_TYPE_PROFIT:
            while (--current_list_position >= 0) {
                otherRide = get_ride(w->list_item_positions[current_list_position]);
                if (ride->profit <= otherRide->profit)
                    break;

                window_bubble_list_item(w, current_list_position);
            }
            break;
        case INFORMATION_TYPE_TOTAL_CUSTOMERS:
            while (--current_list_position >= 0) {
                otherRide = get_ride(w->list_item_positions[current_list_position]);
                if (ride->total_customers <= otherRide->total_customers)
                    break;

                window_bubble_list_item(w, current_list_position);
            }
            break;
        case INFORMATION_TYPE_TOTAL_PROFIT:
            while (--current_list_position >= 0) {
                otherRide = get_ride(w->list_item_positions[current_list_position]);
                if (ride->total_profit <= otherRide->total_profit)
                    break;

                window_bubble_list_item(w, current_list_position);
            }
            break;
        case INFORMATION_TYPE_CUSTOMERS:
            while (--current_list_position >= 0) {
                otherRide = get_ride(w->list_item_positions[current_list_position]);
                if (ride_customers_per_hour(ride) <= ride_customers_per_hour(otherRide))
                    break;

                window_bubble_list_item(w, current_list_position);
            }
            break;
        case INFORMATION_TYPE_AGE:
            while (--current_list_position >= 0) {
                otherRide = get_ride(w->list_item_positions[current_list_position]);
                if (ride->build_date <= otherRide->build_date)
                    break;

                window_bubble_list_item(w, current_list_position);
            }
            break;
        case INFORMATION_TYPE_INCOME:
            while (--current_list_position >= 0) {
                otherRide = get_ride(w->list_item_positions[current_list_position]);
                if (ride->income_per_hour <= otherRide->income_per_hour)
                    break;

                window_bubble_list_item(w, current_list_position);
            }
            break;
        case INFORMATION_TYPE_RUNNING_COST:
            while (--current_list_position >= 0) {
                otherRide = get_ride(w->list_item_positions[current_list_position]);
                if (ride->upkeep_cost <= otherRide->upkeep_cost)
                    break;

                window_bubble_list_item(w, current_list_position);
            }
            break;
        case INFORMATION_TYPE_QUEUE_LENGTH:
            while (--current_list_position >= 0) {
                otherRide = get_ride(w->list_item_positions[current_list_position]);
                if (ride_get_total_queue_length(ride) <= ride_get_total_queue_length(otherRide))
                    break;

                window_bubble_list_item(w, current_list_position);
            }
            break;
        case INFORMATION_TYPE_QUEUE_TIME:
            while (--current_list_position >= 0) {
                otherRide = get_ride(w->list_item_positions[current_list_position]);
                if (ride_get_max_queue_time(ride) <= ride_get_max_queue_time(otherRide))
                    break;

                window_bubble_list_item(w, current_list_position);
            }
            break;
        case INFORMATION_TYPE_RELIABILITY:
            while (--current_list_position >= 0) {
                otherRide = get_ride(w->list_item_positions[current_list_position]);
                if (ride->reliability_percentage <= otherRide->reliability_percentage)
                    break;

                window_bubble_list_item(w, current_list_position);
            }
            break;
        case INFORMATION_TYPE_DOWN_TIME:
            while (--current_list_position >= 0) {
                otherRide = get_ride(w->list_item_positions[current_list_position]);
                if (ride->downtime <= otherRide->downtime)
                    break;

                window_bubble_list_item(w, current_list_position);
            }
            break;
        case INFORMATION_TYPE_GUESTS_FAVOURITE:
            while (--current_list_position >= 0) {
                otherRide = get_ride(w->list_item_positions[current_list_position]);
                if (ride->guests_favourite <= otherRide->guests_favourite)
                    break;

                window_bubble_list_item(w, current_list_position);
            }
            break;
        }

        list_index++;
    }

    w->no_list_items = list_index;
    w->selected_list_item = -1;
    window_invalidate(w);
}

static void window_ride_list_close_all(rct_window *w)
{
    int32_t i;
    Ride *ride;

    FOR_ALL_RIDES(i, ride) {
        if (w->page != gRideClassifications[ride->type])
            continue;
        if (ride->status == RIDE_STATUS_CLOSED)
            continue;
        ride_set_status(i, RIDE_STATUS_CLOSED);
    }
}

static void window_ride_list_open_all(rct_window *w)
{
    int32_t i;
    Ride *ride;

    FOR_ALL_RIDES(i, ride) {
        if (w->page != gRideClassifications[ride->type])
            continue;
        if (ride->status == RIDE_STATUS_OPEN)
            continue;
        ride_set_status(i, RIDE_STATUS_OPEN);
    }
}
