#pragma region Copyright (c) 2014-2017 OpenRCT2 Developers
/*****************************************************************************
 * OpenRCT2, an open source clone of Roller Coaster Tycoon 2.
 *
 * OpenRCT2 is the work of many authors, a full list can be found in contributors.md
 * For more information, visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * A full copy of the GNU General Public License can be found in licence.txt
 *****************************************************************************/
#pragma endregion

#include "../config/Config.h"
#include "../core/Util.hpp"
#include "../Game.h"
#include "../interface/Window.h"
#include "../localisation/Localisation.h"
#include "../ride/Ride.h"
#include "../ride/RideData.h"
#include "../world/Park.h"
#include "../Cheats.h"
#include "Finance.h"
#include "Marketing.h"
#include "NewsItem.h"

const money16 AdvertisingCampaignPricePerWeek[] =
{
    MONEY(50, 00),   // PARK_ENTRY_FREE
    MONEY(50, 00),   // RIDE_FREE
    MONEY(50, 00),   // PARK_ENTRY_HALF_PRICE
    MONEY(50, 00),   // FOOD_OR_DRINK_FREE
    MONEY(350, 00),  // PARK
    MONEY(200, 00)   // RIDE
};

static constexpr const sint32 AdvertisingCampaignGuestGenerationProbabilities[] = {400, 300, 200, 200, 250, 200};

uint8 gMarketingCampaignDaysLeft[20];
uint8 gMarketingCampaignRideIndex[22];

sint32 marketing_get_campaign_guest_generation_probability(sint32 campaign)
{
    sint32 probability = AdvertisingCampaignGuestGenerationProbabilities[campaign];
    Ride   * ride;

    // Lower probability of guest generation if price was already low
    switch (campaign)
    {
    case ADVERTISING_CAMPAIGN_PARK_ENTRY_FREE:
        if (park_get_entrance_fee() < 4)
            probability /= 8;
        break;
    case ADVERTISING_CAMPAIGN_PARK_ENTRY_HALF_PRICE:
        if (park_get_entrance_fee() < 6)
            probability /= 8;
        break;
    case ADVERTISING_CAMPAIGN_RIDE_FREE:
        ride = get_ride(gMarketingCampaignRideIndex[campaign]);
        if (ride->price < 3)
            probability /= 8;
        break;
    }

    return probability;
}

/**
 * Update status of marketing campaigns and send produce a news item when they have finished.
 *  rct2: 0x0069E0C1
 */
void marketing_update()
{
    for (sint32 campaign = 0; campaign < ADVERTISING_CAMPAIGN_COUNT; campaign++)
    {
        if (gCheatsNeverendingMarketing)
            continue;

        sint32 active = (gMarketingCampaignDaysLeft[campaign] & CAMPAIGN_ACTIVE_FLAG) != 0;
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

        sint32 campaignItem = gMarketingCampaignRideIndex[campaign];

        // This sets the string parameters for the marketing types that have an argument.
        if (campaign == ADVERTISING_CAMPAIGN_RIDE_FREE || campaign == ADVERTISING_CAMPAIGN_RIDE)
        {
            Ride * ride = get_ride(campaignItem);
            set_format_arg(0, rct_string_id, ride->name);
            set_format_arg(2, uint32, ride->name_arguments);
        }
        else if (campaign == ADVERTISING_CAMPAIGN_FOOD_OR_DRINK_FREE)
        {
            set_format_arg(0, rct_string_id, ShopItemStringIds[campaignItem].plural);
        }

        if (gConfigNotifications.park_marketing_campaign_finished)
        {
            news_item_add_to_queue(NEWS_ITEM_MONEY, MarketingCampaignNames[campaign][2], 0);
        }
    }
}

void marketing_set_guest_campaign(rct_peep * peep, sint32 campaign)
{
    switch (campaign)
    {
    case ADVERTISING_CAMPAIGN_PARK_ENTRY_FREE:
        peep->item_standard_flags |= PEEP_ITEM_VOUCHER;
        peep->voucher_type             = VOUCHER_TYPE_PARK_ENTRY_FREE;
        break;
    case ADVERTISING_CAMPAIGN_RIDE_FREE:
        peep->item_standard_flags |= PEEP_ITEM_VOUCHER;
        peep->voucher_type             = VOUCHER_TYPE_RIDE_FREE;
        peep->voucher_arguments        = gMarketingCampaignRideIndex[campaign];
        peep->guest_heading_to_ride_id = gMarketingCampaignRideIndex[campaign];
        peep->peep_is_lost_countdown   = 240;
        break;
    case ADVERTISING_CAMPAIGN_PARK_ENTRY_HALF_PRICE:
        peep->item_standard_flags |= PEEP_ITEM_VOUCHER;
        peep->voucher_type             = VOUCHER_TYPE_PARK_ENTRY_HALF_PRICE;
        break;
    case ADVERTISING_CAMPAIGN_FOOD_OR_DRINK_FREE:
        peep->item_standard_flags |= PEEP_ITEM_VOUCHER;
        peep->voucher_type             = VOUCHER_TYPE_FOOD_OR_DRINK_FREE;
        peep->voucher_arguments        = gMarketingCampaignRideIndex[campaign];
        break;
    case ADVERTISING_CAMPAIGN_PARK:
        break;
    case ADVERTISING_CAMPAIGN_RIDE:
        peep->guest_heading_to_ride_id = gMarketingCampaignRideIndex[campaign];
        peep->peep_is_lost_countdown   = 240;
        break;
    }
}

bool marketing_is_campaign_type_applicable(sint32 campaignType)
{
    sint32         i;
    Ride           * ride;
    rct_ride_entry * rideEntry;

    switch (campaignType)
    {
    case ADVERTISING_CAMPAIGN_PARK_ENTRY_FREE:
    case ADVERTISING_CAMPAIGN_PARK_ENTRY_HALF_PRICE:
        if (!park_entry_price_unlocked())
            return false;
        return true;

    case ADVERTISING_CAMPAIGN_RIDE_FREE:
        if (!park_ride_prices_unlocked())
            return false;

        // fall-through
    case ADVERTISING_CAMPAIGN_RIDE:
        // Check if any rides exist
        FOR_ALL_RIDES(i, ride)
        {
            if (gRideClassifications[ride->type] == RIDE_CLASS_RIDE)
            {
                return true;
            }
        }
        return false;

    case ADVERTISING_CAMPAIGN_FOOD_OR_DRINK_FREE:
        // Check if any food or drink stalls exist
        FOR_ALL_RIDES(i, ride)
        {
            rideEntry = get_ride_entry(ride->subtype);
            if (rideEntry == nullptr)
            {
                continue;
            }
            if (
                shop_item_is_food_or_drink(rideEntry->shop_item) ||
                shop_item_is_food_or_drink(rideEntry->shop_item_secondary)
                )
            {
                return true;
            }
        }
        return false;

    default:
        return true;
    }
}
