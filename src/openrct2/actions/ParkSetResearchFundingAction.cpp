/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "ParkSetResearchFundingAction.h"

#include "../Context.h"
#include "../Diagnostic.h"
#include "../GameState.h"
#include "../core/MemoryStream.h"
#include "../localisation/StringIds.h"
#include "../management/Research.h"
#include "../ui/WindowManager.h"
#include "../windows/Intent.h"

using namespace OpenRCT2;

ParkSetResearchFundingAction::ParkSetResearchFundingAction(uint32_t priorities, uint8_t fundingAmount)
    : _priorities(priorities)
    , _fundingAmount(fundingAmount)
{
}

void ParkSetResearchFundingAction::AcceptParameters(GameActionParameterVisitor& visitor)
{
    visitor.Visit("priorities", _priorities);
    visitor.Visit("fundingAmount", _fundingAmount);
}

uint16_t ParkSetResearchFundingAction::GetActionFlags() const
{
    return GameAction::GetActionFlags() | GameActions::Flags::AllowWhilePaused;
}

void ParkSetResearchFundingAction::Serialise(DataSerialiser& stream)
{
    GameAction::Serialise(stream);
    stream << DS_TAG(_priorities) << DS_TAG(_fundingAmount);
}

GameActions::Result ParkSetResearchFundingAction::Query() const
{
    if (_fundingAmount >= RESEARCH_FUNDING_COUNT)
    {
        LOG_ERROR("Invalid research funding amount %d", _fundingAmount);
        return GameActions::Result(
            GameActions::Status::InvalidParameters, STR_ERR_INVALID_PARAMETER, STR_ERR_VALUE_OUT_OF_RANGE);
    }
    return GameActions::Result();
}

GameActions::Result ParkSetResearchFundingAction::Execute() const
{
    auto& gameState = GetGameState();
    gameState.ResearchPriorities = _priorities;
    gameState.ResearchFundingLevel = _fundingAmount;

    auto windowManager = OpenRCT2::Ui::GetWindowManager();
    windowManager->BroadcastIntent(Intent(INTENT_ACTION_UPDATE_RESEARCH));
    return GameActions::Result();
}
