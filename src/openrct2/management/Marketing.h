/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../common.h"
#include "../Cheats.h"
#include "../peep/Peep.h"

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

enum {
    CAMPAIGN_ACTIVE_FLAG = (1 << 7)
};

extern const money16 AdvertisingCampaignPricePerWeek[ADVERTISING_CAMPAIGN_COUNT];
extern uint8 gMarketingCampaignDaysLeft[20];
extern uint8 gMarketingCampaignRideIndex[22];

sint32 marketing_get_campaign_guest_generation_probability(sint32 campaign);
void marketing_update();
void marketing_set_guest_campaign(rct_peep *peep, sint32 campaign);
bool marketing_is_campaign_type_applicable(sint32 campaignType);
