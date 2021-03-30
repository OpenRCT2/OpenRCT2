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

class RideCreateGameActionResult final : public GameActions::Result
{
public:
    RideCreateGameActionResult();
    RideCreateGameActionResult(GameActions::Status error, rct_string_id message);

    ride_id_t rideIndex = RIDE_ID_NULL;
};

DEFINE_GAME_ACTION(RideCreateAction, GameCommand::CreateRide, RideCreateGameActionResult)
{
private:
    int32_t _rideType{ RIDE_ID_NULL };
    ObjectEntryIndex _subType{ OBJECT_ENTRY_INDEX_NULL };
    uint8_t _colour1{ 0xFF };
    uint8_t _colour2{ 0xFF };

public:
    RideCreateAction() = default;
    RideCreateAction(int32_t rideType, ObjectEntryIndex subType, int32_t colour1, int32_t colour2);

    void AcceptParameters(GameActionParameterVisitor & visitor) override;

    int32_t GetRideType() const;
    int32_t GetRideObject() const;
    uint16_t GetActionFlags() const override;

    void Serialise(DataSerialiser & stream) override;
    GameActions::Result::Ptr Query() const override;
    GameActions::Result::Ptr Execute() const override;
};
