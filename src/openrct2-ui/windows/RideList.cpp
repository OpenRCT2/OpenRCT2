/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "../interface/Theme.h"

#include <iterator>
#include <openrct2-ui/interface/Dropdown.h>
#include <openrct2-ui/interface/Widget.h>
#include <openrct2-ui/windows/Window.h>
#include <openrct2/Context.h>
#include <openrct2/Game.h>
#include <openrct2/config/Config.h>
#include <openrct2/drawing/Drawing.h>
#include <openrct2/interface/Colour.h>
#include <openrct2/localisation/Localisation.h>
#include <openrct2/network/network.h>
#include <openrct2/sprites.h>
#include <openrct2/util/Util.h>
#include <openrct2/windows/Intent.h>
#include <openrct2/world/Park.h>

static constexpr const rct_string_id WINDOW_TITLE = STR_NONE;
static constexpr const int32_t WH = 240;
static constexpr const int32_t WW = 340;

enum
{
    PAGE_RIDES,
    PAGE_SHOPS_AND_STALLS,
    PAGE_KIOSKS_AND_FACILITIES,
    PAGE_COUNT
};

enum WINDOW_RIDE_LIST_WIDGET_IDX
{
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

// clang-format off
static rct_widget window_ride_list_widgets[] = {
    WINDOW_SHIM(WINDOW_TITLE, WW, WH),
    MakeWidget({  0, 43}, {340, 197}, WindowWidgetType::Resize,   WindowColour::Secondary                                                                ), // tab page background
    MakeWidget({315, 60}, { 24,  24}, WindowWidgetType::FlatBtn,  WindowColour::Secondary, SPR_TOGGLE_OPEN_CLOSE,      STR_OPEN_OR_CLOSE_ALL_RIDES       ), // open / close all toggle
    MakeWidget({150, 46}, {124,  12}, WindowWidgetType::DropdownMenu, WindowColour::Secondary                                                                ), // current information type
    MakeWidget({262, 47}, { 11,  10}, WindowWidgetType::Button,   WindowColour::Secondary, STR_DROPDOWN_GLYPH,         STR_RIDE_LIST_INFORMATION_TYPE_TIP), // information type dropdown button
    MakeWidget({280, 46}, { 54,  12}, WindowWidgetType::Button,   WindowColour::Secondary, STR_SORT,                   STR_RIDE_LIST_SORT_TIP            ), // sort button
    MakeTab   ({  3, 17},                                                                                STR_LIST_RIDES_TIP                ), // tab 1
    MakeTab   ({ 34, 17},                                                                                STR_LIST_SHOPS_AND_STALLS_TIP     ), // tab 2
    MakeTab   ({ 65, 17},                                                                                STR_LIST_KIOSKS_AND_FACILITIES_TIP), // tab 3
    MakeWidget({  3, 60}, {334, 177}, WindowWidgetType::Scroll,   WindowColour::Secondary, SCROLL_VERTICAL                                               ), // list
    MakeWidget({320, 62}, { 14,  14}, WindowWidgetType::ImgBtn,   WindowColour::Secondary, SPR_G2_RCT1_CLOSE_BUTTON_0                                    ),
    MakeWidget({320, 76}, { 14,  14}, WindowWidgetType::ImgBtn,   WindowColour::Secondary, SPR_G2_RCT1_OPEN_BUTTON_0                                     ),
    MakeWidget({315, 90}, { 24,  24}, WindowWidgetType::FlatBtn,  WindowColour::Secondary, SPR_DEMOLISH,               STR_QUICK_DEMOLISH_RIDE           ),
    WIDGETS_END,
};
// clang-format on


/*
static void window_ride_list_mouseup(rct_window* w, rct_widgetindex widgetIndex);
static void window_ride_list_resize(rct_window* w);
static void window_ride_list_mousedown(rct_window* w, rct_widgetindex widgetIndex, rct_widget* widget);
static void window_ride_list_dropdown(rct_window* w, rct_widgetindex widgetIndex, int32_t dropdownIndex);
static void window_ride_list_update(rct_window* w);
static void window_ride_list_scrollgetsize(rct_window* w, int32_t scrollIndex, int32_t* width, int32_t* height);
static void window_ride_list_scrollmousedown(rct_window* w, int32_t scrollIndex, const ScreenCoordsXY& screenCoords);
static void window_ride_list_scrollmouseover(rct_window* w, int32_t scrollIndex, const ScreenCoordsXY& screenCoords);
static void window_ride_list_invalidate(rct_window* w);
static void window_ride_list_paint(rct_window* w, rct_drawpixelinfo* dpi);
static void window_ride_list_scrollpaint(rct_window* w, rct_drawpixelinfo* dpi, int32_t scrollIndex);

static rct_window_event_list window_ride_list_events([](auto& events) {
    events.mouse_up = &window_ride_list_mouseup;
    events.resize = &window_ride_list_resize;
    events.mouse_down = &window_ride_list_mousedown;
    events.dropdown = &window_ride_list_dropdown;
    events.update = &window_ride_list_update;
    events.get_scroll_size = &window_ride_list_scrollgetsize;
    events.scroll_mousedown = &window_ride_list_scrollmousedown;
    events.scroll_mouseover = &window_ride_list_scrollmouseover;
    events.invalidate = &window_ride_list_invalidate;
    events.paint = &window_ride_list_paint;
    events.scroll_paint = &window_ride_list_scrollpaint;
});
*/

enum
{
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
    DROPDOWN_LIST_COUNT,
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
    STR_GUESTS_FAVOURITE,
};

static constexpr const rct_string_id ride_list_statusbar_count_strings[PAGE_COUNT] = {
    STR_NUMBER_RIDES,
    STR_NUMBER_SHOPS_AND_STALLS,
    STR_NUMBER_RESTROOMS_AND_INFORMATION_KIOSKS,
};

static constexpr const bool ride_info_type_money_mapping[DROPDOWN_LIST_COUNT] = {
    false, // Status
    false, // Popularity
    false, // Satisfaction
    true,  // Profit
    false, // Total customers
    true,  // Total profit
    false, // Customers
    false, // Age
    true,  // Income
    true,  // Running_cost
    false, // Queue length
    false, // Queue time
    false, // Reliability
    false, // Down time
    false, // Guests favourite
};

static constexpr const rct_string_id page_names[] = {
    STR_RIDES,
    STR_SHOPS_AND_STALLS,
    STR_RESTROOMS_AND_INFORMATION_KIOSKS,
};

/*
static void window_ride_list_draw_tab_images(rct_drawpixelinfo* dpi, rct_window* w);
static void window_ride_list_close_all(rct_window* w);
static void window_ride_list_open_all(rct_window* w);
*/

class RideListWindow final : public Window
{
private:
    bool _quickDemolishMode = false;
    int32_t _windowRideListInformationType = INFORMATION_TYPE_STATUS;
    std::vector<ride_id_t> _rideList;

public:
    // window_ride_list_open
    void OnOpen() override
    {
        this->widgets = window_ride_list_widgets;
        this->enabled_widgets = (1ULL << WIDX_CLOSE) | (1ULL << WIDX_OPEN_CLOSE_ALL) | (1ULL << WIDX_CURRENT_INFORMATION_TYPE)
            | (1ULL << WIDX_INFORMATION_TYPE_DROPDOWN) | (1ULL << WIDX_SORT) | (1ULL << WIDX_TAB_1) | (1ULL << WIDX_TAB_2)
            | (1ULL << WIDX_TAB_3) | (1ULL << WIDX_CLOSE_LIGHT) | (1ULL << WIDX_OPEN_LIGHT);
        if (network_get_mode() != NETWORK_MODE_CLIENT)
        {
            this->enabled_widgets |= (1ULL << WIDX_QUICK_DEMOLISH);
        }
        WindowInitScrollWidgets(this);
        this->page = PAGE_RIDES;
        this->selected_list_item = -1;
        this->frame_no = 0;
        this->min_width = 340;
        this->min_height = 240;
        this->max_width = 400;
        this->max_height = 700;
        RefreshList();

        this->list_information_type = 0;
        _windowRideListInformationType = INFORMATION_TYPE_STATUS;
        _quickDemolishMode = false;
    }

    /**
     *
     * window_ride_list_resize
     *  rct2: 0x006B38A7
     */
    void OnResize() override
    {
        this->min_width = 340;
        this->min_height = 124;
        if (this->width < this->min_width)
        {
            this->Invalidate();
            this->width = this->min_width;
        }
        if (this->height < this->min_height)
        {
            this->Invalidate();
            this->height = this->min_height;
        }

        // Refreshing the list can be a very intensive operation
        // owing to its use of ride_has_any_track_elements().
        // This makes sure it's only refreshed every 64 ticks.
        if (!(gCurrentRealTimeTicks & 0x3f))
        {
            RefreshList();
        }
    }

    /**
     *
     *  rct2: 0x006B3511
     */
    // window_ride_list_mouseup
    void OnMouseUp(rct_widgetindex widgetIndex) override
    {
        switch (widgetIndex)
        {
            case WIDX_CLOSE:
                window_close(this);
                break;
            case WIDX_SORT:
                this->list_information_type = _windowRideListInformationType;
                this->selected_list_item = -1;
                RefreshList();
                break;
            case WIDX_TAB_1:
            case WIDX_TAB_2:
            case WIDX_TAB_3:
                if (this->page != widgetIndex - WIDX_TAB_1)
                {
                    this->page = widgetIndex - WIDX_TAB_1;
                    this->frame_no = 0;
                    this->selected_list_item = -1;
                    if (this->page != PAGE_RIDES && _windowRideListInformationType > INFORMATION_TYPE_RUNNING_COST)
                    {
                        _windowRideListInformationType = INFORMATION_TYPE_STATUS;
                    }
                    RefreshList();
                }
                break;
            case WIDX_CLOSE_LIGHT:
                CloseAllRides();
                break;
            case WIDX_OPEN_LIGHT:
                OpenAllRides();
                break;
            case WIDX_QUICK_DEMOLISH:
                if (network_get_mode() != NETWORK_MODE_CLIENT)
                {
                    _quickDemolishMode = !_quickDemolishMode;
                }
                else
                {
                    _quickDemolishMode = false;
                }
                this->Invalidate();
                break;
        }
    }
    
    /**
     *
     *  rct2: 0x006B3532
     */
    // window_ride_list_mousedown
    void OnMouseDown(rct_widgetindex widgetIndex) override
    {
        // static void window_ride_list_mousedown(rct_window* w, rct_widgetindex widgetIndex, rct_widget* widget)
        if (widgetIndex == WIDX_OPEN_CLOSE_ALL)
        {
            auto* widget = &this->widgets[widgetIndex];
            gDropdownItemsFormat[0] = STR_CLOSE_ALL;
            gDropdownItemsFormat[1] = STR_OPEN_ALL;
            WindowDropdownShowText(
                { this->windowPos.x + widget->left, this->windowPos.y + widget->top }, widget->height(), this->colours[1], 0, 2);
        }
        else if (widgetIndex == WIDX_INFORMATION_TYPE_DROPDOWN)
        {
            auto* widget = &this->widgets[widgetIndex];
            widget--; // ~hjort96 why??

            int32_t lastType;
            if (this->page == PAGE_RIDES)
                lastType = INFORMATION_TYPE_GUESTS_FAVOURITE;
            else
                lastType = INFORMATION_TYPE_RUNNING_COST;

            int32_t numItems = 0;
            int32_t selectedIndex = -1;
            for (int32_t type = INFORMATION_TYPE_STATUS; type <= lastType; type++)
            {
                if ((gParkFlags & PARK_FLAGS_NO_MONEY))
                {
                    if (ride_info_type_money_mapping[type])
                    {
                        continue;
                    }
                }

                if (type == _windowRideListInformationType)
                {
                    selectedIndex = numItems;
                }

                gDropdownItemsFormat[numItems] = STR_DROPDOWN_MENU_LABEL;
                gDropdownItemsArgs[numItems] = ride_info_type_string_mapping[type];
                numItems++;
            }

            WindowDropdownShowTextCustomWidth(
                { this->windowPos.x + widget->left, this->windowPos.y + widget->top }, widget->height(), this->colours[1], 0,
                Dropdown::Flag::StayOpen, numItems, widget->width() - 3);
            if (selectedIndex != -1)
            {
                Dropdown::SetChecked(selectedIndex, true);
            }
        }
    }

    /**
     *
     *  rct2: 0x006B3547
     */
    // window_ride_list_dropdown
    void OnDropdown(rct_widgetindex widgetIndex, int32_t dropdownIndex) override
    {
        if (widgetIndex == WIDX_OPEN_CLOSE_ALL)
        {
            if (dropdownIndex == 0)
            {
                CloseAllRides();
            }
            else if (dropdownIndex == 1)
            {
                OpenAllRides();
            }

            this->Invalidate();
        }
        else if (widgetIndex == WIDX_INFORMATION_TYPE_DROPDOWN)
        {
            if (dropdownIndex == -1)
                return;

            int32_t informationType = INFORMATION_TYPE_STATUS;
            uint32_t arg = static_cast<uint32_t>(gDropdownItemsArgs[dropdownIndex]);
            for (size_t i = 0; i < std::size(ride_info_type_string_mapping); i++)
            {
                if (arg == ride_info_type_string_mapping[i])
                {
                    informationType = static_cast<int32_t>(i);
                }
            }

            _windowRideListInformationType = informationType;
            this->Invalidate();
        }
    }

    /**
    *
    *  rct2: 0x006B386B
    */
    // window_ride_list_update
    void OnUpdate() override
    {
        this->frame_no = (this->frame_no + 1) % 64;
        widget_invalidate(this, WIDX_TAB_1 + this->page);
        if (_windowRideListInformationType != INFORMATION_TYPE_STATUS)
            this->Invalidate();
    }

    /**
     *
     *  rct2: 0x006B35A1
     */
    // window_ride_list_scrollgetsize
    ScreenSize OnScrollGetSize(int32_t scrollIndex) override
    {
        // static void window_ride_list_scrollgetsize(rct_window* w, int32_t scrollIndex, int32_t* width, int32_t* height)
        auto h = static_cast<int32_t>(_rideList.size() * SCROLLABLE_ROW_HEIGHT);
        if (this->selected_list_item != -1)
        {
            this->selected_list_item = -1;
            this->Invalidate();
        }

        auto top = h - window_ride_list_widgets[WIDX_LIST].bottom + window_ride_list_widgets[WIDX_LIST].top + 21;
        if (top < 0)
            top = 0;
        if (top < this->scrolls[0].v_top)
        {
            this->scrolls[0].v_top = top;
            this->Invalidate();
        }

        return { 0, h };
    }

    /**
     *
     *  rct2: 0x006B361F
     */
    // window_ride_list_scrollmousedown
    void OnScrollMouseDown(int32_t scrollIndex, const ScreenCoordsXY& screenCoords) override
    {
        auto index = screenCoords.y / SCROLLABLE_ROW_HEIGHT;
        if (index < 0 || static_cast<size_t>(index) >= _rideList.size())
            return;

        // Open ride window
        auto rideIndex = _rideList[index];
        auto r = get_ride(rideIndex);
        if (_quickDemolishMode && network_get_mode() != NETWORK_MODE_CLIENT)
        {
            ride_action_modify(r, RIDE_MODIFY_DEMOLISH, GAME_COMMAND_FLAG_APPLY);
            RefreshList();
        }
        else
        {
            auto intent = Intent(WC_RIDE);
            intent.putExtra(INTENT_EXTRA_RIDE_ID, EnumValue(rideIndex));
            context_open_intent(&intent);
        }
    }

    /**
     *
     *  rct2: 0x006B35EF
     */
    // window_ride_list_scrollmouseover
    void OnScrollMouseOver(int32_t scrollIndex, const ScreenCoordsXY& screenCoords) override
    {
        auto index = screenCoords.y / SCROLLABLE_ROW_HEIGHT;
        if (index < 0 || static_cast<size_t>(index) >= _rideList.size())
            return;

        this->selected_list_item = index;
        this->Invalidate();
    }

    /**
     *
     *  rct2: 0x006B3182
     */
    // window_ride_list_invalidate
    void OnPrepareDraw() override
    {
        // ~hjort96 Maybe change to this->widgets
        window_ride_list_widgets[WIDX_CURRENT_INFORMATION_TYPE].text = ride_info_type_string_mapping
            [_windowRideListInformationType];

        // Set correct active tab
        for (int32_t i = 0; i < 3; i++)
            this->pressed_widgets &= ~(1 << (WIDX_TAB_1 + i));
        this->pressed_widgets |= 1LL << (WIDX_TAB_1 + this->page);

        // ~hjort96 Maybe change to this->widgets
        window_ride_list_widgets[WIDX_TITLE].text = page_names[this->page];

        if (_quickDemolishMode)
            this->pressed_widgets |= (1ULL << WIDX_QUICK_DEMOLISH);
        else
            this->pressed_widgets &= ~(1ULL << WIDX_QUICK_DEMOLISH);

        this->widgets[WIDX_BACKGROUND].right = this->width - 1;
        this->widgets[WIDX_BACKGROUND].bottom = this->height - 1;
        this->widgets[WIDX_PAGE_BACKGROUND].right = this->width - 1;
        this->widgets[WIDX_PAGE_BACKGROUND].bottom = this->height - 1;
        this->widgets[WIDX_TITLE].right = this->width - 2;

        // if close button is on the right then it must move
        this->widgets[WIDX_CLOSE].left = this->width - 13;
        this->widgets[WIDX_CLOSE].right = this->width - 3;

        this->widgets[WIDX_LIST].right = this->width - 26;
        this->widgets[WIDX_LIST].bottom = this->height - 15;
        this->widgets[WIDX_OPEN_CLOSE_ALL].right = this->width - 2;
        this->widgets[WIDX_OPEN_CLOSE_ALL].left = this->width - 25;
        this->widgets[WIDX_CLOSE_LIGHT].right = this->width - 7;
        this->widgets[WIDX_CLOSE_LIGHT].left = this->width - 20;
        this->widgets[WIDX_OPEN_LIGHT].right = this->width - 7;
        this->widgets[WIDX_OPEN_LIGHT].left = this->width - 20;
        this->widgets[WIDX_QUICK_DEMOLISH].right = this->width - 2;
        this->widgets[WIDX_QUICK_DEMOLISH].left = this->width - 25;

        if (ThemeGetFlags() & UITHEME_FLAG_USE_LIGHTS_RIDE)
        {
            this->widgets[WIDX_OPEN_CLOSE_ALL].type = WindowWidgetType::Empty;
            this->widgets[WIDX_CLOSE_LIGHT].type = WindowWidgetType::ImgBtn;
            this->widgets[WIDX_OPEN_LIGHT].type = WindowWidgetType::ImgBtn;

            const auto& rideManager = GetRideManager();
            auto allClosed = true;
            auto allOpen = false;
            if (_rideList.size() > 0 && std::size(rideManager) != 0)
            {
                auto c = static_cast<RideClassification>(this->page);
                allClosed = std::none_of(rideManager.begin(), rideManager.end(), [c](const Ride& r) {
                    return r.GetClassification() == c && r.status == RideStatus::Open;
                });
                allOpen = std::none_of(rideManager.begin(), rideManager.end(), [c](const Ride& r) {
                    return r.GetClassification() == c && r.status != RideStatus::Open;
                });
            }

            this->widgets[WIDX_CLOSE_LIGHT].image = SPR_G2_RCT1_CLOSE_BUTTON_0 + (allClosed ? 1 : 0) * 2
                + WidgetIsPressed(this, WIDX_CLOSE_LIGHT);
            this->widgets[WIDX_OPEN_LIGHT].image = SPR_G2_RCT1_OPEN_BUTTON_0 + (allOpen ? 1 : 0) * 2
                + WidgetIsPressed(this, WIDX_OPEN_LIGHT);
            this->widgets[WIDX_QUICK_DEMOLISH].top = this->widgets[WIDX_OPEN_LIGHT].bottom + 3;
        }
        else
        {
            this->widgets[WIDX_OPEN_CLOSE_ALL].type = WindowWidgetType::FlatBtn;
            this->widgets[WIDX_CLOSE_LIGHT].type = WindowWidgetType::Empty;
            this->widgets[WIDX_OPEN_LIGHT].type = WindowWidgetType::Empty;
            this->widgets[WIDX_QUICK_DEMOLISH].top = this->widgets[WIDX_OPEN_CLOSE_ALL].bottom + 3;
        }
        this->widgets[WIDX_QUICK_DEMOLISH].bottom = this->widgets[WIDX_QUICK_DEMOLISH].top + 23;
        this->widgets[WIDX_QUICK_DEMOLISH].type = network_get_mode() != NETWORK_MODE_CLIENT ? WindowWidgetType::FlatBtn
                                                                                         : WindowWidgetType::Empty;
    }

    /**
     *
     *  rct2: 0x006B3235
     */
    // window_ride_list_paint
    void OnDraw(rct_drawpixelinfo& dpi) override
    {
        WindowDrawWidgets(this, &dpi);
        DrawTabImages(&dpi);

        // Draw number of attractions on bottom
        auto ft = Formatter();
        ft.Add<uint16_t>(static_cast<uint16_t>(_rideList.size()));
        DrawTextBasic(
            &dpi, this->windowPos + ScreenCoordsXY{ 4, this->widgets[WIDX_LIST].bottom + 2 },
            ride_list_statusbar_count_strings[this->page], ft);
    }

    /**
     *
     *  rct2: 0x006B3240
     */
    // window_ride_list_scrollpaint
    void OnScrollDraw(int32_t scrollIndex, rct_drawpixelinfo& dpi) override
    {
        auto dpiCoords = ScreenCoordsXY{ dpi.x, dpi.y };
        gfx_fill_rect(
            &dpi, { dpiCoords, dpiCoords + ScreenCoordsXY{ dpi.width, dpi.height } }, ColourMapA[this->colours[1]].mid_light);

        auto y = 0;
        for (size_t i = 0; i < _rideList.size(); i++)
        {
            rct_string_id format = (_quickDemolishMode ? STR_RED_STRINGID : STR_BLACK_STRING);
            if (i == static_cast<size_t>(this->selected_list_item))
            {
                // Background highlight
                gfx_filter_rect(&dpi, { 0, y, 800, y + SCROLLABLE_ROW_HEIGHT - 1 }, FilterPaletteID::PaletteDarken1);
                format = (_quickDemolishMode ? STR_LIGHTPINK_STRINGID : STR_WINDOW_COLOUR_2_STRINGID);
            }

            // Get ride
            auto r = get_ride(_rideList[i]);
            if (r == nullptr)
                continue;

            // Ride name
            auto ft = Formatter();
            r->FormatNameTo(ft);
            DrawTextEllipsised(&dpi, { 0, y - 1 }, 159, format, ft);

            // Ride information
            ft = Formatter();
            ft.Increment(2);
            auto formatSecondaryEnabled = true;
            rct_string_id formatSecondary = 0;
            switch (_windowRideListInformationType)
            {
                case INFORMATION_TYPE_STATUS:
                    formatSecondaryEnabled = false;
                    ft.Rewind();
                    r->FormatStatusTo(ft);

                    // Make test red and bold if broken down or crashed
                    if ((r->lifecycle_flags & RIDE_LIFECYCLE_BROKEN_DOWN) || (r->lifecycle_flags & RIDE_LIFECYCLE_CRASHED))
                    {
                        format = STR_RED_OUTLINED_STRING;
                    }
                    break;
                case INFORMATION_TYPE_POPULARITY:
                    formatSecondary = STR_POPULARITY_UNKNOWN_LABEL;
                    if (r->popularity != 255)
                    {
                        formatSecondary = STR_POPULARITY_LABEL;
                        ft.Add<uint16_t>(r->popularity * 4);
                    }
                    break;
                case INFORMATION_TYPE_SATISFACTION:
                    formatSecondary = STR_SATISFACTION_UNKNOWN_LABEL;
                    if (r->satisfaction != 255)
                    {
                        formatSecondary = STR_SATISFACTION_LABEL;
                        ft.Add<uint16_t>(r->satisfaction * 5);
                    }
                    break;
                case INFORMATION_TYPE_PROFIT:
                    formatSecondary = 0;
                    if (r->profit != MONEY64_UNDEFINED)
                    {
                        formatSecondary = STR_PROFIT_LABEL;
                        ft.Add<money64>(r->profit);
                    }
                    break;
                case INFORMATION_TYPE_TOTAL_CUSTOMERS:
                    formatSecondary = STR_RIDE_LIST_TOTAL_CUSTOMERS_LABEL;
                    ft.Add<uint32_t>(r->total_customers);
                    break;
                case INFORMATION_TYPE_TOTAL_PROFIT:
                    formatSecondary = 0;
                    if (r->total_profit != MONEY64_UNDEFINED)
                    {
                        formatSecondary = STR_RIDE_LIST_TOTAL_PROFIT_LABEL;
                        ft.Add<money64>(r->total_profit);
                    }
                    break;
                case INFORMATION_TYPE_CUSTOMERS:
                    formatSecondary = STR_RIDE_LIST_CUSTOMERS_PER_HOUR_LABEL;
                    ft.Add<uint32_t>(ride_customers_per_hour(r));
                    break;
                case INFORMATION_TYPE_AGE:
                {
                    int16_t age = date_get_year(r->GetAge());
                    switch (age)
                    {
                        case 0:
                            formatSecondary = STR_RIDE_LIST_BUILT_THIS_YEAR_LABEL;
                            break;
                        case 1:
                            formatSecondary = STR_RIDE_LIST_BUILT_LAST_YEAR_LABEL;
                            break;
                        default:
                            formatSecondary = STR_RIDE_LIST_BUILT_X_YEARS_AGO_LABEL;
                            break;
                    }
                    ft.Add<int16_t>(age);
                    break;
                }
                case INFORMATION_TYPE_INCOME:
                    formatSecondary = 0;
                    if (r->income_per_hour != MONEY64_UNDEFINED)
                    {
                        formatSecondary = STR_RIDE_LIST_INCOME_LABEL;
                        ft.Add<money64>(r->income_per_hour);
                    }
                    break;
                case INFORMATION_TYPE_RUNNING_COST:
                    formatSecondary = STR_RIDE_LIST_RUNNING_COST_UNKNOWN;
                    if (r->upkeep_cost != MONEY16_UNDEFINED)
                    {
                        formatSecondary = STR_RIDE_LIST_RUNNING_COST_LABEL;
                        ft.Add<money64>(r->upkeep_cost * 16);
                    }
                    break;
                case INFORMATION_TYPE_QUEUE_LENGTH:
                {
                    auto queueLength = r->GetTotalQueueLength();
                    ft.Add<uint16_t>(queueLength);

                    if (queueLength == 1)
                    {
                        formatSecondary = STR_QUEUE_ONE_PERSON;
                    }
                    else if (queueLength > 1)
                    {
                        formatSecondary = STR_QUEUE_PEOPLE;
                    }
                    else
                    {
                        formatSecondary = STR_QUEUE_EMPTY;
                    }
                    break;
                }
                case INFORMATION_TYPE_QUEUE_TIME:
                {
                    auto maxQueueTime = r->GetMaxQueueTime();
                    ft.Add<uint16_t>(maxQueueTime);

                    if (maxQueueTime > 1)
                    {
                        formatSecondary = STR_QUEUE_TIME_PLURAL_LABEL;
                    }
                    else
                    {
                        formatSecondary = STR_QUEUE_TIME_LABEL;
                    }
                    break;
                }
                case INFORMATION_TYPE_RELIABILITY:
                    ft.Add<uint16_t>(r->reliability_percentage);
                    formatSecondary = STR_RELIABILITY_LABEL;
                    break;
                case INFORMATION_TYPE_DOWN_TIME:
                    ft.Add<uint16_t>(r->downtime);
                    formatSecondary = STR_DOWN_TIME_LABEL;
                    break;
                case INFORMATION_TYPE_GUESTS_FAVOURITE:
                    formatSecondary = 0;
                    if (r->IsRide())
                    {
                        ft.Add<uint16_t>(r->guests_favourite);
                        formatSecondary = r->guests_favourite == 1 ? STR_GUESTS_FAVOURITE_LABEL
                                                                      : STR_GUESTS_FAVOURITE_PLURAL_LABEL;
                    }
                    break;
            }

            if (formatSecondaryEnabled)
            {
                ft.Rewind();
                ft.Add<rct_string_id>(formatSecondary);
            }
            DrawTextEllipsised(&dpi, { 160, y - 1 }, 157, format, ft);
            y += SCROLLABLE_ROW_HEIGHT;
        }
    }

private:

    /**
     *
     *  rct2: 0x006B38EA
     */
    // window_ride_list_draw_tab_images
    void DrawTabImages(rct_drawpixelinfo* dpi)
    {
        int32_t sprite_idx;

        // Rides tab
        sprite_idx = SPR_TAB_RIDE_0;
        if (this->page == PAGE_RIDES)
            sprite_idx += this->frame_no / 4;
        gfx_draw_sprite(
            dpi, ImageId(sprite_idx), this->windowPos + ScreenCoordsXY{ this->widgets[WIDX_TAB_1].left, this->widgets[WIDX_TAB_1].top });

        // Shops and stalls tab
        sprite_idx = SPR_TAB_SHOPS_AND_STALLS_0;
        if (this->page == PAGE_SHOPS_AND_STALLS)
            sprite_idx += this->frame_no / 4;
        gfx_draw_sprite(
            dpi, ImageId(sprite_idx), this->windowPos + ScreenCoordsXY{ this->widgets[WIDX_TAB_2].left, this->widgets[WIDX_TAB_2].top });

        // Information kiosks and facilities tab
        sprite_idx = SPR_TAB_KIOSKS_AND_FACILITIES_0;
        if (this->page == PAGE_KIOSKS_AND_FACILITIES)
            sprite_idx += (this->frame_no / 4) % 8;
        gfx_draw_sprite(
            dpi, ImageId(sprite_idx), this->windowPos + ScreenCoordsXY{ this->widgets[WIDX_TAB_3].left, this->widgets[WIDX_TAB_3].top });
    }

    /**
     * Bubbles an item one position up in the window list.  This is done by swapping
     * the two locations.
     *  rct2: New function not from rct2
     */
    // window_bubble_list_item
    void BubbleListItem(int32_t index)
    {
        std::swap(_rideList[index], _rideList[index + 1]);
    }

    /**
     *
     *  rct2: 0x006B39A8
     */
    // window_ride_list_refresh_list
public:
    void RefreshList()
    {
        _rideList.clear();

        size_t listIndex = 0;
        for (auto& ridec : GetRideManager())
        {
            auto r = &ridec;
            if (r ->GetClassification() != static_cast<RideClassification>(this->page)
                || (r ->status == RideStatus::Closed && !ride_has_any_track_elements(r )))
                continue;

            if (r ->window_invalidate_flags & RIDE_INVALIDATE_RIDE_LIST)
            {
                r ->window_invalidate_flags &= ~RIDE_INVALIDATE_RIDE_LIST;
            }

            _rideList.push_back(r ->id);
            auto currentListPosition = static_cast<int32_t>(listIndex);
            switch (this->list_information_type)
            {
                case INFORMATION_TYPE_STATUS:
                {
                    auto strA =r ->GetName();
                    while (--currentListPosition >= 0)
                    {
                        auto otherRide = get_ride(_rideList[currentListPosition]);
                        if (otherRide != nullptr)
                        {
                            auto strB = otherRide->GetName();
                            if (strlogicalcmp(strA.c_str(), strB.c_str()) >= 0)
                                break;

                            BubbleListItem(currentListPosition);
                        }
                    }
                    break;
                }
                case INFORMATION_TYPE_POPULARITY:
                    while (--currentListPosition >= 0)
                    {
                        auto otherRide = get_ride(_rideList[currentListPosition]);
                        if (otherRide != nullptr)
                        {
                            if (r ->popularity * 4 <= otherRide->popularity * 4)
                                break;

                            BubbleListItem(currentListPosition);
                        }
                    }
                    break;
                case INFORMATION_TYPE_SATISFACTION:
                    while (--currentListPosition >= 0)
                    {
                        auto otherRide = get_ride(_rideList[currentListPosition]);
                        if (otherRide != nullptr)
                        {
                            if (r ->satisfaction * 5 <= otherRide->satisfaction * 5)
                                break;

                            BubbleListItem(currentListPosition);
                        }
                    }
                    break;
                case INFORMATION_TYPE_PROFIT:
                    while (--currentListPosition >= 0)
                    {
                        auto otherRide = get_ride(_rideList[currentListPosition]);
                        if (otherRide != nullptr)
                        {
                            if (r ->profit <= otherRide->profit)
                                break;

                            BubbleListItem(currentListPosition);
                        }
                    }
                    break;
                case INFORMATION_TYPE_TOTAL_CUSTOMERS:
                    while (--currentListPosition >= 0)
                    {
                        auto otherRide = get_ride(_rideList[currentListPosition]);
                        if (otherRide != nullptr)
                        {
                            if (r ->total_customers <= otherRide->total_customers)
                                break;

                            BubbleListItem(currentListPosition);
                        }
                    }
                    break;
                case INFORMATION_TYPE_TOTAL_PROFIT:
                    while (--currentListPosition >= 0)
                    {
                        auto otherRide = get_ride(_rideList[currentListPosition]);
                        if (otherRide != nullptr)
                        {
                            if (r ->total_profit <= otherRide->total_profit)
                                break;

                            BubbleListItem(currentListPosition);
                        }
                    }
                    break;
                case INFORMATION_TYPE_CUSTOMERS:
                    while (--currentListPosition >= 0)
                    {
                        auto otherRide = get_ride(_rideList[currentListPosition]);
                        if (otherRide != nullptr)
                        {
                            if (ride_customers_per_hour(r ) <= ride_customers_per_hour(otherRide))
                                break;

                            BubbleListItem(currentListPosition);
                        }
                    }
                    break;
                case INFORMATION_TYPE_AGE:
                    while (--currentListPosition >= 0)
                    {
                        auto otherRide = get_ride(_rideList[currentListPosition]);
                        if (otherRide != nullptr)
                        {
                            if (r ->build_date <= otherRide->build_date)
                                break;

                            BubbleListItem(currentListPosition);
                        }
                    }
                    break;
                case INFORMATION_TYPE_INCOME:
                    while (--currentListPosition >= 0)
                    {
                        auto otherRide = get_ride(_rideList[currentListPosition]);
                        if (otherRide != nullptr)
                        {
                            if (r ->income_per_hour <= otherRide->income_per_hour)
                                break;

                            BubbleListItem(currentListPosition);
                        }
                    }
                    break;
                case INFORMATION_TYPE_RUNNING_COST:
                    while (--currentListPosition >= 0)
                    {
                        auto otherRide = get_ride(_rideList[currentListPosition]);
                        if (otherRide != nullptr)
                        {
                            if (r ->upkeep_cost <= otherRide->upkeep_cost)
                                break;

                            BubbleListItem(currentListPosition);
                        }
                    }
                    break;
                case INFORMATION_TYPE_QUEUE_LENGTH:
                    while (--currentListPosition >= 0)
                    {
                        auto otherRide = get_ride(_rideList[currentListPosition]);
                        if (otherRide != nullptr)
                        {
                            if (r ->GetTotalQueueLength() <= otherRide->GetTotalQueueLength())
                                break;

                            BubbleListItem(currentListPosition);
                        }
                    }
                    break;
                case INFORMATION_TYPE_QUEUE_TIME:
                    while (--currentListPosition >= 0)
                    {
                        auto otherRide = get_ride(_rideList[currentListPosition]);
                        if (otherRide != nullptr)
                        {
                            if (r ->GetMaxQueueTime() <= otherRide->GetMaxQueueTime())
                                break;

                            BubbleListItem(currentListPosition);
                        }
                    }
                    break;
                case INFORMATION_TYPE_RELIABILITY:
                    while (--currentListPosition >= 0)
                    {
                        auto otherRide = get_ride(_rideList[currentListPosition]);
                        if (otherRide != nullptr)
                        {
                            if (r ->reliability_percentage <= otherRide->reliability_percentage)
                                break;

                            BubbleListItem(currentListPosition);
                        }
                    }
                    break;
                case INFORMATION_TYPE_DOWN_TIME:
                    while (--currentListPosition >= 0)
                    {
                        auto otherRide = get_ride(_rideList[currentListPosition]);
                        if (otherRide != nullptr)
                        {
                            if (r ->downtime <= otherRide->downtime)
                                break;

                            BubbleListItem(currentListPosition);
                        }
                    }
                    break;
                case INFORMATION_TYPE_GUESTS_FAVOURITE:
                    while (--currentListPosition >= 0)
                    {
                        auto otherRide = get_ride(_rideList[currentListPosition]);
                        if (otherRide != nullptr)
                        {
                            if (r ->guests_favourite <= otherRide->guests_favourite)
                                break;

                            BubbleListItem(currentListPosition);
                        }
                    }
                    break;
            }

            listIndex++;
        }

        this->selected_list_item = -1;
        this->Invalidate();
    }

private:
    // window_ride_list_close_all
    void CloseAllRides()
    {
        for (auto& r : GetRideManager())
        {
            if (r .status != RideStatus::Closed &&r .GetClassification() == static_cast<RideClassification>(this->page))
            {
                ride_set_status(&r , RideStatus::Closed);
            }
        }
    }

    // window_ride_list_open_all
    void OpenAllRides()
    {
        for (auto& r : GetRideManager())
        {
            if (r .status != RideStatus::Open &&r .GetClassification() == static_cast<RideClassification>(this->page))
            {
                ride_set_status(&r , RideStatus::Open);
            }
        }
    }
};

/**
 *
 *  rct2: 0x006B30BC
 */
rct_window* window_ride_list_open()
{
    // Check if window is already open
    auto* window = window_bring_to_front_by_class(WC_RIDE_LIST);
    if (window == nullptr)
    {
        window = WindowCreate<RideListWindow>(WC_RIDE_LIST, ScreenCoordsXY(32, 32), WW, WH);
    }
    return window;

}


void window_ride_list_refresh_list(rct_window* w)
{
    dynamic_cast<RideListWindow*>(w)->RefreshList();
}
