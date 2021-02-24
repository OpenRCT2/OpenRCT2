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

DEFINE_GAME_ACTION(GuestSetFlagsAction, GameCommand::GuestSetFlags, GameActions::Result)
{
private:
    uint16_t _peepId{ SPRITE_INDEX_NULL };
    uint32_t _newFlags{};

public:
    GuestSetFlagsAction() = default;
    GuestSetFlagsAction(uint16_t peepId, uint32_t flags);

    void AcceptParameters(GameActionParameterVisitor & visitor) override;

    uint16_t GetActionFlags() const override;

    void Serialise(DataSerialiser & stream) override;
    GameActions::Result::Ptr Query() const override;
    GameActions::Result::Ptr Execute() const override;
};
