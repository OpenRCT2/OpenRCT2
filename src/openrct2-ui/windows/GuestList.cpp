/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include <cmath>

#include <openrct2/config/Config.h>
#include <openrct2-ui/windows/Window.h>

#include <openrct2/Game.h>
#include <openrct2-ui/interface/Widget.h>
#include <openrct2/localisation/Localisation.h>
#include <openrct2/sprites.h>
#include <openrct2-ui/interface/Dropdown.h>
#include <openrct2/Context.h>
#include <openrct2/util/Util.h>
#include <openrct2/drawing/Drawing.h>
#include <openrct2/world/Sprite.h>
#include <openrct2/scenario/Scenario.h>

// clang-format off
enum {
    PAGE_INDIVIDUAL,
    PAGE_SUMMARISED
};

enum WINDOW_GUEST_LIST_WIDGET_IDX {
    WIDX_BACKGROUND,
    WIDX_TITLE,
    WIDX_CLOSE,
    WIDX_TAB_CONTENT_PANEL,
    WIDX_PAGE_DROPDOWN,
    WIDX_PAGE_DROPDOWN_BUTTON,
    WIDX_INFO_TYPE_DROPDOWN,
    WIDX_INFO_TYPE_DROPDOWN_BUTTON,
    WIDX_MAP,
    WIDX_FILTER_BY_NAME,
    WIDX_TRACKING,
    WIDX_TAB_1,
    WIDX_TAB_2,
    WIDX_GUEST_LIST
};

enum {
    VIEW_ACTIONS,
    VIEW_THOUGHTS,
    VIEW_COUNT
};

static constexpr const rct_string_id pageNames[] = {
    STR_PAGE_1,
    STR_PAGE_2,
    STR_PAGE_3,
    STR_PAGE_4,
    STR_PAGE_5,
};

static constexpr const rct_string_id filterNames[] = {
    STR_GUESTS_FILTER,
    STR_GUESTS_FILTER_THINKING,
    STR_GUESTS_FILTER_THINKING_ABOUT,
};

static constexpr const rct_string_id viewNames[VIEW_COUNT] = {
    STR_ACTIONS,
    STR_THOUGHTS,
};

static rct_widget window_guest_list_widgets[] = {
    { WWT_FRAME,            0,  0,      349,    0,  329,    0xFFFFFFFF,             STR_NONE },                     // panel / background
    { WWT_CAPTION,          0,  1,      348,    1,  14,     STR_GUESTS,             STR_WINDOW_TITLE_TIP },         // title bar
    { WWT_CLOSEBOX,         0,  337,    347,    2,  13,     STR_CLOSE_X,            STR_CLOSE_WINDOW_TIP },         // close x button
    { WWT_RESIZE,           1,  0,      349,    43, 329,    0xFFFFFFFF,             STR_NONE },                     // tab content panel
    { WWT_DROPDOWN,         1,  5,      84,     59, 70,     STR_PAGE_1,             STR_NONE },                     // page dropdown
    { WWT_BUTTON,           1,  73,     83,     60, 69,     STR_DROPDOWN_GLYPH,     STR_NONE },                     // page dropdown button
    { WWT_DROPDOWN,         1,  120,    261,    59, 70,     0xFFFFFFFF,             STR_INFORMATION_TYPE_TIP },     // information type dropdown
    { WWT_BUTTON,           1,  250,    260,    60, 69,     STR_DROPDOWN_GLYPH,     STR_INFORMATION_TYPE_TIP },     // information type dropdown button
    { WWT_FLATBTN,          1,  273,    296,    46, 69,     SPR_MAP,                STR_SHOW_GUESTS_ON_MAP_TIP },   // map
    { WWT_FLATBTN,          1,  297,    320,    46, 69,     SPR_G2_SEARCH,          STR_GUESTS_FILTER_BY_NAME_TIP },// filter by name
    { WWT_FLATBTN,          1,  321,    344,    46, 69,     SPR_TRACK_PEEP,         STR_TRACKED_GUESTS_ONLY_TIP },  // tracking
    { WWT_TAB,              1,  3,      33,     17, 43,     IMAGE_TYPE_REMAP | SPR_TAB,   STR_INDIVIDUAL_GUESTS_TIP },    // tab 1
    { WWT_TAB,              1,  34,     64,     17, 43,     IMAGE_TYPE_REMAP | SPR_TAB,   STR_SUMMARISED_GUESTS_TIP },    // tab 2
    { WWT_SCROLL,           1,  3,      346,    72, 326,    SCROLL_BOTH,            STR_NONE },                     // guest list
    { WIDGETS_END },
};

static constexpr const uint8_t SUMMARISED_GUEST_ROW_HEIGHT = SCROLLABLE_ROW_HEIGHT + 11;

static void window_guest_list_mouseup(rct_window *w, rct_widgetindex widgetIndex);
static void window_guest_list_resize(rct_window *w);
static void window_guest_list_mousedown(rct_window *w, rct_widgetindex widgetIndex, rct_widget* widget);
static void window_guest_list_dropdown(rct_window *w, rct_widgetindex widgetIndex, int32_t dropdownIndex);
static void window_guest_list_update(rct_window *w);
static void window_guest_list_scrollgetsize(rct_window *w, int32_t scrollIndex, int32_t *width, int32_t *height);
static void window_guest_list_scrollmousedown(rct_window *w, int32_t scrollIndex, int32_t x, int32_t y);
static void window_guest_list_scrollmouseover(rct_window *w, int32_t scrollIndex, int32_t x, int32_t y);
static void window_guest_list_tooltip(rct_window* w, rct_widgetindex widgetIndex, rct_string_id *stringId);
static void window_guest_list_invalidate(rct_window *w);
static void window_guest_list_paint(rct_window *w, rct_drawpixelinfo *dpi);
static void window_guest_list_scrollpaint(rct_window *w, rct_drawpixelinfo *dpi, int32_t scrollIndex);
static void window_guest_list_textinput(rct_window *w, rct_widgetindex widgetIndex, char *text);

static rct_window_event_list window_guest_list_events = {
    nullptr,
    window_guest_list_mouseup,
    window_guest_list_resize,
    window_guest_list_mousedown,
    window_guest_list_dropdown,
    nullptr,
    window_guest_list_update,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    window_guest_list_scrollgetsize,
    window_guest_list_scrollmousedown,
    nullptr,
    window_guest_list_scrollmouseover,
    window_guest_list_textinput,
    nullptr,
    nullptr,
    window_guest_list_tooltip,
    nullptr,
    nullptr,
    window_guest_list_invalidate,
    window_guest_list_paint,
    window_guest_list_scrollpaint
};
// clang-format on

static uint32_t _window_guest_list_last_find_groups_tick;
static uint32_t _window_guest_list_last_find_groups_selected_view;
static uint32_t _window_guest_list_last_find_groups_wait;

static int32_t _window_guest_list_highlighted_index; // 0x00F1EE10
static int32_t _window_guest_list_selected_tab;      // 0x00F1EE12
static int32_t _window_guest_list_selected_filter;   // 0x00F1EE06
static int32_t _window_guest_list_selected_page;     // 0x00F1EE07
static uint32_t _window_guest_list_selected_view;     // 0x00F1EE13
static int32_t _window_guest_list_num_pages;         // 0x00F1EE08
static int32_t _window_guest_list_num_groups;        // 0x00F1AF22
static bool _window_guest_list_tracking_only;
static uint16_t _window_guest_list_filter_arguments[4];

static uint16_t _window_guest_list_groups_num_guests[240];
static uint32_t _window_guest_list_groups_argument_1[240];
static uint32_t _window_guest_list_groups_argument_2[240];
static uint8_t _window_guest_list_groups_guest_faces[240 * 58];
static uint8_t _window_guest_list_group_index[240];

static char _window_guest_list_filter_name[32];

static int32_t window_guest_list_is_peep_in_filter(rct_peep* peep);
static void window_guest_list_find_groups();

static void get_arguments_from_peep(rct_peep *peep, uint32_t *argument_1, uint32_t* argument_2);

static bool guest_should_be_visible(rct_peep *peep);

void window_guest_list_init_vars()
{
    _window_guest_list_selected_tab = 0;
    _window_guest_list_selected_view = 0;
    _window_guest_list_last_find_groups_tick = 0xFFFFFFFF;
    _window_guest_list_selected_filter = 0xFF;
    _window_guest_list_last_find_groups_wait = 0;
}

/**
 *
 *  rct2: 0x006992E3
 */
rct_window * window_guest_list_open()
{
    rct_window* window;

    // Check if window is already open
    window = window_bring_to_front_by_class(WC_GUEST_LIST);
    if (window != nullptr)
        return window;

    window = window_create_auto_pos(350, 330, &window_guest_list_events, WC_GUEST_LIST, WF_10 | WF_RESIZABLE);
    window->widgets = window_guest_list_widgets;
    window->enabled_widgets =
        (1 << WIDX_CLOSE) |
        (1 << WIDX_PAGE_DROPDOWN) |
        (1 << WIDX_PAGE_DROPDOWN_BUTTON) |
        (1 << WIDX_INFO_TYPE_DROPDOWN) |
        (1 << WIDX_INFO_TYPE_DROPDOWN_BUTTON) |
        (1 << WIDX_MAP) |
        (1 << WIDX_TRACKING) |
        (1 << WIDX_TAB_1) |
        (1 << WIDX_TAB_2) |
        (1 << WIDX_FILTER_BY_NAME);

    window_init_scroll_widgets(window);
    _window_guest_list_highlighted_index = -1;
    window->list_information_type = 0;
    _window_guest_list_selected_tab = PAGE_INDIVIDUAL;
    _window_guest_list_selected_filter = -1;
    _window_guest_list_selected_page = 0;
    _window_guest_list_num_pages = 1;
    _window_guest_list_tracking_only = false;
    _window_guest_list_filter_name[0] = '\0';
    window_guest_list_widgets[WIDX_TRACKING].type = WWT_FLATBTN;
    window_guest_list_widgets[WIDX_FILTER_BY_NAME].type = WWT_FLATBTN;
    window_guest_list_widgets[WIDX_PAGE_DROPDOWN].type = WWT_EMPTY;
    window_guest_list_widgets[WIDX_PAGE_DROPDOWN_BUTTON].type = WWT_EMPTY;
    window->var_492 = 0;
    window->min_width = 350;
    window->min_height = 330;
    window->max_width = 500;
    window->max_height = 450;

    return window;
}

void window_guest_list_refresh_list()
{
    _window_guest_list_last_find_groups_wait = 0;
    _window_guest_list_last_find_groups_tick = 0;
    window_guest_list_find_groups();
}

/**
 *
 *  rct2: 0x006993BA
 *
 * @param index The number of the ride or index of the thought
 */
rct_window * window_guest_list_open_with_filter(int32_t type, int32_t index)
{
    rct_window * w = window_guest_list_open();

    _window_guest_list_selected_page = 0;
    _window_guest_list_num_pages = 1;
    _window_guest_list_tracking_only = false;

    switch(type) {
    case GLFT_GUESTS_ON_RIDE:
    {
        Ride *ride = get_ride(index & 0x000000FF);
        _window_guest_list_filter_arguments[0] = STR_ON_RIDE;
        if (ride_type_has_flag(ride->type, RIDE_TYPE_FLAG_IN_RIDE)) {
            _window_guest_list_filter_arguments[0] = STR_IN_RIDE;
        }
        _window_guest_list_filter_arguments[1] = ride->name;
        _window_guest_list_filter_arguments[2] = ride->name_arguments_type_name;
        _window_guest_list_filter_arguments[3] = ride->name_arguments_number;

        _window_guest_list_selected_filter = 0;
        _window_guest_list_highlighted_index = 0xFFFF;
        _window_guest_list_selected_tab = 0;
        _window_guest_list_selected_view = 0;
        break;
    }
    case GLFT_GUESTS_IN_QUEUE:
    {
        Ride *ride = get_ride(index & 0x000000FF);
        _window_guest_list_filter_arguments[0] = STR_QUEUING_FOR;
        _window_guest_list_filter_arguments[1] = ride->name;
        _window_guest_list_filter_arguments[2] = ride->name_arguments_type_name;
        _window_guest_list_filter_arguments[3] = ride->name_arguments_number;

        _window_guest_list_selected_filter = 0;
        _window_guest_list_highlighted_index = 0xFFFF;
        _window_guest_list_selected_tab = 0;
        _window_guest_list_selected_view = 0;
        break;
    }
    case GLFT_GUESTS_THINKING_ABOUT_RIDE:
    {
        Ride *ride = get_ride(index & 0x000000FF);
        _window_guest_list_filter_arguments[0] = STR_NONE;
        _window_guest_list_filter_arguments[1] = ride->name;
        _window_guest_list_filter_arguments[2] = ride->name_arguments_type_name;
        _window_guest_list_filter_arguments[3] = ride->name_arguments_number;

        _window_guest_list_selected_filter = 1;
        _window_guest_list_highlighted_index = 0xFFFF;
        _window_guest_list_selected_tab = 0;
        _window_guest_list_selected_view = 1;
        break;
    }
    case GLFT_GUESTS_THINKING_X:
    {
        _window_guest_list_filter_arguments[0] = PeepThoughts[(index & 0x000000FF)];
        _window_guest_list_filter_arguments[1] = 0;
        _window_guest_list_filter_arguments[2] = 0;
        _window_guest_list_filter_arguments[3] = 0;

        _window_guest_list_selected_filter = 1;
        _window_guest_list_highlighted_index = 0xFFFF;
        _window_guest_list_selected_tab = 0;
        _window_guest_list_selected_view = 1;
        break;
    }
    }

    return w;
}

/**
 *
 *  rct2: 0x00699AAF
 */
static void window_guest_list_mouseup(rct_window *w, rct_widgetindex widgetIndex)
{
    switch (widgetIndex) {
    case WIDX_CLOSE:
        window_close(w);
        break;
    case WIDX_MAP:
        context_open_window(WC_MAP);
        break;
    case WIDX_TRACKING:
        _window_guest_list_tracking_only = !_window_guest_list_tracking_only;
        if (_window_guest_list_tracking_only)
            w->pressed_widgets |= (1 << WIDX_TRACKING);
        else
            w->pressed_widgets &= ~(1 << WIDX_TRACKING);
        window_invalidate(w);
        w->scrolls[0].v_top = 0;
        break;
    case WIDX_FILTER_BY_NAME:
        if (strnlen(_window_guest_list_filter_name, sizeof(_window_guest_list_filter_name)) > 0)
        {
            // Unset the search filter.
            _window_guest_list_filter_name[0] = '\0';
            w->pressed_widgets &= ~(1 << WIDX_FILTER_BY_NAME);
        }
        else
        {
            window_text_input_open(
                w,
                WIDX_FILTER_BY_NAME,
                STR_GUESTS_FILTER_BY_NAME,
                STR_GUESTS_ENTER_NAME_TO_SEARCH,
                STR_STRING,
                (uintptr_t) &_window_guest_list_filter_name,
                sizeof(_window_guest_list_filter_name));
        }
        break;
    }
}

/**
 *
 *  rct2: 0x00699EA3
 */
static void window_guest_list_resize(rct_window *w)
{
    w->min_width = 350;
    w->min_height = 330;
    if (w->width < w->min_width) {
        window_invalidate(w);
        w->width = w->min_width;
    }
    if (w->height < w->min_height) {
        window_invalidate(w);
        w->height = w->min_height;
    }
}

/**
 *
 *  rct2: 0x00699AC4
 */
static void window_guest_list_mousedown(rct_window *w, rct_widgetindex widgetIndex, rct_widget* widget)
{
    int32_t i;
    switch (widgetIndex) {
    case WIDX_TAB_1:
    case WIDX_TAB_2:
        if (_window_guest_list_selected_filter == -1)
            if (_window_guest_list_selected_tab == widgetIndex - WIDX_TAB_1)
                break;
        _window_guest_list_selected_tab = widgetIndex - WIDX_TAB_1;
        _window_guest_list_selected_page = 0;
        _window_guest_list_num_pages = 1;
        window_guest_list_widgets[WIDX_TRACKING].type = WWT_EMPTY;
        window_guest_list_widgets[WIDX_FILTER_BY_NAME].type = WWT_EMPTY;
        if (_window_guest_list_selected_tab == PAGE_INDIVIDUAL)
        {
            window_guest_list_widgets[WIDX_TRACKING].type = WWT_FLATBTN;
            window_guest_list_widgets[WIDX_FILTER_BY_NAME].type = WWT_FLATBTN;
        }
        window_guest_list_widgets[WIDX_PAGE_DROPDOWN].type = WWT_EMPTY;
        window_guest_list_widgets[WIDX_PAGE_DROPDOWN_BUTTON].type = WWT_EMPTY;
        w->list_information_type = 0;
        _window_guest_list_selected_filter = -1;
        window_invalidate(w);
        w->scrolls[0].v_top = 0;
        break;
    case WIDX_PAGE_DROPDOWN_BUTTON:
        widget = &w->widgets[widgetIndex - 1];

        window_dropdown_show_text_custom_width(
            w->x + widget->left,
            w->y + widget->top,
            widget->bottom - widget->top + 1,
            w->colours[1],
            0,
            DROPDOWN_FLAG_STAY_OPEN,
            _window_guest_list_num_pages,
            widget->right - widget->left - 3
        );

        for (i = 0; i < _window_guest_list_num_pages; i++) {
            gDropdownItemsFormat[i] = STR_DROPDOWN_MENU_LABEL;
            gDropdownItemsArgs[i] = pageNames[i];
        }
        dropdown_set_checked(_window_guest_list_selected_page, true);
        break;
    case WIDX_INFO_TYPE_DROPDOWN_BUTTON:
        widget = &w->widgets[widgetIndex - 1];

        for (i = 0; i < VIEW_COUNT; i++) {
            gDropdownItemsFormat[i] = STR_DROPDOWN_MENU_LABEL;
            gDropdownItemsArgs[i] = viewNames[i];
        }

        window_dropdown_show_text_custom_width(
            w->x + widget->left,
            w->y + widget->top,
            widget->bottom - widget->top + 1,
            w->colours[1],
            0,
            DROPDOWN_FLAG_STAY_OPEN,
            2,
            widget->right - widget->left - 3
        );

        dropdown_set_checked(_window_guest_list_selected_view, true);
        break;
    }
}

/**
 *
 *  rct2: 0x00699AE1
 */
static void window_guest_list_dropdown(rct_window *w, rct_widgetindex widgetIndex, int32_t dropdownIndex)
{
    switch (widgetIndex) {
    case WIDX_PAGE_DROPDOWN_BUTTON:
        if (dropdownIndex == -1)
            break;
        _window_guest_list_selected_page = dropdownIndex;
        window_invalidate(w);
        break;
    case WIDX_INFO_TYPE_DROPDOWN_BUTTON:
        if (dropdownIndex == -1)
            break;
        _window_guest_list_selected_view = dropdownIndex;
        window_invalidate(w);
        break;
    }
}

/**
 *
 *  rct2: 0x00699E54
 */
static void window_guest_list_update(rct_window *w)
{
    if (_window_guest_list_last_find_groups_wait != 0) {
        _window_guest_list_last_find_groups_wait--;
    }
    w->list_information_type++;
    if (w->list_information_type >= (_window_guest_list_selected_tab == PAGE_INDIVIDUAL ? 24 : 32))
        w->list_information_type = 0;
    widget_invalidate(w, WIDX_TAB_1 + _window_guest_list_selected_tab);
}

/**
 *
 *  rct2: 0x00699C55
 */
static void window_guest_list_scrollgetsize(rct_window *w, int32_t scrollIndex, int32_t *width, int32_t *height)
{
    int32_t i, y, numGuests, spriteIndex;
    rct_peep *peep;

    switch (_window_guest_list_selected_tab) {
    case PAGE_INDIVIDUAL:
        // Count the number of guests
        numGuests = 0;

        FOR_ALL_GUESTS(spriteIndex, peep) {
            if (peep->outside_of_park != 0)
                continue;
            if (_window_guest_list_selected_filter != -1)
                if (window_guest_list_is_peep_in_filter(peep))
                    continue;
            if (!guest_should_be_visible(peep))
                continue;
            numGuests++;
        }
        w->var_492 = numGuests;
        y = numGuests * SCROLLABLE_ROW_HEIGHT;
        _window_guest_list_num_pages = (int32_t) std::ceil((float)numGuests / 3173);
        if (_window_guest_list_num_pages == 0)
            _window_guest_list_selected_page = 0;
        else if (_window_guest_list_selected_page >= _window_guest_list_num_pages)
            _window_guest_list_selected_page = _window_guest_list_num_pages - 1;
        break;
    case PAGE_SUMMARISED:
        // Find the groups
        window_guest_list_find_groups();
        w->var_492 = _window_guest_list_num_groups;
        y = _window_guest_list_num_groups * SUMMARISED_GUEST_ROW_HEIGHT;
        break;
    default:
        log_error("Improper tab selected: %d, bailing out.", _window_guest_list_selected_tab);
        return;
    }

    i = _window_guest_list_selected_page;
    for (i = _window_guest_list_selected_page - 1; i >= 0; i--)
        y -= 0x7BF2;
    if (y < 0)
        y = 0;
    if (y > 0x7BF2)
        y = 0x7BF2;
    if (_window_guest_list_highlighted_index != -1) {
        _window_guest_list_highlighted_index = -1;
        window_invalidate(w);
    }

    i = y - window_guest_list_widgets[WIDX_GUEST_LIST].bottom + window_guest_list_widgets[WIDX_GUEST_LIST].top + 21;
    if (i < 0)
        i = 0;
    if (i < w->scrolls[0].v_top) {
        w->scrolls[0].v_top = i;
        window_invalidate(w);
    }

    *width = 447;
    *height = y;
}

/**
 *
 *  rct2: 0x00699D7D
 */
static void window_guest_list_scrollmousedown(rct_window *w, int32_t scrollIndex, int32_t x, int32_t y)
{
    int32_t i, spriteIndex;
    rct_peep *peep;

    switch (_window_guest_list_selected_tab) {
    case PAGE_INDIVIDUAL:
        i = y / SCROLLABLE_ROW_HEIGHT;
        i += _window_guest_list_selected_page * 3173;
        FOR_ALL_GUESTS(spriteIndex, peep) {
            if (peep->outside_of_park != 0)
                continue;
            if (_window_guest_list_selected_filter != -1)
                if (window_guest_list_is_peep_in_filter(peep))
                    continue;
            if (!guest_should_be_visible(peep))
                continue;

            if (i == 0) {
                // Open guest window
                window_guest_open(peep);

                break;
            } else {
                i--;
            }
        }
        break;
    case PAGE_SUMMARISED:
        i = y / SUMMARISED_GUEST_ROW_HEIGHT;
        if (i < _window_guest_list_num_groups) {
            memcpy(_window_guest_list_filter_arguments + 0, &_window_guest_list_groups_argument_1[i], 4);
            memcpy(_window_guest_list_filter_arguments + 2, &_window_guest_list_groups_argument_2[i], 4);
            _window_guest_list_selected_filter = _window_guest_list_selected_view;
            _window_guest_list_selected_tab = PAGE_INDIVIDUAL;
            window_guest_list_widgets[WIDX_TRACKING].type = WWT_FLATBTN;
            window_invalidate(w);
            w->scrolls[0].v_top = 0;
        }
        break;
    }
}

/**
 *
 *  rct2: 0x00699D3B
 */
static void window_guest_list_scrollmouseover(rct_window *w, int32_t scrollIndex, int32_t x, int32_t y)
{
    int32_t i;

    i = y / (_window_guest_list_selected_tab == PAGE_INDIVIDUAL ? SCROLLABLE_ROW_HEIGHT : SUMMARISED_GUEST_ROW_HEIGHT);
    i += _window_guest_list_selected_page * 3173;
    if (i != _window_guest_list_highlighted_index) {
        _window_guest_list_highlighted_index = i;
        window_invalidate(w);
    }
}

/**
 *
 *  rct2: 0x00699E4A
 */
static void window_guest_list_tooltip(rct_window* w, rct_widgetindex widgetIndex, rct_string_id *stringId)
{
    set_format_arg(0, rct_string_id, STR_LIST);
}

/**
 *
 *  rct2: 0x00699511
 */
static void window_guest_list_invalidate(rct_window *w)
{
    w->pressed_widgets &= ~(1 << WIDX_TAB_1);
    w->pressed_widgets &= ~(1 << WIDX_TAB_2);
    w->pressed_widgets |= (1LL << (_window_guest_list_selected_tab + WIDX_TAB_1));

    window_guest_list_widgets[WIDX_INFO_TYPE_DROPDOWN].text = viewNames[_window_guest_list_selected_view];
    window_guest_list_widgets[WIDX_MAP].type = WWT_EMPTY;
    if (_window_guest_list_selected_tab == PAGE_INDIVIDUAL && _window_guest_list_selected_filter != -1)
        window_guest_list_widgets[WIDX_MAP].type = WWT_FLATBTN;

    window_guest_list_widgets[WIDX_BACKGROUND].right = w->width - 1;
    window_guest_list_widgets[WIDX_BACKGROUND].bottom = w->height - 1;
    window_guest_list_widgets[WIDX_TAB_CONTENT_PANEL].right = w->width - 1;
    window_guest_list_widgets[WIDX_TAB_CONTENT_PANEL].bottom = w->height - 1;
    window_guest_list_widgets[WIDX_TITLE].right = w->width - 2;
    window_guest_list_widgets[WIDX_CLOSE].left = w->width - 13;
    window_guest_list_widgets[WIDX_CLOSE].right = w->width - 3;
    window_guest_list_widgets[WIDX_GUEST_LIST].right = w->width - 4;
    window_guest_list_widgets[WIDX_GUEST_LIST].bottom = w->height - 15;
    window_guest_list_widgets[WIDX_PAGE_DROPDOWN].text = pageNames[_window_guest_list_selected_page];
    window_guest_list_widgets[WIDX_MAP].left = 273 - 350 + w->width;
    window_guest_list_widgets[WIDX_MAP].right = 296 - 350 + w->width;
    window_guest_list_widgets[WIDX_FILTER_BY_NAME].left = 297 - 350 + w->width;
    window_guest_list_widgets[WIDX_FILTER_BY_NAME].right = 320 - 350 + w->width;
    window_guest_list_widgets[WIDX_TRACKING].left = 321 - 350 + w->width;
    window_guest_list_widgets[WIDX_TRACKING].right = 344 - 350 + w->width;

    if (_window_guest_list_num_pages > 1) {
        window_guest_list_widgets[WIDX_PAGE_DROPDOWN].type = WWT_DROPDOWN;
        window_guest_list_widgets[WIDX_PAGE_DROPDOWN_BUTTON].type = WWT_BUTTON;
    }else{
        window_guest_list_widgets[WIDX_PAGE_DROPDOWN].type = WWT_EMPTY;
        window_guest_list_widgets[WIDX_PAGE_DROPDOWN_BUTTON].type = WWT_EMPTY;
    }
}

/**
 *
 *  rct2: 0x006995CC
 */
static void window_guest_list_paint(rct_window *w, rct_drawpixelinfo *dpi)
{
    int32_t i, x, y;
    rct_string_id format;

    // Widgets
    window_draw_widgets(w, dpi);
    // Tab 1 image
    i = (_window_guest_list_selected_tab == 0 ? w->list_information_type & 0x0FFFFFFFC : 0);
    i += g_peep_animation_entries[PEEP_SPRITE_TYPE_NORMAL].sprite_animation->base_image + 1;
    i |= 0xA1600000;
    gfx_draw_sprite(
        dpi,
        i,
        (window_guest_list_widgets[WIDX_TAB_1].left + window_guest_list_widgets[WIDX_TAB_1].right) / 2 + w->x,
        window_guest_list_widgets[WIDX_TAB_1].bottom - 6 + w->y, 0
    );

    // Tab 2 image
    i = (_window_guest_list_selected_tab == 1 ? w->list_information_type / 4 : 0);
    gfx_draw_sprite(
        dpi,
        SPR_TAB_GUESTS_0 + i,
        window_guest_list_widgets[WIDX_TAB_2].left + w->x,
        window_guest_list_widgets[WIDX_TAB_2].top + w->y, 0
    );

    // Filter description
    x = w->x + 6;
    y = w->y + window_guest_list_widgets[WIDX_TAB_CONTENT_PANEL].top + 3;
    if (_window_guest_list_selected_tab == PAGE_INDIVIDUAL) {
        if (_window_guest_list_selected_filter != -1) {
            if (_window_guest_list_filter_arguments[0] != 0xFFFF) {
                format = filterNames[_window_guest_list_selected_filter]; // Not sure whether the index will ever be 2
            } else {
                format = STR_GUESTS_FILTER_THINKING_ABOUT;
            }
        } else {
            format = STR_ALL_GUESTS;
        }
    } else {
        format = STR_ALL_GUESTS_SUMMARISED;
    }
    gfx_draw_string_left_clipped(dpi, format, _window_guest_list_filter_arguments, COLOUR_BLACK, x, y, 310);

    // Number of guests (list items)
    if (_window_guest_list_selected_tab == PAGE_INDIVIDUAL) {
        x = w->x + 4;
        y = w->y + window_guest_list_widgets[WIDX_GUEST_LIST].bottom + 2;
        set_format_arg(0, int16_t, w->var_492);
        gfx_draw_string_left(dpi, (w->var_492 == 1 ? STR_FORMAT_NUM_GUESTS_SINGULAR : STR_FORMAT_NUM_GUESTS_PLURAL), gCommonFormatArgs, COLOUR_BLACK, x, y);
    }
}

/**
 *
 *  rct2: 0x00699701
 */
static void window_guest_list_scrollpaint(rct_window *w, rct_drawpixelinfo *dpi, int32_t scrollIndex)
{
    int32_t spriteIndex, numGuests, i, j, y;
    rct_string_id format;
    rct_peep *peep;
    rct_peep_thought *thought;
    uint32_t argument_1, argument_2;

    // Background fill
    gfx_fill_rect(dpi, dpi->x, dpi->y, dpi->x + dpi->width - 1, dpi->y + dpi->height - 1, ColourMapA[w->colours[1]].mid_light);

    switch (_window_guest_list_selected_tab) {
    case PAGE_INDIVIDUAL:
        i = 0;
        y = _window_guest_list_selected_page * -0x7BF2;

        // For each guest
        FOR_ALL_GUESTS(spriteIndex, peep) {
            sprite_set_flashing((rct_sprite*)peep, false);
            if (peep->outside_of_park != 0)
                continue;
            if (_window_guest_list_selected_filter != -1) {
                if (window_guest_list_is_peep_in_filter(peep))
                    continue;
                gWindowMapFlashingFlags |= (1 << 0);
                sprite_set_flashing((rct_sprite*)peep, true);
            }
            if (!guest_should_be_visible(peep))
                continue;

            // Check if y is beyond the scroll control
            if (y + SCROLLABLE_ROW_HEIGHT + 1 >= -0x7FFF &&
                y + SCROLLABLE_ROW_HEIGHT + 1 > dpi->y &&
                y < 0x7FFF &&
                y < dpi->y + dpi->height) {

                // Highlight backcolour and text colour (format)
                format = STR_BLACK_STRING;
                if (i == _window_guest_list_highlighted_index) {
                    gfx_filter_rect(dpi, 0, y, 800, y + SCROLLABLE_ROW_HEIGHT - 1, PALETTE_DARKEN_1);
                    format = STR_WINDOW_COLOUR_2_STRINGID;
                }

                // Guest name
                set_format_arg(0, rct_string_id, peep->name_string_idx);
                set_format_arg(2, uint32_t, peep->id);
                gfx_draw_string_left_clipped(dpi, format, gCommonFormatArgs, COLOUR_BLACK, 0, y, 113);

                switch (_window_guest_list_selected_view) {
                case VIEW_ACTIONS:
                    // Guest face
                    gfx_draw_sprite(dpi, get_peep_face_sprite_small(peep), 118, y + 1, 0);

                    // Tracking icon
                    if (peep->peep_flags & PEEP_FLAGS_TRACKING)
                        gfx_draw_sprite(dpi, STR_ENTER_SELECTION_SIZE, 112, y + 1, 0);

                    // Action

                    get_arguments_from_action(peep, &argument_1, &argument_2);

                    set_format_arg(0, uint32_t, argument_1);
                    set_format_arg(4, uint32_t, argument_2);
                    gfx_draw_string_left_clipped(dpi, format, gCommonFormatArgs, COLOUR_BLACK, 133, y, 314);
                    break;
                case VIEW_THOUGHTS:
                    // For each thought
                    for (j = 0; j < PEEP_MAX_THOUGHTS; j++) {
                        thought = &peep->thoughts[j];
                        if (thought->type == PEEP_THOUGHT_TYPE_NONE)
                            break;
                        if (thought->freshness == 0)
                            continue;
                        if (thought->freshness > 5)
                            break;

                        peep_thought_set_format_args(&peep->thoughts[j]);
                        gfx_draw_string_left_clipped(dpi, format, gCommonFormatArgs, COLOUR_BLACK, 118, y, 329);
                        break;
                    }
                    break;
                }
            }

            // Increment list item index and y
            i++;
            y += SCROLLABLE_ROW_HEIGHT;
        }
        break;
    case PAGE_SUMMARISED:
        y = 0;

        // For each group of guests
        for (i = 0; i < _window_guest_list_num_groups; i++) {
            // Check if y is beyond the scroll control
            if (y + SUMMARISED_GUEST_ROW_HEIGHT + 1 >= dpi->y) {
                // Check if y is beyond the scroll control
                if (y >= dpi->y + dpi->height)
                    break;

                // Highlight backcolour and text colour (format)
                format = STR_BLACK_STRING;
                if (i == _window_guest_list_highlighted_index) {
                    gfx_filter_rect(dpi, 0, y, 800, y + SUMMARISED_GUEST_ROW_HEIGHT, PALETTE_DARKEN_1);
                    format = STR_WINDOW_COLOUR_2_STRINGID;
                }

                // Draw guest faces
                numGuests = _window_guest_list_groups_num_guests[i];
                for (j = 0; j < 56 && j < numGuests; j++)
                    gfx_draw_sprite(dpi, _window_guest_list_groups_guest_faces[i * 56 + j] + SPR_PEEP_SMALL_FACE_VERY_VERY_UNHAPPY, j * 8, y + 12, 0);

                // Draw action
                set_format_arg(0, uint32_t, _window_guest_list_groups_argument_1[i]);
                set_format_arg(4, uint32_t, _window_guest_list_groups_argument_2[i]);
                set_format_arg(10, uint32_t, numGuests);
                gfx_draw_string_left_clipped(dpi, format, gCommonFormatArgs, COLOUR_BLACK, 0, y, 414);

                // Draw guest count
                set_format_arg(8, rct_string_id, STR_GUESTS_COUNT_COMMA_SEP);
                gfx_draw_string_right(dpi, format, gCommonFormatArgs + 8, COLOUR_BLACK, 326, y);
            }
            y += SUMMARISED_GUEST_ROW_HEIGHT;
        }
        break;
    }
}

static void window_guest_list_textinput(rct_window *w, rct_widgetindex widgetIndex, char *text)
{
    if (text != nullptr && text[0] != '\0')
    {
        safe_strcpy(_window_guest_list_filter_name, text, sizeof(_window_guest_list_filter_name));
        w->pressed_widgets |= (1 << WIDX_FILTER_BY_NAME);
    }
}

/**
 * returns 0 for in filter and 1 for not in filter
 *  rct2: 0x0069B865
 */
static int32_t window_guest_list_is_peep_in_filter(rct_peep* peep)
{
    char temp;

    temp = _window_guest_list_selected_view;
    _window_guest_list_selected_view = _window_guest_list_selected_filter;
    uint32_t argument1, argument2;
    get_arguments_from_peep(peep, &argument1, &argument2);

    _window_guest_list_selected_view = temp;

    if (_window_guest_list_filter_arguments[0] == 0xFFFF && _window_guest_list_selected_filter == 1)
        argument1 |= 0xFFFF;

    uint32_t check1 = _window_guest_list_filter_arguments[0] | (_window_guest_list_filter_arguments[1] << 16);
    uint32_t check2 = _window_guest_list_filter_arguments[2] | (_window_guest_list_filter_arguments[3] << 16);
    if (argument1 == check1 && argument2 == check2) {
        return 0;
    }
    return 1;
}

/**
 * Calculates a hash value (arguments) for comparing peep actions/thoughts
 *  rct2: 0x0069B7EA
 * peep (esi)
 * argument_1 (0x013CE952) gCommonFormatArgs
 * argument_2 (0x013CE954) gCommonFormatArgs + 2
 */
static void get_arguments_from_peep(rct_peep *peep, uint32_t *argument_1, uint32_t* argument_2)
{
    switch (_window_guest_list_selected_view) {
    case VIEW_ACTIONS:
        get_arguments_from_action(peep, argument_1, argument_2);
        break;
    case VIEW_THOUGHTS:
    {
        rct_peep_thought *thought = &peep->thoughts[0];
        if (thought->freshness <= 5 && thought->type != PEEP_THOUGHT_TYPE_NONE) {
            // HACK The out arguments here are used to draw the group text so we just return
            //      gCommonFormatArgs as two uint32_ts.
            memset(gCommonFormatArgs, 0, sizeof(*argument_1) + sizeof(*argument_2));
            peep_thought_set_format_args(thought);
            memcpy(argument_1, gCommonFormatArgs, sizeof(*argument_1));
            memcpy(argument_2, gCommonFormatArgs + sizeof(*argument_1), sizeof(*argument_2));
        } else {
            *argument_1 = 0;
            *argument_2 = 0;
        }
        break;
    }
    default:
        *argument_1 = 0;
        *argument_2 = 0;
        break;
    }
}

/**
 *
 *  rct2: 0x0069B5AE
 */
static void window_guest_list_find_groups()
{
    int32_t spriteIndex, spriteIndex2, groupIndex, faceIndex;
    rct_peep *peep, *peep2;

    uint32_t tick256 = floor2(gScenarioTicks, 256);
    if (_window_guest_list_selected_view == _window_guest_list_last_find_groups_selected_view) {
        if (_window_guest_list_last_find_groups_wait != 0 ||
            _window_guest_list_last_find_groups_tick == tick256
        ) {
            return;
        }
    }

    _window_guest_list_last_find_groups_tick = tick256;
    _window_guest_list_last_find_groups_selected_view = _window_guest_list_selected_view;
    _window_guest_list_last_find_groups_wait = 320;
    _window_guest_list_num_groups = 0;

    // Set all guests to unassigned
    FOR_ALL_GUESTS(spriteIndex, peep)
        if (peep->outside_of_park == 0)
            peep->flags |= SPRITE_FLAGS_PEEP_VISIBLE;

    // For each guest / group
    FOR_ALL_GUESTS(spriteIndex, peep) {
        if (peep->outside_of_park != 0 || !(peep->flags & SPRITE_FLAGS_PEEP_VISIBLE))
            continue;

        // New group, cap at 240 though
        groupIndex = _window_guest_list_num_groups;
        if (groupIndex >= 240)
            break;

        _window_guest_list_num_groups++;
        _window_guest_list_groups_num_guests[groupIndex] = 1;
        peep->flags &= ~(SPRITE_FLAGS_PEEP_VISIBLE);

        get_arguments_from_peep( peep, &_window_guest_list_groups_argument_1[groupIndex], &_window_guest_list_groups_argument_2[groupIndex]);
        memcpy(_window_guest_list_filter_arguments + 0, &_window_guest_list_groups_argument_1[groupIndex], 4);
        memcpy(_window_guest_list_filter_arguments + 2, &_window_guest_list_groups_argument_2[groupIndex], 4);

        _window_guest_list_group_index[groupIndex] = groupIndex;
        faceIndex = groupIndex * 56;
        _window_guest_list_groups_guest_faces[faceIndex++] = get_peep_face_sprite_small(peep) - SPR_PEEP_SMALL_FACE_VERY_VERY_UNHAPPY;

        // Find more peeps that belong to same group
        FOR_ALL_GUESTS(spriteIndex2, peep2) {
            if (peep2->outside_of_park != 0 || !(peep2->flags & SPRITE_FLAGS_PEEP_VISIBLE))
                continue;

            uint32_t argument1, argument2;
            // Get and check if in same group
            get_arguments_from_peep(peep2, &argument1, &argument2);
            if (argument1 != _window_guest_list_groups_argument_1[groupIndex] || argument2 != _window_guest_list_groups_argument_2[groupIndex] )
                continue;

            // Assign guest
            _window_guest_list_groups_num_guests[groupIndex]++;
            peep2->flags &= ~(SPRITE_FLAGS_PEEP_VISIBLE);

            // Add face sprite, cap at 56 though
            if (_window_guest_list_groups_num_guests[groupIndex] >= 56)
                continue;
            _window_guest_list_groups_guest_faces[faceIndex++] = get_peep_face_sprite_small(peep2) - SPR_PEEP_SMALL_FACE_VERY_VERY_UNHAPPY;
        }

        if (_window_guest_list_filter_arguments[0] == 0) {
            _window_guest_list_num_groups--;
            continue;
        }

        int32_t curr_num_guests = _window_guest_list_groups_num_guests[groupIndex];
        int32_t swap_position = 0;
        //This section places the groups in size order.
        bool gotoNextPeep = false;
        while (1) {
            if (swap_position >= groupIndex) {
                gotoNextPeep = true;
                break;
            }
            if (curr_num_guests > _window_guest_list_groups_num_guests[swap_position])
                break;
            swap_position++;
        }
        if (gotoNextPeep) {
            continue;
        }

        int32_t argument_1 = _window_guest_list_groups_argument_1[groupIndex];
        int32_t argument_2 = _window_guest_list_groups_argument_2[groupIndex];
        int32_t bl = _window_guest_list_group_index[groupIndex];

        do {
            int32_t temp = curr_num_guests;
            curr_num_guests = _window_guest_list_groups_num_guests[swap_position];
            _window_guest_list_groups_num_guests[swap_position] = temp;

            temp = argument_1;
            argument_1 = _window_guest_list_groups_argument_1[swap_position];
            _window_guest_list_groups_argument_1[swap_position] = temp;

            temp = argument_2;
            argument_2 = _window_guest_list_groups_argument_2[swap_position];
            _window_guest_list_groups_argument_2[swap_position] = temp;

            uint8_t temp_faces[56];
            memcpy(temp_faces, &(_window_guest_list_groups_guest_faces[groupIndex*56]), 56);
            memcpy(&(_window_guest_list_groups_guest_faces[groupIndex * 56]), &(_window_guest_list_groups_guest_faces[swap_position * 56]), 56);
            memcpy(&(_window_guest_list_groups_guest_faces[swap_position * 56]), temp_faces, 56);

            temp = _window_guest_list_group_index[swap_position];
            _window_guest_list_group_index[swap_position] = bl;
            bl = temp;
        } while (++swap_position <= groupIndex);
    }
}

static bool guest_should_be_visible(rct_peep *peep)
{
    if (_window_guest_list_tracking_only && !(peep->peep_flags & PEEP_FLAGS_TRACKING))
        return false;

    if (_window_guest_list_filter_name[0] != '\0')
    {
        char formatted[256];

        set_format_arg(0, rct_string_id, peep->name_string_idx);
        set_format_arg(2, uint32_t, peep->id);
        format_string(formatted, sizeof(formatted), peep->name_string_idx, gCommonFormatArgs);

        if (strcasestr(formatted, _window_guest_list_filter_name) == nullptr)
            return false;
    }

    return true;
}
