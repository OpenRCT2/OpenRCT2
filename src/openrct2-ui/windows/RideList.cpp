/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
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
#include <openrct2-ui/windows/Windows.h>
#include <openrct2/Context.h>
#include <openrct2/Game.h>
#include <openrct2/GameState.h>
#include <openrct2/SpriteIds.h>
#include <openrct2/actions/RideDemolishAction.h>
#include <openrct2/actions/RideSetStatusAction.h>
#include <openrct2/core/String.hpp>
#include <openrct2/drawing/Drawing.h>
#include <openrct2/interface/Colour.h>
#include <openrct2/localisation/Formatter.h>
#include <openrct2/network/Network.h>
#include <openrct2/ride/RideManager.hpp>
#include <openrct2/ride/RideRatings.h>
#include <openrct2/ui/WindowManager.h>
#include <openrct2/windows/Intent.h>
#include <openrct2/world/Park.h>

namespace OpenRCT2::Ui::Windows
{
    static constexpr StringId kWindowTitle = kStringIdNone;
    static constexpr ScreenSize kWindowSize = { 340, 240 };

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
    static constexpr auto _rideListWidgets = makeWidgets(
        makeWindowShim(kWindowTitle, kWindowSize),
        makeWidget({  0, 43}, {340, 197}, WidgetType::resize,       WindowColour::secondary                                                                        ), // tab page background
        makeWidget({315, 60}, { 24,  24}, WidgetType::flatBtn,      WindowColour::secondary, ImageId(SPR_TOGGLE_OPEN_CLOSE),     STR_OPEN_OR_CLOSE_ALL_RIDES       ), // open / close all toggle
        makeWidget({150, 46}, {124,  12}, WidgetType::dropdownMenu, WindowColour::secondary                                                                        ), // current information type
        makeWidget({262, 47}, { 11,  10}, WidgetType::button,       WindowColour::secondary, STR_DROPDOWN_GLYPH,                 STR_RIDE_LIST_INFORMATION_TYPE_TIP), // information type dropdown button
        makeWidget({280, 46}, { 54,  12}, WidgetType::button,       WindowColour::secondary, STR_SORT,                           STR_RIDE_LIST_SORT_TIP            ), // sort button
        makeTab   ({  3, 17},                                                                STR_LIST_RIDES_TIP                                                    ), // tab 1
        makeTab   ({ 34, 17},                                                                STR_LIST_SHOPS_AND_STALLS_TIP                                         ), // tab 2
        makeTab   ({ 65, 17},                                                                STR_LIST_KIOSKS_AND_FACILITIES_TIP                                    ), // tab 3
        makeWidget({  3, 60}, {334, 177}, WidgetType::scroll,       WindowColour::secondary, SCROLL_VERTICAL                                                       ), // list
        makeWidget({320, 62}, { 14,  14}, WidgetType::imgBtn,       WindowColour::secondary, ImageId(SPR_G2_RCT1_CLOSE_BUTTON_0)                                   ),
        makeWidget({320, 76}, { 14,  14}, WidgetType::imgBtn,       WindowColour::secondary, ImageId(SPR_G2_RCT1_OPEN_BUTTON_0)                                    ),
        makeWidget({315, 90}, { 24,  24}, WidgetType::flatBtn,      WindowColour::secondary, ImageId(SPR_DEMOLISH),              STR_QUICK_DEMOLISH_RIDE           )
    );
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
        INFORMATION_TYPE_LAST_INSPECTION,
        INFORMATION_TYPE_GUESTS_FAVOURITE,
        INFORMATION_TYPE_EXCITEMENT,
        INFORMATION_TYPE_INTENSITY,
        INFORMATION_TYPE_NAUSEA,
        DROPDOWN_LIST_COUNT,
    };

    static constexpr StringId ride_info_type_string_mapping[DROPDOWN_LIST_COUNT] = {
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
        STR_LAST_INSPECTION,
        STR_GUESTS_FAVOURITE,
        STR_RIDE_LIST_EXCITEMENT,
        STR_RIDE_LIST_INTENSITY,
        STR_RIDE_LIST_NAUSEA,
    };

    static constexpr StringId ride_list_statusbar_count_strings[PAGE_COUNT] = {
        STR_NUMBER_RIDES,
        STR_NUMBER_SHOPS_AND_STALLS,
        STR_NUMBER_TOILETS_AND_INFORMATION_KIOSKS,
    };

    static constexpr bool ride_info_type_money_mapping[DROPDOWN_LIST_COUNT] = {
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
        false, // Last inspection
        false, // Guests favourite
        false, // Excitement
        false, // Intensity
        false, // Nausea
    };

    static constexpr StringId page_names[] = {
        STR_RIDES,
        STR_SHOPS_AND_STALLS,
        STR_TOILETS_AND_INFORMATION_KIOSKS,
    };

    class RideListWindow final : public Window
    {
    private:
        bool _quickDemolishMode = false;
        int32_t _windowRideListInformationType = INFORMATION_TYPE_STATUS;

        struct RideListEntry
        {
            RideId Id;
            u8string Name;
        };
        std::vector<RideListEntry> _rideList;

    public:
        void OnOpen() override
        {
            SetWidgets(_rideListWidgets);
            WindowInitScrollWidgets(*this);
            WindowSetResize(*this, { 340, 240 }, { 400, 700 });

            page = PAGE_RIDES;
            selected_list_item = -1;
            frame_no = 0;

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

            widgets[WIDX_SORT].left = width - 60;
            widgets[WIDX_SORT].right = width - 60 + 54;

            auto dropdownStart = widgets[WIDX_CURRENT_INFORMATION_TYPE].top;
            ResizeDropdown(WIDX_CURRENT_INFORMATION_TYPE, { 100, dropdownStart }, { width - 166, kDropdownHeight });

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
                    Close();
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
                WindowDropdownShowText(
                    { windowPos.x + widget.left, windowPos.y + widget.top }, widget.height(), colours[1], 0, 2);
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
                    if ((getGameState().park.Flags & PARK_FLAGS_NO_MONEY))
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
            InvalidateWidget(WIDX_TAB_1 + page);
            if (_windowRideListInformationType != INFORMATION_TYPE_STATUS)
                Invalidate();
        }

        /**
         *
         *  rct2: 0x006B35A1
         */
        ScreenSize OnScrollGetSize(int32_t scrollIndex) override
        {
            const auto newHeight = static_cast<int32_t>(_rideList.size() * kScrollableRowHeight);
            if (selected_list_item != -1)
            {
                selected_list_item = -1;
                Invalidate();
            }

            auto top = newHeight - widgets[WIDX_LIST].bottom + widgets[WIDX_LIST].top + 21;
            if (top < 0)
                top = 0;
            if (top < scrolls[0].contentOffsetY)
            {
                scrolls[0].contentOffsetY = top;
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
            const auto index = screenCoords.y / kScrollableRowHeight;
            if (index < 0 || static_cast<size_t>(index) >= _rideList.size())
                return;

            // Open ride window
            const auto selectedRideId = _rideList[index].Id;
            if (_quickDemolishMode && NetworkGetMode() != NETWORK_MODE_CLIENT)
            {
                auto gameAction = RideDemolishAction(selectedRideId, RideModifyType::demolish);
                GameActions::Execute(&gameAction);
                RefreshList();
            }
            else
            {
                auto intent = Intent(WindowClass::Ride);
                intent.PutExtra(INTENT_EXTRA_RIDE_ID, selectedRideId.ToUnderlying());
                ContextOpenIntent(&intent);
            }
        }

        /**
         *
         *  rct2: 0x006B35EF
         */
        void OnScrollMouseOver(int32_t scrollIndex, const ScreenCoordsXY& screenCoords) override
        {
            const auto index = screenCoords.y / kScrollableRowHeight;
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
                widgets[WIDX_OPEN_CLOSE_ALL].type = WidgetType::empty;
                widgets[WIDX_CLOSE_LIGHT].type = WidgetType::imgBtn;
                widgets[WIDX_OPEN_LIGHT].type = WidgetType::imgBtn;

                const auto& rideManager = GetRideManager();
                auto allClosed = true;
                auto allOpen = false;
                if (_rideList.size() > 0 && std::size(rideManager) != 0)
                {
                    auto c = static_cast<RideClassification>(page);
                    allClosed = std::none_of(rideManager.begin(), rideManager.end(), [c](const Ride& rideRef) {
                        return rideRef.getClassification() == c && rideRef.status == RideStatus::open;
                    });
                    allOpen = std::none_of(rideManager.begin(), rideManager.end(), [c](const Ride& rideRef) {
                        return rideRef.getClassification() == c && rideRef.status != RideStatus::open;
                    });
                }

                const auto closeLightImage(
                    SPR_G2_RCT1_CLOSE_BUTTON_0 + (allClosed ? 1 : 0) * 2 + widgetIsPressed(*this, WIDX_CLOSE_LIGHT));
                widgets[WIDX_CLOSE_LIGHT].image = ImageId(closeLightImage);
                const auto openLightImage = SPR_G2_RCT1_OPEN_BUTTON_0 + (allOpen ? 1 : 0) * 2
                    + widgetIsPressed(*this, WIDX_OPEN_LIGHT);
                widgets[WIDX_OPEN_LIGHT].image = ImageId(openLightImage);
                widgets[WIDX_QUICK_DEMOLISH].top = widgets[WIDX_OPEN_LIGHT].bottom + 3;
            }
            else
            {
                widgets[WIDX_OPEN_CLOSE_ALL].type = WidgetType::flatBtn;
                widgets[WIDX_CLOSE_LIGHT].type = WidgetType::empty;
                widgets[WIDX_OPEN_LIGHT].type = WidgetType::empty;
                widgets[WIDX_QUICK_DEMOLISH].top = widgets[WIDX_OPEN_CLOSE_ALL].bottom + 3;
            }
            widgets[WIDX_QUICK_DEMOLISH].bottom = widgets[WIDX_QUICK_DEMOLISH].top + 23;
            widgets[WIDX_QUICK_DEMOLISH].type = NetworkGetMode() != NETWORK_MODE_CLIENT ? WidgetType::flatBtn
                                                                                        : WidgetType::empty;
        }

        /**
         *
         *  rct2: 0x006B3235
         */
        void OnDraw(RenderTarget& rt) override
        {
            WindowDrawWidgets(*this, rt);
            DrawTabImages(rt);

            // Draw number of attractions on bottom
            auto ft = Formatter();
            ft.Add<uint16_t>(static_cast<uint16_t>(_rideList.size()));
            DrawTextBasic(
                rt, windowPos + ScreenCoordsXY{ 4, widgets[WIDX_LIST].bottom + 2 }, ride_list_statusbar_count_strings[page],
                ft);
        }

        /**
         *
         *  rct2: 0x006B3240
         */
        void OnScrollDraw(int32_t scrollIndex, RenderTarget& rt) override
        {
            auto rtCoords = ScreenCoordsXY{ rt.x, rt.y };
            GfxFillRect(
                rt, { rtCoords, rtCoords + ScreenCoordsXY{ rt.width, rt.height } }, ColourMapA[colours[1].colour].mid_light);

            auto y = 0;
            for (size_t i = 0; i < _rideList.size(); i++)
            {
                StringId format = STR_BLACK_STRING;
                if (_quickDemolishMode)
                    format = STR_RED_STRINGID;

                if (i == static_cast<size_t>(selected_list_item))
                {
                    // Background highlight
                    GfxFilterRect(rt, { 0, y, 800, y + kScrollableRowHeight - 1 }, FilterPaletteID::PaletteDarken1);
                    format = STR_WINDOW_COLOUR_2_STRINGID;
                    if (_quickDemolishMode)
                        format = STR_LIGHTPINK_STRINGID;
                }

                // Get ride
                const auto* ridePtr = GetRide(_rideList[i].Id);
                if (ridePtr == nullptr)
                    continue;

                // Ride name
                auto ft = Formatter();
                ridePtr->formatNameTo(ft);
                DrawTextEllipsised(rt, { 0, y - 1 }, 159, format, ft);

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
                        ridePtr->formatStatusTo(ft);

                        // Make test red and bold if broken down or crashed
                        if ((ridePtr->lifecycleFlags & RIDE_LIFECYCLE_BROKEN_DOWN)
                            || (ridePtr->lifecycleFlags & RIDE_LIFECYCLE_CRASHED))
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
                        if (ridePtr->profit != kMoney64Undefined)
                        {
                            formatSecondary = STR_PROFIT_LABEL;
                            ft.Add<money64>(ridePtr->profit);
                        }
                        break;
                    case INFORMATION_TYPE_TOTAL_CUSTOMERS:
                        formatSecondary = STR_RIDE_LIST_TOTAL_CUSTOMERS_LABEL;
                        ft.Add<uint32_t>(ridePtr->totalCustomers);
                        break;
                    case INFORMATION_TYPE_TOTAL_PROFIT:
                        formatSecondary = 0;
                        if (ridePtr->totalProfit != kMoney64Undefined)
                        {
                            formatSecondary = STR_RIDE_LIST_TOTAL_PROFIT_LABEL;
                            ft.Add<money64>(ridePtr->totalProfit);
                        }
                        break;
                    case INFORMATION_TYPE_CUSTOMERS:
                        formatSecondary = STR_RIDE_LIST_CUSTOMERS_PER_HOUR_LABEL;
                        ft.Add<uint32_t>(RideCustomersPerHour(*ridePtr));
                        break;
                    case INFORMATION_TYPE_AGE:
                    {
                        const auto age = DateGetYear(ridePtr->getAge());
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
                        if (ridePtr->incomePerHour != kMoney64Undefined)
                        {
                            formatSecondary = STR_RIDE_LIST_INCOME_LABEL;
                            ft.Add<money64>(ridePtr->incomePerHour);
                        }
                        break;
                    case INFORMATION_TYPE_RUNNING_COST:
                        formatSecondary = STR_RIDE_LIST_RUNNING_COST_UNKNOWN;
                        if (ridePtr->upkeepCost != kMoney64Undefined)
                        {
                            formatSecondary = STR_RIDE_LIST_RUNNING_COST_LABEL;
                            ft.Add<money64>(ridePtr->upkeepCost * 16);
                        }
                        break;
                    case INFORMATION_TYPE_QUEUE_LENGTH:
                    {
                        const auto queueLength = ridePtr->getTotalQueueLength();
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
                        const auto maxQueueTime = ridePtr->getMaxQueueTime();
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
                        ft.Add<uint16_t>(ridePtr->reliabilityPercentage);
                        formatSecondary = STR_RELIABILITY_LABEL;
                        break;
                    case INFORMATION_TYPE_DOWN_TIME:
                        ft.Add<uint16_t>(ridePtr->downtime);
                        formatSecondary = STR_DOWN_TIME_LABEL;
                        break;
                    case INFORMATION_TYPE_LAST_INSPECTION:
                    {
                        const auto lastInspection = ridePtr->lastInspection;
                        ft.Add<uint16_t>(lastInspection);

                        if (lastInspection <= 1)
                        {
                            formatSecondary = STR_LAST_INSPECTION_LABEL_MINUTE;
                        }
                        else if (lastInspection <= 240)
                        {
                            formatSecondary = STR_LAST_INSPECTION_LABEL_MINUTES;
                        }
                        else
                        {
                            formatSecondary = STR_LAST_INSPECTION_LABEL_MORE_THAN_FOUR_HOURS;
                        }
                        break;
                    }
                    case INFORMATION_TYPE_GUESTS_FAVOURITE:
                        formatSecondary = 0;
                        if (ridePtr->isRide())
                        {
                            ft.Add<uint32_t>(ridePtr->guestsFavourite);
                            formatSecondary = ridePtr->guestsFavourite == 1 ? STR_GUESTS_FAVOURITE_LABEL
                                                                            : STR_GUESTS_FAVOURITE_PLURAL_LABEL;
                        }
                        break;
                    case INFORMATION_TYPE_EXCITEMENT:
                        formatSecondary = STR_RATING_UKNOWN_LABEL;
                        if (RideHasRatings(*ridePtr))
                        {
                            formatSecondary = STR_EXCITEMENT_LABEL;
                            ft.Add<uint16_t>(ridePtr->ratings.excitement);
                        }
                        break;
                    case INFORMATION_TYPE_INTENSITY:
                        formatSecondary = STR_RATING_UKNOWN_LABEL;
                        if (RideHasRatings(*ridePtr))
                        {
                            formatSecondary = STR_INTENSITY_LABEL;
                            ft.Add<uint16_t>(ridePtr->ratings.intensity);
                        }
                        break;
                    case INFORMATION_TYPE_NAUSEA:
                        formatSecondary = STR_RATING_UKNOWN_LABEL;
                        if (RideHasRatings(*ridePtr))
                        {
                            formatSecondary = STR_NAUSEA_LABEL;
                            ft.Add<uint16_t>(ridePtr->ratings.nausea);
                        }
                        break;
                }

                if (formatSecondaryEnabled)
                {
                    ft.Rewind();
                    ft.Add<StringId>(formatSecondary);
                }
                DrawTextEllipsised(rt, { 160, y - 1 }, 157, format, ft);
                y += kScrollableRowHeight;
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
        void DrawTabImages(RenderTarget& rt)
        {
            int32_t sprite_idx;

            // Rides tab
            sprite_idx = SPR_TAB_RIDE_0;
            if (page == PAGE_RIDES)
                sprite_idx += frame_no / 4;
            GfxDrawSprite(
                rt, ImageId(sprite_idx), windowPos + ScreenCoordsXY{ widgets[WIDX_TAB_1].left, widgets[WIDX_TAB_1].top });

            // Shops and stalls tab
            sprite_idx = SPR_TAB_SHOPS_AND_STALLS_0;
            if (page == PAGE_SHOPS_AND_STALLS)
                sprite_idx += frame_no / 4;
            GfxDrawSprite(
                rt, ImageId(sprite_idx), windowPos + ScreenCoordsXY{ widgets[WIDX_TAB_2].left, widgets[WIDX_TAB_2].top });

            // Information kiosks and facilities tab
            sprite_idx = SPR_TAB_KIOSKS_AND_FACILITIES_0;
            if (page == PAGE_KIOSKS_AND_FACILITIES)
                sprite_idx += (frame_no / 4) % 8;
            GfxDrawSprite(
                rt, ImageId(sprite_idx), windowPos + ScreenCoordsXY{ widgets[WIDX_TAB_3].left, widgets[WIDX_TAB_3].top });
        }

        /**
         * Used in RefreshList() to handle the sorting of the list.
         * Uses a lambda function (predicate) as exit criteria for the algorithm.
         */
        template<typename TSortPred>
        void SortListByPredicate(const TSortPred& pred)
        {
            std::sort(_rideList.begin(), _rideList.end(), [&pred](const auto& lhs, const auto& rhs) {
                const Ride* rideLhs = GetRide(lhs.Id);
                const Ride* rideRhs = GetRide(rhs.Id);
                if (rideLhs == nullptr || rideRhs == nullptr)
                {
                    return rideLhs != nullptr;
                }
                return !pred(*rideLhs, *rideRhs);
            });
        }

        void SortListByName()
        {
            std::sort(_rideList.begin(), _rideList.end(), [](const auto& lhs, const auto& rhs) {
                return !(0 <= String::logicalCmp(lhs.Name.c_str(), rhs.Name.c_str()));
            });
        }

        /**
         *
         *  rct2: 0x006B39A8
         */
        void RefreshList()
        {
            _rideList.clear();
            for (auto& rideRef : GetRideManager())
            {
                if (rideRef.getClassification() != static_cast<RideClassification>(page)
                    || (rideRef.status == RideStatus::closed && !RideHasAnyTrackElements(rideRef)))
                {
                    continue;
                }

                if (rideRef.windowInvalidateFlags & RIDE_INVALIDATE_RIDE_LIST)
                {
                    rideRef.windowInvalidateFlags &= ~RIDE_INVALIDATE_RIDE_LIST;
                }

                RideListEntry entry{};
                entry.Id = rideRef.id;
                entry.Name = rideRef.getName();

                _rideList.push_back(std::move(entry));
            }

            switch (list_information_type)
            {
                case INFORMATION_TYPE_STATUS:
                    SortListByName();
                    break;
                case INFORMATION_TYPE_POPULARITY:
                    SortListByPredicate([](const Ride& thisRide, const Ride& otherRide) -> bool {
                        return thisRide.popularity * 4 <= otherRide.popularity * 4;
                    });
                    break;
                case INFORMATION_TYPE_SATISFACTION:
                    SortListByPredicate([](const Ride& thisRide, const Ride& otherRide) -> bool {
                        return thisRide.satisfaction * 5 <= otherRide.satisfaction * 5;
                    });
                    break;
                case INFORMATION_TYPE_PROFIT:
                    SortListByPredicate([](const Ride& thisRide, const Ride& otherRide) -> bool {
                        return thisRide.profit <= otherRide.profit;
                    });
                    break;
                case INFORMATION_TYPE_TOTAL_CUSTOMERS:
                    SortListByPredicate([](const Ride& thisRide, const Ride& otherRide) -> bool {
                        return thisRide.totalCustomers <= otherRide.totalCustomers;
                    });
                    break;
                case INFORMATION_TYPE_TOTAL_PROFIT:
                    SortListByPredicate([](const Ride& thisRide, const Ride& otherRide) -> bool {
                        return thisRide.totalProfit <= otherRide.totalProfit;
                    });
                    break;
                case INFORMATION_TYPE_CUSTOMERS:
                    SortListByPredicate([](const Ride& thisRide, const Ride& otherRide) -> bool {
                        return RideCustomersPerHour(thisRide) <= RideCustomersPerHour(otherRide);
                    });
                    break;
                case INFORMATION_TYPE_AGE:
                    SortListByPredicate([](const Ride& thisRide, const Ride& otherRide) -> bool {
                        return thisRide.buildDate <= otherRide.buildDate;
                    });
                    break;
                case INFORMATION_TYPE_INCOME:
                    SortListByPredicate([](const Ride& thisRide, const Ride& otherRide) -> bool {
                        return thisRide.incomePerHour <= otherRide.incomePerHour;
                    });
                    break;
                case INFORMATION_TYPE_RUNNING_COST:
                    SortListByPredicate([](const Ride& thisRide, const Ride& otherRide) -> bool {
                        return thisRide.upkeepCost <= otherRide.upkeepCost;
                    });
                    break;
                case INFORMATION_TYPE_QUEUE_LENGTH:
                    SortListByPredicate([](const Ride& thisRide, const Ride& otherRide) -> bool {
                        return thisRide.getTotalQueueLength() <= otherRide.getTotalQueueLength();
                    });
                    break;
                case INFORMATION_TYPE_QUEUE_TIME:
                    SortListByPredicate([](const Ride& thisRide, const Ride& otherRide) -> bool {
                        return thisRide.getMaxQueueTime() <= otherRide.getMaxQueueTime();
                    });
                    break;
                case INFORMATION_TYPE_RELIABILITY:
                    SortListByPredicate([](const Ride& thisRide, const Ride& otherRide) -> bool {
                        return thisRide.reliabilityPercentage <= otherRide.reliabilityPercentage;
                    });
                    break;
                case INFORMATION_TYPE_DOWN_TIME:
                    SortListByPredicate([](const Ride& thisRide, const Ride& otherRide) -> bool {
                        return thisRide.downtime <= otherRide.downtime;
                    });
                    break;
                case INFORMATION_TYPE_LAST_INSPECTION:
                    SortListByPredicate([](const Ride& thisRide, const Ride& otherRide) -> bool {
                        return thisRide.lastInspection <= otherRide.lastInspection;
                    });
                    break;
                case INFORMATION_TYPE_GUESTS_FAVOURITE:
                    SortListByPredicate([](const Ride& thisRide, const Ride& otherRide) -> bool {
                        return thisRide.guestsFavourite <= otherRide.guestsFavourite;
                    });
                    break;
                case INFORMATION_TYPE_EXCITEMENT:
                    SortListByPredicate([](const Ride& thisRide, const Ride& otherRide) -> bool {
                        const auto leftValue = thisRide.ratings.isNull() ? kRideRatingUndefined : thisRide.ratings.excitement;
                        const auto rightValue = otherRide.ratings.isNull() ? kRideRatingUndefined
                                                                           : otherRide.ratings.excitement;
                        return leftValue <= rightValue;
                    });
                    break;
                case INFORMATION_TYPE_INTENSITY:
                    SortListByPredicate([](const Ride& thisRide, const Ride& otherRide) -> bool {
                        const auto leftValue = thisRide.ratings.isNull() ? kRideRatingUndefined : thisRide.ratings.intensity;
                        const auto rightValue = otherRide.ratings.isNull() ? kRideRatingUndefined : otherRide.ratings.intensity;
                        return leftValue <= rightValue;
                    });
                    break;
                case INFORMATION_TYPE_NAUSEA:
                    SortListByPredicate([](const Ride& thisRide, const Ride& otherRide) -> bool {
                        const auto leftValue = thisRide.ratings.isNull() ? kRideRatingUndefined : thisRide.ratings.nausea;
                        const auto rightValue = otherRide.ratings.isNull() ? kRideRatingUndefined : otherRide.ratings.nausea;
                        return leftValue <= rightValue;
                    });
                    break;
            }

            selected_list_item = -1;
            Invalidate();
        }

        // window_ride_list_close_all
        void CloseAllRides()
        {
            for (auto& rideRef : GetRideManager())
            {
                if (rideRef.status != RideStatus::closed
                    && rideRef.getClassification() == static_cast<RideClassification>(page))
                {
                    auto gameAction = RideSetStatusAction(rideRef.id, RideStatus::closed);
                    GameActions::Execute(&gameAction);
                }
            }
        }

        // window_ride_list_open_all
        void OpenAllRides()
        {
            for (auto& rideRef : GetRideManager())
            {
                if (rideRef.status != RideStatus::open && rideRef.getClassification() == static_cast<RideClassification>(page))
                {
                    auto gameAction = RideSetStatusAction(rideRef.id, RideStatus::open);
                    GameActions::Execute(&gameAction);
                }
            }
        }
    };

    /**
     *
     *  rct2: 0x006B30BC
     */
    WindowBase* RideListOpen()
    {
        // Check if window is already open
        auto* windowMgr = GetWindowManager();
        auto* window = windowMgr->BringToFrontByClass(WindowClass::RideList);
        if (window == nullptr)
        {
            window = windowMgr->Create<RideListWindow>(
                WindowClass::RideList, ScreenCoordsXY(32, 32), kWindowSize, WF_10 | WF_RESIZABLE);
        }
        return window;
    }

    void WindowRideListRefreshList(WindowBase* w)
    {
        dynamic_cast<RideListWindow*>(w)->RefreshListWrapper();
    }
} // namespace OpenRCT2::Ui::Windows
