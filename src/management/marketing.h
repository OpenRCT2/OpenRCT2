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

#ifndef _MARKETING_H_
#define _MARKETING_H_

#include "../common.h"
#include "../peep/peep.h"

enum {
	ADVERTISING_CAMPAIGN_PARK_ENTRY_FREE,
	ADVERTISING_CAMPAIGN_RIDE_FREE,
	ADVERTISING_CAMPAIGN_PARK_ENTRY_HALF_PRICE,
	ADVERTISING_CAMPAIGN_FOOD_OR_DRINK_FREE,
	ADVERTISING_CAMPAIGN_PARK,
	ADVERTISING_CAMPAIGN_RIDE,
	ADVERTISING_CAMPAIGN_COUNT
};

enum{
	VOUCHER_TYPE_PARK_ENTRY_FREE,
	VOUCHER_TYPE_RIDE_FREE,
	VOUCHER_TYPE_PARK_ENTRY_HALF_PRICE,
	VOUCHER_TYPE_FOOD_OR_DRINK_FREE,
};

#define CAMPAIGN_ACTIVE_FLAG (1 << 7)

extern const money16 AdvertisingCampaignPricePerWeek[6];
extern uint8 *gMarketingCampaignDaysLeft;
extern uint8 *gMarketingCampaignRideIndex;

int marketing_get_campaign_guest_generation_probability(int campaign);
void marketing_update();
void marketing_set_guest_campaign(rct_peep *peep, int campaign);
void marketing_start_campaign(int type, int rideOrItem, int numWeeks);
void game_command_start_campaign(int* eax, int* ebx, int* ecx, int* edx, int* esi, int* edi, int* ebp);

#endif