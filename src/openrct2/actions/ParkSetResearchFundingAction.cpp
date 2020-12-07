/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "ParkSetResearchFundingAction.h"

#include "../Context.h"
#include "../core/MemoryStream.h"
#include "../localisation/StringIds.h"
#include "../management/Research.h"
#include "../ui/UiContext.h"
#include "../ui/WindowManager.h"
#include "../windows/Intent.h"

void ParkSetResearchFundingAction::Serialise(DataSerialiser& stream)
{
    GameAction::Serialise(stream);
    stream << DS_TAG(_priorities) << DS_TAG(_fundingAmount);
}

GameActions::Result::Ptr ParkSetResearchFundingAction::Query() const
{
    if (_fundingAmount >= RESEARCH_FUNDING_COUNT)
    {
        return MakeResult(GameActions::Status::InvalidParameters, STR_NONE);
    }
    return MakeResult();
}

GameActions::Result::Ptr ParkSetResearchFundingAction::Execute() const
{
    gResearchPriorities = _priorities;
    gResearchFundingLevel = _fundingAmount;

    auto windowManager = OpenRCT2::GetContext()->GetUiContext()->GetWindowManager();
    windowManager->BroadcastIntent(Intent(INTENT_ACTION_UPDATE_RESEARCH));
    return MakeResult();
}
