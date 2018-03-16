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
#include "../localisation/StringIds.h"
#include "../management/Research.h"
#include "../ui/UiContext.h"
#include "../ui/WindowManager.h"
#include "../windows/Intent.h"
#include "GameAction.h"

using namespace OpenRCT2;

struct ParkSetResearchFundingAction : public GameActionBase<GAME_COMMAND_SET_RESEARCH_FUNDING, GameActionResult>
{
private:
    // TODO change to std::optional when C++17
    uint32 _priorities;
    uint8 _fundingAmount;

public:
    ParkSetResearchFundingAction() {}
    ParkSetResearchFundingAction(uint32 priorities, uint8 fundingAmount)
        : _priorities(priorities),
          _fundingAmount(fundingAmount)
    {
    }

    uint16 GetActionFlags() const override
    {
        return GameAction::GetActionFlags() | GA_FLAGS::ALLOW_WHILE_PAUSED;
    }

    void Serialise(DataSerialiser& stream) override
    {
        GameAction::Serialise(stream);
        stream << _priorities << _fundingAmount;
    }

    GameActionResult::Ptr Query() const override
    {
        if (_fundingAmount >= RESEARCH_FUNDING_COUNT)
        {
            return MakeResult(GA_ERROR::INVALID_PARAMETERS, STR_NONE);
        }
        return MakeResult();
    }

    GameActionResult::Ptr Execute() const override
    {
        gResearchPriorities = _priorities;
        gResearchFundingLevel = _fundingAmount;

        auto windowManager = GetContext()->GetUiContext()->GetWindowManager();
        windowManager->BroadcastIntent(Intent(INTENT_ACTION_UPDATE_RESEARCH));
        return MakeResult();
    }
};
