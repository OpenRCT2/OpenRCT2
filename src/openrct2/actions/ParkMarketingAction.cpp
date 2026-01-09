/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "ParkMarketingAction.h"

#include "../Context.h"
#include "../GameState.h"
#include "../core/MemoryStream.h"
#include "../localisation/StringIds.h"
#include "../management/Finance.h"
#include "../management/Marketing.h"
#include "../ui/WindowManager.h"
#include "../windows/Intent.h"
#include "../world/Park.h"

#include <iterator>

namespace OpenRCT2::GameActions
{
    ParkMarketingAction::ParkMarketingAction(int32_t type, int32_t item, int32_t numWeeks)
        : _type(type)
        , _item(item)
        , _numWeeks(numWeeks)
    {
    }

    void ParkMarketingAction::AcceptParameters(GameActionParameterVisitor& visitor)
    {
        visitor.Visit("type", _type);
        visitor.Visit("item", _item);
        visitor.Visit("duration", _numWeeks);
    }

    uint16_t ParkMarketingAction::GetActionFlags() const
    {
        return GameAction::GetActionFlags() | Flags::AllowWhilePaused;
    }

    void ParkMarketingAction::Serialise(DataSerialiser& stream)
    {
        GameAction::Serialise(stream);
        stream << DS_TAG(_type) << DS_TAG(_item) << DS_TAG(_numWeeks);
    }

    Result ParkMarketingAction::Query(GameState_t& gameState) const
    {
        if (static_cast<size_t>(_type) >= std::size(AdvertisingCampaignPricePerWeek) || _numWeeks >= 256)
        {
            return Result(Status::invalidParameters, STR_CANT_START_MARKETING_CAMPAIGN, STR_ERR_VALUE_OUT_OF_RANGE);
        }
        if (gameState.park.flags & PARK_FLAGS_FORBID_MARKETING_CAMPAIGN)
        {
            return Result(
                Status::disallowed, STR_CANT_START_MARKETING_CAMPAIGN, STR_MARKETING_CAMPAIGNS_FORBIDDEN_BY_LOCAL_AUTHORITY);
        }

        return CreateResult();
    }

    Result ParkMarketingAction::Execute(GameState_t& gameState) const
    {
        MarketingCampaign campaign{};
        campaign.Type = _type;
        campaign.WeeksLeft = _numWeeks;
        campaign.flags = { MarketingCampaignFlag::firstWeek };
        if (campaign.Type == ADVERTISING_CAMPAIGN_RIDE_FREE || campaign.Type == ADVERTISING_CAMPAIGN_RIDE)
        {
            campaign.RideId = RideId::FromUnderlying(_item);
        }
        else if (campaign.Type == ADVERTISING_CAMPAIGN_FOOD_OR_DRINK_FREE)
        {
            campaign.ShopItemType = ShopItem(_item);
        }
        MarketingNewCampaign(campaign);

        // We are only interested in invalidating the finances (marketing) window
        auto windowManager = Ui::GetWindowManager();
        windowManager->BroadcastIntent(Intent(INTENT_ACTION_UPDATE_CASH));

        return CreateResult();
    }

    Result ParkMarketingAction::CreateResult() const
    {
        auto result = Result();
        result.errorTitle = STR_CANT_START_MARKETING_CAMPAIGN;
        result.expenditure = ExpenditureType::marketing;
        result.cost = CalculatePrice();
        return result;
    }

    money64 ParkMarketingAction::CalculatePrice() const
    {
        return _numWeeks * AdvertisingCampaignPricePerWeek[_type];
    }
} // namespace OpenRCT2::GameActions
