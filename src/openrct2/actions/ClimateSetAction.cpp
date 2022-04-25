/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "ClimateSetAction.h"

ClimateSetAction::ClimateSetAction(ClimateType climate)
    : _climate(climate)
{
}

void ClimateSetAction::AcceptParameters(GameActionParameterVisitor& visitor)
{
    visitor.Visit("climate", _climate);
}

uint16_t ClimateSetAction::GetActionFlags() const
{
    return GameAction::GetActionFlags();
}

void ClimateSetAction::Serialise(DataSerialiser& stream)
{
    GameAction::Serialise(stream);

    stream << DS_TAG(_climate);
}

GameActions::Result ClimateSetAction::Query() const
{
    if (_climate >= ClimateType::Count)
    {
        return GameActions::Result(GameActions::Status::InvalidParameters, STR_INVALID_CLIMATE_ID, STR_NONE);
    }

    return GameActions::Result();
}

GameActions::Result ClimateSetAction::Execute() const
{
    gClimate = ClimateType{ _climate };

    gfx_invalidate_screen();

    return GameActions::Result();
}
