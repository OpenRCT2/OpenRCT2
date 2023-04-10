/*****************************************************************************
 * Copyright (c) 2014-2023 OpenRCT2 developers
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
#include <openrct2/actions/RideDemolishAction.h>
#include <openrct2/actions/RideSetStatusAction.h>
#include <openrct2/config/Config.h>
#include <openrct2/drawing/Drawing.h>
#include <openrct2/interface/Colour.h>
#include <openrct2/localisation/Formatter.h>
#include <openrct2/localisation/Localisation.h>
#include <openrct2/network/network.h>
#include <openrct2/sprites.h>
#include <openrct2/util/Util.h>
#include <openrct2/windows/Intent.h>
#include <openrct2/world/Park.h>

static constexpr const StringId WINDOW_TITLE = STR_NONE;
static constexpr const int32_t WH = 240;
static constexpr const int32_t WW = 340;

enum
{
    PAGE_RIDES,
    PAGE_SHOPS_AND_STALLS,
    PAGE_KIOSKS_AND_FACILITIES,
    PAGE_COUNT
};

enum WindowRideListWidgetIdx
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
static Widget window_ride_list_widgets[] = {
    WINDOW_SHIM(WINDOW_TITLE, WW, WH),
    MakeWidget({  0, 43}, {340, 197}, WindowWidgetType::Resize,   WindowColour::Secondary                                                                ), // tab page background
    MakeWidget({315, 60}, { 24,  24}, WindowWidgetType::FlatBtn,  WindowColour::Secondary, ImageId(SPR_TOGGLE_OPEN_CLOSE),      STR_OPEN_OR_CLOSE_ALL_RIDES       ), // open / close all toggle
    MakeWidget({150, 46}, {124,  12}, WindowWidgetType::DropdownMenu, WindowColour::Secondary                                                                ), // current information type
    MakeWidget({262, 47}, { 11,  10}, WindowWidgetType::Button,   WindowColour::Secondary, STR_DROPDOWN_GLYPH,         STR_RIDE_LIST_INFORMATION_TYPE_TIP), // information type dropdown button
    MakeWidget({280, 46}, { 54,  12}, WindowWidgetType::Button,   WindowColour::Secondary, STR_SORT,                   STR_RIDE_LIST_SORT_TIP            ), // sort button
    MakeTab   ({  3, 17},                                                                                STR_LIST_RIDES_TIP                ), // tab 1
    MakeTab   ({ 34, 17},                                                                                STR_LIST_SHOPS_AND_STALLS_TIP     ), // tab 2
    MakeTab   ({ 65, 17},                                                                                STR_LIST_KIOSKS_AND_FACILITIES_TIP), // tab 3
    MakeWidget({  3, 60}, {334, 177}, WindowWidgetType::Scroll,   WindowColour::Secondary, SCROLL_VERTICAL                                               ), // list
    MakeWidget({320, 62}, { 14,  14}, WindowWidgetType::ImgBtn,   WindowColour::Secondary, ImageId(SPR_G2_RCT1_CLOSE_BUTTON_0)                                    ),
    MakeWidget({320, 76}, { 14,  14}, WindowWidgetType::ImgBtn,   WindowColour::Secondary, ImageId(SPR_G2_RCT1_OPEN_BUTTON_0)                                     ),
    MakeWidget({315, 90}, { 24,  24}, WindowWidgetType::FlatBtn,  WindowColour::Secondary, ImageId(SPR_DEMOLISH),               STR_QUICK_DEMOLISH_RIDE           ),
    WIDGETS_END,
};
// clang-format on

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
    INFORMATION_TYPE_EXCITEMENT,
    INFORMATION_TYPE_INTENSITY,
    INFORMATION_TYPE_NAUSEA,
    DROPDOWN_LIST_COUNT,
};

static constexpr const StringId ride_info_type_string_mapping[DROPDOWN_LIST_COUNT] = {
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
    STR_RIDE_LIST_EXCITEMENT,
    STR_RIDE_LIST_INTENSITY,
    STR_RIDE_LIST_NAUSEA,
};

static constexpr const StringId ride_list_statusbar_count_strings[PAGE_COUNT] = {
    STR_NUMBER_RIDES,
    STR_NUMBER_SHOPS_AND_STALLS,
    STR_NUMBER_TOILETS_AND_INFORMATION_KIOSKS,
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
    false, // Excitement
    false, // Intensity
    false, // Nausea
};

static constexpr const StringId page_names[] = {
    STR_RIDES,
    STR_SHOPS_AND_STALLS,
    STR_TOILETS_AND_INFORMATION_KIOSKS,
};

class RideListWindow final : public Window
{
private:
    bool _quickDemolishMode = false;
    int32_t _windowRideListInformationType = INFORMATION_TYPE_STATUS;
    std::vector<RideId> _rideList;

public:
    void OnOpen() override
    {
        widgets = window_ride_list_widgets;
        WindowInitScrollWidgets(*this);
        page = PAGE_RIDES;
        selected_list_item = -1;
        frame_no = 0;
        min_width = 340;
        min_height = 240;
        max_width = 400;
        max_height = 700;
        RefreshList();

        list_information_type = 0;

        _windowRideListInformationType = INFORMATION_TYPE_STATUS;
        _quickDemolishMode = false;
    }

    /**
     *
     *  rct2: 0x006B38A7
     */
    void OnResize() override
    {
        if (width < min_width)
        {
            Invalidate();
            width = min_width;
        }
        if (height < min_height)
        {
            Invalidate();
            height = min_height;
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
    void OnMouseUp(WidgetIndex widgetIndex) override
    {
        switch (widgetIndex)
        {
            case WIDX_CLOSE:
                WindowClose(*this);
                break;
            case WIDX_SORT:
                list_information_type = _windowRideListInformationType;
                selected_list_item = -1;
                RefreshList();
                break;
            case WIDX_TAB_1:
            case WIDX_TAB_2:
            case WIDX_TAB_3:
                if (page != widgetIndex - WIDX_TAB_1)
                {
                    page = widgetIndex - WIDX_TAB_1;
                    frame_no = 0;
                    selected_list_item = -1;
                    if (page != PAGE_RIDES && _windowRideListInformationType > INFORMATION_TYPE_RUNNING_COST)
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
                if (NetworkGetMode() != NETWORK_MODE_CLIENT)
                {
                    _quickDemolishMode = !_quickDemolishMode;
                }
                else
                {
                    _quickDemolishMode = false;
                }
                Invalidate();
                break;
        }
    }

    /**
     *
     *  rct2: 0x006B3532
     */
    void OnMouseDown(WidgetIndex widgetIndex) override
    {
        if (widgetIndex == WIDX_OPEN_CLOSE_ALL)
        {
            const auto& widget = widgets[widgetIndex];
            gDropdownItems[0].Format = STR_CLOSE_ALL;
            gDropdownItems[1].Format = STR_OPEN_ALL;
            WindowDropdownShowText({ windowPos.x + widget.left, windowPos.y + widget.top }, widget.height(), colours[1], 0, 2);
        }
        else if (widgetIndex == WIDX_INFORMATION_TYPE_DROPDOWN)
        {
            const auto& widget = widgets[widgetIndex - 1];

            int32_t lastType;
            if (page == PAGE_RIDES)
                lastType = INFORMATION_TYPE_NAUSEA;
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

                gDropdownItems[numItems].Format = STR_DROPDOWN_MENU_LABEL;
                gDropdownItems[numItems].Args = ride_info_type_string_mapping[type];
                numItems++;
            }

            WindowDropdownShowTextCustomWidth(
                { windowPos.x + widget.left, windowPos.y + widget.top }, widget.height(), colours[1], 0,
                Dropdown::Flag::StayOpen, numItems, widget.width() - 3);
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
    void OnDropdown(WidgetIndex widgetIndex, int32_t dropdownIndex) override
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

            Invalidate();
        }
        else if (widgetIndex == WIDX_INFORMATION_TYPE_DROPDOWN)
        {
            if (dropdownIndex == -1)
                return;

            int32_t informationType = INFORMATION_TYPE_STATUS;
            uint32_t arg = static_cast<uint32_t>(gDropdownItems[dropdownIndex].Args);
            for (size_t i = 0; i < std::size(ride_info_type_string_mapping); i++)
            {
                if (arg == ride_info_type_string_mapping[i])
                {
                    informationType = static_cast<int32_t>(i);
                }
            }

            _windowRideListInformationType = informationType;
            Invalidate();
        }
    }

    /**
     *
     *  rct2: 0x006B386B
     */
    void OnUpdate() override
    {
        frame_no = (frame_no + 1) % 64;
        WidgetInvalidate(*this, WIDX_TAB_1 + page);
        if (_windowRideListInformationType != INFORMATION_TYPE_STATUS)
            Invalidate();
    }

    /**
     *
     *  rct2: 0x006B35A1
     */
    ScreenSize OnScrollGetSize(int32_t scrollIndex) override
    {
        const auto newHeight = static_cast<int32_t>(_rideList.size() * SCROLLABLE_ROW_HEIGHT);
        if (selected_list_item != -1)
        {
            selected_list_item = -1;
            Invalidate();
        }

        auto top = newHeight - widgets[WIDX_LIST].bottom + widgets[WIDX_LIST].top + 21;
        if (top < 0)
            top = 0;
        if (top < scrolls[0].v_top)
        {
            scrolls[0].v_top = top;
            Invalidate();
        }

        return { 0, newHeight };
    }

    /**
     *
     *  rct2: 0x006B361F
     */
    void OnScrollMouseDown(int32_t scrollIndex, const ScreenCoordsXY& screenCoords) override
    {
        const auto index = screenCoords.y / SCROLLABLE_ROW_HEIGHT;
        if (index < 0 || static_cast<size_t>(index) >= _rideList.size())
            return;

        // Open ride window
        const auto rideIndex = _rideList[index];
        if (_quickDemolishMode && NetworkGetMode() != NETWORK_MODE_CLIENT)
        {
            auto gameAction = RideDemolishAction(rideIndex, RIDE_MODIFY_DEMOLISH);
            GameActions::Execute(&gameAction);
            RefreshList();
        }
        else
        {
            auto intent = Intent(WindowClass::Ride);
            intent.PutExtra(INTENT_EXTRA_RIDE_ID, rideIndex.ToUnderlying());
            ContextOpenIntent(&intent);
        }
    }

    /**
     *
     *  rct2: 0x006B35EF
     */
    void OnScrollMouseOver(int32_t scrollIndex, const ScreenCoordsXY& screenCoords) override
    {
        const auto index = screenCoords.y / SCROLLABLE_ROW_HEIGHT;
        if (index < 0 || static_cast<size_t>(index) >= _rideList.size())
            return;

        selected_list_item = index;
        Invalidate();
    }

    /**
     *
     *  rct2: 0x006B3182
     */
    void OnPrepareDraw() override
    {
        widgets[WIDX_CURRENT_INFORMATION_TYPE].text = ride_info_type_string_mapping[_windowRideListInformationType];

        // Set correct active tab
        for (int32_t i = 0; i < 3; i++)
            pressed_widgets &= ~(1 << (WIDX_TAB_1 + i));
        pressed_widgets |= 1LL << (WIDX_TAB_1 + page);

        widgets[WIDX_TITLE].text = page_names[page];

        if (_quickDemolishMode)
            pressed_widgets |= (1uLL << WIDX_QUICK_DEMOLISH);
        else
            pressed_widgets &= ~(1uLL << WIDX_QUICK_DEMOLISH);

        ResizeFrameWithPage();

        widgets[WIDX_LIST].right = width - 26;
        widgets[WIDX_LIST].bottom = height - 15;
        widgets[WIDX_OPEN_CLOSE_ALL].right = width - 2;
        widgets[WIDX_OPEN_CLOSE_ALL].left = width - 25;
        widgets[WIDX_CLOSE_LIGHT].right = width - 7;
        widgets[WIDX_CLOSE_LIGHT].left = width - 20;
        widgets[WIDX_OPEN_LIGHT].right = width - 7;
        widgets[WIDX_OPEN_LIGHT].left = width - 20;
        widgets[WIDX_QUICK_DEMOLISH].right = width - 2;
        widgets[WIDX_QUICK_DEMOLISH].left = width - 25;

        if (ThemeGetFlags() & UITHEME_FLAG_USE_LIGHTS_RIDE)
        {
            widgets[WIDX_OPEN_CLOSE_ALL].type = WindowWidgetType::Empty;
            widgets[WIDX_CLOSE_LIGHT].type = WindowWidgetType::ImgBtn;
            widgets[WIDX_OPEN_LIGHT].type = WindowWidgetType::ImgBtn;

            const auto& rideManager = GetRideManager();
            auto allClosed = true;
            auto allOpen = false;
            if (_rideList.size() > 0 && std::size(rideManager) != 0)
            {
                auto c = static_cast<RideClassification>(page);
                allClosed = std::none_of(rideManager.begin(), rideManager.end(), [c](const Ride& rideRef) {
                    return rideRef.GetClassification() == c && rideRef.status == RideStatus::Open;
                });
                allOpen = std::none_of(rideManager.begin(), rideManager.end(), [c](const Ride& rideRef) {
                    return rideRef.GetClassification() == c && rideRef.status != RideStatus::Open;
                });
            }

            const auto closeLightImage(
                SPR_G2_RCT1_CLOSE_BUTTON_0 + (allClosed ? 1 : 0) * 2 + WidgetIsPressed(*this, WIDX_CLOSE_LIGHT));
            widgets[WIDX_CLOSE_LIGHT].image = ImageId(closeLightImage);
            const auto openLightImage = SPR_G2_RCT1_OPEN_BUTTON_0 + (allOpen ? 1 : 0) * 2
                + WidgetIsPressed(*this, WIDX_OPEN_LIGHT);
            widgets[WIDX_OPEN_LIGHT].image = ImageId(openLightImage);
            widgets[WIDX_QUICK_DEMOLISH].top = widgets[WIDX_OPEN_LIGHT].bottom + 3;
        }
        else
        {
            widgets[WIDX_OPEN_CLOSE_ALL].type = WindowWidgetType::FlatBtn;
            widgets[WIDX_CLOSE_LIGHT].type = WindowWidgetType::Empty;
            widgets[WIDX_OPEN_LIGHT].type = WindowWidgetType::Empty;
            widgets[WIDX_QUICK_DEMOLISH].top = widgets[WIDX_OPEN_CLOSE_ALL].bottom + 3;
        }
        widgets[WIDX_QUICK_DEMOLISH].bottom = widgets[WIDX_QUICK_DEMOLISH].top + 23;
        widgets[WIDX_QUICK_DEMOLISH].type = NetworkGetMode() != NETWORK_MODE_CLIENT ? WindowWidgetType::FlatBtn
                                                                                    : WindowWidgetType::Empty;
    }

    /**
     *
     *  rct2: 0x006B3235
     */
    void OnDraw(DrawPixelInfo& dpi) override
    {
        WindowDrawWidgets(*this, &dpi);
        DrawTabImages(&dpi);

        // Draw number of attractions on bottom
        auto ft = Formatter();
        ft.Add<uint16_t>(static_cast<uint16_t>(_rideList.size()));
        DrawTextBasic(
            dpi, windowPos + ScreenCoordsXY{ 4, widgets[WIDX_LIST].bottom + 2 }, ride_list_statusbar_count_strings[page], ft);
    }

    /**
     *
     *  rct2: 0x006B3240
     */
    void OnScrollDraw(int32_t scrollIndex, DrawPixelInfo& dpi) override
    {
        auto dpiCoords = ScreenCoordsXY{ dpi.x, dpi.y };
        GfxFillRect(&dpi, { dpiCoords, dpiCoords + ScreenCoordsXY{ dpi.width, dpi.height } }, ColourMapA[colours[1]].mid_light);

        auto y = 0;
        for (size_t i = 0; i < _rideList.size(); i++)
        {
            StringId format = (_quickDemolishMode ? STR_RED_STRINGID : STR_BLACK_STRING);
            if (i == static_cast<size_t>(selected_list_item))
            {
                // Background highlight
                GfxFilterRect(&dpi, { 0, y, 800, y + SCROLLABLE_ROW_HEIGHT - 1 }, FilterPaletteID::PaletteDarken1);
                format = (_quickDemolishMode ? STR_LIGHTPINK_STRINGID : STR_WINDOW_COLOUR_2_STRINGID);
            }

            // Get ride
            const auto* ridePtr = GetRide(_rideList[i]);
            if (ridePtr == nullptr)
                continue;

            // Ride name
            auto ft = Formatter();
            ridePtr->FormatNameTo(ft);
            DrawTextEllipsised(dpi, { 0, y - 1 }, 159, format, ft);

            // Ride information
            ft = Formatter();
            ft.Increment(2);
            auto formatSecondaryEnabled = true;
            StringId formatSecondary = 0;
            switch (_windowRideListInformationType)
            {
                case INFORMATION_TYPE_STATUS:
                    formatSecondaryEnabled = false;
                    ft.Rewind();
                    ridePtr->FormatStatusTo(ft);

                    // Make test red and bold if broken down or crashed
                    if ((ridePtr->lifecycle_flags & RIDE_LIFECYCLE_BROKEN_DOWN)
                        || (ridePtr->lifecycle_flags & RIDE_LIFECYCLE_CRASHED))
                    {
                        format = STR_RED_OUTLINED_STRING;
                    }
                    break;
                case INFORMATION_TYPE_POPULARITY:
                    formatSecondary = STR_POPULARITY_UNKNOWN_LABEL;
                    if (ridePtr->popularity != 255)
                    {
                        formatSecondary = STR_POPULARITY_LABEL;
                        ft.Add<uint16_t>(ridePtr->popularity * 4);
                    }
                    break;
                case INFORMATION_TYPE_SATISFACTION:
                    formatSecondary = STR_SATISFACTION_UNKNOWN_LABEL;
                    if (ridePtr->satisfaction != 255)
                    {
                        formatSecondary = STR_SATISFACTION_LABEL;
                        ft.Add<uint16_t>(ridePtr->satisfaction * 5);
                    }
                    break;
                case INFORMATION_TYPE_PROFIT:
                    formatSecondary = 0;
                    if (ridePtr->profit != MONEY64_UNDEFINED)
                    {
                        formatSecondary = STR_PROFIT_LABEL;
                        ft.Add<money64>(ridePtr->profit);
                    }
                    break;
                case INFORMATION_TYPE_TOTAL_CUSTOMERS:
                    formatSecondary = STR_RIDE_LIST_TOTAL_CUSTOMERS_LABEL;
                    ft.Add<uint32_t>(ridePtr->total_customers);
                    break;
                case INFORMATION_TYPE_TOTAL_PROFIT:
                    formatSecondary = 0;
                    if (ridePtr->total_profit != MONEY64_UNDEFINED)
                    {
                        formatSecondary = STR_RIDE_LIST_TOTAL_PROFIT_LABEL;
                        ft.Add<money64>(ridePtr->total_profit);
                    }
                    break;
                case INFORMATION_TYPE_CUSTOMERS:
                    formatSecondary = STR_RIDE_LIST_CUSTOMERS_PER_HOUR_LABEL;
                    ft.Add<uint32_t>(RideCustomersPerHour(*ridePtr));
                    break;
                case INFORMATION_TYPE_AGE:
                {
                    const auto age = DateGetYear(ridePtr->GetAge());
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
                    if (ridePtr->income_per_hour != MONEY64_UNDEFINED)
                    {
                        formatSecondary = STR_RIDE_LIST_INCOME_LABEL;
                        ft.Add<money64>(ridePtr->income_per_hour);
                    }
                    break;
                case INFORMATION_TYPE_RUNNING_COST:
                    formatSecondary = STR_RIDE_LIST_RUNNING_COST_UNKNOWN;
                    if (ridePtr->upkeep_cost != MONEY64_UNDEFINED)
                    {
                        formatSecondary = STR_RIDE_LIST_RUNNING_COST_LABEL;
                        ft.Add<money64>(ridePtr->upkeep_cost * 16);
                    }
                    break;
                case INFORMATION_TYPE_QUEUE_LENGTH:
                {
                    const auto queueLength = ridePtr->GetTotalQueueLength();
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
                    const auto maxQueueTime = ridePtr->GetMaxQueueTime();
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
                    ft.Add<uint16_t>(ridePtr->reliability_percentage);
                    formatSecondary = STR_RELIABILITY_LABEL;
                    break;
                case INFORMATION_TYPE_DOWN_TIME:
                    ft.Add<uint16_t>(ridePtr->downtime);
                    formatSecondary = STR_DOWN_TIME_LABEL;
                    break;
                case INFORMATION_TYPE_GUESTS_FAVOURITE:
                    formatSecondary = 0;
                    if (ridePtr->IsRide())
                    {
                        ft.Add<uint32_t>(ridePtr->guests_favourite);
                        formatSecondary = ridePtr->guests_favourite == 1 ? STR_GUESTS_FAVOURITE_LABEL
                                                                         : STR_GUESTS_FAVOURITE_PLURAL_LABEL;
                    }
                    break;
                case INFORMATION_TYPE_EXCITEMENT:
                    formatSecondary = STR_RATING_UKNOWN_LABEL;
                    if (RideHasRatings(*ridePtr))
                    {
                        formatSecondary = STR_EXCITEMENT_LABEL;
                        ft.Add<uint16_t>(ridePtr->excitement);
                    }
                    break;
                case INFORMATION_TYPE_INTENSITY:
                    formatSecondary = STR_RATING_UKNOWN_LABEL;
                    if (RideHasRatings(*ridePtr))
                    {
                        formatSecondary = STR_INTENSITY_LABEL;
                        ft.Add<uint16_t>(ridePtr->intensity);
                    }
                    break;
                case INFORMATION_TYPE_NAUSEA:
                    formatSecondary = STR_RATING_UKNOWN_LABEL;
                    if (RideHasRatings(*ridePtr))
                    {
                        formatSecondary = STR_NAUSEA_LABEL;
                        ft.Add<uint16_t>(ridePtr->nausea);
                    }
                    break;
            }

            if (formatSecondaryEnabled)
            {
                ft.Rewind();
                ft.Add<StringId>(formatSecondary);
            }
            DrawTextEllipsised(dpi, { 160, y - 1 }, 157, format, ft);
            y += SCROLLABLE_ROW_HEIGHT;
        }
    }

    void RefreshListWrapper()
    {
        RefreshList();
    }

private:
    /**
     *
     *  rct2: 0x006B38EA
     */
    void DrawTabImages(DrawPixelInfo* dpi)
    {
        int32_t sprite_idx;

        // Rides tab
        sprite_idx = SPR_TAB_RIDE_0;
        if (page == PAGE_RIDES)
            sprite_idx += frame_no / 4;
        GfxDrawSprite(
            dpi, ImageId(sprite_idx), windowPos + ScreenCoordsXY{ widgets[WIDX_TAB_1].left, widgets[WIDX_TAB_1].top });

        // Shops and stalls tab
        sprite_idx = SPR_TAB_SHOPS_AND_STALLS_0;
        if (page == PAGE_SHOPS_AND_STALLS)
            sprite_idx += frame_no / 4;
        GfxDrawSprite(
            dpi, ImageId(sprite_idx), windowPos + ScreenCoordsXY{ widgets[WIDX_TAB_2].left, widgets[WIDX_TAB_2].top });

        // Information kiosks and facilities tab
        sprite_idx = SPR_TAB_KIOSKS_AND_FACILITIES_0;
        if (page == PAGE_KIOSKS_AND_FACILITIES)
            sprite_idx += (frame_no / 4) % 8;
        GfxDrawSprite(
            dpi, ImageId(sprite_idx), windowPos + ScreenCoordsXY{ widgets[WIDX_TAB_3].left, widgets[WIDX_TAB_3].top });
    }

    /**
     * Used in RefreshList() to handle the sorting of the list.
     * Uses a lambda function (predicate) as exit criteria for the algorithm.
     */
    template<typename TSortPred> int32_t SortList(int32_t currentListPosition, const Ride& thisRide, const TSortPred& pred)
    {
        while (--currentListPosition >= 0)
        {
            const auto* otherRide = GetRide(_rideList[currentListPosition]);
            if (otherRide != nullptr)
            {
                if (pred(thisRide, *otherRide))
                    break;
                std::swap(_rideList[currentListPosition], _rideList[currentListPosition + 1]);
            }
        }
        return currentListPosition;
    }
    /**
     *
     *  rct2: 0x006B39A8
     */
    void RefreshList()
    {
        _rideList.clear();

        size_t listIndex = 0;
        for (auto& rideRef : GetRideManager())
        {
            if (rideRef.GetClassification() != static_cast<RideClassification>(page)
                || (rideRef.status == RideStatus::Closed && !RideHasAnyTrackElements(rideRef)))
                continue;

            if (rideRef.window_invalidate_flags & RIDE_INVALIDATE_RIDE_LIST)
            {
                rideRef.window_invalidate_flags &= ~RIDE_INVALIDATE_RIDE_LIST;
            }

            _rideList.push_back(rideRef.id);
            auto currentListPosition = static_cast<int32_t>(listIndex);
            switch (list_information_type)
            {
                case INFORMATION_TYPE_STATUS:
                    currentListPosition = SortList(
                        currentListPosition, rideRef, [](const Ride& thisRide, const Ride& otherRide) -> bool {
                            return 0 <= StrLogicalCmp(thisRide.GetName().c_str(), otherRide.GetName().c_str());
                        });
                    break;
                case INFORMATION_TYPE_POPULARITY:
                    currentListPosition = SortList(
                        currentListPosition, rideRef, [](const Ride& thisRide, const Ride& otherRide) -> bool {
                            return thisRide.popularity * 4 <= otherRide.popularity * 4;
                        });
                    break;
                case INFORMATION_TYPE_SATISFACTION:
                    currentListPosition = SortList(
                        currentListPosition, rideRef, [](const Ride& thisRide, const Ride& otherRide) -> bool {
                            return thisRide.satisfaction * 5 <= otherRide.satisfaction * 5;
                        });
                    break;
                case INFORMATION_TYPE_PROFIT:
                    currentListPosition = SortList(
                        currentListPosition, rideRef, [](const Ride& thisRide, const Ride& otherRide) -> bool {
                            return thisRide.profit <= otherRide.profit;
                        });
                    break;
                case INFORMATION_TYPE_TOTAL_CUSTOMERS:
                    currentListPosition = SortList(
                        currentListPosition, rideRef, [](const Ride& thisRide, const Ride& otherRide) -> bool {
                            return thisRide.total_customers <= otherRide.total_customers;
                        });
                    break;
                case INFORMATION_TYPE_TOTAL_PROFIT:
                    currentListPosition = SortList(
                        currentListPosition, rideRef, [](const Ride& thisRide, const Ride& otherRide) -> bool {
                            return thisRide.total_profit <= otherRide.total_profit;
                        });
                    break;
                case INFORMATION_TYPE_CUSTOMERS:
                    currentListPosition = SortList(
                        currentListPosition, rideRef, [](const Ride& thisRide, const Ride& otherRide) -> bool {
                            return RideCustomersPerHour(thisRide) <= RideCustomersPerHour(otherRide);
                        });
                    break;
                case INFORMATION_TYPE_AGE:
                    currentListPosition = SortList(
                        currentListPosition, rideRef, [](const Ride& thisRide, const Ride& otherRide) -> bool {
                            return thisRide.build_date <= otherRide.build_date;
                        });
                    break;
                case INFORMATION_TYPE_INCOME:
                    currentListPosition = SortList(
                        currentListPosition, rideRef, [](const Ride& thisRide, const Ride& otherRide) -> bool {
                            return thisRide.income_per_hour <= otherRide.income_per_hour;
                        });
                    break;
                case INFORMATION_TYPE_RUNNING_COST:
                    currentListPosition = SortList(
                        currentListPosition, rideRef, [](const Ride& thisRide, const Ride& otherRide) -> bool {
                            return thisRide.upkeep_cost <= otherRide.upkeep_cost;
                        });
                    break;
                case INFORMATION_TYPE_QUEUE_LENGTH:
                    currentListPosition = SortList(
                        currentListPosition, rideRef, [](const Ride& thisRide, const Ride& otherRide) -> bool {
                            return thisRide.GetTotalQueueLength() <= otherRide.GetTotalQueueLength();
                        });
                    break;
                case INFORMATION_TYPE_QUEUE_TIME:
                    currentListPosition = SortList(
                        currentListPosition, rideRef, [](const Ride& thisRide, const Ride& otherRide) -> bool {
                            return thisRide.GetMaxQueueTime() <= otherRide.GetMaxQueueTime();
                        });
                    break;
                case INFORMATION_TYPE_RELIABILITY:
                    currentListPosition = SortList(
                        currentListPosition, rideRef, [](const Ride& thisRide, const Ride& otherRide) -> bool {
                            return thisRide.reliability_percentage <= otherRide.reliability_percentage;
                        });
                    break;
                case INFORMATION_TYPE_DOWN_TIME:
                    currentListPosition = SortList(
                        currentListPosition, rideRef, [](const Ride& thisRide, const Ride& otherRide) -> bool {
                            return thisRide.downtime <= otherRide.downtime;
                        });
                    break;
                case INFORMATION_TYPE_GUESTS_FAVOURITE:
                    currentListPosition = SortList(
                        currentListPosition, rideRef, [](const Ride& thisRide, const Ride& otherRide) -> bool {
                            return thisRide.guests_favourite <= otherRide.guests_favourite;
                        });
                    break;
                case INFORMATION_TYPE_EXCITEMENT:
                    currentListPosition = SortList(
                        currentListPosition, rideRef, [](const Ride& thisRide, const Ride& otherRide) -> bool {
                            return thisRide.excitement <= otherRide.excitement;
                        });
                    break;
                case INFORMATION_TYPE_INTENSITY:
                    currentListPosition = SortList(
                        currentListPosition, rideRef, [](const Ride& thisRide, const Ride& otherRide) -> bool {
                            return thisRide.intensity <= otherRide.intensity;
                        });
                    break;
                case INFORMATION_TYPE_NAUSEA:
                    currentListPosition = SortList(
                        currentListPosition, rideRef, [](const Ride& thisRide, const Ride& otherRide) -> bool {
                            return thisRide.nausea <= otherRide.nausea;
                        });
                    break;
            }
            listIndex++;
        }

        selected_list_item = -1;
        Invalidate();
    }

    // window_ride_list_close_all
    void CloseAllRides()
    {
        for (auto& rideRef : GetRideManager())
        {
            if (rideRef.status != RideStatus::Closed && rideRef.GetClassification() == static_cast<RideClassification>(page))
            {
                auto gameAction = RideSetStatusAction(rideRef.id, RideStatus::Closed);
                GameActions::Execute(&gameAction);
            }
        }
    }

    // window_ride_list_open_all
    void OpenAllRides()
    {
        for (auto& rideRef : GetRideManager())
        {
            if (rideRef.status != RideStatus::Open && rideRef.GetClassification() == static_cast<RideClassification>(page))
            {
                auto gameAction = RideSetStatusAction(rideRef.id, RideStatus::Open);
                GameActions::Execute(&gameAction);
            }
        }
    }
};

/**
 *
 *  rct2: 0x006B30BC
 */
WindowBase* WindowRideListOpen()
{
    // Check if window is already open
    auto* window = WindowBringToFrontByClass(WindowClass::RideList);
    if (window == nullptr)
    {
        window = WindowCreate<RideListWindow>(WindowClass::RideList, ScreenCoordsXY(32, 32), WW, WH, WF_10 | WF_RESIZABLE);
    }
    return window;
}

void WindowRideListRefreshList(WindowBase* w)
{
    dynamic_cast<RideListWindow*>(w)->RefreshListWrapper();
}
