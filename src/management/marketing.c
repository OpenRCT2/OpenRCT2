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

#include "../addresses.h"
#include "../game.h"
#include "../interface/window.h"
#include "../localisation/localisation.h"
#include "../ride/ride.h"
#include "marketing.h"
#include "news_item.h"

const money16 AdvertisingCampaignPricePerWeek[] = {
	MONEY(50,00),	// PARK_ENTRY_FREE,
	MONEY(50,00),	// RIDE_FREE,
	MONEY(50,00),	// PARK_ENTRY_HALF_PRICE,
	MONEY(50,00),	// FOOD_OR_DRINK_FREE,
	MONEY(350,00),	// PARK,
	MONEY(200,00)	// RIDE,
};

const int advertisingCampaignGuestGenerationProbabilities[] = { 400, 300, 200, 200, 250, 200 };

uint8 *gMarketingCampaignDaysLeft = RCT2_ADDRESS(0x01358102, uint8);
uint8 *gMarketingCampaignRideIndex = RCT2_ADDRESS(0x01358116, uint8);

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
		ride = &g_ride_list[gMarketingCampaignRideIndex[campaign]];
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
		int active = (gMarketingCampaignDaysLeft[campaign] & CAMPAIGN_ACTIVE_FLAG) != 0;
		if (gMarketingCampaignDaysLeft[campaign] == 0)
			continue;

		window_invalidate_by_class(WC_FINANCES);

		// High bit marks the campaign as inactive, on first check the campaign is set active
		// this makes campaigns run a full x weeks even when started in the middle of a week
		gMarketingCampaignDaysLeft[campaign] &= ~CAMPAIGN_ACTIVE_FLAG;
		if (active)
			continue;
		
		if (--gMarketingCampaignDaysLeft[campaign] != 0)
			continue;

		int campaignItem = gMarketingCampaignRideIndex[campaign];

		// This sets the string parameters for the marketing types that have an argument.
		if (campaign == ADVERTISING_CAMPAIGN_RIDE_FREE || campaign == ADVERTISING_CAMPAIGN_RIDE) {
			RCT2_GLOBAL(0x013CE952, uint16) = RCT2_GLOBAL(0x01362942 + 304 * campaignItem, uint16);
			RCT2_GLOBAL(0x013CE954, uint32) = RCT2_GLOBAL(0x01362944 + 152 * campaignItem, uint32);
		} else if (campaign == ADVERTISING_CAMPAIGN_FOOD_OR_DRINK_FREE) {
			campaignItem += 2016;
			if (campaignItem >= 2048)
				campaignItem += 96;
			RCT2_GLOBAL(0x013CE952, uint16) = campaignItem;
		}

		news_item_add_to_queue(NEWS_ITEM_MONEY, STR_MARKETING_FINISHED_BASE + campaign, 0);
	}
}

void marketing_set_guest_campaign(rct_peep *peep, int campaign)
{
	switch (campaign) {
	case ADVERTISING_CAMPAIGN_PARK_ENTRY_FREE:
		peep->item_standard_flags |= PEEP_ITEM_VOUCHER;
		peep->voucher_type = VOUCHER_TYPE_PARK_ENTRY_FREE;
		break;
	case ADVERTISING_CAMPAIGN_RIDE_FREE:
		peep->item_standard_flags |= PEEP_ITEM_VOUCHER;
		peep->voucher_type = VOUCHER_TYPE_RIDE_FREE;
		peep->voucher_arguments = gMarketingCampaignRideIndex[campaign];
		peep->guest_heading_to_ride_id = gMarketingCampaignRideIndex[campaign];
		peep->var_C6 = 240;
		break;
	case ADVERTISING_CAMPAIGN_PARK_ENTRY_HALF_PRICE:
		peep->item_standard_flags |= PEEP_ITEM_VOUCHER;
		peep->voucher_type = VOUCHER_TYPE_PARK_ENTRY_HALF_PRICE;
		break;
	case ADVERTISING_CAMPAIGN_FOOD_OR_DRINK_FREE:
		peep->item_standard_flags |= PEEP_ITEM_VOUCHER;
		peep->voucher_type = VOUCHER_TYPE_FOOD_OR_DRINK_FREE;
		peep->voucher_arguments = gMarketingCampaignRideIndex[campaign];
		break;
	case ADVERTISING_CAMPAIGN_PARK:
		break;
	case ADVERTISING_CAMPAIGN_RIDE:
		peep->guest_heading_to_ride_id = gMarketingCampaignRideIndex[campaign];
		peep->var_C6 = 240;
		break;
	}
}

void marketing_start_campaign(int type, int rideOrItem, int numWeeks)
{
	RCT2_GLOBAL(RCT2_ADDRESS_GAME_COMMAND_ERROR_TITLE, uint16) = STR_CANT_START_MARKETING_CAMPAIGN;
	game_do_command(0, (numWeeks << 8) | GAME_COMMAND_FLAG_APPLY, 0, (rideOrItem << 8) | type, GAME_COMMAND_START_MARKETING_CAMPAIGN, 0, 0);
}

/**
 * 
 * rct2: 0x0069E73C
 */
void game_command_start_campaign(int* eax, int* ebx, int* ecx, int* edx, int* esi, int* edi, int* ebp)
{
	int type = *edx & 0xFF;
	int rideOrItem = (*edx >> 8) & 0xFF;
	int numWeeks = (*ebx >> 8) & 0xFF;

	RCT2_GLOBAL(0x0141F56C, uint8) = 44;
	if (RCT2_GLOBAL(RCT2_ADDRESS_PARK_FLAGS, uint32) & PARK_FLAGS_FORBID_MARKETING_CAMPAIGN) {
		RCT2_GLOBAL(RCT2_ADDRESS_GAME_COMMAND_ERROR_TEXT, uint16) = 3048;
		*ebx = MONEY32_UNDEFINED;
		return;
	}

	if (*ebx & GAME_COMMAND_FLAG_APPLY) {
		gMarketingCampaignDaysLeft[type] = numWeeks | CAMPAIGN_ACTIVE_FLAG;
		gMarketingCampaignRideIndex[type] = rideOrItem;

		window_invalidate_by_class(WC_FINANCES);
	}

	*ebx = numWeeks * AdvertisingCampaignPricePerWeek[type];
}