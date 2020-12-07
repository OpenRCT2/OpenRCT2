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
#include "../management/Research.h"
#include "../ui/UiContext.h"
#include "../ui/WindowManager.h"
#include "../windows/Intent.h"
#include "GameAction.h"

DEFINE_GAME_ACTION(ParkSetResearchFundingAction, GAME_COMMAND_SET_RESEARCH_FUNDING, GameActions::Result)
{
private:
    // TODO change to std::optional when C++17
    uint32_t _priorities{};
    uint8_t _fundingAmount{};

public:
    ParkSetResearchFundingAction() = default;
    ParkSetResearchFundingAction(uint32_t priorities, uint8_t fundingAmount)
        : _priorities(priorities)
        , _fundingAmount(fundingAmount)
    {
    }

    uint16_t GetActionFlags() const override
    {
        return GameAction::GetActionFlags() | GameActions::Flags::AllowWhilePaused;
    }

    void Serialise(DataSerialiser & stream) override
    {
        GameAction::Serialise(stream);
        stream << DS_TAG(_priorities) << DS_TAG(_fundingAmount);
    }

    GameActions::Result::Ptr Query() const override
    {
        if (_fundingAmount >= RESEARCH_FUNDING_COUNT)
        {
            return MakeResult(GameActions::Status::InvalidParameters, STR_NONE);
        }
        return MakeResult();
    }

    GameActions::Result::Ptr Execute() const override
    {
        gResearchPriorities = _priorities;
        gResearchFundingLevel = _fundingAmount;

        auto windowManager = OpenRCT2::GetContext()->GetUiContext()->GetWindowManager();
        windowManager->BroadcastIntent(Intent(INTENT_ACTION_UPDATE_RESEARCH));
        return MakeResult();
    }
};
