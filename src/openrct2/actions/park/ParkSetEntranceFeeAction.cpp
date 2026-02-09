/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
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

namespace OpenRCT2::GameActions
{
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
        return GameAction::GetActionFlags() | Flags::AllowWhilePaused;
    }

    void ParkSetEntranceFeeAction::Serialise(DataSerialiser& stream)
    {
        GameAction::Serialise(stream);

        stream << DS_TAG(_fee);
    }

    Result ParkSetEntranceFeeAction::Query(GameState_t& gameState) const
    {
        if ((gameState.park.flags & PARK_FLAGS_NO_MONEY) != 0)
        {
            LOG_ERROR("Can't set park entrance fee because the park has no money");
            return Result(Status::disallowed, STR_ERR_CANT_CHANGE_PARK_ENTRANCE_FEE, kStringIdNone);
        }
        else if (!Park::EntranceFeeUnlocked())
        {
            LOG_ERROR("Park entrance fee is locked");
            return Result(Status::disallowed, STR_ERR_CANT_CHANGE_PARK_ENTRANCE_FEE, kStringIdNone);
        }
        else if (_fee < 0.00_GBP || _fee > kMaxEntranceFee)
        {
            LOG_ERROR("Invalid park entrance fee %d", _fee);
            return Result(Status::invalidParameters, STR_ERR_INVALID_PARAMETER, STR_ERR_VALUE_OUT_OF_RANGE);
        }

        return Result();
    }

    Result ParkSetEntranceFeeAction::Execute(GameState_t& gameState) const
    {
        gameState.park.entranceFee = _fee;

        auto* windowMgr = Ui::GetWindowManager();
        windowMgr->InvalidateByClass(WindowClass::parkInformation);

        return Result();
    }
} // namespace OpenRCT2::GameActions
