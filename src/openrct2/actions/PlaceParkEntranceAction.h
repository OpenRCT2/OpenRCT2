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

DEFINE_GAME_ACTION(PlaceParkEntranceAction, GAME_COMMAND_PLACE_PARK_ENTRANCE, GameActions::Result)
{
private:
    CoordsXYZD _loc;

public:
    PlaceParkEntranceAction() = default;
    PlaceParkEntranceAction(const CoordsXYZD& location)
        : _loc(location)
    {
    }

    uint16_t GetActionFlags() const override
    {
        return GameActionBase::GetActionFlags() | GameActions::Flags::EditorOnly;
    }

    void Serialise(DataSerialiser & stream) override;

    GameActions::Result::Ptr Query() const override;

    GameActions::Result::Ptr Execute() const override;
};