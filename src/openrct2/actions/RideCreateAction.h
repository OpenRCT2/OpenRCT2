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

class RideCreateAction final : public GameActionBase<GameCommand::CreateRide>
{
private:
    ObjectEntryIndex _rideType{ OBJECT_ENTRY_INDEX_NULL };
    ObjectEntryIndex _subType{ OBJECT_ENTRY_INDEX_NULL };
    ObjectEntryIndex _entranceObjectIndex{ OBJECT_ENTRY_INDEX_NULL };
    uint8_t _colour1{ 0xFF };
    uint8_t _colour2{ 0xFF };

public:
    RideCreateAction() = default;
    RideCreateAction(
        int32_t rideType, ObjectEntryIndex subType, int32_t colour1, int32_t colour2, ObjectEntryIndex entranceStyleIndex);

    void AcceptParameters(GameActionParameterVisitor& visitor) override;

    int32_t GetRideType() const;
    int32_t GetRideObject() const;
    uint16_t GetActionFlags() const override;

    void Serialise(DataSerialiser& stream) override;
    GameActions::Result Query() const override;
    GameActions::Result Execute() const override;
};
