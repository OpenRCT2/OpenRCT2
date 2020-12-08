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

DEFINE_GAME_ACTION(TrackRemoveAction, GAME_COMMAND_REMOVE_TRACK, GameActions::Result)
{
private:
    int32_t _trackType{};
    int32_t _sequence{};
    CoordsXYZD _origin;

public:
    TrackRemoveAction() = default;
    TrackRemoveAction(int32_t trackType, int32_t sequence, const CoordsXYZD& origin)
        : _trackType(trackType)
        , _sequence(sequence)
        , _origin(origin)
    {
        _origin.direction &= 3;
    }

    void AcceptParameters(GameActionParameterVisitor & visitor) override;

    uint16_t GetActionFlags() const override final
    {
        return GameAction::GetActionFlags();
    }

    void Serialise(DataSerialiser & stream) override;
    GameActions::Result::Ptr Query() const override;
    GameActions::Result::Ptr Execute() const override;
};
