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

DEFINE_GAME_ACTION(LargeSceneryRemoveAction, GAME_COMMAND_REMOVE_LARGE_SCENERY, GameActions::Result)
{
private:
    CoordsXYZD _loc;
    uint16_t _tileIndex{};

public:
    LargeSceneryRemoveAction() = default;

    LargeSceneryRemoveAction(const CoordsXYZD& location, uint16_t tileIndex)
        : _loc(location)
        , _tileIndex(tileIndex)
    {
    }

    void AcceptParameters(GameActionParameterVisitor & visitor) override;

    uint16_t GetActionFlags() const override
    {
        return GameAction::GetActionFlags();
    }

    void Serialise(DataSerialiser & stream) override;

    GameActions::Result::Ptr Query() const override;

    GameActions::Result::Ptr Execute() const override;

private:
    TileElement* FindLargeSceneryElement() const;
};
