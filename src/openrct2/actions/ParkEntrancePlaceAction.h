/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "GameAction.h"

class ParkEntrancePlaceAction final : public GameActionBase<GameCommand::PlaceParkEntrance>
{
private:
    CoordsXYZD _loc;
    ObjectEntryIndex _pathType;
    ObjectEntryIndex _entranceType;

public:
    ParkEntrancePlaceAction() = default;
    ParkEntrancePlaceAction(const CoordsXYZD& location, ObjectEntryIndex pathType, ObjectEntryIndex entranceType);

    void AcceptParameters(GameActionParameterVisitor& visitor) override;

    uint16_t GetActionFlags() const override;

    void Serialise(DataSerialiser& stream) override;
    OpenRCT2::GameActions::Result Query() const override;
    OpenRCT2::GameActions::Result Execute() const override;

private:
    bool CheckMapCapacity(int16_t numTiles) const;
};
