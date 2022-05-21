/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../Cheats.h"
#include "../common.h"
#include "../ride/RideTypes.h"
#include "../ride/ShopItem.h"

#include <vector>

struct Guest;

enum
{
    ADVERTISING_CAMPAIGN_PARK_ENTRY_FREE,
    ADVERTISING_CAMPAIGN_RIDE_FREE,
    ADVERTISING_CAMPAIGN_PARK_ENTRY_HALF_PRICE,
    ADVERTISING_CAMPAIGN_FOOD_OR_DRINK_FREE,
    ADVERTISING_CAMPAIGN_PARK,
    ADVERTISING_CAMPAIGN_RIDE,
    ADVERTISING_CAMPAIGN_COUNT
};

enum
{
    VOUCHER_TYPE_PARK_ENTRY_FREE,
    VOUCHER_TYPE_RIDE_FREE,
    VOUCHER_TYPE_PARK_ENTRY_HALF_PRICE,
    VOUCHER_TYPE_FOOD_OR_DRINK_FREE,
};

enum
{
    CAMPAIGN_FIRST_WEEK_FLAG = (1 << 6),
    CAMPAIGN_ACTIVE_FLAG = (1 << 7)
};

struct MarketingCampaign
{
    uint8_t Type{};
    uint8_t WeeksLeft{};
    uint8_t Flags{};
    union
    {
        ::RideId RideId{};
        ShopItemIndex ShopItemType;
    };
};

namespace MarketingCampaignFlags
{
    constexpr uint8_t FIRST_WEEK = 1 << 0;
}

extern const money16 AdvertisingCampaignPricePerWeek[ADVERTISING_CAMPAIGN_COUNT];
extern std::vector<MarketingCampaign> gMarketingCampaigns;

uint16_t marketing_get_campaign_guest_generation_probability(int32_t campaign);
void marketing_update();
void marketing_set_guest_campaign(Guest* peep, int32_t campaign);
bool marketing_is_campaign_type_applicable(int32_t campaignType);
MarketingCampaign* marketing_get_campaign(int32_t campaignType);
void marketing_new_campaign(const MarketingCampaign& campaign);
void MarketingCancelCampaignsForRide(const RideId rideId);
