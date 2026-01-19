/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "ParkSetNameAction.h"

#include "../Context.h"
#include "../Diagnostic.h"
#include "../GameState.h"
#include "../core/MemoryStream.h"
#include "../drawing/Drawing.h"
#include "../management/Finance.h"
#include "../network/Network.h"
#include "../ui/WindowManager.h"
#include "../windows/Intent.h"
#include "../world/Park.h"

namespace OpenRCT2::GameActions
{
    ParkSetNameAction::ParkSetNameAction(const std::string& name)
        : _name(name)
    {
    }

    void ParkSetNameAction::AcceptParameters(GameActionParameterVisitor& visitor)
    {
        visitor.Visit("name", _name);
    }

    uint16_t ParkSetNameAction::GetActionFlags() const
    {
        return GameAction::GetActionFlags() | Flags::AllowWhilePaused;
    }

    void ParkSetNameAction::Serialise(DataSerialiser& stream)
    {
        GameAction::Serialise(stream);
        stream << DS_TAG(_name);
    }

    Result ParkSetNameAction::Query(GameState_t& gameState) const
    {
        if (_name.empty())
        {
            LOG_ERROR("Can't set park name to empty string");
            return Result(Status::invalidParameters, STR_CANT_RENAME_PARK, STR_INVALID_NAME_FOR_PARK);
        }
        return Result();
    }

    Result ParkSetNameAction::Execute(GameState_t& gameState) const
    {
        // Do a no-op if new name is the same as the current name is the same
        auto& park = gameState.park;
        if (_name != park.name)
        {
            park.name = _name;
            Drawing::ScrollingText::invalidate();
            GfxInvalidateScreen();
        }
        return Result();
    }
} // namespace OpenRCT2::GameActions
