/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "Marketing.h"

#include "../Cheats.h"
#include "../Game.h"
#include "../GameState.h"
#include "../config/Config.h"
#include "../entity/Guest.h"
#include "../localisation/Formatter.h"
#include "../profiling/Profiling.h"
#include "../ride/Ride.h"
#include "../ride/RideData.h"
#include "../ride/RideManager.hpp"
#include "../ride/ShopItem.h"
#include "../ui/WindowManager.h"
#include "../world/Park.h"
#include "Finance.h"
#include "NewsItem.h"

using namespace OpenRCT2;

const money64 AdvertisingCampaignPricePerWeek[] = {
    50.00_GBP,  // PARK_ENTRY_FREE
    50.00_GBP,  // RIDE_FREE
    50.00_GBP,  // PARK_ENTRY_HALF_PRICE
    50.00_GBP,  // FOOD_OR_DRINK_FREE
    350.00_GBP, // PARK
    200.00_GBP, // RIDE
};

// clang-format off
const StringId kMarketingCampaignNames[ADVERTISING_CAMPAIGN_COUNT][3] = {
    { STR_MARKETING_VOUCHERS_FOR_FREE_ENTRY_TO_THE_PARK,            STR_VOUCHERS_FOR_FREE_ENTRY_TO,         STR_MARKETING_FINISHED_FREE_ENTRY },        // ADVERTISING_CAMPAIGN_PARK_ENTRY_FREE,
    { STR_MARKETING_VOUCHERS_FOR_FREE_RIDES_ON_A_PARTICULAR_RIDE,   STR_VOUCHERS_FOR_FREE_RIDE_ON,          STR_MARKETING_FINISHED_FREE_RIDES },        // ADVERTISING_CAMPAIGN_RIDE_FREE,
    { STR_MARKETING_VOUCHERS_FOR_HALF_PRICE_ENTRY_TO_THE_PARK,      STR_VOUCHERS_FOR_HALF_PRICE_ENTRY_TO,   STR_MARKETING_FINISHED_HALF_PRICE_ENTRY },  // ADVERTISING_CAMPAIGN_PARK_ENTRY_HALF_PRICE,
    { STR_MARKETING_VOUCHERS_FOR_FREE_FOOD_OR_DRINK,                STR_VOUCHERS_FOR_FREE,                  STR_MARKETING_FINISHED_FREE_RIDE },         // ADVERTISING_CAMPAIGN_FOOD_OR_DRINK_FREE,
    { STR_MARKETING_ADVERTISING_CAMPAIGN_FOR_THE_PARK,              STR_ADVERTISING_CAMPAIGN_FOR_1,         STR_MARKETING_FINISHED_PARK_ADS },          // ADVERTISING_CAMPAIGN_PARK,
    { STR_MARKETING_ADVERTISING_CAMPAIGN_FOR_A_PARTICULAR_RIDE,     STR_ADVERTISING_CAMPAIGN_FOR_2,         STR_MARKETING_FINISHED_RIDE_ADS },          // ADVERTISING_CAMPAIGN_RIDE,
};
// clang-format on

static constexpr uint16_t AdvertisingCampaignGuestGenerationProbabilities[] = {
    400, 300, 200, 200, 250, 200,
};

uint16_t MarketingGetCampaignGuestGenerationProbability(int32_t campaignType)
{
    auto campaign = MarketingGetCampaign(campaignType);
    if (campaign == nullptr)
        return 0;

    // Lower probability of guest generation if price was already low
    auto probability = AdvertisingCampaignGuestGenerationProbabilities[campaign->Type];
    switch (campaign->Type)
    {
        case ADVERTISING_CAMPAIGN_PARK_ENTRY_FREE:
            if (Park::GetEntranceFee() < 4.00_GBP)
                probability /= 8;
            break;
        case ADVERTISING_CAMPAIGN_PARK_ENTRY_HALF_PRICE:
            if (Park::GetEntranceFee() < 6.00_GBP)
                probability /= 8;
            break;
        case ADVERTISING_CAMPAIGN_RIDE_FREE:
        {
            auto ride = GetRide(campaign->RideId);
            if (ride == nullptr || ride->price[0] < 0.30_GBP)
                probability /= 8;
            break;
        }
    }

    return probability;
}

static void MarketingRaiseFinishedNotification(const MarketingCampaign& campaign)
{
    if (Config::Get().notifications.parkMarketingCampaignFinished)
    {
        Formatter ft;
        // This sets the string parameters for the marketing types that have an argument.
        if (campaign.Type == ADVERTISING_CAMPAIGN_RIDE_FREE || campaign.Type == ADVERTISING_CAMPAIGN_RIDE)
        {
            auto ride = GetRide(campaign.RideId);
            if (ride != nullptr)
            {
                ride->formatNameTo(ft);
            }
        }
        else if (campaign.Type == ADVERTISING_CAMPAIGN_FOOD_OR_DRINK_FREE)
        {
            ft.Add<StringId>(GetShopItemDescriptor(campaign.ShopItemType).Naming.Plural);
        }

        News::AddItemToQueue(News::ItemType::campaign, kMarketingCampaignNames[campaign.Type][2], 0, ft);
    }
}

/**
 * Update status of marketing campaigns and produce a news item when they have finished.
 *  rct2: 0x0069E0C1
 */
void MarketingUpdate()
{
    PROFILED_FUNCTION();

    auto& gameState = getGameState();

    if (gameState.cheats.neverendingMarketing)
        return;

    for (auto it = gameState.park.marketingCampaigns.begin(); it != gameState.park.marketingCampaigns.end();)
    {
        auto& campaign = *it;
        if (campaign.flags.has(MarketingCampaignFlag::firstWeek))
        {
            // This ensures the campaign is active for x full weeks if started within the
            // middle of a week.
            campaign.flags.unset(MarketingCampaignFlag::firstWeek);
        }
        else if (campaign.WeeksLeft > 0)
        {
            campaign.WeeksLeft--;
        }

        if (campaign.WeeksLeft == 0)
        {
            MarketingRaiseFinishedNotification(campaign);
            it = gameState.park.marketingCampaigns.erase(it);
        }
        else
        {
            ++it;
        }
    }

    auto* windowMgr = Ui::GetWindowManager();
    windowMgr->InvalidateByClass(WindowClass::finances);
}

void MarketingSetGuestCampaign(Guest* peep, int32_t campaignType)
{
    auto campaign = MarketingGetCampaign(campaignType);
    if (campaign == nullptr)
        return;

    switch (campaign->Type)
    {
        case ADVERTISING_CAMPAIGN_PARK_ENTRY_FREE:
            peep->GiveItem(ShopItem::voucher);
            peep->VoucherType = VOUCHER_TYPE_PARK_ENTRY_FREE;
            break;
        case ADVERTISING_CAMPAIGN_RIDE_FREE:
            peep->GiveItem(ShopItem::voucher);
            peep->VoucherType = VOUCHER_TYPE_RIDE_FREE;
            peep->VoucherRideId = campaign->RideId;
            peep->GuestHeadingToRideId = campaign->RideId;
            peep->GuestIsLostCountdown = 240;
            break;
        case ADVERTISING_CAMPAIGN_PARK_ENTRY_HALF_PRICE:
            peep->GiveItem(ShopItem::voucher);
            peep->VoucherType = VOUCHER_TYPE_PARK_ENTRY_HALF_PRICE;
            break;
        case ADVERTISING_CAMPAIGN_FOOD_OR_DRINK_FREE:
            peep->GiveItem(ShopItem::voucher);
            peep->VoucherType = VOUCHER_TYPE_FOOD_OR_DRINK_FREE;
            peep->VoucherShopItem = campaign->ShopItemType;
            break;
        case ADVERTISING_CAMPAIGN_PARK:
            break;
        case ADVERTISING_CAMPAIGN_RIDE:
            peep->GuestHeadingToRideId = campaign->RideId;
            peep->GuestIsLostCountdown = 240;
            break;
    }
}

bool MarketingIsCampaignTypeApplicable(int32_t campaignType)
{
    auto& gameState = getGameState();

    switch (campaignType)
    {
        case ADVERTISING_CAMPAIGN_PARK_ENTRY_FREE:
        case ADVERTISING_CAMPAIGN_PARK_ENTRY_HALF_PRICE:
            if (!Park::EntranceFeeUnlocked())
                return false;
            return true;

        case ADVERTISING_CAMPAIGN_RIDE_FREE:
            if (!Park::RidePricesUnlocked())
                return false;

            [[fallthrough]];
        case ADVERTISING_CAMPAIGN_RIDE:
            // Check if any rides exist
            for (auto& ride : RideManager(gameState))
            {
                if (ride.isRide())
                {
                    return true;
                }
            }
            return false;

        case ADVERTISING_CAMPAIGN_FOOD_OR_DRINK_FREE:
            // Check if any food or drink stalls exist
            for (auto& ride : RideManager(gameState))
            {
                auto rideEntry = ride.getRideEntry();
                if (rideEntry != nullptr)
                {
                    for (auto& item : rideEntry->shop_item)
                    {
                        if (GetShopItemDescriptor(item).IsFoodOrDrink())
                        {
                            return true;
                        }
                    }
                }
            }
            return false;

        default:
            return true;
    }
}

MarketingCampaign* MarketingGetCampaign(int32_t campaignType)
{
    for (auto& campaign : getGameState().park.marketingCampaigns)
    {
        if (campaign.Type == campaignType)
        {
            return &campaign;
        }
    }
    return nullptr;
}

void MarketingNewCampaign(const MarketingCampaign& campaign)
{
    // Do not allow the same campaign twice, just overwrite
    auto currentCampaign = MarketingGetCampaign(campaign.Type);
    if (currentCampaign != nullptr)
    {
        *currentCampaign = campaign;
    }
    else
    {
        getGameState().park.marketingCampaigns.push_back(campaign);
    }
}

void MarketingCancelCampaignsForRide(const RideId rideId)
{
    auto isCampaignForRideFn = [&rideId](MarketingCampaign& campaign) {
        if (campaign.Type == ADVERTISING_CAMPAIGN_RIDE_FREE || campaign.Type == ADVERTISING_CAMPAIGN_RIDE)
        {
            return campaign.RideId == rideId;
        }
        return false;
    };

    auto& v = getGameState().park.marketingCampaigns;
    auto removedIt = std::remove_if(v.begin(), v.end(), isCampaignForRideFn);
    v.erase(removedIt, v.end());
}
