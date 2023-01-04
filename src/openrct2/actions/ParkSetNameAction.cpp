/*****************************************************************************
 * Copyright (c) 2014-2023 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "ParkSetNameAction.h"

#include "../Context.h"
#include "../GameState.h"
#include "../config/Config.h"
#include "../core/MemoryStream.h"
#include "../drawing/Drawing.h"
#include "../localisation/Localisation.h"
#include "../management/Finance.h"
#include "../network/network.h"
#include "../ui/UiContext.h"
#include "../ui/WindowManager.h"
#include "../windows/Intent.h"
#include "../world/Park.h"

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
    return GameAction::GetActionFlags() | GameActions::Flags::AllowWhilePaused;
}

void ParkSetNameAction::Serialise(DataSerialiser& stream)
{
    GameAction::Serialise(stream);
    stream << DS_TAG(_name);
}

GameActions::Result ParkSetNameAction::Query() const
{
    if (_name.empty())
    {
        return GameActions::Result(GameActions::Status::InvalidParameters, STR_CANT_RENAME_PARK, STR_INVALID_NAME_FOR_PARK);
    }
    return GameActions::Result();
}

GameActions::Result ParkSetNameAction::Execute() const
{
    // Do a no-op if new name is the same as the current name is the same
    auto& park = OpenRCT2::GetContext()->GetGameState()->GetPark();
    if (_name != park.Name)
    {
        park.Name = _name;
        scrolling_text_invalidate();
        gfx_invalidate_screen();
    }
    return GameActions::Result();
}
