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
#include "../interface/themes.h"

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

/**
 *  Open new_campaign window
 *  rct2: 0x0069E16F
 */
void window_new_campaign_open(sint16 campaignType)
{
	rct_window *w;
	
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
	rct_ride *ride;
	int numItems;
	rct_widget *dropdownWidget = widget - 1;

	switch (widgetIndex) {

	case WIDX_RIDE_DROPDOWN_BUTTON:
		
		if (w->campaign.campaign_type == ADVERTISING_CAMPAIGN_FOOD_OR_DRINK_FREE)
		{
			//Generate list of available (unique) shop items
			ride_load_list_of_shop_items();

			for (numItems = 0; numItems < 40; numItems++) {
				//Stop when this isn't a shop item
				if (ride_list_of_shop_items[numItems] == 255)
					break;

				//Set name
				gDropdownItemsArgs[numItems] = convert_RideID_to_RCTStringID(ride_list_of_shop_items[numItems]);

				//Set 1142? as Format
				gDropdownItemsFormat[numItems] = 1142;
			}
		} 
		else
		{
			//Generate list of available rides
			ride_load_list_of_rides();

			for (numItems = 0; numItems < 40; numItems++) {
				//Stop when this isn't ride
				if (ride_list_of_rides[numItems] == 255)
					break;

				ride = GET_RIDE(ride_list_of_rides[numItems]);

				//Set name
				gDropdownItemsArgs[numItems] = ((uint64)ride->name_arguments << 16ULL) | ride->name;

				//Set 1142? as Format
				gDropdownItemsFormat[numItems] = 1142;
			}
		}

		//Show the dropdown box
		window_dropdown_show_text_custom_width(
			w->x + dropdownWidget->left,
			w->y + dropdownWidget->top,
			dropdownWidget->bottom - dropdownWidget->top + 1,
			w->colours[1],
			DROPDOWN_FLAG_STAY_OPEN,
			numItems,
			dropdownWidget->right - dropdownWidget->left - 3
			);

		break;

	case WIDX_WEEKS_INCREASE_BUTTON:
		//Increase amount of weeks (Maximum of 6)
		w->campaign.no_weeks = min(w->campaign.no_weeks + 1, 6);
		window_invalidate(w);
		break;
	case WIDX_WEEKS_DECREASE_BUTTON:
		//Decrease amoun of weeks (Minimum of 2)
		w->campaign.no_weeks = max(w->campaign.no_weeks - 1, 2);
		window_invalidate(w);
		break;
	}
}

/**
 *  Closing the dropdown list
 *  rct2: 0x0069E537
 */
static void window_new_campaign_dropdown()
{
	rct_window *w;
	short widgetIndex, dropdownIndex;

	window_dropdown_get_registers(w, widgetIndex, dropdownIndex);

	if (widgetIndex != WIDX_RIDE_DROPDOWN_BUTTON)
		return;

	//When user didn't choose one, select the first
	if (dropdownIndex == -1 && w->campaign.ride_id == SELECTED_RIDE_UNDEFINED)
		dropdownIndex = 0;

	//When user didn't choose one and has already a selected ride
	if (dropdownIndex == -1 && w->campaign.ride_id != SELECTED_RIDE_UNDEFINED)
		return;

	if (w->campaign.campaign_type == ADVERTISING_CAMPAIGN_FOOD_OR_DRINK_FREE) {
		//Get stringid of choosen shopitem
		w->campaign.ride_id = convert_RCTStringID_to_RideID((uint16)gDropdownItemsArgs[dropdownIndex]);
	} else {
		//Get id of ride
		w->campaign.ride_id = ride_list_of_rides[dropdownIndex];
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
			//Show name when a shopitem is selected
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
			//Show name when a ride is selected
			window_new_campaign_widgets[WIDX_RIDE_DROPDOWN].image = convert_RideID_to_RCTStringID(w->campaign.ride_id);
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