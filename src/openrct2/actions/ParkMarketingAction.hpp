#pragma region Copyright (c) 2014-2018 OpenRCT2 Developers
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

using namespace OpenRCT2;

struct ParkMarketingAction : public GameActionBase<GAME_COMMAND_START_MARKETING_CAMPAIGN, GameActionResult>
{
private:
    sint32 _type;
    sint32 _item;
    sint32 _numWeeks;

public:
    ParkMarketingAction() {}
    ParkMarketingAction(sint32 type, sint32 item, sint32 numWeeks)
        : _type(type),
          _item(item),
          _numWeeks(numWeeks)
    {
    }

    uint16 GetActionFlags() const override
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
        auto windowManager = GetContext()->GetUiContext()->GetWindowManager();
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
