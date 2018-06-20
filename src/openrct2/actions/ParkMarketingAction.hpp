/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../Context.h"
#include "../core/MemoryStream.h"
#include "../core/Util.hpp"
#include "../localisation/StringIds.h"
#include "../management/Finance.h"
#include "../management/Marketing.h"
#include "../ui/UiContext.h"
#include "../ui/WindowManager.h"
#include "../windows/Intent.h"
#include "../world/Park.h"
#include "GameAction.h"

struct ParkMarketingAction : public GameActionBase<GAME_COMMAND_START_MARKETING_CAMPAIGN, GameActionResult>
{
private:
    int32_t _type;
    int32_t _item;
    int32_t _numWeeks;

public:
    ParkMarketingAction() {}
    ParkMarketingAction(int32_t type, int32_t item, int32_t numWeeks)
        : _type(type),
          _item(item),
          _numWeeks(numWeeks)
    {
    }

    uint16_t GetActionFlags() const override
    {
        return GameAction::GetActionFlags() | GA_FLAGS::ALLOW_WHILE_PAUSED;
    }

    void Serialise(DataSerialiser& stream) override
    {
        GameAction::Serialise(stream);
        stream << _type << _item << _numWeeks;
    }

    GameActionResult::Ptr Query() const override
    {
        if ((size_t)_type >= Util::CountOf(AdvertisingCampaignPricePerWeek) ||
            _numWeeks >= 256)
        {
            return MakeResult(GA_ERROR::INVALID_PARAMETERS, STR_CANT_START_MARKETING_CAMPAIGN);
        }
        if (gParkFlags & PARK_FLAGS_FORBID_MARKETING_CAMPAIGN)
        {
            return MakeResult(GA_ERROR::DISALLOWED, STR_CANT_START_MARKETING_CAMPAIGN, STR_MARKETING_CAMPAIGNS_FORBIDDEN_BY_LOCAL_AUTHORITY);
        }

        return CreateResult();
    }

    GameActionResult::Ptr Execute() const override
    {
        gMarketingCampaignDaysLeft[_type]  = _numWeeks | CAMPAIGN_ACTIVE_FLAG;
        gMarketingCampaignRideIndex[_type] = _item;

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
        result->ExpenditureType = RCT_EXPENDITURE_TYPE_MARKETING;
        result->Cost = CalculatePrice();
        return result;
    }

    money32 CalculatePrice() const
    {
        return _numWeeks * AdvertisingCampaignPricePerWeek[_type];
    }
};
