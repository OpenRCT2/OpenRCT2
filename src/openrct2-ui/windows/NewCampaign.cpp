/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include <openrct2/actions/ParkMarketingAction.hpp>
#include <openrct2/config/Config.h>
#include <openrct2/core/Math.hpp>
#include <openrct2/drawing/Drawing.h>
#include <openrct2/Game.h>
#include <openrct2/localisation/Localisation.h>
#include <openrct2/ride/Ride.h>
#include <openrct2/ride/RideData.h>
#include <openrct2/ride/ShopItem.h>
#include <openrct2-ui/interface/Dropdown.h>
#include <openrct2-ui/interface/Widget.h>
#include <openrct2-ui/windows/Window.h>

#define SELECTED_RIDE_UNDEFINED ((uint16_t)0xFFFF)

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
    { WWT_FRAME,            0,      0,      349,    0,      106,        0xFFFFFFFF,                                     STR_NONE },             // panel / background
    { WWT_CAPTION,          0,      1,      348,    1,      14,         0,                                              STR_WINDOW_TITLE_TIP }, // title bar
    { WWT_CLOSEBOX,         0,      337,    347,    2,      13,         STR_CLOSE_X,                                    STR_CLOSE_WINDOW_TIP }, // close x button
    { WWT_LABEL,            0,      14,     139,    24,     35,         0,                                              STR_NONE },             // ride label
    { WWT_DROPDOWN,         0,      100,    341,    24,     35,         0,                                              STR_NONE },             // ride dropdown
    { WWT_BUTTON,           0,      330,    340,    25,     34,         STR_DROPDOWN_GLYPH,                             STR_NONE },             // ride dropdown button
    { WWT_LABEL,            0,      14,     139,    41,     52,         STR_LENGTH_OF_TIME,                             STR_NONE },             // weeks label
      SPINNER_WIDGETS      (0,      120,    219,    41,     52,         0,                                              STR_NONE),              // weeks (3 widgets)
    { WWT_BUTTON,           0,      14,     335,    89,     100,        STR_MARKETING_START_THIS_MARKETING_CAMPAIGN,    STR_NONE },             // start button
    { WIDGETS_END }
};


static void window_new_campaign_mouseup(rct_window *w, rct_widgetindex widgetIndex);
static void window_new_campaign_mousedown(rct_window *w, rct_widgetindex widgetIndex, rct_widget* widget);
static void window_new_campaign_dropdown(rct_window *w, rct_widgetindex widgetIndex, int32_t dropdownIndex);
static void window_new_campaign_invalidate(rct_window *w);
static void window_new_campaign_paint(rct_window *w, rct_drawpixelinfo *dpi);

static rct_window_event_list window_new_campaign_events = {
    nullptr,
    window_new_campaign_mouseup,
    nullptr,
    window_new_campaign_mousedown,
    window_new_campaign_dropdown,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    window_new_campaign_invalidate,
    window_new_campaign_paint,
    nullptr
};
// clang-format on

static uint8_t window_new_campaign_rides[MAX_RIDES];
static uint8_t window_new_campaign_shop_items[64];

static int32_t ride_value_compare(const void *a, const void *b)
{
    Ride *rideA, *rideB;

    rideA = get_ride(*((uint8_t*)a));
    rideB = get_ride(*((uint8_t*)b));
    return rideB->value - rideA->value;
}

static int32_t ride_name_compare(const void *a, const void *b)
{
    char rideAName[256], rideBName[256];
    Ride *rideA, *rideB;

    rideA = get_ride(*((uint8_t*)a));
    rideB = get_ride(*((uint8_t*)b));

    format_string(rideAName, 256, rideA->name, &rideA->name_arguments);
    format_string(rideBName, 256, rideB->name, &rideB->name_arguments);

    return _strcmpi(rideAName, rideBName);
}

/**
 *
 *  rct2: 0x0069E16F
 */
rct_window * window_new_campaign_open(int16_t campaignType)
{
    rct_window *w;
    Ride *ride;
    int32_t i, numApplicableRides;

    w = window_bring_to_front_by_class(WC_NEW_CAMPAIGN);
    if (w != nullptr) {
        if (w->campaign.campaign_type == campaignType)
            return w;

        window_close(w);
    }

    w = window_create_auto_pos(350, 107, &window_new_campaign_events, WC_NEW_CAMPAIGN, 0);
    w->widgets = window_new_campaign_widgets;
    w->enabled_widgets =
        (1 << WIDX_CLOSE) |
        (1 << WIDX_RIDE_DROPDOWN) |
        (1 << WIDX_RIDE_DROPDOWN_BUTTON) |
        (1 << WIDX_WEEKS_INCREASE_BUTTON) |
        (1 << WIDX_WEEKS_DECREASE_BUTTON) |
        (1 << WIDX_START_BUTTON);
    w->hold_down_widgets =
        (1 << WIDX_WEEKS_INCREASE_BUTTON) |
        (1 << WIDX_WEEKS_DECREASE_BUTTON);
    window_init_scroll_widgets(w);

    window_new_campaign_widgets[WIDX_TITLE].text = MarketingCampaignNames[campaignType][0];

    // Campaign type
    w->campaign.campaign_type = campaignType;

    // Number of weeks
    w->campaign.no_weeks = 2;

    // Currently selected ride
    w->campaign.ride_id = SELECTED_RIDE_UNDEFINED;

    // Get all applicable rides
    numApplicableRides = 0;
    window_new_campaign_rides[0] = 255;
    FOR_ALL_RIDES(i, ride)
    {
        if (ride->status != RIDE_STATUS_OPEN)
        {
            continue;
        }
        if (ride_type_has_flag(ride->type, RIDE_TYPE_FLAG_IS_SHOP | RIDE_TYPE_FLAG_SELLS_FOOD | RIDE_TYPE_FLAG_SELLS_DRINKS | RIDE_TYPE_FLAG_IS_BATHROOM))
        {
            continue;
        }

        window_new_campaign_rides[numApplicableRides++] = i;
    }

    // Take top 128 most valuable rides
    if (numApplicableRides > DROPDOWN_ITEMS_MAX_SIZE)
    {
        qsort(window_new_campaign_rides, numApplicableRides, sizeof(uint8_t), ride_value_compare);
        numApplicableRides = DROPDOWN_ITEMS_MAX_SIZE;
    }

    // Sort rides by name
    qsort(window_new_campaign_rides, numApplicableRides, sizeof(uint8_t), ride_name_compare);

    window_new_campaign_rides[numApplicableRides] = 255;

    return w;
}

/**
 *
 *  rct2: 0x0069E320
 */
static void window_new_campaign_get_shop_items()
{
    int32_t i, numItems;
    Ride * ride;

    uint64_t items = 0;
    FOR_ALL_RIDES(i, ride)
    {
        rct_ride_entry * rideEntry = get_ride_entry(ride->subtype);
        if (rideEntry == nullptr)
        {
            continue;
        }
        uint8_t itemType = rideEntry->shop_item;
        if (itemType != SHOP_ITEM_NONE && shop_item_is_food_or_drink(itemType))
            items |= 1ULL << itemType;
    }

    //
    numItems = 0;
    for (i = 0; i < 64; i++)
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
static void window_new_campaign_mouseup(rct_window *w, rct_widgetindex widgetIndex)
{
    switch (widgetIndex)
    {
    case WIDX_CLOSE:
        window_close(w);
        break;
    case WIDX_START_BUTTON:
        {
            auto gameAction = ParkMarketingAction(w->campaign.campaign_type, w->campaign.ride_id, w->campaign.no_weeks);
            gameAction.SetCallback(
                [](const GameAction *ga, const GameActionResult * result)
                {
                    if (result->Error == GA_ERROR::OK)
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
static void window_new_campaign_mousedown(rct_window *w, rct_widgetindex widgetIndex, rct_widget* widget)
{
    rct_widget *dropdownWidget;

    switch (widgetIndex) {
    case WIDX_RIDE_DROPDOWN_BUTTON:
        dropdownWidget = widget - 1;

        if (w->campaign.campaign_type == ADVERTISING_CAMPAIGN_FOOD_OR_DRINK_FREE) {
            window_new_campaign_get_shop_items();
            if (window_new_campaign_shop_items[0] != 255) {
                int32_t numItems = 0;
                for (int32_t i = 0; i < DROPDOWN_ITEMS_MAX_SIZE; i++) {
                    if (window_new_campaign_shop_items[i] == 255)
                        break;

                    gDropdownItemsFormat[i] = STR_DROPDOWN_MENU_LABEL;
                    gDropdownItemsArgs[i] = ShopItemStringIds[window_new_campaign_shop_items[i]].plural;
                    numItems++;
                }

                window_dropdown_show_text_custom_width(
                    w->x + dropdownWidget->left,
                    w->y + dropdownWidget->top,
                    dropdownWidget->bottom - dropdownWidget->top + 1,
                    w->colours[1],
                    0,
                    DROPDOWN_FLAG_STAY_OPEN,
                    numItems,
                    dropdownWidget->right - dropdownWidget->left - 3
                );
            }
        } else {
            int32_t numItems = 0;
            for (int32_t i = 0; i < DROPDOWN_ITEMS_MAX_SIZE; i++)
            {
                if (window_new_campaign_rides[i] == 255)
                    break;

                Ride * ride = get_ride(window_new_campaign_rides[i]);
                gDropdownItemsFormat[i] = STR_DROPDOWN_MENU_LABEL;
                gDropdownItemsArgs[i] = ((uint64_t)ride->name_arguments << 16ULL) | ride->name;
                numItems++;
            }

            window_dropdown_show_text_custom_width(
                w->x + dropdownWidget->left,
                w->y + dropdownWidget->top,
                dropdownWidget->bottom - dropdownWidget->top + 1,
                w->colours[1],
                0,
                DROPDOWN_FLAG_STAY_OPEN,
                numItems,
                dropdownWidget->right - dropdownWidget->left - 3
            );
        }
        break;
    // In RCT2, the maximum was 6 weeks
    case WIDX_WEEKS_INCREASE_BUTTON:
        w->campaign.no_weeks = std::min(w->campaign.no_weeks + 1, 12);
        window_invalidate(w);
        break;
    case WIDX_WEEKS_DECREASE_BUTTON:
        w->campaign.no_weeks = std::max(w->campaign.no_weeks - 1, 2);
        window_invalidate(w);
        break;
    }
}

/**
 *
 *  rct2: 0x0069E537
 */
static void window_new_campaign_dropdown(rct_window *w, rct_widgetindex widgetIndex, int32_t dropdownIndex)
{
    if (widgetIndex != WIDX_RIDE_DROPDOWN_BUTTON)
        return;

    if (dropdownIndex == -1)
        return;

    if (w->campaign.campaign_type == ADVERTISING_CAMPAIGN_FOOD_OR_DRINK_FREE) {
        w->campaign.ride_id = window_new_campaign_shop_items[dropdownIndex];
    } else {
        w->campaign.ride_id = window_new_campaign_rides[dropdownIndex];
    }

    window_invalidate(w);
}

/**
 *
 *  rct2: 0x0069E397
 */
static void window_new_campaign_invalidate(rct_window *w)
{
    window_new_campaign_widgets[WIDX_RIDE_LABEL].type = WWT_EMPTY;
    window_new_campaign_widgets[WIDX_RIDE_DROPDOWN].type = WWT_EMPTY;
    window_new_campaign_widgets[WIDX_RIDE_DROPDOWN_BUTTON].type = WWT_EMPTY;
    window_new_campaign_widgets[WIDX_RIDE_DROPDOWN].text = STR_MARKETING_NOT_SELECTED;
    switch (w->campaign.campaign_type) {
    case ADVERTISING_CAMPAIGN_RIDE_FREE:
    case ADVERTISING_CAMPAIGN_RIDE:
        window_new_campaign_widgets[WIDX_RIDE_LABEL].type = WWT_LABEL;
        window_new_campaign_widgets[WIDX_RIDE_DROPDOWN].type = WWT_DROPDOWN;
        window_new_campaign_widgets[WIDX_RIDE_DROPDOWN_BUTTON].type = WWT_BUTTON;
        window_new_campaign_widgets[WIDX_RIDE_LABEL].text = STR_MARKETING_RIDE;
        if (w->campaign.ride_id != SELECTED_RIDE_UNDEFINED) {
            Ride *ride = get_ride(w->campaign.ride_id);
            window_new_campaign_widgets[WIDX_RIDE_DROPDOWN].text = ride->name;
            set_format_arg(0, uint32_t, ride->name_arguments);
        }
        break;
    case ADVERTISING_CAMPAIGN_FOOD_OR_DRINK_FREE:
        window_new_campaign_widgets[WIDX_RIDE_LABEL].type = WWT_LABEL;
        window_new_campaign_widgets[WIDX_RIDE_DROPDOWN].type = WWT_DROPDOWN;
        window_new_campaign_widgets[WIDX_RIDE_DROPDOWN_BUTTON].type = WWT_BUTTON;
        window_new_campaign_widgets[WIDX_RIDE_LABEL].text = STR_MARKETING_ITEM;
        if (w->campaign.ride_id != SELECTED_RIDE_UNDEFINED) {
            window_new_campaign_widgets[WIDX_RIDE_DROPDOWN].text = ShopItemStringIds[w->campaign.ride_id].plural;
        }
        break;
    }

    // Set current number of weeks spinner (moved to paint due to required parameter)
    window_new_campaign_widgets[WIDX_WEEKS_SPINNER].text = STR_NONE;

    // Enable / disable start button based on ride dropdown
    w->disabled_widgets &= ~(1 << WIDX_START_BUTTON);
    if (window_new_campaign_widgets[WIDX_RIDE_DROPDOWN].type == WWT_DROPDOWN && w->campaign.ride_id == SELECTED_RIDE_UNDEFINED)
        w->disabled_widgets |= 1 << WIDX_START_BUTTON;
}

/**
 *
 *  rct2: 0x0069E493
 */
static void window_new_campaign_paint(rct_window *w, rct_drawpixelinfo *dpi)
{
    int32_t x, y;

    window_draw_widgets(w, dpi);

    // Number of weeks
    rct_widget *spinnerWidget = &window_new_campaign_widgets[WIDX_WEEKS_SPINNER];
    gfx_draw_string_left(
        dpi,
        w->campaign.no_weeks == 1 ? STR_MARKETING_1_WEEK : STR_X_WEEKS,
        &w->campaign.no_weeks,
        w->colours[0],
        w->x + spinnerWidget->left + 1,
        w->y + spinnerWidget->top
    );

    x = w->x + 14;
    y = w->y + 60;

    // Price per week
    money32 pricePerWeek = AdvertisingCampaignPricePerWeek[w->campaign.campaign_type];
    gfx_draw_string_left(dpi, STR_MARKETING_COST_PER_WEEK, &pricePerWeek, COLOUR_BLACK, x, y);
    y += 13;

    // Total price
    money32 totalPrice = AdvertisingCampaignPricePerWeek[w->campaign.campaign_type] * w->campaign.no_weeks;
    gfx_draw_string_left(dpi, STR_MARKETING_TOTAL_COST, &totalPrice, COLOUR_BLACK, x, y);
}
