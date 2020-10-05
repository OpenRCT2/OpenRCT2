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

DEFINE_GAME_ACTION(TrackSetBrakeSpeedAction, GameCommand::SetBrakesSpeed, GameActions::Result)
{
private:
    CoordsXYZ _loc;
    track_type_t _trackType{};
    uint8_t _brakeSpeed{};

public:
    TrackSetBrakeSpeedAction() = default;
    TrackSetBrakeSpeedAction(const CoordsXYZ& loc, track_type_t trackType, uint8_t brakeSpeed);

    void AcceptParameters(GameActionParameterVisitor & visitor) override;

    uint16_t GetActionFlags() const override final;

    void Serialise(DataSerialiser & stream) override;
    GameActions::Result::Ptr Query() const override;
    GameActions::Result::Ptr Execute() const override;

private:
    GameActions::Result::Ptr QueryExecute(bool isExecuting) const;
};
