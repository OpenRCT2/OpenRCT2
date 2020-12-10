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

DEFINE_GAME_ACTION(LargeScenerySetColourAction, GAME_COMMAND_SET_LARGE_SCENERY_COLOUR, GameActions::Result)
{
private:
    CoordsXYZD _loc;
    uint8_t _tileIndex{};
    uint8_t _primaryColour{};
    uint8_t _secondaryColour{};

public:
    LargeScenerySetColourAction() = default;

    LargeScenerySetColourAction(const CoordsXYZD& loc, uint8_t tileIndex, uint8_t primaryColour, uint8_t secondaryColour)
        : _loc(loc)
        , _tileIndex(tileIndex)
        , _primaryColour(primaryColour)
        , _secondaryColour(secondaryColour)
    {
    }

    uint16_t GetActionFlags() const override
    {
        return GameAction::GetActionFlags() | GameActions::Flags::AllowWhilePaused;
    }

    void Serialise(DataSerialiser & stream) override;
    GameActions::Result::Ptr Query() const override;
    GameActions::Result::Ptr Execute() const override;

private:
    GameActions::Result::Ptr QueryExecute(bool isExecuting) const;
};
