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

DEFINE_GAME_ACTION(BannerSetColourAction, GAME_COMMAND_SET_BANNER_COLOUR, GameActions::Result)
{
private:
    CoordsXYZD _loc;
    uint8_t _primaryColour{};

public:
    BannerSetColourAction() = default;

    BannerSetColourAction(const CoordsXYZD& loc, uint8_t primaryColour)
        : _loc(loc)
        , _primaryColour(primaryColour)
    {
    }

    void AcceptParameters(GameActionParameterVisitor & visitor) override;

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
