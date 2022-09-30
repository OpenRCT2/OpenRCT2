/*****************************************************************************
 * Copyright (c) 2014-2022 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "SetParkEntranceFeeAction.h"

#include "../Cheats.h"
#include "../core/MemoryStream.h"
#include "../interface/Window.h"
#include "../localisation/StringIds.h"
#include "../world/Park.h"

SetParkEntranceFeeAction::SetParkEntranceFeeAction(money16 fee)
    : _fee(fee)
{
}

void SetParkEntranceFeeAction::AcceptParameters(GameActionParameterVisitor& visitor)
{
    visitor.Visit("value", _fee);
}

uint16_t SetParkEntranceFeeAction::GetActionFlags() const
{
    return GameAction::GetActionFlags() | GameActions::Flags::AllowWhilePaused;
}

void SetParkEntranceFeeAction::Serialise(DataSerialiser& stream)
{
    GameAction::Serialise(stream);

    stream << DS_TAG(_fee);
}

GameActions::Result SetParkEntranceFeeAction::Query() const
{
    bool noMoney = (gParkFlags & PARK_FLAGS_NO_MONEY) != 0;
    bool forceFreeEntry = !park_entry_price_unlocked();
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

GameActions::Result SetParkEntranceFeeAction::Execute() const
{
    gParkEntranceFee = _fee;
    window_invalidate_by_class(WindowClass::ParkInformation);
    return GameActions::Result();
}
