/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "Marketing.h"

#include "../Cheats.h"
#include "../Game.h"
#include "../config/Config.h"
#include "../interface/Window.h"
#include "../localisation/Localisation.h"
#include "../ride/Ride.h"
#include "../ride/RideData.h"
#include "../ride/ShopItem.h"
#include "../world/Park.h"
#include "Finance.h"
#include "NewsItem.h"

const money16 AdvertisingCampaignPricePerWeek[] = {
    MONEY(50, 00),  // PARK_ENTRY_FREE
    MONEY(50, 00),  // RIDE_FREE
    MONEY(50, 00),  // PARK_ENTRY_HALF_PRICE
    MONEY(50, 00),  // FOOD_OR_DRINK_FREE
    MONEY(350, 00), // PARK
    MONEY(200, 00), // RIDE
};

static constexpr const uint16_t AdvertisingCampaignGuestGenerationProbabilities[] = {
    400, 300, 200, 200, 250, 200,
};

std::vector<MarketingCampaign> gMarketingCampaigns;

uint16_t marketing_get_campaign_guest_generation_probability(int32_t campaignType)
{
    auto campaign = marketing_get_campaign(campaignType);
    if (campaign == nullptr)
        return 0;

    // Lower probability of guest generation if price was already low
    auto probability = AdvertisingCampaignGuestGenerationProbabilities[campaign->Type];
    switch (campaign->Type)
    {
        case ADVERTISING_CAMPAIGN_PARK_ENTRY_FREE:
            if (park_get_entrance_fee() < MONEY(4, 00))
                probability /= 8;
            break;
        case ADVERTISING_CAMPAIGN_PARK_ENTRY_HALF_PRICE:
            if (park_get_entrance_fee() < MONEY(6, 00))
                probability /= 8;
            break;
        case ADVERTISING_CAMPAIGN_RIDE_FREE:
        {
            auto ride = get_ride(campaign->RideId);
            if (ride == nullptr || ride->price < MONEY(0, 30))
                probability /= 8;
            break;
        }
    }

    return probability;
}

static void marketing_raise_finished_notification(const MarketingCampaign& campaign)
{
    if (gConfigNotifications.park_marketing_campaign_finished)
    {
        // This sets the string parameters for the marketing types that have an argument.
        if (campaign.Type == ADVERTISING_CAMPAIGN_RIDE_FREE || campaign.Type == ADVERTISING_CAMPAIGN_RIDE)
        {
            Ride* ride = get_ride(campaign.RideId);
            set_format_arg(0, rct_string_id, ride->name);
            set_format_arg(2, uint32_t, ride->name_arguments);
        }
        else if (campaign.Type == ADVERTISING_CAMPAIGN_FOOD_OR_DRINK_FREE)
        {
            set_format_arg(0, rct_string_id, ShopItemStringIds[campaign.ShopItemType].plural);
        }

        news_item_add_to_queue(NEWS_ITEM_MONEY, MarketingCampaignNames[campaign.Type][2], 0);
    }
}

/**
 * Update status of marketing campaigns and send produce a news item when they have finished.
 *  rct2: 0x0069E0C1
 */
void marketing_update()
{
    if (gCheatsNeverendingMarketing)
        return;

    for (auto it = gMarketingCampaigns.begin(); it != gMarketingCampaigns.end();)
    {
        auto& campaign = *it;
        if (campaign.Flags & MarketingCampaignFlags::FIRST_WEEK)
        {
            // This ensures the campaign is active for x full weeks if started within the
            // middle of a week.
            campaign.Flags &= ~MarketingCampaignFlags::FIRST_WEEK;
        }
        else if (campaign.WeeksLeft > 0)
        {
            campaign.WeeksLeft--;
        }

        if (campaign.WeeksLeft == 0)
        {
            marketing_raise_finished_notification(campaign);
            it = gMarketingCampaigns.erase(it);
        }
        else
        {
            ++it;
        }
    }

    window_invalidate_by_class(WC_FINANCES);
}

void marketing_set_guest_campaign(rct_peep* peep, int32_t campaignType)
{
    auto campaign = marketing_get_campaign(campaignType);
    if (campaign == nullptr)
        return;

    switch (campaign->Type)
    {
        case ADVERTISING_CAMPAIGN_PARK_ENTRY_FREE:
            peep->item_standard_flags |= PEEP_ITEM_VOUCHER;
            peep->voucher_type = VOUCHER_TYPE_PARK_ENTRY_FREE;
            break;
        case ADVERTISING_CAMPAIGN_RIDE_FREE:
            peep->item_standard_flags |= PEEP_ITEM_VOUCHER;
            peep->voucher_type = VOUCHER_TYPE_RIDE_FREE;
            peep->voucher_arguments = campaign->RideId;
            peep->guest_heading_to_ride_id = campaign->RideId;
            peep->peep_is_lost_countdown = 240;
            break;
        case ADVERTISING_CAMPAIGN_PARK_ENTRY_HALF_PRICE:
            peep->item_standard_flags |= PEEP_ITEM_VOUCHER;
            peep->voucher_type = VOUCHER_TYPE_PARK_ENTRY_HALF_PRICE;
            break;
        case ADVERTISING_CAMPAIGN_FOOD_OR_DRINK_FREE:
            peep->item_standard_flags |= PEEP_ITEM_VOUCHER;
            peep->voucher_type = VOUCHER_TYPE_FOOD_OR_DRINK_FREE;
            peep->voucher_arguments = campaign->ShopItemType;
            break;
        case ADVERTISING_CAMPAIGN_PARK:
            break;
        case ADVERTISING_CAMPAIGN_RIDE:
            peep->guest_heading_to_ride_id = campaign->RideId;
            peep->peep_is_lost_countdown = 240;
            break;
    }
}

bool marketing_is_campaign_type_applicable(int32_t campaignType)
{
    int32_t i;
    Ride* ride;
    rct_ride_entry* rideEntry;

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
            FOR_ALL_RIDES (i, ride)
            {
                if (gRideClassifications[ride->type] == RIDE_CLASS_RIDE)
                {
                    return true;
                }
            }
            return false;

        case ADVERTISING_CAMPAIGN_FOOD_OR_DRINK_FREE:
            // Check if any food or drink stalls exist
            FOR_ALL_RIDES (i, ride)
            {
                rideEntry = get_ride_entry(ride->subtype);
                if (rideEntry == nullptr)
                {
                    continue;
                }
                if (shop_item_is_food_or_drink(rideEntry->shop_item)
                    || shop_item_is_food_or_drink(rideEntry->shop_item_secondary))
                {
                    return true;
                }
            }
            return false;

        default:
            return true;
    }
}

MarketingCampaign* marketing_get_campaign(int32_t campaignType)
{
    for (auto& campaign : gMarketingCampaigns)
    {
        if (campaign.Type == campaignType)
        {
            return &campaign;
        }
    }
    return nullptr;
}

void marketing_new_campaign(const MarketingCampaign& campaign)
{
    // Do not allow same campaign twice, just overwrite
    auto currentCampaign = marketing_get_campaign(campaign.Type);
    if (currentCampaign != nullptr)
    {
        *currentCampaign = campaign;
    }
    else
    {
        gMarketingCampaigns.push_back(campaign);
    }
}
