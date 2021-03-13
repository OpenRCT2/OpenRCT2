/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include <algorithm>
#include <openrct2-ui/interface/Dropdown.h>
#include <openrct2-ui/interface/Widget.h>
#include <openrct2-ui/windows/Window.h>
#include <openrct2/Game.h>
#include <openrct2/actions/ParkMarketingAction.h>
#include <openrct2/config/Config.h>
#include <openrct2/drawing/Drawing.h>
#include <openrct2/localisation/Localisation.h>
#include <openrct2/ride/Ride.h>
#include <openrct2/ride/RideData.h>
#include <openrct2/ride/ShopItem.h>

static constexpr const rct_string_id WINDOW_TITLE = STR_NONE;
static constexpr const int32_t WH = 109;
static constexpr const int32_t WW = 350;
constexpr uint16_t SELECTED_RIDE_UNDEFINED = 0xFFFF;

// clang-format off
enum WINDOW_NEW_CAMPAIGN_WIDGET_IDX {
    WIDX_BACKGROUND,
    WIDX_TITLE,
    WIDX_CLOSE,
    WIDX_RIDE_LABEL,
    WIDX_RIDE_DROPDOWN,
    WIDX_RIDE_DROPDOWN_BUTTON,
    WIDX_WEEKS_LABEL,
    WIDX_WEEKS_SPINNER,
    WIDX_WEEKS_INCREASE_BUTTON,
    WIDX_WEEKS_DECREASE_BUTTON,
    WIDX_START_BUTTON
};

static rct_widget window_new_campaign_widgets[] = {
    WINDOW_SHIM(WINDOW_TITLE, WW, WH),
    MakeWidget        ({ 14, 24}, {126, 12}, WindowWidgetType::Label,    WindowColour::Primary, STR_EMPTY                                  ), // ride label
    MakeWidget        ({100, 24}, {242, 12}, WindowWidgetType::DropdownMenu, WindowColour::Primary, STR_EMPTY                                  ), // ride dropdown
    MakeWidget        ({330, 25}, { 11, 10}, WindowWidgetType::Button,   WindowColour::Primary, STR_DROPDOWN_GLYPH                         ), // ride dropdown button
    MakeWidget        ({ 14, 41}, {126, 14}, WindowWidgetType::Label,    WindowColour::Primary, STR_LENGTH_OF_TIME                         ), // weeks label
    MakeSpinnerWidgets({120, 41}, {100, 14}, WindowWidgetType::Spinner,  WindowColour::Primary, STR_EMPTY                                  ), // weeks (3 widgets)
    MakeWidget        ({ 14, 89}, {322, 14}, WindowWidgetType::Button,   WindowColour::Primary, STR_MARKETING_START_THIS_MARKETING_CAMPAIGN), // start button
    { WIDGETS_END }
};


static void window_new_campaign_mouseup(rct_window *w, rct_widgetindex widgetIndex);
static void window_new_campaign_mousedown(rct_window *w, rct_widgetindex widgetIndex, rct_widget* widget);
static void window_new_campaign_dropdown(rct_window *w, rct_widgetindex widgetIndex, int32_t dropdownIndex);
static void window_new_campaign_invalidate(rct_window *w);
static void window_new_campaign_paint(rct_window *w, rct_drawpixelinfo *dpi);

static rct_window_event_list window_new_campaign_events([](auto& events)
{
    events.mouse_up = &window_new_campaign_mouseup;
    events.mouse_down = &window_new_campaign_mousedown;
    events.dropdown = &window_new_campaign_dropdown;
    events.invalidate = &window_new_campaign_invalidate;
    events.paint = &window_new_campaign_paint;
});
// clang-format on

static std::vector<ride_id_t> window_new_campaign_rides;
static uint8_t window_new_campaign_shop_items[64];

static int32_t ride_value_compare(const void* a, const void* b)
{
    auto valueA = 0;
    auto rideA = get_ride(*(static_cast<const uint8_t*>(a)));
    if (rideA != nullptr)
        valueA = rideA->value;

    auto valueB = 0;
    auto rideB = get_ride(*(static_cast<const uint8_t*>(b)));
    if (rideB != nullptr)
        valueB = rideB->value;

    return valueB - valueA;
}

static int32_t ride_name_compare(const void* a, const void* b)
{
    std::string rideAName;
    auto rideA = get_ride(*(static_cast<const uint8_t*>(a)));
    if (rideA != nullptr)
        rideAName = rideA->GetName();

    std::string rideBName;
    auto rideB = get_ride(*(static_cast<const uint8_t*>(b)));
    if (rideB != nullptr)
        rideBName = rideB->GetName();

    return _strcmpi(rideAName.c_str(), rideBName.c_str());
}

/**
 *
 *  rct2: 0x0069E16F
 */
rct_window* window_new_campaign_open(int16_t campaignType)
{
    auto w = window_bring_to_front_by_class(WC_NEW_CAMPAIGN);
    if (w != nullptr)
    {
        if (w->campaign.campaign_type == campaignType)
            return w;

        window_close(w);
    }

    w = WindowCreateAutoPos(WW, WH, &window_new_campaign_events, WC_NEW_CAMPAIGN, 0);
    w->widgets = window_new_campaign_widgets;
    w->enabled_widgets = (1 << WIDX_CLOSE) | (1 << WIDX_RIDE_DROPDOWN) | (1 << WIDX_RIDE_DROPDOWN_BUTTON)
        | (1 << WIDX_WEEKS_INCREASE_BUTTON) | (1 << WIDX_WEEKS_DECREASE_BUTTON) | (1 << WIDX_START_BUTTON);
    w->hold_down_widgets = (1 << WIDX_WEEKS_INCREASE_BUTTON) | (1 << WIDX_WEEKS_DECREASE_BUTTON);
    WindowInitScrollWidgets(w);

    window_new_campaign_widgets[WIDX_TITLE].text = MarketingCampaignNames[campaignType][0];

    // Campaign type
    w->campaign.campaign_type = campaignType;

    // Number of weeks
    w->campaign.no_weeks = 2;

    // Currently selected ride
    w->campaign.RideId = SELECTED_RIDE_UNDEFINED;

    WindowCampaignRefreshRides();
    return w;
}

/**
 *
 *  rct2: 0x0069E320
 */
static void window_new_campaign_get_shop_items()
{
    uint64_t items = 0;
    for (auto& ride : GetRideManager())
    {
        auto rideEntry = ride.GetRideEntry();
        if (rideEntry != nullptr)
        {
            for (const auto itemType : rideEntry->shop_item)
            {
                if (itemType != ShopItem::None && GetShopItemDescriptor(itemType).IsFoodOrDrink())
                {
                    items |= EnumToFlag(itemType);
                }
            }
        }
    }

    //
    auto numItems = 0;
    for (auto i = 0; i < 64; i++)
    {
        if (items & (1ULL << i))
        {
            window_new_campaign_shop_items[numItems++] = i;
        }
    }
    window_new_campaign_shop_items[numItems] = 255;
}

/**
 *
 *  rct2: 0x0069E50B
 */
static void window_new_campaign_mouseup(rct_window* w, rct_widgetindex widgetIndex)
{
    switch (widgetIndex)
    {
        case WIDX_CLOSE:
            window_close(w);
            break;
        case WIDX_START_BUTTON:
        {
            auto gameAction = ParkMarketingAction(w->campaign.campaign_type, w->campaign.RideId, w->campaign.no_weeks);
            gameAction.SetCallback([](const GameAction* ga, const GameActions::Result* result) {
                if (result->Error == GameActions::Status::Ok)
                {
                    window_close_by_class(WC_NEW_CAMPAIGN);
                }
            });
            GameActions::Execute(&gameAction);
            break;
        }
    }
}

/**
 *
 *  rct2: 0x0069E51C
 */
static void window_new_campaign_mousedown(rct_window* w, rct_widgetindex widgetIndex, rct_widget* widget)
{
    rct_widget* dropdownWidget;

    switch (widgetIndex)
    {
        case WIDX_RIDE_DROPDOWN_BUTTON:
            dropdownWidget = widget - 1;

            if (w->campaign.campaign_type == ADVERTISING_CAMPAIGN_FOOD_OR_DRINK_FREE)
            {
                window_new_campaign_get_shop_items();
                if (window_new_campaign_shop_items[0] != 255)
                {
                    int32_t numItems = 0;
                    for (int32_t i = 0; i < Dropdown::ItemsMaxSize; i++)
                    {
                        if (window_new_campaign_shop_items[i] == 255)
                            break;

                        gDropdownItemsFormat[i] = STR_DROPDOWN_MENU_LABEL;
                        gDropdownItemsArgs[i] = GetShopItemDescriptor(ShopItem(window_new_campaign_shop_items[i]))
                                                    .Naming.Plural;
                        numItems++;
                    }

                    WindowDropdownShowTextCustomWidth(
                        { w->windowPos.x + dropdownWidget->left, w->windowPos.y + dropdownWidget->top },
                        dropdownWidget->height() + 1, w->colours[1], 0, Dropdown::Flag::StayOpen, numItems,
                        dropdownWidget->width() - 3);
                }
            }
            else
            {
                int32_t numItems = 0;
                for (auto rideId : window_new_campaign_rides)
                {
                    auto ride = get_ride(rideId);
                    if (ride != nullptr)
                    {
                        // HACK until dropdown items have longer argument buffers
                        gDropdownItemsFormat[numItems] = STR_DROPDOWN_MENU_LABEL;
                        Formatter ft(reinterpret_cast<uint8_t*>(&gDropdownItemsArgs[numItems]));
                        if (ride->custom_name.empty())
                        {
                            ride->FormatNameTo(ft);
                        }
                        else
                        {
                            gDropdownItemsFormat[numItems] = STR_OPTIONS_DROPDOWN_ITEM;
                            ft.Add<const char*>(ride->custom_name.c_str());
                        }
                        numItems++;
                    }
                }

                WindowDropdownShowTextCustomWidth(
                    { w->windowPos.x + dropdownWidget->left, w->windowPos.y + dropdownWidget->top },
                    dropdownWidget->height() + 1, w->colours[1], 0, Dropdown::Flag::StayOpen, numItems,
                    dropdownWidget->width() - 3);
            }
            break;
        // In RCT2, the maximum was 6 weeks
        case WIDX_WEEKS_INCREASE_BUTTON:
            w->campaign.no_weeks = std::min(w->campaign.no_weeks + 1, 12);
            w->Invalidate();
            break;
        case WIDX_WEEKS_DECREASE_BUTTON:
            w->campaign.no_weeks = std::max(w->campaign.no_weeks - 1, 2);
            w->Invalidate();
            break;
    }
}

/**
 *
 *  rct2: 0x0069E537
 */
static void window_new_campaign_dropdown(rct_window* w, rct_widgetindex widgetIndex, int32_t dropdownIndex)
{
    if (widgetIndex != WIDX_RIDE_DROPDOWN_BUTTON)
        return;

    if (dropdownIndex < 0)
        return;

    if (w->campaign.campaign_type == ADVERTISING_CAMPAIGN_FOOD_OR_DRINK_FREE)
    {
        if (static_cast<size_t>(dropdownIndex) >= std::size(window_new_campaign_shop_items))
            return;

        if (window_new_campaign_shop_items[dropdownIndex] == 255)
            return;

        w->campaign.ShopItemId = window_new_campaign_shop_items[dropdownIndex];
    }
    else
    {
        if (static_cast<size_t>(dropdownIndex) >= window_new_campaign_rides.size())
            return;

        w->campaign.RideId = window_new_campaign_rides[dropdownIndex];
    }

    w->Invalidate();
}

/**
 *
 *  rct2: 0x0069E397
 */
static void window_new_campaign_invalidate(rct_window* w)
{
    window_new_campaign_widgets[WIDX_RIDE_LABEL].type = WindowWidgetType::Empty;
    window_new_campaign_widgets[WIDX_RIDE_DROPDOWN].type = WindowWidgetType::Empty;
    window_new_campaign_widgets[WIDX_RIDE_DROPDOWN_BUTTON].type = WindowWidgetType::Empty;
    window_new_campaign_widgets[WIDX_RIDE_DROPDOWN].text = STR_MARKETING_NOT_SELECTED;
    switch (w->campaign.campaign_type)
    {
        case ADVERTISING_CAMPAIGN_RIDE_FREE:
        case ADVERTISING_CAMPAIGN_RIDE:
            window_new_campaign_widgets[WIDX_RIDE_LABEL].type = WindowWidgetType::Label;
            window_new_campaign_widgets[WIDX_RIDE_DROPDOWN].type = WindowWidgetType::DropdownMenu;
            window_new_campaign_widgets[WIDX_RIDE_DROPDOWN_BUTTON].type = WindowWidgetType::Button;
            window_new_campaign_widgets[WIDX_RIDE_LABEL].text = STR_MARKETING_RIDE;
            if (w->campaign.RideId != SELECTED_RIDE_UNDEFINED)
            {
                auto ride = get_ride(w->campaign.RideId);
                if (ride != nullptr)
                {
                    window_new_campaign_widgets[WIDX_RIDE_DROPDOWN].text = STR_STRINGID;

                    auto ft = Formatter::Common();
                    ride->FormatNameTo(ft);
                }
            }
            break;
        case ADVERTISING_CAMPAIGN_FOOD_OR_DRINK_FREE:
            window_new_campaign_widgets[WIDX_RIDE_LABEL].type = WindowWidgetType::Label;
            window_new_campaign_widgets[WIDX_RIDE_DROPDOWN].type = WindowWidgetType::DropdownMenu;
            window_new_campaign_widgets[WIDX_RIDE_DROPDOWN_BUTTON].type = WindowWidgetType::Button;
            window_new_campaign_widgets[WIDX_RIDE_LABEL].text = STR_MARKETING_ITEM;
            if (w->campaign.ShopItemId != SELECTED_RIDE_UNDEFINED)
            {
                window_new_campaign_widgets[WIDX_RIDE_DROPDOWN].text = GetShopItemDescriptor(ShopItem(w->campaign.ShopItemId))
                                                                           .Naming.Plural;
            }
            break;
    }

    // Set current number of weeks spinner (moved to paint due to required parameter)
    window_new_campaign_widgets[WIDX_WEEKS_SPINNER].text = STR_NONE;

    // Enable / disable start button based on ride dropdown
    w->disabled_widgets &= ~(1 << WIDX_START_BUTTON);
    if (window_new_campaign_widgets[WIDX_RIDE_DROPDOWN].type == WindowWidgetType::DropdownMenu
        && w->campaign.RideId == SELECTED_RIDE_UNDEFINED)
        w->disabled_widgets |= 1 << WIDX_START_BUTTON;
}

/**
 *
 *  rct2: 0x0069E493
 */
static void window_new_campaign_paint(rct_window* w, rct_drawpixelinfo* dpi)
{
    ScreenCoordsXY screenCoords{};

    WindowDrawWidgets(w, dpi);

    // Number of weeks
    rct_widget* spinnerWidget = &window_new_campaign_widgets[WIDX_WEEKS_SPINNER];
    DrawTextBasic(
        dpi, w->windowPos + ScreenCoordsXY{ spinnerWidget->left + 1, spinnerWidget->top },
        w->campaign.no_weeks == 1 ? STR_MARKETING_1_WEEK : STR_X_WEEKS, &w->campaign.no_weeks, { w->colours[0] });

    screenCoords = w->windowPos + ScreenCoordsXY{ 14, 60 };

    // Price per week
    money32 pricePerWeek = AdvertisingCampaignPricePerWeek[w->campaign.campaign_type];
    DrawTextBasic(dpi, screenCoords, STR_MARKETING_COST_PER_WEEK, &pricePerWeek);
    screenCoords.y += 13;

    // Total price
    money32 totalPrice = AdvertisingCampaignPricePerWeek[w->campaign.campaign_type] * w->campaign.no_weeks;
    DrawTextBasic(dpi, screenCoords, STR_MARKETING_TOTAL_COST, &totalPrice);
}

void WindowCampaignRefreshRides()
{
    auto window = window_find_by_class(WC_NEW_CAMPAIGN);
    if (window == nullptr)
    {
        return;
    }

    // Get all applicable rides
    window_new_campaign_rides.clear();
    for (const auto& ride : GetRideManager())
    {
        if (ride.status == RIDE_STATUS_OPEN)
        {
            if (!ride.GetRideTypeDescriptor().HasFlag(
                    RIDE_TYPE_FLAG_IS_SHOP | RIDE_TYPE_FLAG_SELLS_FOOD | RIDE_TYPE_FLAG_SELLS_DRINKS
                    | RIDE_TYPE_FLAG_IS_TOILET))
            {
                window_new_campaign_rides.push_back(ride.id);
            }
        }
    }

    // Take top 128 most valuable rides
    if (window_new_campaign_rides.size() > Dropdown::ItemsMaxSize)
    {
        qsort(window_new_campaign_rides.data(), window_new_campaign_rides.size(), sizeof(ride_id_t), ride_value_compare);
        window_new_campaign_rides.resize(Dropdown::ItemsMaxSize);
    }

    // Sort rides by name
    qsort(window_new_campaign_rides.data(), window_new_campaign_rides.size(), sizeof(ride_id_t), ride_name_compare);
}
