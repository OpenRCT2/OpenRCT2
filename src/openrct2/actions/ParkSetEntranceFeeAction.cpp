/*****************************************************************************
 * Copyright (c) 2014-2023 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "ParkSetEntranceFeeAction.h"

#include "../Cheats.h"
#include "../core/MemoryStream.h"
#include "../interface/Window.h"
#include "../localisation/StringIds.h"
#include "../world/Park.h"

ParkSetEntranceFeeAction::ParkSetEntranceFeeAction(money16 fee)
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
    bool noMoney = (gParkFlags & PARK_FLAGS_NO_MONEY) != 0;
    bool forceFreeEntry = !ParkEntranceFeeUnlocked();
    if (noMoney || forceFreeEntry)
    {
        return GameActions::Result(GameActions::Status::Disallowed, STR_NONE, STR_NONE);
    }
    if (_fee < 0.00_GBP || _fee > MAX_ENTRANCE_FEE)
    {
        return GameActions::Result(GameActions::Status::InvalidParameters, STR_NONE, STR_NONE);
    }
    return GameActions::Result();
}

GameActions::Result ParkSetEntranceFeeAction::Execute() const
{
    gParkEntranceFee = _fee;
    window_invalidate_by_class(WindowClass::ParkInformation);
    return GameActions::Result();
}
