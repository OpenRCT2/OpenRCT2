/*****************************************************************************
 * Copyright (c) 2014 Ted John
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

#include "addresses.h"
#include "marketing.h"
#include "news_item.h"
#include "rct2.h"
#include "ride.h"
#include "string_ids.h"
#include "window.h"

const int advertisingCampaignGuestGenerationProbabilities[] = { 400, 300, 200, 200, 250, 200 };

int marketing_get_campaign_guest_generation_probability(int campaign)
{
	int probability = advertisingCampaignGuestGenerationProbabilities[campaign];
	rct_ride *ride;

	// Lower probability of guest generation if price was already low
	switch (campaign) {
	case ADVERTISING_CAMPAIGN_PARK_ENTRY_FREE:
		if (RCT2_GLOBAL(RCT2_ADDRESS_PARK_ENTRANCE_FEE, money16) < 4)
			probability /= 8;
		break;
	case ADVERTISING_CAMPAIGN_PARK_ENTRY_HALF_PRICE:
		if (RCT2_GLOBAL(RCT2_ADDRESS_PARK_ENTRANCE_FEE, money16) < 6)
			probability /= 8;
		break;
	case ADVERTISING_CAMPAIGN_RIDE_FREE:
		ride = &g_ride_list[RCT2_ADDRESS(0x01358116, uint8)[campaign]];
		if (ride->price < 3)
			probability /= 8;
		break;
	}

	return probability;
}

/*
 * Update status of marketing campaigns and send produce a news item when they have finished.
 * rct2: 0x0069E0C1
 **/
void marketing_update()
{
	for (int campaign = 0; campaign < ADVERTISING_CAMPAIGN_COUNT; campaign++) {
		uint8 campaign_weeks_left = RCT2_ADDRESS(0x01358102, uint8)[campaign];
		if (campaign_weeks_left == 0)
			continue;

		window_invalidate_by_id(WC_FINANCES, 0);

		// High bit marks the campaign as inactive, on first check the campaign is set actice
		// this makes campaigns run a full x weeks even when started in the middle of a week
		RCT2_ADDRESS(0x01358102, uint8)[campaign] &= ~(1 << 7);
		if (campaign_weeks_left & (1 << 7))
			continue;
		
		RCT2_ADDRESS(0x01358102, uint8)[campaign]--;
		if (campaign_weeks_left - 1 != 0)
			continue;

		int campaign_item = RCT2_ADDRESS(0x01358116, uint8)[campaign];

		// This sets the string parameters for the marketing types that have an argument.
		if (campaign == ADVERTISING_CAMPAIGN_RIDE_FREE || campaign == ADVERTISING_CAMPAIGN_RIDE) {
			RCT2_GLOBAL(0x013CE952, uint16) = RCT2_GLOBAL(0x01362942 + 304 * campaign_item, uint16);
			RCT2_GLOBAL(0x013CE954, uint32) = RCT2_GLOBAL(0x01362944 + 152 * campaign_item, uint32);
		} else if (campaign == ADVERTISING_CAMPAIGN_FOOD_OR_DRINK_FREE) {
			campaign_item += 2016;
			if (campaign_item >= 2048)
				campaign_item += 96;
			RCT2_GLOBAL(0x013CE952, uint16) = campaign_item;
		}

		news_item_add_to_queue(NEWS_ITEM_MONEY, STR_MARKETING_FINISHED_BASE + campaign, 0);
	}
}

void marketing_set_guest_campaign(rct_peep *peep, int campaign)
{
	switch (campaign) {
	case ADVERTISING_CAMPAIGN_PARK_ENTRY_FREE:
		peep->item_standard_flags |= PEEP_ITEM_VOUCHER;
		peep->var_F0 = 0;
		break;
	case ADVERTISING_CAMPAIGN_RIDE_FREE:
		peep->item_standard_flags |= PEEP_ITEM_VOUCHER;
		peep->var_F0 = 1;
		peep->var_F1 = RCT2_ADDRESS(0x01358116, uint8)[campaign];
		peep->var_C5 = RCT2_ADDRESS(0x01358116, uint8)[campaign];
		peep->var_C6 = 240;
		break;
	case ADVERTISING_CAMPAIGN_PARK_ENTRY_HALF_PRICE:
		peep->item_standard_flags |= PEEP_ITEM_VOUCHER;
		peep->var_F0 = 2;
		break;
	case ADVERTISING_CAMPAIGN_FOOD_OR_DRINK_FREE:
		peep->item_standard_flags |= PEEP_ITEM_VOUCHER;
		peep->var_F0 = 3;
		peep->var_F1 = RCT2_ADDRESS(0x01358116, uint8)[campaign];
		break;
	case ADVERTISING_CAMPAIGN_PARK:
		break;
	case ADVERTISING_CAMPAIGN_RIDE:
		peep->var_C5 = RCT2_ADDRESS(0x01358116, uint8)[campaign];
		peep->var_C6 = 240;
		break;
	}
}