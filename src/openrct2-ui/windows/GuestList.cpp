/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include <cmath>
#include <openrct2-ui/interface/Dropdown.h>
#include <openrct2-ui/interface/Widget.h>
#include <openrct2-ui/windows/Window.h>
#include <openrct2/Context.h>
#include <openrct2/Game.h>
#include <openrct2/config/Config.h>
#include <openrct2/drawing/Drawing.h>
#include <openrct2/localisation/Localisation.h>
#include <openrct2/ride/RideData.h>
#include <openrct2/scenario/Scenario.h>
#include <openrct2/sprites.h>
#include <openrct2/util/Util.h>
#include <openrct2/world/Sprite.h>
#include <vector>

static constexpr const rct_string_id WINDOW_TITLE = STR_GUESTS;
static constexpr const int32_t WH = 330;
static constexpr const int32_t WW = 350;

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
    WINDOW_SHIM(WINDOW_TITLE, WW, WH),
    MakeWidget({  0, 43}, {350, 287}, WWT_RESIZE,   WindowColour::Secondary                                                   ), // tab content panel
    MakeWidget({  5, 59}, { 80,  12}, WWT_DROPDOWN, WindowColour::Secondary, STR_ARG_4_PAGE_X                                 ), // page dropdown
    MakeWidget({ 73, 60}, { 11,  10}, WWT_BUTTON,   WindowColour::Secondary, STR_DROPDOWN_GLYPH                               ), // page dropdown button
    MakeWidget({120, 59}, {142,  12}, WWT_DROPDOWN, WindowColour::Secondary, 0xFFFFFFFF,         STR_INFORMATION_TYPE_TIP     ), // information type dropdown
    MakeWidget({250, 60}, { 11,  10}, WWT_BUTTON,   WindowColour::Secondary, STR_DROPDOWN_GLYPH, STR_INFORMATION_TYPE_TIP     ), // information type dropdown button
    MakeWidget({273, 46}, { 24,  24}, WWT_FLATBTN,  WindowColour::Secondary, SPR_MAP,            STR_SHOW_GUESTS_ON_MAP_TIP   ), // map
    MakeWidget({297, 46}, { 24,  24}, WWT_FLATBTN,  WindowColour::Secondary, SPR_G2_SEARCH,      STR_GUESTS_FILTER_BY_NAME_TIP), // filter by name
    MakeWidget({321, 46}, { 24,  24}, WWT_FLATBTN,  WindowColour::Secondary, SPR_TRACK_PEEP,     STR_TRACKED_GUESTS_ONLY_TIP  ), // tracking
    MakeTab   ({  3, 17},                                                                        STR_INDIVIDUAL_GUESTS_TIP    ), // tab 1
    MakeTab   ({ 34, 17},                                                                        STR_SUMMARISED_GUESTS_TIP    ), // tab 2
    MakeWidget({  3, 72}, {344, 255}, WWT_SCROLL,   WindowColour::Secondary, SCROLL_BOTH                                      ), // guest list
    { WIDGETS_END },
};

static constexpr const uint8_t SUMMARISED_GUEST_ROW_HEIGHT = SCROLLABLE_ROW_HEIGHT + 11;
static constexpr const auto GUESTS_PER_PAGE = 2000;
static constexpr const auto GUEST_PAGE_HEIGHT = GUESTS_PER_PAGE * SCROLLABLE_ROW_HEIGHT;

static std::vector<uint16_t> GuestList;

static void window_guest_list_mouseup(rct_window *w, rct_widgetindex widgetIndex);
static void window_guest_list_resize(rct_window *w);
static void window_guest_list_mousedown(rct_window *w, rct_widgetindex widgetIndex, rct_widget* widget);
static void window_guest_list_dropdown(rct_window *w, rct_widgetindex widgetIndex, int32_t dropdownIndex);
static void window_guest_list_update(rct_window *w);
static void window_guest_list_scrollgetsize(rct_window *w, int32_t scrollIndex, int32_t *width, int32_t *height);
static void window_guest_list_scrollmousedown(rct_window *w, int32_t scrollIndex, const ScreenCoordsXY& screenCoords);
static void window_guest_list_scrollmouseover(rct_window *w, int32_t scrollIndex, const ScreenCoordsXY& screenCoords);
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
    nullptr,
    nullptr,
    nullptr,
    window_guest_list_invalidate,
    window_guest_list_paint,
    window_guest_list_scrollpaint
};
// clang-format on

struct FilterArguments
{
    uint8_t args[12]{};

    rct_string_id GetFirstStringId()
    {
        rct_string_id firstStrId{};
        std::memcpy(&firstStrId, args, sizeof(firstStrId));
        return firstStrId;
    }
};
static bool operator==(const FilterArguments& l, const FilterArguments& r)
{
    return std::memcmp(l.args, r.args, sizeof(l.args)) == 0;
}
static bool operator!=(const FilterArguments& l, const FilterArguments& r)
{
    return !(l == r);
}

static uint32_t _window_guest_list_last_find_groups_tick;
static uint32_t _window_guest_list_last_find_groups_selected_view;
static uint32_t _window_guest_list_last_find_groups_wait;

static uint32_t _window_guest_list_highlighted_index; // 0x00F1EE10
static int32_t _window_guest_list_selected_tab;       // 0x00F1EE12
static int32_t _window_guest_list_selected_filter;    // 0x00F1EE06
static int32_t _window_guest_list_selected_page;      // 0x00F1EE07
static uint32_t _window_guest_list_selected_view;     // 0x00F1EE13
static uint16_t _window_guest_list_num_pages;         // 0x00F1EE08
static uint32_t _window_guest_list_num_groups;        // 0x00F1AF22
static bool _window_guest_list_tracking_only;
static FilterArguments _window_guest_list_filter_arguments;

static uint16_t _window_guest_list_groups_num_guests[240];
static FilterArguments _window_guest_list_groups_arguments[240];
static uint8_t _window_guest_list_groups_guest_faces[240 * 58];
static uint8_t _window_guest_list_group_index[240];

static char _window_guest_list_filter_name[32];

static int32_t window_guest_list_is_peep_in_filter(Peep* peep);
static void window_guest_list_find_groups();

static FilterArguments get_arguments_from_peep(const Peep* peep);

static bool guest_should_be_visible(Peep* peep);

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
rct_window* window_guest_list_open()
{
    rct_window* window;

    // Check if window is already open
    window = window_bring_to_front_by_class(WC_GUEST_LIST);
    if (window != nullptr)
        return window;

    window = window_create_auto_pos(350, 330, &window_guest_list_events, WC_GUEST_LIST, WF_10 | WF_RESIZABLE);
    window->widgets = window_guest_list_widgets;
    window->enabled_widgets = (1 << WIDX_CLOSE) | (1 << WIDX_PAGE_DROPDOWN) | (1 << WIDX_PAGE_DROPDOWN_BUTTON)
        | (1 << WIDX_INFO_TYPE_DROPDOWN) | (1 << WIDX_INFO_TYPE_DROPDOWN_BUTTON) | (1 << WIDX_MAP) | (1 << WIDX_TRACKING)
        | (1 << WIDX_TAB_1) | (1 << WIDX_TAB_2) | (1 << WIDX_FILTER_BY_NAME);

    window_init_scroll_widgets(window);
    _window_guest_list_highlighted_index = 0xFFFF;
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
    window->min_width = 350;
    window->min_height = 330;
    window->max_width = 500;
    window->max_height = 450;
    window_guest_list_refresh_list();
    return window;
}

void window_guest_list_refresh_list()
{
    if (window_find_by_class(WC_GUEST_LIST) == nullptr)
    {
        return;
    }

    // Only the individual tab uses the GuestList so no point calculating it
    if (_window_guest_list_selected_tab != PAGE_INDIVIDUAL)
    {
        _window_guest_list_last_find_groups_wait = 0;
        _window_guest_list_last_find_groups_tick = 0;
        window_guest_list_find_groups();
        return;
    }

    GuestList.clear();
    for (auto peep : EntityList<Guest>(EntityListId::Peep))
    {
        sprite_set_flashing(peep, false);
        if (peep->OutsideOfPark)
            continue;
        if (_window_guest_list_selected_filter != -1)
        {
            if (window_guest_list_is_peep_in_filter(peep))
                continue;
            sprite_set_flashing(peep, true);
        }
        if (!guest_should_be_visible(peep))
            continue;
        GuestList.push_back(peep->sprite_index);
    }

    std::sort(GuestList.begin(), GuestList.end(), [](const uint16_t a, const uint16_t b) { return peep_compare(a, b) < 0; });
}

/**
 *
 *  rct2: 0x006993BA
 *
 * @param index The number of the ride or index of the thought
 */
rct_window* window_guest_list_open_with_filter(GuestListFilterType type, int32_t index)
{
    rct_window* w = window_guest_list_open();

    _window_guest_list_selected_page = 0;
    _window_guest_list_num_pages = 1;
    _window_guest_list_tracking_only = false;
    _window_guest_list_filter_arguments = {};

    Formatter ft(_window_guest_list_filter_arguments.args);

    switch (type)
    {
        case GuestListFilterType::GuestsOnRide:
        {
            auto ride = get_ride(index & 0xFF);
            if (ride != nullptr)
            {
                ft.Add<rct_string_id>(ride_type_has_flag(ride->type, RIDE_TYPE_FLAG_IN_RIDE) ? STR_IN_RIDE : STR_ON_RIDE);
                ride->FormatNameTo(ft);

                _window_guest_list_selected_filter = 0;
                _window_guest_list_highlighted_index = 0xFFFF;
                _window_guest_list_selected_tab = 0;
                _window_guest_list_selected_view = 0;
            }
            break;
        }
        case GuestListFilterType::GuestsInQueue:
        {
            auto ride = get_ride(index & 0xFF);
            if (ride != nullptr)
            {
                ft.Add<rct_string_id>(STR_QUEUING_FOR);
                ride->FormatNameTo(ft);

                _window_guest_list_selected_filter = 0;
                _window_guest_list_highlighted_index = 0xFFFF;
                _window_guest_list_selected_tab = 0;
                _window_guest_list_selected_view = 0;
            }
            break;
        }
        case GuestListFilterType::GuestsThinkingAboutRide:
        {
            auto ride = get_ride(index & 0xFF);
            if (ride != nullptr)
            {
                ft.Add<rct_string_id>(STR_NONE);
                ride->FormatNameTo(ft);

                _window_guest_list_selected_filter = 1;
                _window_guest_list_highlighted_index = 0xFFFF;
                _window_guest_list_selected_tab = 0;
                _window_guest_list_selected_view = 1;
            }
            break;
        }
        case GuestListFilterType::GuestsThinkingX:
        {
            ft.Add<rct_string_id>(PeepThoughts[index & 0xFF]);

            _window_guest_list_selected_filter = 1;
            _window_guest_list_highlighted_index = 0xFFFF;
            _window_guest_list_selected_tab = 0;
            _window_guest_list_selected_view = 1;
            break;
        }
    }

    window_guest_list_refresh_list();
    return w;
}

/**
 *
 *  rct2: 0x00699AAF
 */
static void window_guest_list_mouseup(rct_window* w, rct_widgetindex widgetIndex)
{
    switch (widgetIndex)
    {
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
            w->Invalidate();
            w->scrolls[0].v_top = 0;
            window_guest_list_refresh_list();
            break;
        case WIDX_FILTER_BY_NAME:
            if (strnlen(_window_guest_list_filter_name, sizeof(_window_guest_list_filter_name)) > 0)
            {
                // Unset the search filter.
                _window_guest_list_filter_name[0] = '\0';
                w->pressed_widgets &= ~(1 << WIDX_FILTER_BY_NAME);
                window_guest_list_refresh_list();
            }
            else
            {
                window_text_input_open(
                    w, WIDX_FILTER_BY_NAME, STR_GUESTS_FILTER_BY_NAME, STR_GUESTS_ENTER_NAME_TO_SEARCH, STR_STRING,
                    reinterpret_cast<uintptr_t>(&_window_guest_list_filter_name), sizeof(_window_guest_list_filter_name));
            }
            break;
    }
}

/**
 *
 *  rct2: 0x00699EA3
 */
static void window_guest_list_resize(rct_window* w)
{
    w->min_width = 350;
    w->min_height = 330;
    if (w->width < w->min_width)
    {
        w->Invalidate();
        w->width = w->min_width;
    }
    if (w->height < w->min_height)
    {
        w->Invalidate();
        w->height = w->min_height;
    }
}

/**
 *
 *  rct2: 0x00699AC4
 */
static void window_guest_list_mousedown(rct_window* w, rct_widgetindex widgetIndex, rct_widget* widget)
{
    int32_t i;
    switch (widgetIndex)
    {
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
            w->Invalidate();
            w->scrolls[0].v_top = 0;
            window_guest_list_refresh_list();
            break;
        case WIDX_PAGE_DROPDOWN_BUTTON:
            widget = &w->widgets[widgetIndex - 1];

            window_dropdown_show_text_custom_width(
                { w->windowPos.x + widget->left, w->windowPos.y + widget->top }, widget->height() + 1, w->colours[1], 0,
                DROPDOWN_FLAG_STAY_OPEN, _window_guest_list_num_pages, widget->width() - 3);

            for (i = 0; i < _window_guest_list_num_pages; i++)
            {
                gDropdownItemsFormat[i] = STR_DROPDOWN_MENU_LABEL;
                uint16_t* args = reinterpret_cast<uint16_t*>(&gDropdownItemsArgs[i]);
                args[0] = STR_PAGE_X;
                args[1] = i + 1;
            }
            dropdown_set_checked(_window_guest_list_selected_page, true);
            break;
        case WIDX_INFO_TYPE_DROPDOWN_BUTTON:
            widget = &w->widgets[widgetIndex - 1];

            for (i = 0; i < VIEW_COUNT; i++)
            {
                gDropdownItemsFormat[i] = STR_DROPDOWN_MENU_LABEL;
                gDropdownItemsArgs[i] = viewNames[i];
            }

            window_dropdown_show_text_custom_width(
                { w->windowPos.x + widget->left, w->windowPos.y + widget->top }, widget->height() + 1, w->colours[1], 0,
                DROPDOWN_FLAG_STAY_OPEN, 2, widget->width() - 3);

            dropdown_set_checked(_window_guest_list_selected_view, true);
            break;
    }
}

/**
 *
 *  rct2: 0x00699AE1
 */
static void window_guest_list_dropdown(rct_window* w, rct_widgetindex widgetIndex, int32_t dropdownIndex)
{
    switch (widgetIndex)
    {
        case WIDX_PAGE_DROPDOWN_BUTTON:
            if (dropdownIndex == -1)
                break;
            _window_guest_list_selected_page = dropdownIndex;
            w->Invalidate();
            break;
        case WIDX_INFO_TYPE_DROPDOWN_BUTTON:
            if (dropdownIndex == -1)
                break;
            _window_guest_list_selected_view = dropdownIndex;
            w->Invalidate();
            break;
    }
}

/**
 *
 *  rct2: 0x00699E54
 */
static void window_guest_list_update(rct_window* w)
{
    if (_window_guest_list_last_find_groups_wait != 0)
    {
        _window_guest_list_last_find_groups_wait--;
    }
    w->list_information_type++;
    if (w->list_information_type >= (_window_guest_list_selected_tab == PAGE_INDIVIDUAL ? 24 : 32))
        w->list_information_type = 0;
    widget_invalidate(w, WIDX_TAB_1 + _window_guest_list_selected_tab);
    gWindowMapFlashingFlags |= (1 << 0);
}

/**
 *
 *  rct2: 0x00699C55
 */
static void window_guest_list_scrollgetsize(rct_window* w, int32_t scrollIndex, int32_t* width, int32_t* height)
{
    int32_t y = 0;
    switch (_window_guest_list_selected_tab)
    {
        case PAGE_INDIVIDUAL:
            // Count the number of guests
            y = static_cast<int32_t>(GuestList.size()) * SCROLLABLE_ROW_HEIGHT;
            _window_guest_list_num_pages = 1 + (static_cast<int16_t>(GuestList.size()) - 1) / GUESTS_PER_PAGE;
            if (_window_guest_list_num_pages == 0)
                _window_guest_list_selected_page = 0;
            else if (_window_guest_list_selected_page >= _window_guest_list_num_pages)
                _window_guest_list_selected_page = _window_guest_list_num_pages - 1;
            break;
        case PAGE_SUMMARISED:
            // Find the groups
            window_guest_list_find_groups();
            y = _window_guest_list_num_groups * SUMMARISED_GUEST_ROW_HEIGHT;
            break;
        default:
            log_error("Improper tab selected: %d, bailing out.", _window_guest_list_selected_tab);
            return;
    }

    y -= GUEST_PAGE_HEIGHT * _window_guest_list_selected_page;
    y = std::max(0, std::min(y, GUEST_PAGE_HEIGHT));

    if (_window_guest_list_highlighted_index != 0xFFFF)
    {
        _window_guest_list_highlighted_index = 0xFFFF;
        w->Invalidate();
    }

    auto i = y - window_guest_list_widgets[WIDX_GUEST_LIST].bottom + window_guest_list_widgets[WIDX_GUEST_LIST].top + 21;
    if (i < 0)
        i = 0;
    if (i < w->scrolls[0].v_top)
    {
        w->scrolls[0].v_top = i;
        w->Invalidate();
    }

    *width = 447;
    *height = y;
}

/**
 *
 *  rct2: 0x00699D7D
 */
static void window_guest_list_scrollmousedown(rct_window* w, int32_t scrollIndex, const ScreenCoordsXY& screenCoords)
{
    uint32_t i = 0;

    switch (_window_guest_list_selected_tab)
    {
        case PAGE_INDIVIDUAL:
            i = screenCoords.y / SCROLLABLE_ROW_HEIGHT;
            i += _window_guest_list_selected_page * GUESTS_PER_PAGE;
            for (auto spriteIndex : GuestList)
            {
                if (i == 0)
                {
                    auto guest = GetEntity<Guest>(spriteIndex);
                    if (guest != nullptr)
                    {
                        window_guest_open(guest);
                    }
                    break;
                }
                i--;
            }
            break;
        case PAGE_SUMMARISED:
            i = screenCoords.y / SUMMARISED_GUEST_ROW_HEIGHT;
            if (i < _window_guest_list_num_groups)
            {
                _window_guest_list_filter_arguments = _window_guest_list_groups_arguments[i];
                _window_guest_list_selected_filter = _window_guest_list_selected_view;
                _window_guest_list_selected_tab = PAGE_INDIVIDUAL;
                window_guest_list_widgets[WIDX_TRACKING].type = WWT_FLATBTN;
                w->Invalidate();
                w->scrolls[0].v_top = 0;
                window_guest_list_refresh_list();
            }
            break;
    }
}

/**
 *
 *  rct2: 0x00699D3B
 */
static void window_guest_list_scrollmouseover(rct_window* w, int32_t scrollIndex, const ScreenCoordsXY& screenCoords)
{
    uint32_t i;

    i = screenCoords.y
        / (_window_guest_list_selected_tab == PAGE_INDIVIDUAL ? SCROLLABLE_ROW_HEIGHT : SUMMARISED_GUEST_ROW_HEIGHT);
    i += _window_guest_list_selected_page * GUESTS_PER_PAGE;
    if (i != _window_guest_list_highlighted_index)
    {
        _window_guest_list_highlighted_index = i;
        w->Invalidate();
    }
}

/**
 *
 *  rct2: 0x00699511
 */
static void window_guest_list_invalidate(rct_window* w)
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
    window_guest_list_widgets[WIDX_MAP].left = 273 - 350 + w->width;
    window_guest_list_widgets[WIDX_MAP].right = 296 - 350 + w->width;
    window_guest_list_widgets[WIDX_FILTER_BY_NAME].left = 297 - 350 + w->width;
    window_guest_list_widgets[WIDX_FILTER_BY_NAME].right = 320 - 350 + w->width;
    window_guest_list_widgets[WIDX_TRACKING].left = 321 - 350 + w->width;
    window_guest_list_widgets[WIDX_TRACKING].right = 344 - 350 + w->width;

    if (_window_guest_list_num_pages > 1)
    {
        window_guest_list_widgets[WIDX_PAGE_DROPDOWN].type = WWT_DROPDOWN;
        window_guest_list_widgets[WIDX_PAGE_DROPDOWN_BUTTON].type = WWT_BUTTON;
        auto ft = Formatter::Common();
        ft.Increment(4);
        ft.Add<uint16_t>(_window_guest_list_selected_page + 1);
    }
    else
    {
        window_guest_list_widgets[WIDX_PAGE_DROPDOWN].type = WWT_EMPTY;
        window_guest_list_widgets[WIDX_PAGE_DROPDOWN_BUTTON].type = WWT_EMPTY;
    }
}

/**
 *
 *  rct2: 0x006995CC
 */
static void window_guest_list_paint(rct_window* w, rct_drawpixelinfo* dpi)
{
    int32_t i;
    rct_string_id format;

    // Widgets
    window_draw_widgets(w, dpi);
    // Tab 1 image
    i = (_window_guest_list_selected_tab == 0 ? w->list_information_type & 0x0FFFFFFFC : 0);
    i += g_peep_animation_entries[PEEP_SPRITE_TYPE_NORMAL].sprite_animation->base_image + 1;
    i |= 0xA1600000;
    gfx_draw_sprite(
        dpi, i,
        w->windowPos
            + ScreenCoordsXY{ window_guest_list_widgets[WIDX_TAB_1].midX(), window_guest_list_widgets[WIDX_TAB_1].bottom - 6 },
        0);

    // Tab 2 image
    i = (_window_guest_list_selected_tab == 1 ? w->list_information_type / 4 : 0);
    gfx_draw_sprite(
        dpi, SPR_TAB_GUESTS_0 + i,
        w->windowPos + ScreenCoordsXY{ window_guest_list_widgets[WIDX_TAB_2].left, window_guest_list_widgets[WIDX_TAB_2].top },
        0);

    // Filter description
    auto screenCoords = w->windowPos + ScreenCoordsXY{ 6, window_guest_list_widgets[WIDX_TAB_CONTENT_PANEL].top + 3 };
    if (_window_guest_list_selected_tab == PAGE_INDIVIDUAL)
    {
        if (_window_guest_list_selected_filter != -1)
        {
            if (_window_guest_list_filter_arguments.GetFirstStringId() != STR_NONE)
            {
                format = filterNames[_window_guest_list_selected_filter]; // Not sure whether the index will ever be 2
            }
            else
            {
                format = STR_GUESTS_FILTER_THINKING_ABOUT;
            }
        }
        else
        {
            format = STR_ALL_GUESTS;
        }
    }
    else
    {
        format = STR_ALL_GUESTS_SUMMARISED;
    }
    gfx_draw_string_left_clipped(dpi, format, _window_guest_list_filter_arguments.args, COLOUR_BLACK, screenCoords, 310);

    // Number of guests (list items)
    if (_window_guest_list_selected_tab == PAGE_INDIVIDUAL)
    {
        screenCoords = w->windowPos + ScreenCoordsXY{ 4, window_guest_list_widgets[WIDX_GUEST_LIST].bottom + 2 };
        auto ft = Formatter::Common();
        ft.Add<int32_t>(static_cast<int32_t>(GuestList.size()));
        gfx_draw_string_left(
            dpi, (GuestList.size() == 1 ? STR_FORMAT_NUM_GUESTS_SINGULAR : STR_FORMAT_NUM_GUESTS_PLURAL), gCommonFormatArgs,
            COLOUR_BLACK, screenCoords);
    }
}

/**
 *
 *  rct2: 0x00699701
 */
static void window_guest_list_scrollpaint(rct_window* w, rct_drawpixelinfo* dpi, int32_t scrollIndex)
{
    int32_t y;
    rct_string_id format;
    rct_peep_thought* thought;

    // Background fill
    gfx_fill_rect(
        dpi, { { dpi->x, dpi->y }, { dpi->x + dpi->width - 1, dpi->y + dpi->height - 1 } },
        ColourMapA[w->colours[1]].mid_light);
    switch (_window_guest_list_selected_tab)
    {
        case PAGE_INDIVIDUAL:
        {
            uint32_t i = 0;
            y = _window_guest_list_selected_page * -GUEST_PAGE_HEIGHT;

            // For each guest
            for (auto spriteIndex : GuestList)
            {
                // Check if y is beyond the scroll control
                if (y + SCROLLABLE_ROW_HEIGHT + 1 >= -0x7FFF && y + SCROLLABLE_ROW_HEIGHT + 1 > dpi->y && y < 0x7FFF
                    && y < dpi->y + dpi->height)
                {
                    // Highlight backcolour and text colour (format)
                    format = STR_BLACK_STRING;
                    if (i == _window_guest_list_highlighted_index)
                    {
                        gfx_filter_rect(dpi, 0, y, 800, y + SCROLLABLE_ROW_HEIGHT - 1, PALETTE_DARKEN_1);
                        format = STR_WINDOW_COLOUR_2_STRINGID;
                    }

                    // Guest name
                    auto peep = GetEntity<Guest>(spriteIndex);
                    if (peep == nullptr)
                    {
                        continue;
                    }
                    auto ft = Formatter::Common();
                    peep->FormatNameTo(ft);
                    gfx_draw_string_left_clipped(dpi, format, gCommonFormatArgs, COLOUR_BLACK, { 0, y }, 113);

                    switch (_window_guest_list_selected_view)
                    {
                        case VIEW_ACTIONS:
                            // Guest face
                            gfx_draw_sprite(dpi, get_peep_face_sprite_small(peep), { 118, y + 1 }, 0);

                            // Tracking icon
                            if (peep->PeepFlags & PEEP_FLAGS_TRACKING)
                                gfx_draw_sprite(dpi, STR_ENTER_SELECTION_SIZE, { 112, y + 1 }, 0);

                            // Action
                            ft = Formatter::Common();
                            peep->FormatActionTo(ft);
                            gfx_draw_string_left_clipped(dpi, format, gCommonFormatArgs, COLOUR_BLACK, { 133, y }, 314);
                            break;
                        case VIEW_THOUGHTS:
                            // For each thought
                            for (uint32_t j = 0; j < PEEP_MAX_THOUGHTS; j++)
                            {
                                thought = &peep->Thoughts[j];
                                if (thought->type == PEEP_THOUGHT_TYPE_NONE)
                                    break;
                                if (thought->freshness == 0)
                                    continue;
                                if (thought->freshness > 5)
                                    break;

                                peep_thought_set_format_args(&peep->Thoughts[j]);
                                gfx_draw_string_left_clipped(dpi, format, gCommonFormatArgs, COLOUR_BLACK, { 118, y }, 329);
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
        }
        case PAGE_SUMMARISED:
            y = 0;

            // For each group of guests
            for (uint32_t i = 0; i < _window_guest_list_num_groups; i++)
            {
                // Check if y is beyond the scroll control
                if (y + SUMMARISED_GUEST_ROW_HEIGHT + 1 >= dpi->y)
                {
                    // Check if y is beyond the scroll control
                    if (y >= dpi->y + dpi->height)
                        break;

                    // Highlight backcolour and text colour (format)
                    format = STR_BLACK_STRING;
                    if (i == _window_guest_list_highlighted_index)
                    {
                        gfx_filter_rect(dpi, 0, y, 800, y + SUMMARISED_GUEST_ROW_HEIGHT, PALETTE_DARKEN_1);
                        format = STR_WINDOW_COLOUR_2_STRINGID;
                    }

                    // Draw guest faces
                    uint32_t numGuests = _window_guest_list_groups_num_guests[i];
                    for (uint32_t j = 0; j < 56 && j < numGuests; j++)
                        gfx_draw_sprite(
                            dpi, _window_guest_list_groups_guest_faces[i * 56 + j] + SPR_PEEP_SMALL_FACE_VERY_VERY_UNHAPPY,
                            { static_cast<int32_t>(j) * 8, y + 12 }, 0);

                    // Draw action
                    std::memcpy(
                        gCommonFormatArgs, _window_guest_list_groups_arguments[i].args,
                        std::min(sizeof(gCommonFormatArgs), sizeof(_window_guest_list_groups_arguments[i].args)));
                    gfx_draw_string_left_clipped(dpi, format, gCommonFormatArgs, COLOUR_BLACK, { 0, y }, 414);

                    // Draw guest count
                    auto ft = Formatter::Common();
                    ft.Add<rct_string_id>(STR_GUESTS_COUNT_COMMA_SEP);
                    ft.Add<uint32_t>(numGuests);
                    gfx_draw_string_right(dpi, format, gCommonFormatArgs, COLOUR_BLACK, { 326, y });
                }
                y += SUMMARISED_GUEST_ROW_HEIGHT;
            }
            break;
    }
}

static void window_guest_list_textinput(rct_window* w, rct_widgetindex widgetIndex, char* text)
{
    if (text != nullptr && text[0] != '\0')
    {
        safe_strcpy(_window_guest_list_filter_name, text, sizeof(_window_guest_list_filter_name));
        w->pressed_widgets |= (1 << WIDX_FILTER_BY_NAME);
        window_guest_list_refresh_list();
    }
}

/**
 * returns 0 for in filter and 1 for not in filter
 *  rct2: 0x0069B865
 */
static int32_t window_guest_list_is_peep_in_filter(Peep* peep)
{
    char temp = _window_guest_list_selected_view;
    _window_guest_list_selected_view = _window_guest_list_selected_filter;
    auto peepArgs = get_arguments_from_peep(peep);

    _window_guest_list_selected_view = temp;

    if (_window_guest_list_filter_arguments.GetFirstStringId() == STR_NONE && _window_guest_list_selected_filter == 1)
    {
        Formatter(peepArgs.args).Add<rct_string_id>(STR_NONE);
    }

    if (_window_guest_list_filter_arguments == peepArgs)
    {
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
static FilterArguments get_arguments_from_peep(const Peep* peep)
{
    FilterArguments result;
    Formatter ft(result.args);
    switch (_window_guest_list_selected_view)
    {
        case VIEW_ACTIONS:
            peep->FormatActionTo(ft);
            break;
        case VIEW_THOUGHTS:
        {
            auto thought = &peep->Thoughts[0];
            if (thought->freshness <= 5 && thought->type != PEEP_THOUGHT_TYPE_NONE)
            {
                std::memset(gCommonFormatArgs, 0, sizeof(gCommonFormatArgs));
                peep_thought_set_format_args(thought);
                std::memcpy(result.args, gCommonFormatArgs, std::min(sizeof(gCommonFormatArgs), sizeof(result.args)));
            }
            break;
        }
    }
    return result;
}

/**
 *
 *  rct2: 0x0069B5AE
 */
static void window_guest_list_find_groups()
{
    uint32_t tick256 = floor2(gScenarioTicks, 256);
    if (_window_guest_list_selected_view == _window_guest_list_last_find_groups_selected_view)
    {
        if (_window_guest_list_last_find_groups_wait != 0 || _window_guest_list_last_find_groups_tick == tick256)
        {
            return;
        }
    }

    _window_guest_list_last_find_groups_tick = tick256;
    _window_guest_list_last_find_groups_selected_view = _window_guest_list_selected_view;
    _window_guest_list_last_find_groups_wait = 320;
    _window_guest_list_num_groups = 0;

    // Set all guests to unassigned
    {
        for (auto peep : EntityList<Guest>(EntityListId::Peep))
        {
            if (!peep->OutsideOfPark)
            {
                peep->flags |= SPRITE_FLAGS_PEEP_VISIBLE;
            }
        }
    }
    // For each guest / group
    for (auto peep : EntityList<Guest>(EntityListId::Peep))
    {
        if (peep->OutsideOfPark || !(peep->flags & SPRITE_FLAGS_PEEP_VISIBLE))
            continue;

        // New group, cap at 240 though
        int32_t groupIndex = _window_guest_list_num_groups;
        if (groupIndex >= 240)
            break;

        _window_guest_list_num_groups++;
        _window_guest_list_groups_num_guests[groupIndex] = 1;
        peep->flags &= ~(SPRITE_FLAGS_PEEP_VISIBLE);

        _window_guest_list_groups_arguments[groupIndex] = get_arguments_from_peep(peep);
        _window_guest_list_filter_arguments = _window_guest_list_groups_arguments[groupIndex];

        _window_guest_list_group_index[groupIndex] = groupIndex;
        auto faceIndex = groupIndex * 56;
        _window_guest_list_groups_guest_faces[faceIndex++] = get_peep_face_sprite_small(peep)
            - SPR_PEEP_SMALL_FACE_VERY_VERY_UNHAPPY;

        // Find more peeps that belong to same group
        for (auto peep2 : EntityList<Guest>(EntityListId::Peep))
        {
            if (peep2->OutsideOfPark || !(peep2->flags & SPRITE_FLAGS_PEEP_VISIBLE))
                continue;

            // Get and check if in same group
            auto argument12 = get_arguments_from_peep(peep2);
            if (argument12 != _window_guest_list_groups_arguments[groupIndex])
                continue;

            // Assign guest
            _window_guest_list_groups_num_guests[groupIndex]++;
            peep2->flags &= ~(SPRITE_FLAGS_PEEP_VISIBLE);

            // Add face sprite, cap at 56 though
            if (_window_guest_list_groups_num_guests[groupIndex] >= 56)
                continue;
            _window_guest_list_groups_guest_faces[faceIndex++] = get_peep_face_sprite_small(peep2)
                - SPR_PEEP_SMALL_FACE_VERY_VERY_UNHAPPY;
        }

        if (_window_guest_list_filter_arguments.GetFirstStringId() == 0)
        {
            _window_guest_list_num_groups--;
            continue;
        }

        auto curr_num_guests = _window_guest_list_groups_num_guests[groupIndex];
        int32_t swap_position = 0;
        // This section places the groups in size order.
        bool gotoNextPeep = false;
        while (1)
        {
            if (swap_position >= groupIndex)
            {
                gotoNextPeep = true;
                break;
            }
            if (curr_num_guests > _window_guest_list_groups_num_guests[swap_position])
                break;
            swap_position++;
        }
        if (gotoNextPeep)
        {
            continue;
        }

        auto arguments12 = _window_guest_list_groups_arguments[groupIndex];
        auto bl = _window_guest_list_group_index[groupIndex];

        do
        {
            std::swap(curr_num_guests, _window_guest_list_groups_num_guests[swap_position]);
            std::swap(arguments12, _window_guest_list_groups_arguments[swap_position]);

            uint8_t temp_faces[56];
            std::memcpy(temp_faces, &(_window_guest_list_groups_guest_faces[groupIndex * 56]), 56);
            std::memcpy(
                &(_window_guest_list_groups_guest_faces[groupIndex * 56]),
                &(_window_guest_list_groups_guest_faces[swap_position * 56]), 56);
            std::memcpy(&(_window_guest_list_groups_guest_faces[swap_position * 56]), temp_faces, 56);

            std::swap(bl, _window_guest_list_group_index[swap_position]);
        } while (++swap_position <= groupIndex);
    }
}

static bool guest_should_be_visible(Peep* peep)
{
    if (_window_guest_list_tracking_only && !(peep->PeepFlags & PEEP_FLAGS_TRACKING))
        return false;

    if (_window_guest_list_filter_name[0] != '\0')
    {
        char name[256]{};
        uint8_t args[32]{};

        Formatter ft(args);
        peep->FormatNameTo(ft);
        format_string(name, sizeof(name), STR_STRINGID, args);
        if (strcasestr(name, _window_guest_list_filter_name) == nullptr)
        {
            return false;
        }
    }

    return true;
}
