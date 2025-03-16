/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "ParkSetEntranceFeeAction.h"

#include "../Cheats.h"
#include "../Diagnostic.h"
#include "../GameState.h"
#include "../core/MemoryStream.h"
#include "../localisation/StringIds.h"
#include "../ui/WindowManager.h"
#include "../world/Park.h"

using namespace OpenRCT2;

ParkSetEntranceFeeAction::ParkSetEntranceFeeAction(money64 fee)
    : _fee(fee)
{
}

void ParkSetEntranceFeeAction::AcceptParameters(GameActionParameterVisitor& visitor)
{
    visitor.Visit("value", _fee);
}

uint16_t ParkSetEntranceFeeAction::GetActionFlags() const
{
    return GameAction::GetActionFlags() | GameActions::Flags::AllowWhilePaused;
}

void ParkSetEntranceFeeAction::Serialise(DataSerialiser& stream)
{
    GameAction::Serialise(stream);

    stream << DS_TAG(_fee);
}

GameActions::Result ParkSetEntranceFeeAction::Query() const
{
    if ((GetGameState().Park.Flags & PARK_FLAGS_NO_MONEY) != 0)
    {
        LOG_ERROR("Can't set park entrance fee because the park has no money");
        return GameActions::Result(GameActions::Status::Disallowed, STR_ERR_CANT_CHANGE_PARK_ENTRANCE_FEE, kStringIdNone);
    }
    else if (!Park::EntranceFeeUnlocked())
    {
        LOG_ERROR("Park entrance fee is locked");
        return GameActions::Result(GameActions::Status::Disallowed, STR_ERR_CANT_CHANGE_PARK_ENTRANCE_FEE, kStringIdNone);
    }
    else if (_fee < 0.00_GBP || _fee > kMaxEntranceFee)
    {
        LOG_ERROR("Invalid park entrance fee %d", _fee);
        return GameActions::Result(
            GameActions::Status::InvalidParameters, STR_ERR_INVALID_PARAMETER, STR_ERR_VALUE_OUT_OF_RANGE);
    }

    return GameActions::Result();
}

GameActions::Result ParkSetEntranceFeeAction::Execute() const
{
    GetGameState().Park.EntranceFee = _fee;

    auto* windowMgr = Ui::GetWindowManager();
    windowMgr->InvalidateByClass(WindowClass::ParkInformation);

    return GameActions::Result();
}
