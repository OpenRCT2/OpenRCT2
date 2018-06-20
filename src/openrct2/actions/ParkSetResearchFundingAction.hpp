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
#include "../localisation/StringIds.h"
#include "../management/Research.h"
#include "../ui/UiContext.h"
#include "../ui/WindowManager.h"
#include "../windows/Intent.h"
#include "GameAction.h"

struct ParkSetResearchFundingAction : public GameActionBase<GAME_COMMAND_SET_RESEARCH_FUNDING, GameActionResult>
{
private:
    // TODO change to std::optional when C++17
    uint32_t _priorities;
    uint8_t _fundingAmount;

public:
    ParkSetResearchFundingAction() {}
    ParkSetResearchFundingAction(uint32_t priorities, uint8_t fundingAmount)
        : _priorities(priorities),
          _fundingAmount(fundingAmount)
    {
    }

    uint16_t GetActionFlags() const override
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

        auto windowManager = OpenRCT2::GetContext()->GetUiContext()->GetWindowManager();
        windowManager->BroadcastIntent(Intent(INTENT_ACTION_UPDATE_RESEARCH));
        return MakeResult();
    }
};
