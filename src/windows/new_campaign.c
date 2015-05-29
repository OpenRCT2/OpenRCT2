/*****************************************************************************
 * Copyright (c) 2014 Ted John, Peter Hill, Duncan Frost
 * OpenRCT2, an open source clone of Roller Coaster Tycoon 2.
 *
 * This file is part of OpenRCT2.
 *
 * OpenRCT2 is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *****************************************************************************/

#include "../addresses.h"
#include "../config.h"
#include "../game.h"
#include "../localisation/localisation.h"
#include "../interface/widget.h"
#include "../interface/window.h"
#include "../management/marketing.h"
#include "../ride/ride.h"
#include "dropdown.h"
#include "../interface/colour_schemes.h"

#define SELECTED_RIDE_UNDEFINED ((uint16)0xFFFF)

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
	{ WWT_FRAME,			0,		0,		349,	0,		106,		0xFFFFFFFF,										STR_NONE },				// panel / background
	{ WWT_CAPTION,			0,		1,		348,	1,		14,			0,												STR_WINDOW_TITLE_TIP },	// title bar
	{ WWT_CLOSEBOX,			0,		337,	347,	2,		13,			STR_CLOSE_X,									STR_CLOSE_WINDOW_TIP },	// close x button
	{ WWT_24,				0,		0,		139,	24,		35,			0,												STR_NONE },				// ride label
	{ WWT_DROPDOWN,			0,		100,	341,	24,		35,			0,												STR_NONE },				// ride dropdown
	{ WWT_DROPDOWN_BUTTON,	0,		330,	340,	25,		34,			876,											STR_NONE },				// ride dropdown button
	{ WWT_24,				0,		0,		139,	41,		52,			STR_LENGTH_OF_TIME,								STR_NONE },				// weeks label
	{ WWT_SPINNER,			0,		120,	219,	41,		52,			0,												STR_NONE },				// weeks
	{ WWT_DROPDOWN_BUTTON,	0,		208,	218,	42,		46,			STR_NUMERIC_UP,									STR_NONE },				// weeks +
	{ WWT_DROPDOWN_BUTTON,	0,		208,	218,	47,		51,			STR_NUMERIC_DOWN,								STR_NONE },				// weeks -
	{ WWT_DROPDOWN_BUTTON,	0,		14,		335,	89,		100,		STR_MARKETING_START_THIS_MARKETING_CAMPAIGN,	STR_NONE },				// start button
	{ WIDGETS_END }
};


static void window_new_campaign_emptysub() { }
static void window_new_campaign_mouseup();
static void window_new_campaign_mousedown(int widgetIndex, rct_window*w, rct_widget* widget);
static void window_new_campaign_dropdown();
static void window_new_campaign_invalidate();
static void window_new_campaign_paint();

static void* window_new_campaign_events[] = {
	window_new_campaign_emptysub,
	window_new_campaign_mouseup,
	window_new_campaign_emptysub,
	window_new_campaign_mousedown,
	window_new_campaign_dropdown,
	window_new_campaign_emptysub,
	window_new_campaign_emptysub,
	window_new_campaign_emptysub,
	window_new_campaign_emptysub,
	window_new_campaign_emptysub,
	window_new_campaign_emptysub,
	window_new_campaign_emptysub,
	window_new_campaign_emptysub,
	window_new_campaign_emptysub,
	window_new_campaign_emptysub,
	window_new_campaign_emptysub,
	window_new_campaign_emptysub,
	window_new_campaign_emptysub,
	window_new_campaign_emptysub,
	window_new_campaign_emptysub,
	window_new_campaign_emptysub,
	window_new_campaign_emptysub,
	window_new_campaign_emptysub,
	window_new_campaign_emptysub,
	window_new_campaign_emptysub,
	window_new_campaign_invalidate,
	window_new_campaign_paint,
	window_new_campaign_emptysub
};

uint8 window_new_campaign_rides[MAX_RIDES];
uint8 window_new_campaign_shop_items[64];

int ride_value_compare(const void *a, const void *b)
{
	rct_ride *rideA, *rideB;

	rideA = GET_RIDE(*((uint8*)a));
	rideB = GET_RIDE(*((uint8*)b));
	return rideB->value - rideA->value;
}

int ride_name_compare(const void *a, const void *b)
{
	char rideAName[256], rideBName[256];
	rct_ride *rideA, *rideB;

	rideA = GET_RIDE(*((uint8*)a));
	rideB = GET_RIDE(*((uint8*)b));

	format_string(rideAName, rideA->name, &rideA->name_arguments);
	format_string(rideBName, rideB->name, &rideB->name_arguments);

	return _strcmpi(rideAName, rideBName);
}

/**
 * 
 *  rct2: 0x0069E16F
 */
void window_new_campaign_open(sint16 campaignType)
{
	rct_window *w;
	rct_ride *ride;
	int i, numApplicableRides;
	
	w = window_bring_to_front_by_class(WC_NEW_CAMPAIGN);
	if (w != NULL) {
		if (w->campaign.campaign_type == campaignType)
			return;

		window_close(w);
	}

	w = window_create_auto_pos(350, 107, (uint32*)window_new_campaign_events, WC_NEW_CAMPAIGN, 0);
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

	window_new_campaign_widgets[WIDX_TITLE].image = STR_MARKETING_VOUCHERS_FOR_FREE_ENTRY_TO_THE_PARK + campaignType;

	// Campaign type
	w->campaign.campaign_type = campaignType;

	// Number of weeks
	w->campaign.no_weeks = 2;

	// Currently selected ride
	w->campaign.ride_id = SELECTED_RIDE_UNDEFINED;

	// Get all applicable rides
	numApplicableRides = 0;
	window_new_campaign_rides[0] = 255;
	FOR_ALL_RIDES(i, ride) {
		if (!ride_type_has_flag(ride->type, RIDE_TYPE_FLAG_IS_SHOP | RIDE_TYPE_FLAG_23 | RIDE_TYPE_FLAG_SELLS_DRINKS | RIDE_TYPE_FLAG_IS_BATHROOM))
			window_new_campaign_rides[numApplicableRides++] = i;
	}

	// Take top 40 most reliable rides
	if (numApplicableRides > 40) {
		qsort(window_new_campaign_rides, countof(window_new_campaign_rides), sizeof(uint8), ride_value_compare);
		numApplicableRides = 40;
	}

	// Sort rides by name
	qsort(window_new_campaign_rides, numApplicableRides, sizeof(uint8), ride_name_compare);

	window_new_campaign_rides[numApplicableRides] = 255;
}

/**
 * 
 *  rct2: 0x0069E320
 */
static void window_new_campaign_get_shop_items()
{
	int i, numItems;
	rct_ride *ride;

	uint64 items = 0;
	FOR_ALL_RIDES(i, ride) {
		rct_ride_type *rideType = gRideTypeList[ride->subtype];
		uint8 itemType = rideType->shop_item;
		if (itemType != 255)
			items |= 1LL << itemType;
	}

	// Remove certain items?
	items &= 0x0011FF78036BA3E0;

	// 
	numItems = 0;
	for (i = 0; i < 64; i++)
		if (items & (1LL << i))
			window_new_campaign_shop_items[numItems++] = i;
	window_new_campaign_shop_items[numItems] = 255;
}

/**
 * 
 *  rct2: 0x0069E50B
 */
static void window_new_campaign_mouseup()
{
	rct_window *w;
	short widgetIndex;

	window_widget_get_registers(w, widgetIndex);

	switch (widgetIndex) {
	case WIDX_CLOSE:
		window_close(w);
		break;
	case WIDX_START_BUTTON:
		marketing_start_campaign(w->campaign.campaign_type, w->campaign.ride_id, w->campaign.no_weeks);
		window_close(w);
		break;
	}
}

/**
 * 
 *  rct2: 0x0069E51C
 */
static void window_new_campaign_mousedown(int widgetIndex, rct_window *w, rct_widget* widget)
{
	rct_widget *dropdownWidget;

	switch (widgetIndex) {
	case WIDX_RIDE_DROPDOWN_BUTTON:
		dropdownWidget = widget - 1;

		if (w->campaign.campaign_type == ADVERTISING_CAMPAIGN_FOOD_OR_DRINK_FREE) {
			window_new_campaign_get_shop_items();
			if (window_new_campaign_shop_items[0] != 255) {
				int numItems = 0;
				for (int i = 0; i < 40; i++) {
					if (window_new_campaign_shop_items[i] == 255)
						break;

					rct_string_id itemStringId = window_new_campaign_shop_items[i] + 2016;
					if (itemStringId >= 2048)
						itemStringId += 96;

					gDropdownItemsFormat[i] = 1142;
					gDropdownItemsArgs[i] = itemStringId;
					numItems++;
				}

				window_dropdown_show_text_custom_width(
					w->x + dropdownWidget->left,
					w->y + dropdownWidget->top,
					dropdownWidget->bottom - dropdownWidget->top + 1,
					w->colours[1],
					DROPDOWN_FLAG_STAY_OPEN,
					numItems,
					dropdownWidget->right - dropdownWidget->left - 3
				);
			}
		} else {
			int numItems = 0;
			for (int i = 0; i < 40; i++) {
				if (window_new_campaign_rides[i] == 255)
					break;

				rct_ride *ride = GET_RIDE(window_new_campaign_rides[i]);
				gDropdownItemsFormat[i] = 1142;
				gDropdownItemsArgs[i] = ((uint64)ride->name_arguments << 16ULL) | ride->name;
				numItems++;
			}

			window_dropdown_show_text_custom_width(
				w->x + dropdownWidget->left,
				w->y + dropdownWidget->top,
				dropdownWidget->bottom - dropdownWidget->top + 1,
				w->colours[1],
				DROPDOWN_FLAG_STAY_OPEN,
				numItems,
				dropdownWidget->right - dropdownWidget->left - 3
			);
		}
		break;
	case WIDX_WEEKS_INCREASE_BUTTON:
		w->campaign.no_weeks = min(w->campaign.no_weeks + 1, 6);
		window_invalidate(w);
		break;
	case WIDX_WEEKS_DECREASE_BUTTON:
		w->campaign.no_weeks = max(w->campaign.no_weeks - 1, 2);
		window_invalidate(w);
		break;
	}
}

/**
 * 
 *  rct2: 0x0069E537
 */
static void window_new_campaign_dropdown()
{
	rct_window *w;
	short widgetIndex, dropdownIndex;

	window_dropdown_get_registers(w, widgetIndex, dropdownIndex);

	if (widgetIndex != WIDX_RIDE_DROPDOWN_BUTTON)
		return;

	if (w->campaign.campaign_type == ADVERTISING_CAMPAIGN_FOOD_OR_DRINK_FREE) {
		rct_string_id itemStringId = (uint16)gDropdownItemsArgs[dropdownIndex] - 2016;
		if (itemStringId >= 32)
			itemStringId -= 96;
		w->campaign.ride_id = itemStringId;
	} else {
		w->campaign.ride_id = window_new_campaign_rides[dropdownIndex];
	}

	window_invalidate(w);
}

/**
 * 
 *  rct2: 0x0069E397
 */
static void window_new_campaign_invalidate()
{
	rct_window *w;

	window_get_register(w);
	colour_scheme_update(w);

	window_new_campaign_widgets[WIDX_RIDE_LABEL].type = WWT_EMPTY;
	window_new_campaign_widgets[WIDX_RIDE_DROPDOWN].type = WWT_EMPTY;
	window_new_campaign_widgets[WIDX_RIDE_DROPDOWN_BUTTON].type = WWT_EMPTY;
	window_new_campaign_widgets[WIDX_RIDE_DROPDOWN].image = STR_MARKETING_NOT_SELECTED;
	switch (w->campaign.campaign_type) {
	case ADVERTISING_CAMPAIGN_RIDE_FREE:
	case ADVERTISING_CAMPAIGN_RIDE:
		window_new_campaign_widgets[WIDX_RIDE_LABEL].type = WWT_24;
		window_new_campaign_widgets[WIDX_RIDE_DROPDOWN].type = WWT_DROPDOWN;
		window_new_campaign_widgets[WIDX_RIDE_DROPDOWN_BUTTON].type = WWT_DROPDOWN_BUTTON;
		window_new_campaign_widgets[WIDX_RIDE_LABEL].image = STR_MARKETING_RIDE;
		if (w->campaign.ride_id != SELECTED_RIDE_UNDEFINED) {
			rct_ride *ride = GET_RIDE(w->campaign.ride_id);
			window_new_campaign_widgets[WIDX_RIDE_DROPDOWN].image = ride->name;
			RCT2_GLOBAL(0x013CE952, uint32) = ride->name_arguments;
		}
		break;
	case ADVERTISING_CAMPAIGN_FOOD_OR_DRINK_FREE:
		window_new_campaign_widgets[WIDX_RIDE_LABEL].type = WWT_24;
		window_new_campaign_widgets[WIDX_RIDE_DROPDOWN].type = WWT_DROPDOWN;
		window_new_campaign_widgets[WIDX_RIDE_DROPDOWN_BUTTON].type = WWT_DROPDOWN_BUTTON;
		window_new_campaign_widgets[WIDX_RIDE_LABEL].image = STR_MARKETING_ITEM;
		if (w->campaign.ride_id != SELECTED_RIDE_UNDEFINED) {
			rct_string_id itemStringId = w->campaign.ride_id + 2016;
			if (itemStringId >= 2048)
				itemStringId += 96;
			window_new_campaign_widgets[WIDX_RIDE_DROPDOWN].image = itemStringId;
		}
		break;
	}

	// Set current number of weeks spinner
	window_new_campaign_widgets[WIDX_WEEKS_SPINNER].image = (STR_MARKETING_1_WEEK - 1) + w->campaign.no_weeks;

	// Enable / disable start button based on ride dropdown
	w->disabled_widgets &= ~(1 << WIDX_START_BUTTON);
	if (window_new_campaign_widgets[WIDX_RIDE_DROPDOWN].type == WWT_DROPDOWN && w->campaign.ride_id == SELECTED_RIDE_UNDEFINED)
		w->disabled_widgets |= 1 << WIDX_START_BUTTON;
}

/**
 * 
 *  rct2: 0x0069E493
 */
static void window_new_campaign_paint()
{
	rct_window *w;
	rct_drawpixelinfo *dpi;
	int x, y;

	window_paint_get_registers(w, dpi);

	window_draw_widgets(w, dpi);

	x = w->x + 14;
	y = w->y + 60;

	// Price per week
	money32 pricePerWeek = AdvertisingCampaignPricePerWeek[w->campaign.campaign_type];
	gfx_draw_string_left(dpi, STR_MARKETING_COST_PER_WEEK, &pricePerWeek, 0, x, y);
	y += 13;

	// Total price
	money32 totalPrice = AdvertisingCampaignPricePerWeek[w->campaign.campaign_type] * w->campaign.no_weeks;
	gfx_draw_string_left(dpi, STR_MARKETING_TOTAL_COST, &totalPrice, 0, x, y);
}
