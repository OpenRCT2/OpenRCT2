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

DEFINE_GAME_ACTION(ParkEntranceRemoveAction, GAME_COMMAND_REMOVE_PARK_ENTRANCE, GameActions::Result)
{
private:
    CoordsXYZ _loc;

public:
    ParkEntranceRemoveAction() = default;

    ParkEntranceRemoveAction(const CoordsXYZ& loc)
        : _loc(loc)
    {
    }

    uint16_t GetActionFlags() const override
    {
        return GameAction::GetActionFlags() | GameActions::Flags::EditorOnly;
    }

    void Serialise(DataSerialiser & stream) override;
    GameActions::Result::Ptr Query() const override;
    GameActions::Result::Ptr Execute() const override;

private:
    void ParkEntranceRemoveSegment(const CoordsXYZ& loc) const;
};
