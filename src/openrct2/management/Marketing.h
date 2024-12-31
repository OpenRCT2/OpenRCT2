/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../Cheats.h"
#include "../localisation/StringIdType.h"
#include "../ride/RideTypes.h"
#include "../ride/ShopItem.h"

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

namespace OpenRCT2::MarketingCampaignFlags
{
    constexpr uint8_t FIRST_WEEK = 1 << 0;
}

extern const money64 AdvertisingCampaignPricePerWeek[ADVERTISING_CAMPAIGN_COUNT];

extern const StringId kMarketingCampaignNames[ADVERTISING_CAMPAIGN_COUNT][3];

uint16_t MarketingGetCampaignGuestGenerationProbability(int32_t campaign);
void MarketingUpdate();
void MarketingSetGuestCampaign(Guest* peep, int32_t campaign);
bool MarketingIsCampaignTypeApplicable(int32_t campaignType);
MarketingCampaign* MarketingGetCampaign(int32_t campaignType);
void MarketingNewCampaign(const MarketingCampaign& campaign);
void MarketingCancelCampaignsForRide(const RideId rideId);
