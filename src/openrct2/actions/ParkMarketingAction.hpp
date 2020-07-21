/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../Context.h"
#include "../core/MemoryStream.h"
#include "../localisation/StringIds.h"
#include "../management/Finance.h"
#include "../management/Marketing.h"
#include "../ui/UiContext.h"
#include "../ui/WindowManager.h"
#include "../windows/Intent.h"
#include "../world/Park.h"
#include "GameAction.h"

#include <iterator>

DEFINE_GAME_ACTION(ParkMarketingAction, GAME_COMMAND_START_MARKETING_CAMPAIGN, GameActionResult)
{
private:
    int32_t _type;
    int32_t _item;
    int32_t _numWeeks;

public:
    ParkMarketingAction()
    {
    }
    ParkMarketingAction(int32_t type, int32_t item, int32_t numWeeks)
        : _type(type)
        , _item(item)
        , _numWeeks(numWeeks)
    {
    }

    uint16_t GetActionFlags() const override
    {
        return GameAction::GetActionFlags() | GA_FLAGS::ALLOW_WHILE_PAUSED;
    }

    void Serialise(DataSerialiser & stream) override
    {
        GameAction::Serialise(stream);
        stream << DS_TAG(_type) << DS_TAG(_item) << DS_TAG(_numWeeks);
    }

    GameActionResult::Ptr Query() const override
    {
        if (static_cast<size_t>(_type) >= std::size(AdvertisingCampaignPricePerWeek) || _numWeeks >= 256)
        {
            return MakeResult(GA_ERROR::INVALID_PARAMETERS, STR_CANT_START_MARKETING_CAMPAIGN);
        }
        if (gParkFlags & PARK_FLAGS_FORBID_MARKETING_CAMPAIGN)
        {
            return MakeResult(
                GA_ERROR::DISALLOWED, STR_CANT_START_MARKETING_CAMPAIGN, STR_MARKETING_CAMPAIGNS_FORBIDDEN_BY_LOCAL_AUTHORITY);
        }

        return CreateResult();
    }

    GameActionResult::Ptr Execute() const override
    {
        MarketingCampaign campaign{};
        campaign.Type = _type;
        campaign.WeeksLeft = _numWeeks;
        campaign.Flags = MarketingCampaignFlags::FIRST_WEEK;
        if (campaign.Type == ADVERTISING_CAMPAIGN_RIDE_FREE || campaign.Type == ADVERTISING_CAMPAIGN_RIDE)
        {
            campaign.RideId = _item;
        }
        else if (campaign.Type == ADVERTISING_CAMPAIGN_FOOD_OR_DRINK_FREE)
        {
            campaign.ShopItemType = _item;
        }
        marketing_new_campaign(campaign);

        // We are only interested in invalidating the finances (marketing) window
        auto windowManager = OpenRCT2::GetContext()->GetUiContext()->GetWindowManager();
        windowManager->BroadcastIntent(Intent(INTENT_ACTION_UPDATE_CASH));

        return CreateResult();
    }

private:
    GameActionResult::Ptr CreateResult() const
    {
        auto result = MakeResult();
        result->ErrorTitle = STR_CANT_START_MARKETING_CAMPAIGN;
        result->Expenditure = ExpenditureType::Marketing;
        result->Cost = CalculatePrice();
        return result;
    }

    money32 CalculatePrice() const
    {
        return _numWeeks * AdvertisingCampaignPricePerWeek[_type];
    }
};
