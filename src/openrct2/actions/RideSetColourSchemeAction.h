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

DEFINE_GAME_ACTION(RideSetColourSchemeAction, GAME_COMMAND_SET_COLOUR_SCHEME, GameActions::Result)
{
private:
    CoordsXYZD _loc;
    int32_t _trackType{};
    uint16_t _newColourScheme{};

public:
    RideSetColourSchemeAction() = default;
    RideSetColourSchemeAction(const CoordsXYZD& location, int32_t trackType, uint16_t newColourScheme);

    void AcceptParameters(GameActionParameterVisitor & visitor) override;

    uint16_t GetActionFlags() const override;

    void Serialise(DataSerialiser & stream) override;
    GameActions::Result::Ptr Query() const override;
    GameActions::Result::Ptr Execute() const override;
};
