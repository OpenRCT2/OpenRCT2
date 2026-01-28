/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include <iterator>
#include <openrct2-ui/interface/Dropdown.h>
#include <openrct2-ui/interface/Theme.h>
#include <openrct2-ui/interface/Widget.h>
#include <openrct2-ui/windows/Windows.h>
#include <openrct2/Context.h>
#include <openrct2/Game.h>
#include <openrct2/GameState.h>
#include <openrct2/SpriteIds.h>
#include <openrct2/actions/RideDemolishAction.h>
#include <openrct2/actions/RideSetStatusAction.h>
#include <openrct2/core/String.hpp>
#include <openrct2/drawing/ColourMap.h>
#include <openrct2/drawing/Drawing.h>
#include <openrct2/drawing/Rectangle.h>
#include <openrct2/localisation/Formatter.h>
#include <openrct2/network/Network.h>
#include <openrct2/ride/RideData.h>
#include <openrct2/ride/RideManager.hpp>
#include <openrct2/ride/RideRatings.h>
#include <openrct2/ui/WindowManager.h>
#include <openrct2/windows/Intent.h>
#include <openrct2/world/Park.h>

using namespace OpenRCT2::Drawing;

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
        WIDX_TAB_1,
        WIDX_TAB_2,
        WIDX_TAB_3,
        WIDX_SEARCH_TEXT_BOX,
        WIDX_SEARCH_CLEAR_BUTTON,
        WIDX_HEADER_NAME,
        WIDX_HEADER_OTHER,
        WIDX_HEADER_CUSTOMISE,
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
        makeTab   ({  3, 17},                                                                STR_LIST_RIDES_TIP                                                    ), // tab 1
        makeTab   ({ 34, 17},                                                                STR_LIST_SHOPS_AND_STALLS_TIP                                         ), // tab 2
        makeTab   ({ 65, 17},                                                                STR_LIST_KIOSKS_AND_FACILITIES_TIP                                    ), // tab 3
        makeWidget({  3, 47}, {264,  14}, WidgetType::textBox,      WindowColour::secondary                                                                        ), // search text box
        makeWidget({264, 47}, { 70,  14}, WidgetType::button,       WindowColour::secondary, STR_OBJECT_SEARCH_CLEAR                                               ), // search clear button
        makeWidget({  3, 62}, {150,  14}, WidgetType::tableHeader,  WindowColour::secondary                                                                        ), // name header
        makeWidget({153, 62}, {147,  14}, WidgetType::tableHeader,  WindowColour::secondary                                                                        ), // other header
        makeWidget({300, 62}, { 14,  14}, WidgetType::button,       WindowColour::secondary, STR_DROPDOWN_GLYPH                                                    ), // customise button
        makeWidget({  3, 75}, {334, 177}, WidgetType::scroll,       WindowColour::secondary, SCROLL_VERTICAL                                                       ), // list
        makeWidget({320, 77}, { 14,  14}, WidgetType::imgBtn,       WindowColour::secondary, ImageId(SPR_G2_RCT1_CLOSE_BUTTON_0)                                   ),
        makeWidget({320, 91}, { 14,  14}, WidgetType::imgBtn,       WindowColour::secondary, ImageId(SPR_G2_RCT1_OPEN_BUTTON_0)                                    ),
        makeWidget({315,105}, { 24,  24}, WidgetType::flatBtn,      WindowColour::secondary, ImageId(SPR_DEMOLISH),              STR_QUICK_DEMOLISH_RIDE           )
    );
    // clang-format on

    enum InformationType
    {
        INFORMATION_TYPE_STATUS,
        INFORMATION_TYPE_RIDETYPE,
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
        STR_RIDE_LIST_RIDETYPE,
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
        false, // Ride Type
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
        InformationType _windowRideListInformationType = INFORMATION_TYPE_STATUS;
        bool _windowListSortDescending = false;
        u8string _searchFilter;

        struct RideListEntry
        {
            RideId Id;
            u8string Name;
            bool Visible;
        };
        std::vector<RideListEntry> _rideList;

    public:
        void onOpen() override
        {
            setWidgets(_rideListWidgets);
            WindowInitScrollWidgets(*this);
            WindowSetResize(*this, kWindowSize, kWindowSize * 2);

            page = PAGE_RIDES;
            currentFrame = 0;

            listInformationType = INFORMATION_TYPE_STATUS;
            RefreshList();

            _windowRideListInformationType = INFORMATION_TYPE_STATUS;
            _quickDemolishMode = false;
            _searchFilter.clear();
        }

        /**
         *
         *  rct2: 0x006B38A7
         */
        void onResize() override
        {
            if (width < minWidth)
            {
                invalidate();
                width = minWidth;
            }
            if (height < minHeight)
            {
                invalidate();
                height = minHeight;
            }
        }

        /**
         *
         *  rct2: 0x006B3511
         */
        void onMouseUp(WidgetIndex widgetIndex) override
        {
            switch (widgetIndex)
            {
                case WIDX_CLOSE:
                    close();
                    break;
                case WIDX_HEADER_NAME:
                    if (listInformationType != INFORMATION_TYPE_STATUS)
                    {
                        listInformationType = INFORMATION_TYPE_STATUS;
                        _windowListSortDescending = false;
                    }
                    else
                    {
                        _windowListSortDescending ^= true;
                    }
                    SortList();
                    break;
                case WIDX_HEADER_OTHER:
                    if (listInformationType != _windowRideListInformationType)
                    {
                        listInformationType = _windowRideListInformationType;
                        _windowListSortDescending = true;
                    }
                    else
                    {
                        _windowListSortDescending ^= true;
                    }
                    SortList();
                    break;
                case WIDX_TAB_1:
                case WIDX_TAB_2:
                case WIDX_TAB_3:
                    if (page != widgetIndex - WIDX_TAB_1)
                    {
                        page = widgetIndex - WIDX_TAB_1;
                        currentFrame = 0;
                        if (page != PAGE_RIDES && _windowRideListInformationType > INFORMATION_TYPE_RUNNING_COST)
                        {
                            _windowRideListInformationType = INFORMATION_TYPE_STATUS;
                        }
                        RefreshList();
                    }
                    break;
                case WIDX_SEARCH_TEXT_BOX:
                    WindowStartTextbox(*this, widgetIndex, _searchFilter, kTextInputSize);
                    break;
                case WIDX_SEARCH_CLEAR_BUTTON:
                    _searchFilter.clear();
                    ApplySearchQuery();
                    invalidateWidget(WIDX_SEARCH_TEXT_BOX);
                    break;
                case WIDX_CLOSE_LIGHT:
                    CloseAllRides();
                    break;
                case WIDX_OPEN_LIGHT:
                    OpenAllRides();
                    break;
                case WIDX_QUICK_DEMOLISH:
                    if (Network::GetMode() != Network::Mode::client)
                    {
                        _quickDemolishMode = !_quickDemolishMode;
                    }
                    else
                    {
                        _quickDemolishMode = false;
                    }
                    invalidate();
                    break;
            }
        }

        /**
         *
         *  rct2: 0x006B3532
         */
        void onMouseDown(WidgetIndex widgetIndex) override
        {
            if (widgetIndex == WIDX_OPEN_CLOSE_ALL)
            {
                const auto& widget = widgets[widgetIndex];
                gDropdown.items[0] = Dropdown::PlainMenuLabel(STR_CLOSE_ALL);
                gDropdown.items[1] = Dropdown::PlainMenuLabel(STR_OPEN_ALL);
                WindowDropdownShowText(
                    { windowPos.x + widget.left, windowPos.y + widget.top }, widget.height() - 1, colours[1], 0, 2);
            }
            else if (widgetIndex == WIDX_HEADER_CUSTOMISE)
            {
                int32_t lastType;
                if (page == PAGE_RIDES)
                    lastType = INFORMATION_TYPE_NAUSEA;
                else
                    lastType = INFORMATION_TYPE_RUNNING_COST;

                int32_t numItems = 0;
                for (int32_t type = INFORMATION_TYPE_STATUS; type <= lastType; type++)
                {
                    if ((getGameState().park.flags & PARK_FLAGS_NO_MONEY))
                    {
                        if (ride_info_type_money_mapping[type])
                        {
                            continue;
                        }
                    }

                    gDropdown.items[numItems] = Dropdown::MenuLabel(ride_info_type_string_mapping[type]);
                    gDropdown.items[numItems].value = type;
                    if (type == _windowRideListInformationType)
                    {
                        gDropdown.items[numItems].setChecked(true);
                    }
                    numItems++;
                }

                const auto& headerWidget = widgets[WIDX_HEADER_OTHER];
                const auto& customWidget = widgets[WIDX_HEADER_CUSTOMISE];
                auto totalWidth = headerWidget.width() - 1 + customWidget.width() - 1;

                WindowDropdownShowTextCustomWidth(
                    { windowPos.x + headerWidget.left, windowPos.y + headerWidget.top }, headerWidget.height() - 1, colours[1],
                    0, Dropdown::Flag::StayOpen, numItems, totalWidth);
            }
        }

        /**
         *
         *  rct2: 0x006B3547
         */
        void onDropdown(WidgetIndex widgetIndex, int32_t dropdownIndex) override
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

                invalidate();
            }
            else if (widgetIndex == WIDX_HEADER_CUSTOMISE)
            {
                if (dropdownIndex == -1)
                    return;

                int32_t informationType = INFORMATION_TYPE_STATUS;
                auto selectedValue = gDropdown.items[dropdownIndex].value;
                if (selectedValue < std::size(ride_info_type_string_mapping))
                    informationType = selectedValue;

                _windowRideListInformationType = InformationType(informationType);
                invalidate();

                // Automatically change sort if we're sorting by the custom/info column
                if (listInformationType != INFORMATION_TYPE_STATUS)
                {
                    listInformationType = _windowRideListInformationType;
                    SortList();
                }
            }
        }

        /**
         *
         *  rct2: 0x006B386B
         */
        void onUpdate() override
        {
            currentFrame = (currentFrame + 1) % 64;
            invalidateWidget(WIDX_TAB_1 + page);
            if (_windowRideListInformationType != INFORMATION_TYPE_STATUS)
                invalidate();
        }

        void onPeriodicUpdate() override
        {
            SortList();
        }

        void onTextInput(WidgetIndex widgetIndex, std::string_view text) override
        {
            if (widgetIndex != WIDX_SEARCH_TEXT_BOX)
                return;

            if (text == _searchFilter)
                return;

            _searchFilter.assign(text);
            ApplySearchQuery();
        }

        /**
         *
         *  rct2: 0x006B35A1
         */
        ScreenSize onScrollGetSize(int32_t scrollIndex) override
        {
            const auto newHeight = static_cast<int32_t>(CountVisibleItems() * kScrollableRowHeight);
            auto top = std::max(0, newHeight - widgets[WIDX_LIST].bottom + widgets[WIDX_LIST].top + 21);
            if (top < scrolls[0].contentOffsetY)
            {
                scrolls[0].contentOffsetY = top;
                invalidate();
            }

            return { 0, newHeight };
        }

        int32_t GetNthVisibleItemIndex(int32_t target) const
        {
            auto j = 0;
            for (auto i = 0u; i < _rideList.size(); i++)
            {
                if (!_rideList[i].Visible)
                    continue;

                if (j == target)
                    return i;

                j++;
            }
            return -1;
        }

        /**
         *
         *  rct2: 0x006B361F
         */
        void onScrollMouseDown(int32_t scrollIndex, const ScreenCoordsXY& screenCoords) override
        {
            const auto index = GetNthVisibleItemIndex(screenCoords.y / kScrollableRowHeight);
            if (index < 0)
                return;

            // Open ride window
            const auto selectedRideId = _rideList[index].Id;
            if (_quickDemolishMode && Network::GetMode() != Network::Mode::client)
            {
                auto gameAction = GameActions::RideDemolishAction(selectedRideId, GameActions::RideModifyType::demolish);
                GameActions::Execute(&gameAction, getGameState());
                RefreshList();
            }
            else
            {
                auto intent = Intent(WindowClass::ride);
                intent.PutExtra(INTENT_EXTRA_RIDE_ID, selectedRideId.ToUnderlying());
                ContextOpenIntent(&intent);
            }
        }

        /**
         *
         *  rct2: 0x006B35EF
         */
        void onScrollMouseOver(int32_t scrollIndex, const ScreenCoordsXY& screenCoords) override
        {
            const auto index = GetNthVisibleItemIndex(screenCoords.y / kScrollableRowHeight);
            if (index < 0)
                return;

            selectedListItem = index;
            invalidate();
        }

        /**
         *
         *  rct2: 0x006B3182
         */
        void onPrepareDraw() override
        {
            auto ft = Formatter();
            ft.Add<StringId>(STR_UP);

            // Set correct active tab
            for (int32_t i = 0; i < 3; i++)
                pressedWidgets &= ~(1 << (WIDX_TAB_1 + i));
            pressedWidgets |= 1LL << (WIDX_TAB_1 + page);

            widgets[WIDX_TITLE].text = page_names[page];

            if (_quickDemolishMode)
                pressedWidgets |= (1uLL << WIDX_QUICK_DEMOLISH);
            else
                pressedWidgets &= ~(1uLL << WIDX_QUICK_DEMOLISH);

            widgets[WIDX_LIST].right = width - 26;
            widgets[WIDX_LIST].bottom = height - 15;

            widgets[WIDX_SEARCH_CLEAR_BUTTON].right = widgets[WIDX_LIST].right - 1;
            widgets[WIDX_SEARCH_CLEAR_BUTTON].left = widgets[WIDX_SEARCH_CLEAR_BUTTON].right - 70;
            widgets[WIDX_SEARCH_TEXT_BOX].right = widgets[WIDX_SEARCH_CLEAR_BUTTON].left - 2;
            widgets[WIDX_SEARCH_TEXT_BOX].string = _searchFilter.data();

            widgets[WIDX_HEADER_CUSTOMISE].right = widgets[WIDX_LIST].right - 1;
            widgets[WIDX_HEADER_CUSTOMISE].left = widgets[WIDX_HEADER_CUSTOMISE].right - 14;

            auto columnWidth = (widgets[WIDX_LIST].width() - 1 - widgets[WIDX_HEADER_CUSTOMISE].width() - 1) / 2;

            widgets[WIDX_HEADER_OTHER].right = widgets[WIDX_HEADER_CUSTOMISE].left - 1;
            widgets[WIDX_HEADER_OTHER].left = widgets[WIDX_HEADER_OTHER].right - columnWidth + 1;

            widgets[WIDX_HEADER_NAME].right = widgets[WIDX_HEADER_NAME].left + columnWidth - 1;

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

                const auto& gameState = getGameState();
                const auto& rideManager = RideManager(gameState);
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
            widgets[WIDX_QUICK_DEMOLISH].type = Network::GetMode() != Network::Mode::client ? WidgetType::flatBtn
                                                                                            : WidgetType::empty;
        }

        /**
         *
         *  rct2: 0x006B3235
         */
        void onDraw(RenderTarget& rt) override
        {
            WindowDrawWidgets(*this, rt);
            DrawTabImages(rt);

            const auto drawButtonCaption = [rt, this](Widget& widget, StringId strId, InformationType sortType) {
                StringId indicatorId = kStringIdNone;
                if (listInformationType == sortType && !(strId == STR_STATUS && sortType == INFORMATION_TYPE_STATUS))
                    indicatorId = _windowListSortDescending ? STR_DOWN : STR_UP;

                auto ft = Formatter();
                ft.Add<StringId>(strId);
                ft.Add<StringId>(indicatorId);

                auto cRT = const_cast<const RenderTarget&>(rt);
                DrawTextEllipsised(
                    cRT, windowPos + ScreenCoordsXY{ widget.left + 1, widget.top + 1 }, widget.width() - 1,
                    STR_RIDE_LIST_HEADER_FORMAT, ft, { colours[1] });
            };

            drawButtonCaption(widgets[WIDX_HEADER_NAME], STR_RIDE_LIST_NAME, INFORMATION_TYPE_STATUS);

            auto otherStringId = ride_info_type_string_mapping[_windowRideListInformationType];
            drawButtonCaption(widgets[WIDX_HEADER_OTHER], otherStringId, _windowRideListInformationType);

            // Draw number of attractions on bottom
            auto ft = Formatter();
            ft.Add<uint16_t>(static_cast<uint16_t>(CountVisibleItems()));
            DrawTextBasic(
                rt, windowPos + ScreenCoordsXY{ 4, widgets[WIDX_LIST].bottom + 2 }, ride_list_statusbar_count_strings[page],
                ft);
        }

        /**
         *
         *  rct2: 0x006B3240
         */
        void onScrollDraw(int32_t scrollIndex, RenderTarget& rt) override
        {
            auto rtCoords = ScreenCoordsXY{ rt.x, rt.y };
            Rectangle::fill(
                rt, { rtCoords, rtCoords + ScreenCoordsXY{ rt.width, rt.height } }, getColourMap(colours[1].colour).midLight);

            auto y = 0;
            for (size_t i = 0; i < _rideList.size(); i++)
            {
                if (!_rideList[i].Visible)
                    continue;

                StringId format = STR_BLACK_STRING;
                if (_quickDemolishMode)
                    format = STR_RED_STRINGID;

                if (i == static_cast<size_t>(selectedListItem))
                {
                    // Background highlight
                    Rectangle::filter(rt, { 0, y, 800, y + kScrollableRowHeight - 1 }, FilterPaletteID::paletteDarken1);
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

                auto& nameHeader = widgets[WIDX_HEADER_NAME];
                DrawTextEllipsised(rt, { 0, y - 1 }, nameHeader.width() - 3, format, ft);

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
                    case INFORMATION_TYPE_RIDETYPE:
                    {
                        const auto rideTypeName = ridePtr->getTypeNaming().Name;
                        formatSecondary = STR_STRINGID;
                        ft.Add<StringId>(rideTypeName);
                        break;
                    }
                    case INFORMATION_TYPE_NAUSEA:
                        formatSecondary = STR_RATING_UKNOWN_LABEL;
                        if (RideHasRatings(*ridePtr))
                        {
                            formatSecondary = STR_NAUSEA_LABEL;
                            ft.Add<uint16_t>(ridePtr->ratings.nausea);
                        }
                        break;
                    default:
                        break;
                }

                if (formatSecondaryEnabled)
                {
                    ft.Rewind();
                    ft.Add<StringId>(formatSecondary);
                }

                auto infoHeader = widgets[WIDX_HEADER_OTHER];
                DrawTextEllipsised(rt, { infoHeader.left - 4, y - 1 }, infoHeader.width() - 3, format, ft);
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
                sprite_idx += currentFrame / 4;
            GfxDrawSprite(
                rt, ImageId(sprite_idx), windowPos + ScreenCoordsXY{ widgets[WIDX_TAB_1].left, widgets[WIDX_TAB_1].top });

            // Shops and stalls tab
            sprite_idx = SPR_TAB_SHOPS_AND_STALLS_0;
            if (page == PAGE_SHOPS_AND_STALLS)
                sprite_idx += currentFrame / 4;
            GfxDrawSprite(
                rt, ImageId(sprite_idx), windowPos + ScreenCoordsXY{ widgets[WIDX_TAB_2].left, widgets[WIDX_TAB_2].top });

            // Information kiosks and facilities tab
            sprite_idx = SPR_TAB_KIOSKS_AND_FACILITIES_0;
            if (page == PAGE_KIOSKS_AND_FACILITIES)
                sprite_idx += (currentFrame / 4) % 8;
            GfxDrawSprite(
                rt, ImageId(sprite_idx), windowPos + ScreenCoordsXY{ widgets[WIDX_TAB_3].left, widgets[WIDX_TAB_3].top });
        }

        /**
         * Used in SortList() to handle the sorting of the list.
         * Uses a lambda function (predicate) as exit criteria for the algorithm.
         */
        template<typename TSortPred>
        void SortListByPredicate(const TSortPred& pred)
        {
            bool desc = _windowListSortDescending;
            std::stable_sort(_rideList.begin(), _rideList.end(), [desc, pred](const auto& lhs, const auto& rhs) {
                const Ride* rideLhs = GetRide(lhs.Id);
                const Ride* rideRhs = GetRide(rhs.Id);
                if (rideLhs == nullptr || rideRhs == nullptr)
                {
                    return rideLhs != nullptr;
                }
                bool result = pred(*rideLhs, *rideRhs);
                return desc ? !result : result;
            });
        }

        void SortListByName()
        {
            bool desc = _windowListSortDescending;
            std::stable_sort(_rideList.begin(), _rideList.end(), [desc](const auto& lhs, const auto& rhs) {
                bool result = (0 > String::logicalCmp(lhs.Name.c_str(), rhs.Name.c_str()));
                return desc ? !result : result;
            });
        }

        bool IsFiltered(const u8string& rideName)
        {
            if (_searchFilter.empty())
                return true;

            // Check if the filter matches the ride name
            return String::contains(u8string_view(rideName), _searchFilter, true);
        }

        void ApplySearchQuery()
        {
            for (auto& entry : _rideList)
            {
                entry.Visible = IsFiltered(entry.Name);
            }
        }

        size_t CountVisibleItems() const
        {
            return std::count_if(_rideList.begin(), _rideList.end(), [](const RideListEntry& entry) { return entry.Visible; });
        }

        /**
         *
         *  rct2: 0x006B39A8
         */
        void RefreshList()
        {
            _rideList.clear();
            const auto& gameState = getGameState();
            for (auto& rideRef : RideManager(gameState))
            {
                if (rideRef.getClassification() != static_cast<RideClassification>(page)
                    || (rideRef.status == RideStatus::closed && !RideHasAnyTrackElements(rideRef)))
                {
                    continue;
                }

                // Get the ride name once and use it for both filtering and storage
                auto rideName = rideRef.getName();

                if (rideRef.windowInvalidateFlags.has(RideInvalidateFlag::list))
                {
                    rideRef.windowInvalidateFlags.unset(RideInvalidateFlag::list);
                }

                const auto filterApplies = IsFiltered(rideName);

                RideListEntry entry{
                    .Id = rideRef.id,
                    .Name = std::move(rideName),
                    .Visible = filterApplies,
                };

                _rideList.push_back(std::move(entry));
            }

            selectedListItem = -1;
            SortList();
        }

        void SortList()
        {
            // Maintain stability by first sorting by ride id.
            SortListByPredicate([](const Ride& thisRide, const Ride& otherRide) -> bool {
                return thisRide.id.ToUnderlying() < otherRide.id.ToUnderlying();
            });

            switch (listInformationType)
            {
                case INFORMATION_TYPE_STATUS:
                    SortListByName();
                    break;
                case INFORMATION_TYPE_POPULARITY:
                    SortListByPredicate([](const Ride& thisRide, const Ride& otherRide) -> bool {
                        return static_cast<int8_t>(thisRide.popularity) < static_cast<int8_t>(otherRide.popularity);
                    });
                    break;
                case INFORMATION_TYPE_SATISFACTION:
                    SortListByPredicate([](const Ride& thisRide, const Ride& otherRide) -> bool {
                        return static_cast<int8_t>(thisRide.satisfaction) < static_cast<int8_t>(otherRide.satisfaction);
                    });
                    break;
                case INFORMATION_TYPE_PROFIT:
                    SortListByPredicate(
                        [](const Ride& thisRide, const Ride& otherRide) -> bool { return thisRide.profit < otherRide.profit; });
                    break;
                case INFORMATION_TYPE_TOTAL_CUSTOMERS:
                    SortListByPredicate([](const Ride& thisRide, const Ride& otherRide) -> bool {
                        return thisRide.totalCustomers < otherRide.totalCustomers;
                    });
                    break;
                case INFORMATION_TYPE_TOTAL_PROFIT:
                    SortListByPredicate([](const Ride& thisRide, const Ride& otherRide) -> bool {
                        return thisRide.totalProfit < otherRide.totalProfit;
                    });
                    break;
                case INFORMATION_TYPE_CUSTOMERS:
                    SortListByPredicate([](const Ride& thisRide, const Ride& otherRide) -> bool {
                        return RideCustomersPerHour(thisRide) < RideCustomersPerHour(otherRide);
                    });
                    break;
                case INFORMATION_TYPE_AGE:
                    SortListByPredicate([](const Ride& thisRide, const Ride& otherRide) -> bool {
                        return thisRide.buildDate < otherRide.buildDate;
                    });
                    break;
                case INFORMATION_TYPE_INCOME:
                    SortListByPredicate([](const Ride& thisRide, const Ride& otherRide) -> bool {
                        return thisRide.incomePerHour < otherRide.incomePerHour;
                    });
                    break;
                case INFORMATION_TYPE_RUNNING_COST:
                    SortListByPredicate([](const Ride& thisRide, const Ride& otherRide) -> bool {
                        return thisRide.upkeepCost < otherRide.upkeepCost;
                    });
                    break;
                case INFORMATION_TYPE_QUEUE_LENGTH:
                    SortListByPredicate([](const Ride& thisRide, const Ride& otherRide) -> bool {
                        return thisRide.getTotalQueueLength() < otherRide.getTotalQueueLength();
                    });
                    break;
                case INFORMATION_TYPE_QUEUE_TIME:
                    SortListByPredicate([](const Ride& thisRide, const Ride& otherRide) -> bool {
                        return thisRide.getMaxQueueTime() < otherRide.getMaxQueueTime();
                    });
                    break;
                case INFORMATION_TYPE_RELIABILITY:
                    SortListByPredicate([](const Ride& thisRide, const Ride& otherRide) -> bool {
                        return thisRide.reliabilityPercentage < otherRide.reliabilityPercentage;
                    });
                    break;
                case INFORMATION_TYPE_DOWN_TIME:
                    SortListByPredicate([](const Ride& thisRide, const Ride& otherRide) -> bool {
                        return thisRide.downtime < otherRide.downtime;
                    });
                    break;
                case INFORMATION_TYPE_LAST_INSPECTION:
                    SortListByPredicate([](const Ride& thisRide, const Ride& otherRide) -> bool {
                        return thisRide.lastInspection < otherRide.lastInspection;
                    });
                    break;
                case INFORMATION_TYPE_GUESTS_FAVOURITE:
                    SortListByPredicate([](const Ride& thisRide, const Ride& otherRide) -> bool {
                        return thisRide.guestsFavourite < otherRide.guestsFavourite;
                    });
                    break;
                case INFORMATION_TYPE_EXCITEMENT:
                    SortListByPredicate([](const Ride& thisRide, const Ride& otherRide) -> bool {
                        const auto leftValue = thisRide.ratings.isNull() ? RideRating::kUndefined : thisRide.ratings.excitement;
                        const auto rightValue = otherRide.ratings.isNull() ? RideRating::kUndefined
                                                                           : otherRide.ratings.excitement;
                        return leftValue < rightValue;
                    });
                    break;
                case INFORMATION_TYPE_INTENSITY:
                    SortListByPredicate([](const Ride& thisRide, const Ride& otherRide) -> bool {
                        const auto leftValue = thisRide.ratings.isNull() ? RideRating::kUndefined : thisRide.ratings.intensity;
                        const auto rightValue = otherRide.ratings.isNull() ? RideRating::kUndefined
                                                                           : otherRide.ratings.intensity;
                        return leftValue < rightValue;
                    });
                    break;
                case INFORMATION_TYPE_RIDETYPE:
                    SortListByPredicate([](const Ride& thisRide, const Ride& otherRide) -> bool {
                        auto* ridePtrLhs = GetRide(thisRide.id);
                        auto* ridePtrRhs = GetRide(otherRide.id);
                        if (!ridePtrLhs || !ridePtrRhs)
                            return ridePtrLhs != nullptr;

                        auto rideTypeNameLhs = LanguageGetString(ridePtrLhs->getTypeNaming().Name);
                        auto rideTypeNameRhs = LanguageGetString(ridePtrRhs->getTypeNaming().Name);

                        return String::logicalCmp(rideTypeNameLhs, rideTypeNameRhs) > 0;
                    });
                    break;
                case INFORMATION_TYPE_NAUSEA:
                    SortListByPredicate([](const Ride& thisRide, const Ride& otherRide) -> bool {
                        const auto leftValue = thisRide.ratings.isNull() ? RideRating::kUndefined : thisRide.ratings.nausea;
                        const auto rightValue = otherRide.ratings.isNull() ? RideRating::kUndefined : otherRide.ratings.nausea;
                        return leftValue < rightValue;
                    });
                    break;
            }

            selectedListItem = -1;
            invalidate();
        }

        // window_ride_list_close_all
        void CloseAllRides()
        {
            const auto& gameState = getGameState();
            for (auto& rideRef : RideManager(gameState))
            {
                if (rideRef.status != RideStatus::closed
                    && rideRef.getClassification() == static_cast<RideClassification>(page))
                {
                    auto gameAction = GameActions::RideSetStatusAction(rideRef.id, RideStatus::closed);
                    GameActions::Execute(&gameAction, getGameState());
                }
            }
        }

        // window_ride_list_open_all
        void OpenAllRides()
        {
            const auto& gameState = getGameState();
            for (auto& rideRef : RideManager(gameState))
            {
                if (rideRef.status != RideStatus::open && rideRef.getClassification() == static_cast<RideClassification>(page))
                {
                    auto gameAction = GameActions::RideSetStatusAction(rideRef.id, RideStatus::open);
                    GameActions::Execute(&gameAction, getGameState());
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
        auto* window = windowMgr->BringToFrontByClass(WindowClass::rideList);
        if (window == nullptr)
        {
            window = windowMgr->Create<RideListWindow>(
                WindowClass::rideList, ScreenCoordsXY(32, 32), kWindowSize,
                { WindowFlag::higherContrastOnPress, WindowFlag::resizable });
        }
        return window;
    }

    void WindowRideListRefreshList(WindowBase* w)
    {
        dynamic_cast<RideListWindow*>(w)->RefreshListWrapper();
    }
} // namespace OpenRCT2::Ui::Windows
