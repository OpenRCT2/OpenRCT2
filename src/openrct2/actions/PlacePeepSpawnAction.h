/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "GameAction.h"

DEFINE_GAME_ACTION(PlacePeepSpawnAction, GAME_COMMAND_PLACE_PEEP_SPAWN, GameActions::Result)
{
private:
    CoordsXYZD _location;

public:
    PlacePeepSpawnAction() = default;
    PlacePeepSpawnAction(const CoordsXYZD& location)
        : _location(location)
    {
    }

    uint16_t GetActionFlags() const override
    {
        return GameActionBase::GetActionFlags() | GameActions::Flags::EditorOnly | GameActions::Flags::AllowWhilePaused;
    }

    void Serialise(DataSerialiser & stream) override;

    GameActions::Result::Ptr Query() const override;

    GameActions::Result::Ptr Execute() const override;
};
